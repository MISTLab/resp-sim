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
	    std::set<unsigned int> toIncrease;
	    std::set<unsigned int> toDecrease;
	    std::set<unsigned int> canBeDecreased;
	    for(int i = 0; i < currSw->activeBuffers; i++){
	      unsigned int currDropped = noc->getDropped(mapIt->first,i);
	      if(currDropped > currSw->currDropped[i]){
	        unsigned int delta = currDropped - currSw->currDropped[i];
	        currSw->currDropped[i] = currDropped;
	        currSw->threshold[i] = currSw->threshold[i] * pow(2,delta);
//	        std::cout << " change th " << i << " " << currSw->threshold[i] << " ";
	        if(currSw->threshold[i] >= this->max_threshold || noc->getBufferSize(mapIt->first,i) == 0)
	          toIncrease.insert(i);
	      }
	      else{
	        currSw->threshold[i]--;
	        if(currSw->threshold[i] < this->min_threshold){
	          currSw->threshold[i] = this->init_threshold;
	          if (noc->getBufferCurrentFreeSlots(mapIt->first,i) > 0 && noc->getBufferSize(mapIt->first,i) > 0)
	            toDecrease.insert(i);
	        } else{
	          if (noc->getBufferCurrentFreeSlots(mapIt->first,i) > 0 && noc->getBufferSize(mapIt->first,i) > 0)
	            canBeDecreased.insert(i);
	        }
//	        std::cout << " change th " << i << " " << currSw->threshold[i] << " ";
	      }
	    }
//	    std::cout << std::endl;
	    bool changed = true;
	    std::set<unsigned int> justchanged;
	    //TODO toIncrease, canBeDecreased and toDecrease should be ordered according to threshold value
	    for(std::set<unsigned int>::iterator setIt = toIncrease.begin(); setIt!=toIncrease.end() && changed;setIt++){
	      changed = false;
	      if(toDecrease.begin() != toDecrease.end()){
	        unsigned int currDecreased = *(toDecrease.begin());
	        unsigned int currBufferSize1 = noc->getBufferSize(mapIt->first, *setIt);
	        noc->changeBufferSize(mapIt->first, *setIt, currBufferSize1 + 1);
	        unsigned int currBufferSize2 = noc->getBufferSize(mapIt->first, currDecreased);
	        noc->changeBufferSize(mapIt->first, currDecreased, currBufferSize2 -1);
	        currSw->threshold[*setIt] = this->init_threshold;
	        currSw->threshold[currDecreased] = this->init_threshold;
	        toDecrease.erase(toDecrease.begin());
	        justchanged.insert(*setIt);
	        justchanged.insert(currDecreased);	        
	        changed = true;
//	        std::cout << " switch" << mapIt->first << ": increase " << (*setIt) << "(" << currBufferSize1 << "->" << (currBufferSize1+1) 
//	                  << ") decrease " <<  currDecreased << "(" << currBufferSize2 << "->" << (currBufferSize2-1) << ")" << std::endl;
	      } else if(canBeDecreased.begin() != canBeDecreased.end()){
	        unsigned int currDecreased = *(canBeDecreased.begin());
	        unsigned int currBufferSize1 = noc->getBufferSize(mapIt->first, *setIt);
	        noc->changeBufferSize(mapIt->first, *setIt, currBufferSize1 + 1);
	        unsigned int currBufferSize2 = noc->getBufferSize(mapIt->first, currDecreased);
	        noc->changeBufferSize(mapIt->first, currDecreased, currBufferSize2 - 1);
	        currSw->threshold[*setIt] = this->init_threshold;
	        currSw->threshold[currDecreased] = this->init_threshold;
	        canBeDecreased.erase(canBeDecreased.begin());
	        justchanged.insert(*setIt);
	        justchanged.insert(currDecreased);	        
	        changed = true;
//	        std::cout << " switch" << mapIt->first << ": increase " << (*setIt) << "(" << currBufferSize1 << "->" << (currBufferSize1+1) 
//	                  << ") decrease " <<  currDecreased << "(" << currBufferSize2 << "->" << (currBufferSize2-1) << ")" << std::endl;
	      }
	    }
	    	    
	    changed = true;
	    //TODO toIncrease, canBeDecreased and toDecrease should be ordered according to threshold value
	    for(std::set<unsigned int>::iterator setIt = toDecrease.begin(); setIt!=toDecrease.end() && changed;setIt++){
	      changed = false;
	      bool found = false;
	      unsigned int selected, selectedTH;
	      selectedTH = this->max_threshold;
	      for(int j = 0; j < currSw->activeBuffers && !found; j++){
	        if(noc->getBufferSize(mapIt->first, j) == 0 && justchanged.find(j)==justchanged.end() && selectedTH > currSw->threshold[j]){
	          selectedTH = currSw->threshold[j];
	          selected = j;
	          found = true;
	        }
	      }
	      if(found){
	        unsigned int currBufferSize1 = noc->getBufferSize(mapIt->first, *setIt);
	        noc->changeBufferSize(mapIt->first, *setIt, currBufferSize1 - 1);
	        unsigned int currBufferSize2 = noc->getBufferSize(mapIt->first, selected);
	        noc->changeBufferSize(mapIt->first, selected, currBufferSize1 + 1);	        
	        currSw->threshold[*setIt] = this->init_threshold;
	        currSw->threshold[selected] = this->init_threshold;
	        justchanged.insert(selected);
	        justchanged.insert(*setIt);
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



