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

#ifndef CHECKERTOOL_HPP
#define CHECKERTOOL_HPP

#include <map>
#include <set>
#include <string>
#include <iostream>
#include <fstream>

#include <systemc.h>

#include "ABIIf.hpp"
#include "ToolsIf.hpp"
#include "instructionBase.hpp"
#include "bfdWrapper.hpp"
#include "utils.hpp"

using namespace trap;

template<typename issueWidth> class checkerTool: public ToolsIf<issueWidth> {

private:
  
  enum param_type {VALUE, ADDRESS_RETURN, ADDRESS_CALL};
  
  //structure representing a parameter of a function
  struct param_t{
    int id;
    param_type type;
    issueWidth value;
    int size;
    int num_saved_files;
  };

  //structure representing a "checkpoint" for the monitoring (function call or return)
  //at the present moment it stores the data in am array of integers (actually they stores a character)
  struct checkpoint_t{
    std::string name;
    int size;
    unsigned int* data;    
  };

  //stuff for function profiling
	std::string execName;
	ABIIf<issueWidth> &processorInstance;
	std::string currFunc;
	BFDWrapper* bfdFE;
	
	std::map<std::string, std::vector<param_t> > functionsToLog; //

  //stores the checkpoints
  std::map<std::string, checkpoint_t > checkpoints;
  
  std::map<std::string, int> errors; //number of errors in the checkpointed data
  std::string controlFlowError;
  
  //loaded calltrace
  std::vector<std::string> calltrace;
	bool enableTrace;
	int currTraceElement;

	std::string dataFolderName;

  
public:
	checkerTool(ABIIf<issueWidth> &processorInstance, std::string exec, std::string functionDescriptor, 
	        std::string checkpointsDescriptor, std::string callTraceFileName, std::string dataFolderName = "") : 
	        processorInstance(processorInstance), execName(exec), dataFolderName(dataFolderName) {
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

    //load checkpoints data
		std::ifstream checkpointsFile;
    checkpointsFile.open(checkpointsDescriptor.c_str());
    std::string currCheckpoint;
    int currCpSize;
    unsigned int currDatum;
    if(checkpointsFile.is_open()){
      checkpointsFile >> currCheckpoint >> currCpSize;
      while (!checkpointsFile.eof()) {
        checkpoint_t currCheck;
        currCheck.name = currCheckpoint;
        currCheck.size = currCpSize;
        currCheck.data = new unsigned int[currCpSize];
        std::string currCheckpointFilename = MAKE_STRING(this->dataFolderName << "/" <<currCheckpoint);
        std::ifstream currCheckpointFile(currCheckpointFilename.c_str(), ios::in);
        if(!currCheckpointFile.is_open())
          THROW_EXCEPTION(currCheckpointFilename<<" file not found!");
        for(int i=0; i<currCheck.size; i++){
          currCheckpointFile >> currDatum;
          currCheck.data[i] = currDatum;
        }
        currCheckpointFile.close();
        checkpoints[currCheckpoint] = currCheck;
        checkpointsFile >> currCheckpoint >> currCpSize;
      }
      descriptorFile.close();
    }
    else
      THROW_EXCEPTION(checkpointsDescriptor<<" file not found!");
	  
	  //load call trace
	  std::ifstream callTraceFile;
    callTraceFile.open(callTraceFileName.c_str());
    std::string currCall;
    if(callTraceFile.is_open()){
      callTraceFile >> currCall;
      while (!callTraceFile.eof()) {
        calltrace.push_back(currCall);
        callTraceFile >> currCall;
      }
      callTraceFile.close();
    }
    else
      THROW_EXCEPTION(callTraceFileName<<" file not found!");
	  currTraceElement = 0;
	  enableTrace = false;
	  
	  controlFlowError = "";
	}

	~checkerTool(){
	  //TODO free memory
	}


	///Method called at every instruction issue, it returns true in case the instruction
	///has to be skipped, false otherwise
	bool newIssue(const issueWidth &curPC, const InstructionBase *curInstr) throw(){
  	if(this->currFunc != this->bfdFE->functionAt(curPC) && this->bfdFE->isRoutineEntry(curPC)){ //function entry
	    this->currFunc = this->bfdFE->functionAt(curPC);
	    
	    //trace check
	    if(this->currFunc == "main") 
	      enableTrace = true;
	    if(enableTrace){
	      std::string currElem = MAKE_STRING("enter:"<<this->currFunc);
	      if(calltrace[currTraceElement] != currElem){
	          controlFlowError = MAKE_STRING("Line " << currTraceElement << " Expected: " 
	              << calltrace[currTraceElement] << " Obtained: " << currElem);
	          sc_stop();
	      }
	      currTraceElement++;
	    }
	    
	    //it is necessary to save parameters of the function call to be able to check pointed data on the return
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
	        this->functionsToLog[this->currFunc][i].value = currValue;
	      }		      
	    }
	    
  	}else if(this->currFunc != this->bfdFE->functionAt(curPC)){ //function re-enter if the address belongs to another function
  	    this->currFunc = this->bfdFE->functionAt(curPC);
  	      	    
  	    //trace check
  	    if(enableTrace){
	        std::string currElem = MAKE_STRING("re-enter:"<<this->currFunc);
	        if(calltrace[currTraceElement] != currElem){
	          controlFlowError = MAKE_STRING("Line " << currTraceElement << " Expected: " << calltrace[currTraceElement] << " Obtained: " << currElem);
	          sc_stop();
	        }
	        currTraceElement++;
	      }
	      
  	}else if(processorInstance.isRoutineExit(curInstr)){ //routine exit
  	    this->currFunc = this->bfdFE->functionAt(curPC);
  	    //check produced data
  	    std::vector<param_t> currPars = this->functionsToLog[this->currFunc];
  	    for(int i=0; i < currPars.size(); i++){
  	      if(currPars[i].type==ADDRESS_RETURN){
  	        std::string checkpointName = MAKE_STRING(this->currFunc << "_" << currPars[i].id << "_" 
  	                    << currPars[i].num_saved_files << "_exit.dat");
  	        int currErrors=0;
            if(checkpoints.count(checkpointName)==0){
              std::cout << "checkpoint not found " << checkpointName << std::endl;
            }else{
    	        std::string diffFileName = MAKE_STRING(this->dataFolderName << "/" <<this->currFunc << "_" << currPars[i].id << "_" 
  	                    << currPars[i].num_saved_files << "_exit_diff.dat");
  	          std::ofstream diffFile(diffFileName.c_str());
  	          for(int j = 0; j < currPars[i].size; j++){
    	          if((unsigned int)processorInstance.readCharMem(currPars[i].value + j) != (unsigned int)checkpoints[checkpointName].data[j]){
    	            currErrors++;
    	            diffFile << "1" << std::endl;
    	          }
    	          else
    	           diffFile << "0" << std::endl;
    	        }
    	        errors[checkpointName] = currErrors;
    	        diffFile.close();
            }
  	      }
  	    }
  	    
  	    //trace check
  	    if(enableTrace) {
	        std::string currElem = MAKE_STRING("exit:"<<this->currFunc);
	        if(calltrace[currTraceElement] != currElem){
	          controlFlowError = MAKE_STRING("Line " << currTraceElement << " Expected: " << calltrace[currTraceElement] << " Obtained: " << currElem);
	          sc_stop();
	        }
	        currTraceElement++;
	      }
	      
        //disable trace
        if(this->currFunc == "main") 
          enableTrace = false;  

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
		this->currFunc="";
		this->bfdFE = &BFDWrapper::getInstance(this->execName);
	}
	
	std::string getLog(){
    std::string log = "";
    
    log = MAKE_STRING("Controlflow error:\n- " << (this->controlFlowError==""?"No controlflow error":this->controlFlowError) << std::endl << std::endl);
    
	  log = MAKE_STRING(log << "# Data errors per checkpoints:" << std::endl);
	  for(std::map<std::string, int>::iterator it = errors.begin(); it != errors.end(); it++){
	      log = log + "- " + it->first + " :";
	      for(int i = 0; i < 35 - it->first.size(); i++)
	        log = log + " ";
	      log = MAKE_STRING(log << it->second << std::endl);
	  }
    return log;
	}

};

#endif // CHEKERTOOL_HPP
