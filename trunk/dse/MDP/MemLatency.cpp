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
#include <algorithm>
#include <boost/bimap.hpp>
#include <boost/lexical_cast.hpp>

#include "MemLatency.hpp"
#include "pluginIf.hpp"

#include "utils.hpp"

///Creator variable: this variable is initialized when the dll
///is loaded; this action also automatically registers the plugin
///in the application
static MemLatencyCreator creator;

MemLatencyIf::MemLatencyIf(const std::vector<std::string> &values, std::string pluginName) : IntegerPlugin(pluginName){
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
std::pair<float, float> MemLatencyIf::changeValue(std::map<PluginIf*, int> &parameters, int newValue, const std::string &metric,
                                    const std::map<std::string, float> &centroidMap, const std::map<std::string, float> &statistics,
                                                                            const std::map<PluginIf *, std::string> &parameter_values){
    //I get the difference in latency (oldValue - newValue) I compute the new execution time as:
    //oldValue - newValue > 0 : T_min = T_cur - num_accesses*(oldValue - newValue) ## T_max = T_cur
    //oldValue - newValue < 0 : T_min = T_cur ## T_max = T_cur  + num_accesses*(newValue - oldValue)
    //Regarding power ...
    int oldValue = parameters[this];
    std::pair<float, float> retVal;
    if(statistics.find("numMemAccesses") == statistics.end())
        THROW_EXCEPTION("Unable to find the statistic numMemAccesses");
    double numAccesses = statistics.find("numMemAccesses")->second;
    double numBusAccesses = statistics.find("numBusAccesses")->second;

    double bus_latency = 10;
    std::map<std::string, PluginIf* >::const_iterator foundPlugin;
    foundPlugin = plugin_handler.find("bus_latencyNS");
    if(foundPlugin != plugin_handler.end()){
        if( parameter_values.find(foundPlugin->second) != parameter_values.end())
            bus_latency = boost::lexical_cast<double>(parameter_values.find(foundPlugin->second)->second);
    }

    if(centroidMap.find("execTime") == centroidMap.end())
        THROW_EXCEPTION("Unable to find the metric execTime");
    if(centroidMap.find("energy") == centroidMap.end())
        THROW_EXCEPTION("Unable to find the metric energy");

    unsigned int oldLatency = this->getMemoryLatency(oldValue);
    unsigned int newLatency = this->getMemoryLatency(newValue);

    double ratio = (double) newLatency/ (double) oldLatency;

    #ifndef NDEBUG
    if(oldLatency == newLatency)
        THROW_EXCEPTION("After action the values for the old and new memory latencies are still the same");
    #endif
    if(metric == "execTime"){
        double curMetricCentroid = centroidMap.find("execTime")->second;
        if(oldLatency > newLatency){
            // Decrease the latency:
            // Lower bound, the performance increases by (oldLatency - NewLatency)*numAccesses
            // Upper bound, the performance is NOT changed (memory was too fast anyway)
            retVal.first = curMetricCentroid*ratio;
            retVal.second = curMetricCentroid;
        }
        else{
            // Increase the latency:
            // Lower bound, the performance is NOT changed (memory was too fast anyway)
            // Upper bound, the performance decreases by (NewLatency - oldLatency)*numAccesses
            retVal.first = curMetricCentroid;
            retVal.second = curMetricCentroid*ratio;
        }


    }
    else if(metric == "energy"){
        double curMetricCentroid = centroidMap.find("energy")->second;
        double execTime = centroidMap.find("execTime")->second;
        if(oldLatency > newLatency){
            // Decrease the latency:
            // Lower bound, the performance increases by (oldLatency - NewLatency)*numAccesses. Energy is reduced in proportion (standby, leakage...).
            // Upper bound, the performance is NOT changed (memory was too fast anyway). Energy is unaffected since cost is per access.
            retVal.first = curMetricCentroid*ratio;
            retVal.second = curMetricCentroid;
        }
        else{
            // Increase the latency:
            // Lower bound, the performance is NOT changed (memory was too fast anyway). Energy is unaffected.
            // Upper bound, the performance decreases by (NewLatency - oldLatency)*numAccesses. Energy is increased in proportion (standby, leakage...).
            retVal.first = curMetricCentroid;
            retVal.second = curMetricCentroid*ratio;
        }
    }
    else
        THROW_EXCEPTION(metric << " Unrecognized metric");

    if( retVal.second < 0 || retVal.first < 0 )
        THROW_EXCEPTION( this->pluginName+" has negative value ("+boost::lexical_cast<std::string>(retVal.first)+","+boost::lexical_cast<std::string>(retVal.second)+")");

    if( retVal.second < retVal.first )
        THROW_EXCEPTION( this->pluginName+" has inverse values ("+boost::lexical_cast<std::string>(retVal.first)+","+boost::lexical_cast<std::string>(retVal.second)+")");


    return retVal;
}

///It computes the new value of the statistics according to
///the specified value and returns it
void MemLatencyIf::updateStatistics(std::map<std::string, float> &curStats, int oldValue, int action, const std::map<PluginIf *, std::string> &parameter_values){
}

///Using the current instance of ReSPClient, it queries ReSP for the new
///values of the metrics (i.e. CPI, frequency etc. for a processor) and returns
///this value
void MemLatencyIf::getStats(RespClient &client, std::map<std::string, float> &toUpdateStats){
    //Lets read from the simulator the number of memory accesses performed
    client.get_probe_value("numMemAccesses", toUpdateStats["numMemAccesses"]);
}

///Given the enumeration representing the parameter value of this plugin (i.e. the memory
///latency) it returns the corresponding memory latency
unsigned int MemLatencyIf::getMemoryLatency(int latencyEnum){
    bm_type::left_map::const_iterator foundVal = this->param2Enum.left.find(latencyEnum);
    if(foundVal == this->param2Enum.left.end())
        THROW_EXCEPTION("Unable to find the parameter value corresponding to " << latencyEnum);
    return foundVal->second;
}

/************************** Methods of the plugin creator factory **********************/

MemLatencyCreator::MemLatencyCreator(){
    plugin_creator_handler.insert(std::pair<std::string, PluginCreator *>("memLatencyNS", this));
}

MemLatencyCreator::~MemLatencyCreator(){
    plugin_creator_handler.erase("memLatencyNS");
}

///Creates an instance of the plugin
PluginIf * MemLatencyCreator::create(const std::vector<std::string> &values, std::string pluginName){
    MemLatencyIf * pluginInstance = new MemLatencyIf(values, pluginName);
    this->instances.insert(pluginInstance);
    return pluginInstance;
}

///Destroys an instance of the plugin
void MemLatencyCreator::destroy(PluginIf * toDestroy){
    this->instances.erase(toDestroy);
    delete dynamic_cast<MemLatencyIf *>(toDestroy);
}
