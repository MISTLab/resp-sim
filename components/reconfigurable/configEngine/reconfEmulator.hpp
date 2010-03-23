/***************************************************************************\
 *
 *
 *            ___        ___           ___           ___
 *           /  /\      /  /\         /  /\         /  /\
 *          /  /:/     /  /::\       /  /::\       /  /::\
 *         /  /:/     /  /:/\:\     /  /:/\:\     /  /:/\:\
 *        /  /:/     /  /:/~/:/    /  /:/~/::\   /  /:/~/:/
 *       /  /::\    /__/:/ /:/___ /__/:/ /:/\:\ /__/:/ /:/
 *      /__/:/\:\   \  \:\/:::::/ \  \:\/:/__\/ \  \:\/:/
 *      \__\/  \:\   \  \::/~~~~   \  \::/       \  \::/
 *           \  \:\   \  \:\        \  \:\        \  \:\
 *            \  \ \   \  \:\        \  \:\        \  \:\
 *             \__\/    \__\/         \__\/         \__\/
 *
 *
 *
 *
 *   This file is part of TRAP.
 *
 *   TRAP is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *   or see <http://www.gnu.org/licenses/>.
 *
 *
 *
 *   (c) Luca Fossati, fossati@elet.polimi.it
 *
 *   Component modified by: Fabio Arlati arlati.fabio@gmail.com
 *
\***************************************************************************/

#ifndef RECONFEMULATOR_HPP
#define RECONFEMULATOR_HPP

#include <map>
#include <set>
#include <string>

#include <systemc.h>

#ifdef __GNUC__
#ifdef __GNUC_MINOR__
#if (__GNUC__ >= 4 && __GNUC_MINOR__ >= 3)
#include <tr1/unordered_map>
#define template_map std::tr1::unordered_map
#else
#include <ext/hash_map>
#define  template_map __gnu_cxx::hash_map
#endif
#else
#include <ext/hash_map>
#define  template_map __gnu_cxx::hash_map
#endif
#else
#ifdef _WIN32
#include <hash_map>
#define  template_map stdext::hash_map
#else
#include <map>
#define  template_map std::map
#endif
#endif

#include "ABIIf.hpp"
#include "ToolsIf.hpp"
#include "instructionBase.hpp"

#ifndef EXTERNAL_BFD
#include "bfdFrontend.hpp"
#else
#include "bfdWrapper.hpp"
#define BFDFrontend BFDWrapper
#endif

using namespace std;
using namespace trap;

namespace reconfEmu {

class reconfEmulatorBase {
public:
	virtual set<string> getRegisteredFunctions() = 0;

	map<string, string> env;
	map<string, int> sysconfmap;
	vector<string> programArgs;
	map<int, unsigned int> heapPointer;
};

template<class issueWidth> class reconfCB {
public:
	sc_time latency;
	unsigned int width, height;
	reconfCB(sc_time latency = SC_ZERO_TIME, unsigned int w = 1, unsigned int h = 1) :
		latency(latency), width(w), height(h)	{}
	virtual ~reconfCB(){}
	virtual bool operator()(ABIIf<issueWidth> &processorInstance) = 0;
};

template<class issueWidth> class reconfEmulator: public ToolsIf<issueWidth>, reconfEmulatorBase {
private:
	template_map<issueWidth, reconfCB<issueWidth>* > syscCallbacks;
	typename template_map<issueWidth, reconfCB<issueWidth>* >::const_iterator syscCallbacksEnd;
	ABIIf<issueWidth> &processorInstance;

public:
	reconfEmulator(ABIIf<issueWidth> &processorInstance) : processorInstance(processorInstance){
		this->syscCallbacksEnd = this->syscCallbacks.end();
	}

	bool register_call(string funName, reconfCB<issueWidth> &callBack){
		BFDFrontend &bfdFE = BFDFrontend::getInstance();
		bool valid = false;
		unsigned int symAddr = bfdFE.getSymAddr(funName, valid);
		if(!valid){
			return false;
		}

		typename template_map<issueWidth, reconfCB<issueWidth>* >::iterator foundSysc = this->syscCallbacks.find(symAddr);
		if(foundSysc != this->syscCallbacks.end()){
		int numMatch = 0;
		typename template_map<issueWidth, reconfCB<issueWidth>* >::iterator allCallIter, allCallEnd;
		for(allCallIter = this->syscCallbacks.begin(), allCallEnd = this->syscCallbacks.end(); allCallIter != allCallEnd; allCallIter++){
			if(allCallIter->second == foundSysc->second)
				numMatch++;
			}
			if(numMatch <= 1){
				delete foundSysc->second;
			}
		}

		this->syscCallbacks[symAddr] = &callBack;
		this->syscCallbacksEnd = this->syscCallbacks.end();

		return true;
	}

	set<string> getRegisteredFunctions(){
		BFDFrontend &bfdFE = BFDFrontend::getInstance();
		set<string> registeredFunctions;
		typename template_map<issueWidth, reconfCB<issueWidth>* >::iterator emuIter, emuEnd;
		for(emuIter = this->syscCallbacks.begin(), emuEnd = this->syscCallbacks.end(); emuIter != emuEnd; emuIter++){
			registeredFunctions.insert(bfdFE.symbolAt(emuIter->first));
		}
		return registeredFunctions;
	}

	void printRegisteredFunctions() {
		set<string>::iterator tmp;
		set<string> ss = this->getRegisteredFunctions();
		for (tmp=ss.begin(); tmp!=ss.end(); tmp++) {cerr << *tmp << endl;}
	}

	void initExec(string execName, int group = 0) {
//		if(reconfEmulatorBase::heapPointer.find(group) == reconfEmulatorBase::heapPointer.end())
//			reconfEmulatorBase::heapPointer[group] = (unsigned int)this->processorInstance.getCodeLimit() + sizeof(unsigned int);
		BFDFrontend::getInstance(execName);
	}

	///Method called at every instruction issue, it returns true in case the instruction
	///has to be skipped, false otherwise
	bool newIssue(const issueWidth &curPC, const InstructionBase *curInstr) throw(){
		//I have to go over all the registered system calls and check if there is one
		//that matches the current program counter. In case I simply call the corresponding
		//callback.
		typename template_map<issueWidth, reconfCB<issueWidth>* >::const_iterator foundSysc = this->syscCallbacks.find(curPC);
		if(foundSysc != this->syscCallbacksEnd){
			return (*(foundSysc->second))(this->processorInstance);
		}
		return false;
	}

	///Method called to know if the instruction at the current address has to be skipped:
	///if true the instruction has to be skipped, otherwise the instruction can
	///be executed
	bool emptyPipeline(const issueWidth &curPC) const throw(){
		if(this->syscCallbacks.find(curPC) != this->syscCallbacksEnd){
			return true;
		}
		return false;
	}

	///Resets the whole concurrency emulator, reinitializing it and preparing it for a new simulation
	void reset(){
		this->syscCallbacks.clear();
		this->syscCallbacksEnd = this->syscCallbacks.end();
		reconfEmulatorBase::env.clear();
		reconfEmulatorBase::sysconfmap.clear();
		reconfEmulatorBase::programArgs.clear();
		reconfEmulatorBase::heapPointer.clear();
	}

	virtual ~reconfEmulator(){}

};

};

#endif // RECONFEMULATOR_HPP
