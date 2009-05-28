/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *                                                                         *
 ***************************************************************************/

#include <map>
#include <string>
#include <boost/bimap.hpp>
#include <boost/lexical_cast.hpp>
#include <set>
#include <vector>
#include "RespClient.hpp"
#include "processor.hpp"

#include "ProcNum.hpp"
#include "pluginIf.hpp"

#include "utils.hpp"

#ifdef MEMORY_DEBUG
#include <mpatrol.h>
#endif

///Creator variable: this variable is initialized when the dll
///is loaded; this action also automatically registers the plugin
///in the application
static ProcNumCreator creator;

ProcNumIf::ProcNumIf(const std::vector<std::string> &values, std::string pluginName) : IntegerPlugin(pluginName) , p_model(){
    int numInserted = 0;
    //First of all I sort the numerical values
    std::vector<std::string> sortedValues = values;
    std::sort(sortedValues.begin(), sortedValues.end(), PluginIf::numericSorter);
    std::vector<std::string>::const_iterator valueIter, valueEnd;
    for(valueIter = sortedValues.begin(), valueEnd = sortedValues.end(); valueIter != valueEnd; valueIter++){
        this->param2Enum.insert(bm_type::value_type(numInserted, boost::lexical_cast<unsigned int>(*valueIter)));
        numInserted++;
    }
}

///Given a metric and the new parameter value, we obtain an
///estimation of the metric change from the old value to the new
///one
std::pair<float, float> ProcNumIf::changeValue(plugin_int_map &parameters, int newValue, const std::string &metric,
                                const float_map &centroidMap, const float_map &statistics,
                                                                        const std::map<PluginIf *, std::string> &parameter_values){
    //I get the difference in frequency (oldValue - newValue) I compute the new execution time and power consumption
    int oldValue = parameters[this];
    std::pair<float, float> retVal;

    if(centroidMap.find("execTime") == centroidMap.end())
        THROW_EXCEPTION("Unable to find the metric execTime");
    if(centroidMap.find("energy") == centroidMap.end())
        THROW_EXCEPTION("Unable to find the metric energy");

    unsigned int oldNum = this->getProcNum(oldValue);
    unsigned int newNum = this->getProcNum(newValue);

    double avgLoad = statistics.find("avgLoad")->second;
    if( avgLoad > 1 )
        THROW_EXCEPTION( "avgLoad greater than 1!");

    double numBusAccesses = statistics.find("numBusAccesses")->second;
    double numMemAccesses = statistics.find("numMemAccesses")->second;
    double bus_latency = 10, mem_latency = 10; // Standard value better ideas?
    std::map<std::string, PluginIf* >::const_iterator foundPlugin;
    foundPlugin = plugin_handler.find("bus_latencyNS");
    if(foundPlugin != plugin_handler.end()){
        if( parameter_values.find(foundPlugin->second) != parameter_values.end())
            bus_latency = boost::lexical_cast<double>(parameter_values.find(foundPlugin->second)->second);
    }
    foundPlugin = plugin_handler.find("memLatencyNS");
    if(foundPlugin != plugin_handler.end()){
        if ( parameter_values.find(foundPlugin->second) != parameter_values.end())
            mem_latency = boost::lexical_cast<double>(parameter_values.find(foundPlugin->second)->second);
    }

    double frequency = 250.0;
    foundPlugin = plugin_handler.find("procFreqMHz");
    if(foundPlugin != plugin_handler.end()){
        if ( parameter_values.find(foundPlugin->second) != parameter_values.end())
            frequency = boost::lexical_cast<double>(parameter_values.find(foundPlugin->second)->second);
    }

    double ratio = (double) oldNum/(double)newNum;

    #ifndef NDEBUG
    if(oldNum == newNum)
        THROW_EXCEPTION("After action the number of processors is still the same");
    #endif
    if(metric == "execTime"){
        double curMetricCentroid = centroidMap.find("execTime")->second;
        if(oldNum > newNum){
            // Decrease the number of processors:
            // Lower bound, the performance is NOT changed (one processor was useless)
            // Upper bound, the performance decreases by 1/x with x processors
            retVal.first = curMetricCentroid*0.9;
            retVal.second = curMetricCentroid*ratio*1.1;
        }
        else{
            // Increase the number of processors
            // Lower bound, the performance is increased by 1/x
            // Upper bound, the performance is unchanged (or decreased negligibly)
            retVal.first = curMetricCentroid*ratio*0.9;
            retVal.second = curMetricCentroid*1.1;
        }
//         cout << "T Worst: " << retVal.second << " Best: " << retVal.first << endl;

    }
    else if(metric == "energy"){
        double curMetricCentroid = centroidMap.find("energy")->second;
        //cout << "Current energy: " << curMetricCentroid << endl;

        // Determine the power of a processor at full and standby power
        double PEStandbyPower = 0.0, PEPower = 0.0;
        double execTime = centroidMap.find("execTime")->second;

        this->p_model.set_probe("idleTime", execTime*(1-avgLoad));
        this->p_model.set_probe("activeTime", execTime*avgLoad);
        this->p_model.set_probe("frequency", frequency);
        PEPower = this->p_model.get_energy();

        this->p_model.set_probe("activeTime", 0.0);
        this->p_model.set_probe("idleTime", execTime*ratio);
        PEStandbyPower = this->p_model.get_energy();

        if(oldNum > newNum){
            // Decrease the number of processors:
            // Lower Bound, E is reducd by N processors
            // Upper Bound, E is not reduced

            retVal.first = curMetricCentroid*((float) 1/ratio)*0.9;
            retVal.second = curMetricCentroid*1.1;
        }
        else {
            // Inccrease the number of processors:
            // Lower Bound, E is increased by a processor at standby/leakage power
            // Upper Bound, E is increased by a processor at full power

            retVal.first = curMetricCentroid*0.9;
            retVal.second = curMetricCentroid *((float) 1/ratio) + PEStandbyPower*(newNum-oldNum); //+ PEPower*(newNum-oldNum);
        }
//         cout << "Worst: " << retVal.second << " Best: " << retVal.first << " AVG: " << avgLoad << " RA: " << ratio << endl;
    }
    else
        THROW_EXCEPTION(metric << " Unrecognized metric");

    if( retVal.second < 0 || retVal.first < 0 )
        THROW_EXCEPTION( this->pluginName+" has negative value ("+boost::lexical_cast<std::string>(retVal.first)+","+boost::lexical_cast<std::string>(retVal.second)+")");

    if( retVal.second < retVal.first ) {
        std::cout << " ExecTime = " <<  centroidMap.find("execTime")->second << std::endl;
        std::cout << " Bus Accesses = " << numBusAccesses << " Latency = " << bus_latency << std::endl;
        std::cout << " Mem Accesses = " << numMemAccesses << " Latency = " << bus_latency << std::endl;
        std::cout << " OldProc = " << oldNum << " NewProc = " << newNum << std::endl;
        THROW_EXCEPTION( this->pluginName+" has inverse values ("+boost::lexical_cast<std::string>(retVal.first)+","+boost::lexical_cast<std::string>(retVal.second)+")");
    }

    return retVal;
}

///It computes the new value of the metrics according to
///the specified value and returns it
void ProcNumIf::updateStatistics(float_map &curStats, int oldValue, int action,const std::map<PluginIf *, std::string> &parameter_values){
    if( action == 1 ) {
        // Decrement
        unsigned int oldNum = this->getProcNum(oldValue);
        unsigned int newNum = this->getProcNum(this->applyAction(oldValue, action));
        curStats["avgLoad"] /= ((double) oldNum/(double) newNum)/(double) 2;
        curStats["avgLoad"] = (curStats["avgLoad"] > 1)?1:curStats["avgLoad"];
    }
}

///Using the current instance of ReSPClient, it queries ReSP for the new
///values of the metrics (i.e. CPI, frequency etc. for a processor) and returns
///this value
void ProcNumIf::getStats(RespClient &client, float_map &toUpdateStats){
    //Lets read from the simulator the number of instructions executed
    client.get_probe_value("avgLoad", toUpdateStats["avgLoad"]);
    client.get_probe_value("numBusAccesses", toUpdateStats["numBusAccesses"]);
    client.get_probe_value("numMemAccesses", toUpdateStats["numMemAccesses"]);
    if( toUpdateStats["avgLoad"] > 1 )
        THROW_EXCEPTION( "avgLoad greater than 1!");
}

///Given the enumeration representing the parameter value of this plugin (i.e. the processor
///frequency) it returns the corresponding memory latency
unsigned int ProcNumIf::getProcNum(int numberEnum){
    bm_type::left_map::const_iterator foundVal = this->param2Enum.left.find(numberEnum);
    if(foundVal == this->param2Enum.left.end())
        THROW_EXCEPTION("Unable to find the parameter value corresponding to " << numberEnum);
    return foundVal->second;
}

/************************** Methods of the plugin creator factory **********************/

ProcNumCreator::ProcNumCreator(){
    plugin_creator_handler.insert(std::pair<std::string, PluginCreator *>("procNum", this));
}

ProcNumCreator::~ProcNumCreator(){
    plugin_creator_handler.erase("procNum");
}

///Creates an instance of the plugin
PluginIf * ProcNumCreator::create(const std::vector<std::string> &values, std::string pluginName){
    ProcNumIf * pluginInstance = new ProcNumIf(values, pluginName);
    this->instances.insert(pluginInstance);
    return pluginInstance;
}

///Destroys an instance of the plugin
void ProcNumCreator::destroy(PluginIf * toDestroy){
    this->instances.erase(toDestroy);
    delete dynamic_cast<ProcNumIf *>(toDestroy);
}
