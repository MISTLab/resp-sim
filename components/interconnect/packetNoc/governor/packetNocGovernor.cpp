#include "packetNocGovernor.hpp"
#include <cmath>

packetNocGovernor::packetNocGovernor(sc_module_name module_name, packetNoc& pnObj, sc_time cycleLatency):
		sc_module(module_name), intClock("clock", cycleLatency), clock(intClock), enabled(false) {
	noc = &pnObj;
	SC_HAS_PROCESS(packetNocGovernor);
	SC_METHOD(activity);
	sensitive << clock.pos();
	
	end_module();
}

packetNocGovernor::~packetNocGovernor() {
  for(std::map<unsigned int, switch_data_t*>::iterator mapIt = this->noc_data.begin(); mapIt != this->noc_data.end(); mapIt++){
    switch_data_t* tmp = mapIt->second;
    mapIt->second = NULL;
    delete tmp;
  }
  this->noc_data.clear();
}

void packetNocGovernor::enableGovernor(bool enabled) {
  this->enabled = enabled;
}

bool packetNocGovernor::isEnabled() {
  return enabled;
}

void packetNocGovernor::initialize(unsigned int bufferSize, unsigned int init_threshold, unsigned int max_threshold, unsigned int min_threshold){
  for(std::map<unsigned int, switch_data_t*>::iterator mapIt = this->noc_data.begin(); mapIt != this->noc_data.end(); mapIt++){
    switch_data_t* tmp = mapIt->second;
    mapIt->second = NULL;
    delete tmp;
  }
  this->noc_data.clear();

  this->init_threshold = init_threshold;
  this->max_threshold = max_threshold;
  this->min_threshold = min_threshold;
  if (min_threshold >= max_threshold)
    THROW_EXCEPTION("max threshold must be greater than min threshold");

  std::vector<unsigned int> ids = noc->getSwitchIds();
  for(std::vector<unsigned int>::iterator it = ids.begin(); it!= ids.end(); it++){
    this->noc_data[*it] = new switch_data_t;
    std::vector<unsigned int> activePorts = noc->getActivePorts(*it);
    this->noc_data[*it]->activeBuffers = activePorts.size();    
    for(int i = 0; i < this->noc_data[*it]->activeBuffers; i++){
      this->noc_data[*it]->currDropped[i] = 0;
      this->noc_data[*it]->threshold[i] = this->init_threshold;
      noc->changeBufferSize(*it, i, bufferSize);
    }    
  } 
}

void packetNocGovernor::activity() {
	if (enabled){
//   std::cout << "TIME: " << sc_time_stamp().to_default_time_units() << std::endl;
	  for(std::map<unsigned int, switch_data_t*>::iterator mapIt = this->noc_data.begin(); mapIt != this->noc_data.end(); mapIt++){
//  	  std::cout << " switch " << mapIt->first << " -> ";
	    switch_data_t* currSw = mapIt->second;
	    std::vector<unsigned int> toIncrease;
	    std::vector<unsigned int> toDecrease;
	    std::vector<unsigned int> canBeDecreased;
	    bool noChange = true;
	    for(int i = 0; i < currSw->activeBuffers; i++){
	      unsigned int currDropped = noc->getDropped(mapIt->first,i);
	      if(currDropped > currSw->currDropped[i]){
	        unsigned int delta = currDropped - currSw->currDropped[i];
	        currSw->currDropped[i] = currDropped;
	        currSw->threshold[i] = currSw->threshold[i] * pow(2,delta);
//	        std::cout << " change th " << i << " " << currSw->threshold[i] << " ";
	        if(currSw->threshold[i] >= this->max_threshold || noc->getBufferSize(mapIt->first,i) == 0){
	          toIncrease.push_back(i);	        
	        }
	        noChange = false;
	      }
	      else{
	        if(noc->getBufferCurrentFreeSlots(mapIt->first,i) > 0){ //decrease if there is any free slot otherwise it would generate a drop
	          currSw->threshold[i]--;
	          if(currSw->threshold[i] <= this->min_threshold){
	            currSw->threshold[i] = this->min_threshold; //useless with <= condition!
	            if (noc->getBufferSize(mapIt->first,i) > 0)
	              toDecrease.push_back(i);
	          } else{
	            if (noc->getBufferSize(mapIt->first,i) > 0)
	              canBeDecreased.push_back(i);
	          }
	        }
//	        std::cout << " change th " << i << " " << currSw->threshold[i] << " ";
	      }
	    }
//	    std::cout << std::endl;
      
      //order according to priority the toIncrease and canBeDecreased vectors 
      //(it is useless to order the other one since all thresholds are equal)
      int vetLength = (int) toIncrease.size();
      for(int k=0; k < vetLength; k++)
        for(int j=k; j < vetLength; j++)
          if((currSw->threshold[toIncrease.at(k)]) < (currSw->threshold[toIncrease.at(j)])){
            unsigned int temp = toIncrease.at(j);
            toIncrease[j] = toIncrease.at(k);
            toIncrease[k] = temp;
          }
          
      vetLength = (int) canBeDecreased.size();
      for(int k=0; k < vetLength; k++)
        for(int j=k; j < vetLength; j++)
          if((currSw->threshold[canBeDecreased.at(k)]) > (currSw->threshold[canBeDecreased.at(j)])){
            unsigned int temp = canBeDecreased.at(j);
            canBeDecreased[j] = canBeDecreased.at(k);
            canBeDecreased[k] = temp;
          }

	    //TODO improvement: if nothing is changed scale thresholds. usefull when init_threshold >> min_threshold

	    bool changed = true;
	    std::set<unsigned int> justchanged;
	    for(std::vector<unsigned int>::iterator vetIt = toIncrease.begin(); vetIt!=toIncrease.end() && changed;vetIt++){
	      changed = false;
	      if(toDecrease.begin() != toDecrease.end()){
	        unsigned int currDecreased = *(toDecrease.begin());
	        unsigned int currBufferSize1 = noc->getBufferSize(mapIt->first, *vetIt);
	        noc->changeBufferSize(mapIt->first, *vetIt, currBufferSize1 + 1);
	        unsigned int currBufferSize2 = noc->getBufferSize(mapIt->first, currDecreased);
	        noc->changeBufferSize(mapIt->first, currDecreased, currBufferSize2 -1);
	        currSw->threshold[*vetIt] = this->init_threshold;
	        currSw->threshold[currDecreased] = this->init_threshold;
	        toDecrease.erase(toDecrease.begin());
	        justchanged.insert(*vetIt);
	        justchanged.insert(currDecreased);	        
	        changed = true;
//	        std::cout << " switch" << mapIt->first << ": increase " << (*vetIt) << "(" << currBufferSize1 << "->" << (currBufferSize1+1) 
//	                  << ") decrease " <<  currDecreased << "(" << currBufferSize2 << "->" << (currBufferSize2-1) << ")" << std::endl;
	      } else if(canBeDecreased.begin() != canBeDecreased.end()){
	        unsigned int currDecreased = *(canBeDecreased.begin());
	        unsigned int currBufferSize1 = noc->getBufferSize(mapIt->first, *vetIt);
	        noc->changeBufferSize(mapIt->first, *vetIt, currBufferSize1 + 1);
	        unsigned int currBufferSize2 = noc->getBufferSize(mapIt->first, currDecreased);
	        noc->changeBufferSize(mapIt->first, currDecreased, currBufferSize2 - 1);
	        currSw->threshold[*vetIt] = this->init_threshold;
	        currSw->threshold[currDecreased] = this->init_threshold;
	        canBeDecreased.erase(canBeDecreased.begin());
	        justchanged.insert(*vetIt);
	        justchanged.insert(currDecreased);	        
	        changed = true;
//	        std::cout << " switch" << mapIt->first << ": increase " << (*vetIt) << "(" << currBufferSize1 << "->" << (currBufferSize1+1) 
//	                  << ") decrease " <<  currDecreased << "(" << currBufferSize2 << "->" << (currBufferSize2-1) << ")" << std::endl;
	      }
	    }
	    	    
	    changed = true;
	    for(std::vector<unsigned int>::iterator vetIt = toDecrease.begin(); vetIt!=toDecrease.end() && changed;vetIt++){
	      changed = false;
	      bool found = false;
	      unsigned int selected;
	      for(int j = 0; j < currSw->activeBuffers && !found; j++){ //select a buffer with the minimum size
          if(noc->getBufferSize(mapIt->first, j) == 0 && justchanged.find(j)==justchanged.end()){
            selected = j;
            found = true;
          }
	      }
	      if(found){
	        unsigned int currBufferSize1 = noc->getBufferSize(mapIt->first, *vetIt);
	        noc->changeBufferSize(mapIt->first, *vetIt, currBufferSize1 - 1);
	        unsigned int currBufferSize2 = noc->getBufferSize(mapIt->first, selected);
	        noc->changeBufferSize(mapIt->first, selected, currBufferSize2 + 1);	        
	        currSw->threshold[*vetIt] = this->init_threshold;
	        currSw->threshold[selected] = this->init_threshold;
	        justchanged.insert(selected);
	        justchanged.insert(*vetIt);
	        changed = true;
	      } 
	    }
	  }
	}
}

void packetNocGovernor::printStatus(){
  for(std::map<unsigned int, switch_data_t*>::iterator mapIt = this->noc_data.begin(); mapIt != this->noc_data.end(); mapIt++){
    switch_data_t* currSw = mapIt->second;
    std::cout << " switch " << mapIt->first << " -> ";
    for(int i = 0; i < currSw->activeBuffers; i++){
      std::cout << "threshold " << i << " " << currSw->threshold[i] << " ";
    }
    std::cout << std::endl;
  }
}



