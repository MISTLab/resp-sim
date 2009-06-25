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
 *   (c) Giovanni Beltrame, Luca Fossati
 *       Giovanni.Beltrame@esa.int fossati@elet.polimi.it
 *
\***************************************************************************/

#include <map>
#include <string>
#include <boost/bimap.hpp>
#include <boost/lexical_cast.hpp>
#include <set>
#include <vector>
#include "RespClient.hpp"

#include "ProcFreq.hpp"
#include "pluginIf.hpp"

#include "utils.hpp"

///Creator variable: this variable is initialized when the dll
///is loaded; this action also automatically registers the plugin
///in the application
static ProcFreqCreator creator;

ProcFreqIf::ProcFreqIf(const std::vector<std::string> &values, std::string pluginName) : IntegerPlugin(pluginName){
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
std::pair<float, float> ProcFreqIf::changeValue(std::map<PluginIf*, int> &parameters, int newValue, const std::string &metric,
                                const std::map<std::string, float> &centroidMap, const std::map<std::string, float> &statistics,
                                                                        const std::map<PluginIf *, std::string> &parameter_values){
    //I get the difference in frequency (oldValue - newValue) I compute the new execution time and power consumption
    int oldValue = parameters[this];
    std::pair<float, float> retVal;
    unsigned int oldFreq = this->getProcFrequency(oldValue);
    unsigned int newFreq = this->getProcFrequency(newValue);

    if(centroidMap.find("execTime") == centroidMap.end())
        THROW_EXCEPTION("Unable to find the metric execTime");
    if(centroidMap.find("energy") == centroidMap.end())
        THROW_EXCEPTION("Unable to find the metric energy");

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

    double ratio = (double) oldFreq/ (double) newFreq;

    #ifndef NDEBUG
    if(oldFreq == newFreq)
        THROW_EXCEPTION("After action the frequency of processors is still the same");
    #endif
    if(metric == "execTime"){
        double curMetricCentroid = centroidMap.find("execTime")->second;
        if(oldFreq > newFreq){
            //Frequency decrement: I'm going slower
            retVal.first = curMetricCentroid*0.9;
            retVal.second = curMetricCentroid*ratio*1.1;
        }
        else{
            retVal.first = curMetricCentroid*ratio*0.9;
            retVal.second = curMetricCentroid*1.1;
        }

    }
    else if(metric == "energy"){
        double curMetricCentroid = centroidMap.find("energy")->second;
        if(oldFreq > newFreq){
            //Frequency decrement: I'm going slower
            retVal.first = curMetricCentroid*(1.0/ratio)*0.9;
            retVal.second = curMetricCentroid*1.1; //*ratio;
        }
        else{
            retVal.first = curMetricCentroid*0.9; //*ratio;
            retVal.second = curMetricCentroid*(1.0/ratio)*1.1;
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

///It computes the new value of the metrics according to
///the specified value and returns it
void ProcFreqIf::updateStatistics(std::map<std::string, float> &curStats, int oldValue, int action, const std::map<PluginIf *, std::string> &parameter_values){
}

///Using the current instance of ReSPClient, it queries ReSP for the new
///values of the metrics (i.e. CPI, frequency etc. for a processor) and returns
///this value
void ProcFreqIf::getStats(RespClient &client, std::map<std::string, float> &toUpdateStats){
    //Lets read from the simulator the number of instructions executed
    client.get_probe_value("numBusAccesses", toUpdateStats["numBusAccesses"]);
    client.get_probe_value("numMemAccesses", toUpdateStats["numMemAccesses"]);
}

///Given the enumeration representing the parameter value of this plugin (i.e. the processor
///frequency) it returns the corresponding memory latency
unsigned int ProcFreqIf::getProcFrequency(int frequencyEnum){
    bm_type::left_map::const_iterator foundVal = this->param2Enum.left.find(frequencyEnum);
    if(foundVal == this->param2Enum.left.end())
        THROW_EXCEPTION("Unable to find the parameter value corresponding to " << frequencyEnum);
    return foundVal->second;
}


/************************** Methods of the plugin creator factory **********************/

ProcFreqCreator::ProcFreqCreator(){
    plugin_creator_handler.insert(std::pair<std::string, PluginCreator *>("procFreqMHz", this));
}

ProcFreqCreator::~ProcFreqCreator(){
    plugin_creator_handler.erase("procFreqMHz");
}

///Creates an instance of the plugin
PluginIf * ProcFreqCreator::create(const std::vector<std::string> &values, std::string pluginName){
    ProcFreqIf * pluginInstance = new ProcFreqIf(values, pluginName);
    this->instances.insert(pluginInstance);
    return pluginInstance;
}

///Destroys an instance of the plugin
void ProcFreqCreator::destroy(PluginIf * toDestroy){
    this->instances.erase(toDestroy);
    delete dynamic_cast<ProcFreqIf *>(toDestroy);
}
