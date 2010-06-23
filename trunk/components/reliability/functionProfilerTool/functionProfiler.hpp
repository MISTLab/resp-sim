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
#include <iostream>
#include <fstream>
#include <sstream>

#include <systemc.h>

#include "ABIIf.hpp"
#include "ToolsIf.hpp"
#include "instructionBase.hpp"
#include "bfdWrapper.hpp"
#include "utils.hpp"
#include <iomanip>

using namespace trap;

/*This trap tool is devoted to the profiling of the function calls during the execution of a program on a multi-processor architecture.
*/
template<typename issueWidth> class functionProfiler: public ToolsIf<issueWidth> {

private:
  
  enum param_type {VALUE, ADDRESS_RETURN, ADDRESS_CALL};
  
  struct param_t{
    int id;
    param_type type;
    issueWidth value;
    int size;
    int num_saved_files;
  };
  
	std::string execName;
	ABIIf<issueWidth> &processorInstance;
	std::string currFunc;
	BFDWrapper* bfdFE;
	std::vector<std::string> log;
	std::vector<std::string> printLog;
	std::map<std::string, std::vector<param_t> > functionsToLog;
	std::vector<std::string> callTrace;
	bool enableTrace;
	std::string dataFolderName;

public:
	functionProfiler(ABIIf<issueWidth> &processorInstance, std::string exec, std::string functionDescriptor, std::string dataFolderName = "") : 
	        processorInstance(processorInstance), execName(exec), dataFolderName(dataFolderName) {
		this->log.clear();
		this->currFunc = "";
		bfdFE = &(BFDWrapper::getInstance(this->execName));
		
		//load functions descriptor
		std::ifstream descriptorFile;
    descriptorFile.open(functionDescriptor.c_str());
    int currNumParams;
    std::string currParamType;
    std::string currFunction;
    int currSize;
    if(descriptorFile.is_open()){
      descriptorFile >> currFunction >> currNumParams;
      while (!descriptorFile.eof()) {
        for(int j = 0; j < currNumParams; j++){
          param_t curr_par;
          curr_par.id = j;
          curr_par.num_saved_files = 0;
          descriptorFile >> currParamType;
          if(currParamType == "address"){
            descriptorFile >> currParamType;
            if(currParamType == "call")
              curr_par.type = ADDRESS_CALL;
            else
              curr_par.type = ADDRESS_RETURN;
            descriptorFile >> currSize;
            curr_par.size = currSize;
          }else{
            curr_par.type = VALUE;
            curr_par.size = 0;
          }
          this->functionsToLog[currFunction].push_back(curr_par);
        }
        descriptorFile >> currFunction >> currNumParams;
      }
    }
    else
      THROW_EXCEPTION(functionDescriptor<<" file not found!");
    
    enableTrace = false;    
	}

	///Method called at every instruction issue, it returns true in case the instruction
	///has to be skipped, false otherwise
	bool newIssue(const issueWidth &curPC, const InstructionBase *curInstr) throw(){
    //analyze current instruction to identify: routine entry, routine exit and routine re-entering
  	
  	//routine entry
  	if(this->currFunc != this->bfdFE->functionAt(curPC) && this->bfdFE->isRoutineEntry(curPC)){
	    //save current function name
	    this->currFunc = this->bfdFE->functionAt(curPC);
	    
	    //log event
	    std::string curr_log_item = MAKE_STRING((((long)sc_time_stamp().to_default_time_units())) << "\t" 
	           << std::hex << curPC << std::dec << "\tEnter\t" << currFunc << "\t" 
	           << this->processorInstance.getProcessorID());
	    printLog.push_back(formatLogOutput((long)sc_time_stamp().to_default_time_units(), curPC, "Enter", currFunc, this->processorInstance.getProcessorID()));
	    
	    if(this->currFunc == "main") //enable tracing when entering the main function
	      enableTrace = true;
	    
	    if(enableTrace) //save trace
	      callTrace.push_back(MAKE_STRING("enter:"<<this->currFunc));
	    
	    //log parameter values
	    if(this->functionsToLog.count(this->currFunc) != 0){
  	    int numOfParams = this->functionsToLog[this->currFunc].size();
  	    std::vector< issueWidth > callArgs = processorInstance.readArgs();
	      //log parameter values
	      for(int i = 0; i < numOfParams; i++){
	        issueWidth currValue;
	        if(i<4){
	          currValue = callArgs[i];
	        } else{
	          currValue = processorInstance.readMem(processorInstance.readSP()+sizeof(issueWidth)*(i-4));
	        }
          curr_log_item = MAKE_STRING(curr_log_item <<"\t" << currValue);
	        this->functionsToLog[this->currFunc][i].value = currValue;
	      }		      
	    }

	    log.push_back(curr_log_item);

      //save memory content
      std::vector<param_t> currPars = this->functionsToLog[this->currFunc];
	    for(int i=0; i < currPars.size(); i++){
	      if(currPars[i].type==ADDRESS_CALL){
	        std::string filename = MAKE_STRING(this->currFunc << "_" << currPars[i].id << "_" << currPars[i].num_saved_files << "_call.dat");
 	        ofstream logFile;
	        logFile.open (filename.c_str());
	        for(int j = 0; j < currPars[i].size; j++){
	          logFile << (unsigned int )processorInstance.readCharMem(currPars[i].value + j) << std::endl;
	        } 
	        logFile.close();
	      }
	    }

  	}else if(this->currFunc != this->bfdFE->functionAt(curPC)){ //routine re-entering	      
      //save current function name
      this->currFunc = this->bfdFE->functionAt(curPC);
	    
      //save trace
      if(enableTrace)
          callTrace.push_back(MAKE_STRING("re-enter:"<<this->currFunc));
	    
	    //log event
	    log.push_back(MAKE_STRING((((long)sc_time_stamp().to_default_time_units())) << "\t" 
	           << std::hex << curPC << std::dec << "\tRe-enter\t" << currFunc << "\t" 
	           << this->processorInstance.getProcessorID()));
	    printLog.push_back(formatLogOutput((long)sc_time_stamp().to_default_time_units(), curPC, "Re-enter", currFunc, this->processorInstance.getProcessorID()));

  	}else if(processorInstance.isRoutineExit(curInstr)){ //routine exit
  	  //this->currFunc = this->bfdFE->functionAt(curPC);
	    log.push_back(MAKE_STRING((((long)sc_time_stamp().to_default_time_units())) << "\t" 
	           << std::hex << curPC << std::dec << "\tExit\t" << currFunc << "\t" 
	           << this->processorInstance.getProcessorID()));
	    printLog.push_back(formatLogOutput((long)sc_time_stamp().to_default_time_units(), curPC, "Exit", currFunc, this->processorInstance.getProcessorID()));
   	    
 	    //save trace
 	    if(enableTrace) 
        callTrace.push_back(MAKE_STRING("exit:"<<this->currFunc));
      
      //disable trace
      if(this->currFunc == "main") 
        enableTrace = false;  
    
      //save memory content
      std::vector<param_t> currPars = this->functionsToLog[this->currFunc];
	    for(int i=0; i < currPars.size(); i++){
	      if(currPars[i].type==ADDRESS_RETURN){
	        std::string filename = MAKE_STRING(dataFolderName << "/" << this->currFunc << "_" << currPars[i].id << "_" << currPars[i].num_saved_files << "_exit.dat");
 	        ofstream logFile;
	        logFile.open (filename.c_str());
	        for(int j = 0; j < currPars[i].size; j++){
	          logFile << (unsigned int )processorInstance.readCharMem(currPars[i].value + j) << std::endl;
	        } 
	        logFile.close();
	      }
	    }
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
	
  void showLog(){
    std::cout << "FUNCTION CALL TRACE:" << std::endl;
    for(int i = 0; i < this->printLog.size(); i++){
	      std::cout << this->printLog.at(i) << std::endl;
	  }
	  std::cout << std::endl;
  }

	void saveLog(){
    ofstream outfile;
    outfile.open (MAKE_STRING(this->dataFolderName << "/trace.txt").c_str());
	  outfile << "Time\tAddress\tEvent\tFunction\tProcessorId\tParameters" << std::endl;
	  for(int i = 0; i < this->log.size(); i++){
	      outfile << this->log.at(i) << std::endl;
	  }
    outfile.close();
    
    ofstream tracefile;
    tracefile.open (MAKE_STRING(this->dataFolderName << "/short_trace.txt").c_str());
    for(int i=0; i < callTrace.size(); i++)
      tracefile << callTrace[i] << std::endl;
    tracefile.close();
	}

	~functionProfiler(){}
	
private:
  std::string formatLogOutput(long long int time, long int address, std::string event, std::string function, int processor){
    std::string timeString = "";
    
    int timeLenght = this->numberLenght(time);
    for(int i = 0; i < 10-timeLenght; i++){
      timeString =  timeString + " ";
    }
    timeString = MAKE_STRING(timeString << time);

    std::string addressString = "";
    int addressLenght = this->hexNumberLenght(address);
    for(int i = 0; i < 6-addressLenght; i++){
      addressString =  addressString + " ";
    }
    addressString = MAKE_STRING(addressString << std::hex << address << std::dec);
    
    std::string eventString ="";
    eventString = eventString + event;
    for(int i = 0; i < 10-event.size(); i++){
      eventString =  eventString + " ";
    }

    std::string functionString = "";
    functionString = functionString + function;
    for(int i = 0; i < 20-function.size(); i++){
      functionString =  functionString + " ";
    }
    
    return MAKE_STRING("TIME: " << timeString << " -> ADDRESS:" << addressString << " " << eventString << " " << functionString << " on Processor " << processor);    
  }

	int numberLenght(long long int datum){	  
    std::ostringstream o;
    o << datum;
    return o.str().size();
	}

	int hexNumberLenght(long long int datum){	  
    std::ostringstream o;
    o << std::hex << datum << std::dec;
    return o.str().size();
	}


};

#endif // FUNCTIONPROFILER_HPP
