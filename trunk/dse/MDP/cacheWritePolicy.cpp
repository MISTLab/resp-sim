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

#include "cacheWritePolicy.hpp"
#include "pluginIf.hpp"

#include "utils.hpp"

///Creator variable: this variable is initialized when the dll
///is loaded; this action also automatically registers the plugin
///in the application
static CacheWritePolicyCreator creator;

CacheWritePolicyIf::CacheWritePolicyIf(const std::vector<std::string> &values, std::string pluginName) : StringPlugin(pluginName), model(){
    int numInserted = 0;
    std::vector<std::string>::const_iterator valueIter, valueEnd;
    for(valueIter = values.begin(), valueEnd = values.end(); valueIter != valueEnd; valueIter++){
        this->param2Enum.insert(bm_type::value_type(numInserted, *valueIter));
        numInserted++;
    }
}

///Given a metric and the new parameter value, we obtain an
///estimation of the metric change from the old value to the new
///one
std::pair<float, float> CacheWritePolicyIf::changeValue(std::map<PluginIf*, int> &parameters,
            int newValue, const std::string &metric, const std::map<std::string, float> &centroidMap,
                const std::map<std::string, float> &statistics, const std::map<PluginIf *, std::string> &parameter_values){
    // Note that actually I do not consider the write policy of the instruction cache, so no
    // action can be performed if we are in the instruction cache
    std::pair<float, float> retVal;
    if(this->pluginName[0] == 'i'){
        if(metric == "execTime"){
            retVal.first = centroidMap.find("execTime")->second;
            retVal.second = centroidMap.find("execTime")->second;
        }
        else{
            retVal.first = centroidMap.find("energy")->second;
            retVal.second = centroidMap.find("energy")->second;
        }
        return retVal;
    }

    int oldValue = parameters[this];

    if(centroidMap.find("execTime") == centroidMap.end())
        THROW_EXCEPTION("Unable to find the metric execTime");
    if(centroidMap.find("energy") == centroidMap.end())
        THROW_EXCEPTION("Unable to find the metric energy");

    // Now I have, using the difference between the old and new misses,
    // to compute the new value for the metrics.

    unsigned int oldReadHitNum = (unsigned int)statistics.find("dreadHitNum")->second;
    unsigned int oldWriteHitNum = (unsigned int)statistics.find("dwriteHitNum")->second;
    unsigned int oldReadMissNum = (unsigned int)statistics.find("dreadMissNum")->second;
    unsigned int oldWriteMissNum = (unsigned int)statistics.find("dwriteMissNum")->second;

    std::map<std::string, PluginIf* >::const_iterator foundPlugin;

    std::string newValueString = this->getParameterName(newValue);
    std::string newCacheWritePolicy;
    if(newValueString.find("THROUGH") != std::string::npos)
        newCacheWritePolicy = "TH";
    else if(newValueString.find("BACK") != std::string::npos)
        newCacheWritePolicy = "B";
    else if(newValueString.find("THROUGH_ALL") != std::string::npos)
        newCacheWritePolicy = "THA";
    std::string curAllPolicy = "LRU";
    foundPlugin = plugin_handler.find("dcacheSubstsPolicy");
    if(foundPlugin != plugin_handler.end()){
        if(parameter_values.find(foundPlugin->second) != parameter_values.end()){
            curAllPolicy = parameter_values.find(foundPlugin->second)->second;
            if(curAllPolicy.find("LRU") != std::string::npos)
                curAllPolicy = "LRU";
            else if(curAllPolicy.find("LRR") != std::string::npos)
                curAllPolicy = "LRR";
            else if(curAllPolicy.find("RANDOM") != std::string::npos)
                curAllPolicy = "RAND";
        }
    }
    unsigned int newReadHitNum = (unsigned int)statistics.find("readHitNum_dcache" + curAllPolicy + "_" + newCacheWritePolicy)->second;
    unsigned int newWriteHitNum = (unsigned int)statistics.find("writeHitNum_dcache" + curAllPolicy + "_" + newCacheWritePolicy)->second;
    unsigned int newReadMissNum = (unsigned int)statistics.find("readMissNum_dcache" + curAllPolicy + "_" + newCacheWritePolicy)->second;
    unsigned int newWriteMissNum = (unsigned int)statistics.find("writeMissNum_dcache" + curAllPolicy + "_" + newCacheWritePolicy)->second;

    double cacheSize = 4*1024;
    foundPlugin = plugin_handler.find("d_cache_size");
    if(foundPlugin != plugin_handler.end()){
        if(parameter_values.find(foundPlugin->second) != parameter_values.end()){
            cacheSize = boost::lexical_cast<double>(parameter_values.find(foundPlugin->second)->second);
        }
    }
    double execTime = centroidMap.find("execTime")->second;
    double bus_latency = 10, mem_latency = 10; // Standard value better ideas?
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
    // Now I have to compute the execution time after the parameter change. Note that
    // I acutally have the new number of misses and hits with the new policy
    double newExecTime = execTime + ((newReadMissNum+newWriteMissNum) - (oldReadMissNum+oldWriteMissNum) + (oldReadHitNum+oldWriteHitNum) - (newReadHitNum+newWriteHitNum))*(bus_latency + mem_latency);

     if(metric == "execTime"){
        retVal.first = newExecTime*0.9;
        retVal.second = newExecTime*1.1;
    }
    else if(metric == "energy"){
        double curMetricCentroid = centroidMap.find("energy")->second;

        // Get new energy values from ReSP
        this->model.set_parameter("size", cacheSize);
        this->model.set_probe("execution_time", execTime);
        this->model.set_probe("readHitNum", oldReadHitNum);
        this->model.set_probe("writeHitNum", oldWriteHitNum);
        this->model.set_probe("readMissNum", oldReadMissNum);
        this->model.set_probe("writeMissNum", oldWriteMissNum);
        this->model.update_parameters();
        curMetricCentroid -= this->model.get_energy();

        this->model.set_probe("readHitNum", newReadHitNum);
        this->model.set_probe("writeHitNum", newWriteHitNum);
        this->model.set_probe("readMissNum", newReadMissNum);
        this->model.set_probe("writeMissNum", newWriteMissNum);
        double newEnergy = this->model.get_energy();

        retVal.first = curMetricCentroid + newEnergy*0.9;
        retVal.second = curMetricCentroid + newEnergy*1.1;
    }
    else
        THROW_EXCEPTION(metric << " Unrecognized metric");
    return retVal;
}

///It computes the new value of the metrics according to
///the specified value and returns it
void CacheWritePolicyIf::updateStatistics(std::map<std::string, float> &curStats, int oldValue, int action,
                                                        const std::map<PluginIf *, std::string> &parameter_values){
    //I have to update the generic statistics of the cache with the numbers
    //from the actual statistic
    if(this->pluginName[0] == 'i'){
        return;
    }
    std::string newValueString = this->getParameterName(this->applyAction(oldValue, action));
    std::string newCacheWritePolicy;
    if(newValueString.find("THROUGH") != std::string::npos)
        newCacheWritePolicy = "TH";
    else if(newValueString.find("BACK") != std::string::npos)
        newCacheWritePolicy = "B";
    else if(newValueString.find("THROUGH_ALL") != std::string::npos)
        newCacheWritePolicy = "THA";
    std::string curAllPolicy = "LRU";
    std::map<std::string, PluginIf* >::const_iterator foundPlugin;
    foundPlugin = plugin_handler.find("dcacheSubstsPolicy");
    if(foundPlugin != plugin_handler.end()){
        if(parameter_values.find(foundPlugin->second) != parameter_values.end()){
            curAllPolicy = parameter_values.find(foundPlugin->second)->second;
            if(curAllPolicy.find("LRU") != std::string::npos)
                curAllPolicy = "LRU";
            else if(curAllPolicy.find("LRR") != std::string::npos)
                curAllPolicy = "LRR";
            else if(curAllPolicy.find("RANDOM") != std::string::npos)
                curAllPolicy = "RAND";
        }
    }
    curStats["dreadHitNum"] = curStats["readMissNum_dcache" + curAllPolicy + "_" + newCacheWritePolicy];
    curStats["dreadMissNum"] = curStats["readHitNum_dcache" + curAllPolicy + "_" + newCacheWritePolicy];
    curStats["dwriteHitNum"] = curStats["writeHitNum_dcache" + curAllPolicy + "_" + newCacheWritePolicy];
    curStats["dwriteMissNum"] = curStats["writeMissNum_dcache" + curAllPolicy + "_" + newCacheWritePolicy];
}

///Using the current instance of ReSPClient, it queries ReSP for the new
///values of the metrics (i.e. CPI, frequency etc. for a processor) and returns
///this value
void CacheWritePolicyIf::getStats(RespClient &client, std::map<std::string, float> &toUpdateStats){
    //Here I get the stats for all the policies, the current one and all the
    //others
    std::string curCache = this->pluginName.substr(0, 1);
    std::string cacheStatus;
    client.get_probe_value(curCache + "CacheStatus", cacheStatus);
    if(cacheStatus.find("DISABLED") != std::string::npos)
        THROW_EXCEPTION("Error, the " << curCache << " cache is not enabled");

    client.get_probe_value(curCache + "readHitNum", toUpdateStats[curCache + "readHitNum"]);
    client.get_probe_value(curCache + "readMissNum", toUpdateStats[curCache + "readMissNum"]);
    client.get_probe_value(curCache + "writeHitNum", toUpdateStats[curCache + "writeHitNum"]);
    client.get_probe_value(curCache + "writeMissNum", toUpdateStats[curCache + "writeMissNum"]);

    if(curCache == "d"){
        client.get_probe_value("readMissNum_dcacheLRU_TH", toUpdateStats["readMissNum_dcacheLRU_TH"]);
        client.get_probe_value("readMissNum_dcacheLRU_THA", toUpdateStats["readMissNum_dcacheLRU_THA"]);
        client.get_probe_value("readMissNum_dcacheLRU_B", toUpdateStats["readMissNum_dcacheLRU_B"]);
        client.get_probe_value("readMissNum_dcacheLRR_TH", toUpdateStats["readMissNum_dcacheLRR_TH"]);
        client.get_probe_value("readMissNum_dcacheLRR_THA", toUpdateStats["readMissNum_dcacheLRR_THA"]);
        client.get_probe_value("readMissNum_dcacheLRR_B", toUpdateStats["readMissNum_dcacheLRR_B"]);
        client.get_probe_value("readMissNum_dcacheRAND_TH", toUpdateStats["readMissNum_dcacheRAND_TH"]);
        client.get_probe_value("readMissNum_dcacheRAND_THA", toUpdateStats["readMissNum_dcacheRAND_THA"]);
        client.get_probe_value("readMissNum_dcacheRAND_B", toUpdateStats["readMissNum_dcacheRAND_B"]);
        client.get_probe_value("writeHitNum_dcacheLRU_TH", toUpdateStats["writeHitNum_dcacheLRU_TH"]);
        client.get_probe_value("writeHitNum_dcacheLRU_THA", toUpdateStats["writeHitNum_dcacheLRU_THA"]);
        client.get_probe_value("writeHitNum_dcacheLRU_B", toUpdateStats["writeHitNum_dcacheLRU_B"]);
        client.get_probe_value("writeHitNum_dcacheLRR_TH", toUpdateStats["writeHitNum_dcacheLRR_TH"]);
        client.get_probe_value("writeHitNum_dcacheLRR_THA", toUpdateStats["writeHitNum_dcacheLRR_THA"]);
        client.get_probe_value("writeHitNum_dcacheLRR_B", toUpdateStats["writeHitNum_dcacheLRR_B"]);
        client.get_probe_value("writeHitNum_dcacheRAND_TH", toUpdateStats["writeHitNum_dcacheRAND_TH"]);
        client.get_probe_value("writeHitNum_dcacheRAND_THA", toUpdateStats["writeHitNum_dcacheRAND_THA"]);
        client.get_probe_value("writeHitNum_dcacheRAND_B", toUpdateStats["writeHitNum_dcacheRAND_B"]);
        client.get_probe_value("writeMissNum_dcacheLRU_TH", toUpdateStats["writeMissNum_dcacheLRU_TH"]);
        client.get_probe_value("writeMissNum_dcacheLRU_THA", toUpdateStats["writeMissNum_dcacheLRU_THA"]);
        client.get_probe_value("writeMissNum_dcacheLRU_B", toUpdateStats["writeMissNum_dcacheLRU_B"]);
        client.get_probe_value("writeMissNum_dcacheLRR_TH", toUpdateStats["writeMissNum_dcacheLRR_TH"]);
        client.get_probe_value("writeMissNum_dcacheLRR_THA", toUpdateStats["writeMissNum_dcacheLRR_THA"]);
        client.get_probe_value("writeMissNum_dcacheLRR_B", toUpdateStats["writeMissNum_dcacheLRR_B"]);
        client.get_probe_value("writeMissNum_dcacheRAND_TH", toUpdateStats["writeMissNum_dcacheRAND_TH"]);
        client.get_probe_value("writeMissNum_dcacheRAND_THA", toUpdateStats["writeMissNum_dcacheRAND_THA"]);
        client.get_probe_value("writeMissNum_dcacheRAND_B", toUpdateStats["writeMissNum_dcacheRAND_B"]);
        client.get_probe_value("readHitNum_dcacheLRU_TH", toUpdateStats["readHitNum_dcacheLRU_TH"]);
        client.get_probe_value("readHitNum_dcacheLRU_THA", toUpdateStats["readHitNum_dcacheLRU_THA"]);
        client.get_probe_value("readHitNum_dcacheLRU_B", toUpdateStats["readHitNum_dcacheLRU_B"]);
        client.get_probe_value("readHitNum_dcacheLRR_TH", toUpdateStats["readHitNum_dcacheLRR_TH"]);
        client.get_probe_value("readHitNum_dcacheLRR_THA", toUpdateStats["readHitNum_dcacheLRR_THA"]);
        client.get_probe_value("readHitNum_dcacheLRR_B", toUpdateStats["readHitNum_dcacheLRR_B"]);
        client.get_probe_value("readHitNum_dcacheRAND_TH", toUpdateStats["readHitNum_dcacheRAND_TH"]);
        client.get_probe_value("readHitNum_dcacheRAND_THA", toUpdateStats["readHitNum_dcacheRAND_THA"]);
        client.get_probe_value("readHitNum_dcacheRAND_B", toUpdateStats["readHitNum_dcacheRAND_B"]);
    }
}

///Returns a vector containing the IDs of the valid actions
///for this parameter
std::vector<int> CacheWritePolicyIf::getActionList(int parameter){
    std::vector<int> retVal;

    // No action can be performed if we are in the instruction cache:
    // we cannot change the write policy of the instruction cache
    if(this->pluginName[0] == 'i')
        return retVal;

    if(parameter != this->param2Enum.size() -1)
        retVal.push_back(0); //increment
    if(parameter != 0)
        retVal.push_back(1); //decrement
    return retVal;
}


/************************** Methods of the plugin creator factory **********************/

CacheWritePolicyCreator::CacheWritePolicyCreator(){
    plugin_creator_handler.insert(std::pair<std::string, PluginCreator *>("dcacheWritePolicy", this));
}

CacheWritePolicyCreator::~CacheWritePolicyCreator(){
    plugin_creator_handler.erase("dcacheWritePolicy");
}

///Creates an instance of the plugin
PluginIf * CacheWritePolicyCreator::create(const std::vector<std::string> &values, std::string pluginName){
    CacheWritePolicyIf * pluginInstance = new CacheWritePolicyIf(values, pluginName);
    this->instances.insert(pluginInstance);
    return pluginInstance;
}

///Destroys an instance of the plugin
void CacheWritePolicyCreator::destroy(PluginIf * toDestroy){
    this->instances.erase(toDestroy);
    delete dynamic_cast<CacheWritePolicyIf *>(toDestroy);
}
