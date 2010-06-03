/***************************************************************************\
 *
 *
 *         ___           ___           ___           ___
 *        /  /\         /  /\         /  /\         /  /\
 *       /  /::\       /  /:/_       /  /:/_       /  /::\
 *      /  /:/\:\     /  /:/ /\     /  /:/ /\     /  /:/\:\
 *     /  /:/~/:/    /  /:/ /:/_   /  /:/ /::\   /  /:/~/:/
 *    /__/:/ /:/___ /__/:/ /:/ /\ /__/:/ /:/\:\ /__/:/ /:/
 *    \  \:\/:::::/ \  \:\/:/ /:/ \  \:\/:/~/:/ \  \:\/:/
 *     \  \::/~~~~   \  \::/ /:/   \  \::/ /:/   \  \::/
 *      \  \:\        \  \:\/:/     \__\/ /:/     \  \:\
 *       \  \:\        \  \::/        /__/:/       \  \:\
 *        \__\/         \__\/         \__\/         \__\/
 *
 *
 *
 *
 *   This file is part of ReSP.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   The following code is derived, directly or indirectly, from the SystemC
 *   source code Copyright (c) 1996-2004 by all Contributors.
 *   All Rights reserved.
 *
 *   The contents of this file are subject to the restrictions and limitations
 *   set forth in the SystemC Open Source License Version 2.4 (the "License");
 *   You may not use this file except in compliance with such restrictions and
 *   limitations. You may obtain instructions on how to receive a copy of the
 *   License at http://www.systemc.org/. Software distributed by Contributors
 *   under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 *   ANY KIND, either express or implied. See the License for the specific
 *   language governing rights and limitations under the License.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *   or see <http://www.gnu.org/licenses/>.
 *
 *
 *
 *   (c) Giovanni Beltrame, Luca Fossati
 *       Giovanni.Beltrame@esa.int fossati@elet.polimi.it
 *
 *   Component developed by: Antonio Miele miele@elet.polimi.it
 *
\***************************************************************************/

#ifndef FUNCTIONPROFILER_HPP
#define FUNCTIONPROFILER_HPP

#include <map>
#include <set>
#include <string>

#include <systemc.h>

#include "ABIIf.hpp"
#include "ToolsIf.hpp"
#include "instructionBase.hpp"
#include "bfdWrapper.hpp"
#include "utils.hpp"

using namespace trap;

/*This trap tool is devoted to the profiling of the function calls during the execution of a program on a multi-processor architecture.
The result is the trace of the function calls and returns; each entry of the trace contains the name of the function, the time instant and the 
id of the processor running that function. DO NOTE: the profiling is based on the data provided by the BDF frontend; some parts of the trace related 
to the entry routines and exit routines of the program may contain some inaccuracy
*/
template<typename issueWidth> class functionProfiler: public ToolsIf<issueWidth> {
private:
	std::string execName;
	ABIIf<issueWidth> &processorInstance;
	std::string currFunc;
	BFDWrapper* bfdFE;
	std::vector<std::string> log;

public:
	functionProfiler(ABIIf<issueWidth> &processorInstance, std::string exec) : processorInstance(processorInstance), execName(exec) {
		this->log.clear();
		this->currFunc = "";
		bfdFE = &(BFDWrapper::getInstance(this->execName));
	}

	///Method called at every instruction issue, it returns true in case the instruction
	///has to be skipped, false otherwise
	bool newIssue(const issueWidth &curPC, const InstructionBase *curInstr) throw(){
  	if(this->currFunc != this->bfdFE->functionAt(curPC) && this->bfdFE->isRoutineEntry(curPC)){
  	    this->currFunc = this->bfdFE->functionAt(curPC);
  	    log.push_back(MAKE_STRING("time: " << (((long)sc_time_stamp().to_default_time_units())) << " - address: " << std::hex << curPC << std::dec << " - Enter: " << currFunc << " - Processor: " << this->processorInstance.getProcessorID()));
  	}else if(this->currFunc != this->bfdFE->functionAt(curPC)){
  	    this->currFunc = this->bfdFE->functionAt(curPC);
  	    log.push_back(MAKE_STRING("time: " << (((long)sc_time_stamp().to_default_time_units())) <<" - address: " << std::hex << curPC << std::dec << " - Re-enter: " << currFunc << " - Processor: " << this->processorInstance.getProcessorID()));
  	}else if(this->bfdFE->isRoutineExit(curPC)){
  	    log.push_back(MAKE_STRING("time: " << (((long)sc_time_stamp().to_default_time_units())) << " - address: " << std::hex << curPC << std::dec << " - Exit: " << currFunc << " - Processor: " << this->processorInstance.getProcessorID()));
  	}
		return false;
	}

	///Method called to know if the instruction at the current address has to be skipped:
	///if true the instruction has to be skipped, otherwise the instruction can
	///be executed
	bool emptyPipeline(const issueWidth &curPC) const throw(){
		return false;
	}

	///Resets the whole concurrency emulator, reinitializing it and preparing it for a new simulation
	void reset(){
		this->log.clear();
		this->currFunc="";
		this->bfdFE = &BFDWrapper::getInstance(this->execName);
	}
	
	void printLog(){
	  std::cout << "Function trace:" << std::endl;
	  for(int i = 0; i < this->log.size(); i++){
	      std::cout << this->log.at(i) << std::endl;
	  }
	}

	void saveLog(std::string filename){
    ofstream outfile;
    outfile.open (filename.c_str());
	  outfile << "Function trace:" << std::endl;
	  for(int i = 0; i < this->log.size(); i++){
	      outfile << this->log.at(i) << std::endl;
	  }
    outfile.close();
	}


	~functionProfiler(){}
};

#endif // FUNCTIONPROFILER_HPP
