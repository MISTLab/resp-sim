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
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

#include "cacheSubstsPolicy.hpp"
#include "pluginIf.hpp"

#include "utils.hpp"

///Creator variable: this variable is initialized when the dll
///is loaded; this action also automatically registers the plugin
///in the application
static CacheSubstsPolicyCreator creator;

CacheSubstsPolicyIf::CacheSubstsPolicyIf(const std::vector<std::string> &values, std::string pluginName) : StringPlugin(pluginName), model(){
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
std::pair<float, float> CacheSubstsPolicyIf::changeValue(std::map<PluginIf*, int> &parameters, int newValue, const std::string &metric,
                                                const std::map<std::string, float> &centroidMap, const std::map<std::string, float> &statistics,
                                                                                            const std::map<PluginIf *, std::string> &parameter_values){
    // Note that actually I do not consider the write policy of the instruction cache, so no
    // action can be performed if we are in the instruction cache
    std::pair<float, float> retVal;
    std::string curCache = this->pluginName.substr(0, 1);

    std::map<std::string, PluginIf* >::const_iterator foundPlugin;

    int oldValue = parameters[this];

    if(centroidMap.find("execTime") == centroidMap.end())
        THROW_EXCEPTION("Unable to find the metric execTime");
    if(centroidMap.find("energy") == centroidMap.end())
        THROW_EXCEPTION("Unable to find the metric energy");

    // Now I have, using the difference between the old and new misses,
    // to compute the new value for the metrics.

    unsigned int oldReadHitNum = (unsigned int)statistics.find(curCache + "readHitNum")->second;
    unsigned int oldWriteHitNum = (unsigned int)statistics.find(curCache + "writeHitNum")->second;
    unsigned int oldReadMissNum = (unsigned int)statistics.find(curCache + "readMissNum")->second;
    unsigned int oldWriteMissNum = (unsigned int)statistics.find(curCache + "writeMissNum")->second;

    double modifier = 1.0;

    std::string newValueString = this->getParameterName(newValue);
    std::string newAllPolicy;
    if(newValueString.find("LRU") != std::string::npos)
        newAllPolicy = "LRU";
    else if(newValueString.find("LRR") != std::string::npos)
        newAllPolicy = "LRR";
    else if(newValueString.find("RANDOM") != std::string::npos) {
        newAllPolicy = "RAND";
        modifier = 1.15;
    }

    unsigned int newReadHitNum = 0;
    unsigned int newWriteHitNum = 0;
    unsigned int newReadMissNum = 0;
    unsigned int newWriteMissNum = 0;
    if(curCache == "d"){
        std::string curCacheWritePolicy = "TH";
        foundPlugin = plugin_handler.find("dcacheWritePolicy");
        if(foundPlugin != plugin_handler.end()){
            if(parameter_values.find(foundPlugin->second) != parameter_values.end()){
                curCacheWritePolicy = parameter_values.find(foundPlugin->second)->second;
                if(curCacheWritePolicy.find("THROUGH") != std::string::npos)
                    curCacheWritePolicy = "TH";
                else if(curCacheWritePolicy.find("BACK") != std::string::npos)
                    curCacheWritePolicy = "B";
                else if(curCacheWritePolicy.find("THROUGH_ALL") != std::string::npos)
                    curCacheWritePolicy = "THA";
            }
        }
        newReadHitNum = (unsigned int)statistics.find("readHitNum_dcache" + newAllPolicy + "_" + curCacheWritePolicy)->second;
        newWriteHitNum = (unsigned int)statistics.find("writeHitNum_dcache" + newAllPolicy + "_" + curCacheWritePolicy)->second;
        newReadMissNum = (unsigned int)statistics.find("readMissNum_dcache" + newAllPolicy + "_" + curCacheWritePolicy)->second;
        newWriteMissNum = (unsigned int)statistics.find("writeMissNum_dcache" + newAllPolicy + "_" + curCacheWritePolicy)->second;
    }
    else{
        newReadHitNum = (unsigned int)statistics.find("readHitNum_icache" + newAllPolicy)->second;
        newWriteHitNum = oldWriteHitNum;
        newReadMissNum = (unsigned int)statistics.find("readMissNum_icache" + newAllPolicy)->second;
        newWriteMissNum = oldWriteMissNum;
    }

    double cacheSize = 4*1024;
    foundPlugin = plugin_handler.find(curCache + "_cache_size");
    if(foundPlugin != plugin_handler.end()){
        if(parameter_values.find(foundPlugin->second) != parameter_values.end()){
            cacheSize = boost::lexical_cast<double>(parameter_values.find(foundPlugin->second)->second);
        }
    }
    double execTime = centroidMap.find("execTime")->second;

    double ratio = (double) (newReadMissNum+newWriteMissNum)/(double) (oldReadMissNum+oldWriteMissNum);


    // Now I have to compute the execution time after the parameter change. Note that
    // I acutally have the new number of misses and hits with the new policy
    double newExecTime = execTime*ratio;

//     cout <<  "Cache subst Time: " << newExecTime << endl;
//     cout <<  "Miss: " << newReadMissNum+newWriteMissNum << " Was: " <<  (oldReadMissNum+oldWriteMissNum) << endl;
//     cout <<  "Cache subst ratio: " << ratio << endl;

    if(metric == "execTime"){
        if( ratio > 1.0 ) {
            retVal.first = execTime*(1.0/modifier);
            retVal.second = execTime*ratio*modifier;
        } else {
            retVal.first = execTime*ratio*(1.0/modifier);
            retVal.second = execTime*modifier;
        }
    }
    else if(metric == "energy"){
        double curMetricCentroid = centroidMap.find("energy")->second;

//         // Get new energy values from ReSP
//         this->model.set_parameter("size", cacheSize);
//         this->model.set_probe("execution_time", execTime);
//         this->model.set_probe("readHitNum", oldReadHitNum);
//         this->model.set_probe("writeHitNum", oldWriteHitNum);
//         this->model.set_probe("readMissNum", oldReadMissNum);
//         this->model.set_probe("writeMissNum", oldWriteMissNum);
//         this->model.update_parameters();
//         double oldEnergy = this->model.get_energy();
//
//         this->model.set_probe("readHitNum", newReadHitNum);
//         this->model.set_probe("writeHitNum", newWriteHitNum);
//         this->model.set_probe("readMissNum", newReadMissNum);
//         this->model.set_probe("writeMissNum", newWriteMissNum);
//         this->model.set_probe("execution_time", newExecTime);
//         double newEnergyRatio = this->model.get_energy()/oldEnergy;

        if( ratio > 1.0 ) {
            retVal.first = curMetricCentroid*(1.0/modifier);
            retVal.second = curMetricCentroid*ratio*modifier;
        } else {
            retVal.first = curMetricCentroid*ratio*(1.0/modifier);
            retVal.second = curMetricCentroid*modifier;
        }
    }
    else
        THROW_EXCEPTION(metric << " Unrecognized metric");

    if( retVal.second < 0 || retVal.first < 0 )
        THROW_EXCEPTION( this->pluginName+" has negative value ("+boost::lexical_cast<std::string>(retVal.first)+","+boost::lexical_cast<std::string>(retVal.second)+")");

    return retVal;
}

void CacheSubstsPolicyIf::updateStatistics(std::map<std::string, float> &curStats, int oldValue, int action, const std::map<PluginIf *, std::string> &parameter_values){
    //I have to update the generic statistics of the cache with the numbers
    //from the actual statistic
    std::string newValueString = this->getParameterName(this->applyAction(oldValue, action));
    std::string newAllPolicy;
    if(newValueString.find("LRU") != std::string::npos)
        newAllPolicy = "LRU";
    else if(newValueString.find("LRR") != std::string::npos)
        newAllPolicy = "LRR";
    else if(newValueString.find("RANDOM") != std::string::npos)
        newAllPolicy = "RAND";
    if(this->pluginName[0] == 'i'){
        curStats["numBusAccesses"] += curStats["ireadMissNum"]-curStats["readMissNum_icache" + newAllPolicy];
        curStats["numMemAccesses"] += curStats["ireadMissNum"]-curStats["readMissNum_icache" + newAllPolicy];

        curStats["ireadHitNum"] = curStats["readHitNum_icache" + newAllPolicy];
        curStats["ireadMissNum"] = curStats["readMissNum_icache" + newAllPolicy];
    }
    else{
        std::string curCacheWritePolicy = "TH";
        std::map<std::string, PluginIf* >::const_iterator foundPlugin;
        foundPlugin = plugin_handler.find("dcacheWritePolicy");
        if(foundPlugin != plugin_handler.end()){
            if(parameter_values.find(foundPlugin->second) != parameter_values.end()){
                curCacheWritePolicy = parameter_values.find(foundPlugin->second)->second;
                if(curCacheWritePolicy.find("THROUGH") != std::string::npos)
                    curCacheWritePolicy = "TH";
                else if(curCacheWritePolicy.find("BACK") != std::string::npos)
                    curCacheWritePolicy = "B";
                else if(curCacheWritePolicy.find("THROUGH_ALL") != std::string::npos)
                    curCacheWritePolicy = "THA";
            }
        }
        curStats["numBusAccesses"] += (curStats["dreadMissNum"]-curStats["readMissNum_dcache" + newAllPolicy + "_" + curCacheWritePolicy]) +
                                      (curStats["dwriteMissNum"]-curStats["writeMissNum_dcache" + newAllPolicy + "_" + curCacheWritePolicy]);
        curStats["numMemAccesses"] += (curStats["dreadMissNum"]-curStats["readMissNum_dcache" + newAllPolicy + "_" + curCacheWritePolicy]) +
                                      (curStats["dwriteMissNum"]-curStats["writeMissNum_dcache" + newAllPolicy + "_" + curCacheWritePolicy]);

        curStats["dreadHitNum"] = curStats["readHitNum_dcache" + newAllPolicy + "_" + curCacheWritePolicy];
        curStats["dreadMissNum"] = curStats["readMissNum_dcache" + newAllPolicy + "_" + curCacheWritePolicy];
        curStats["dwriteHitNum"] = curStats["writeHitNum_dcache" + newAllPolicy + "_" + curCacheWritePolicy];
        curStats["dwriteMissNum"] = curStats["writeMissNum_dcache" + newAllPolicy + "_" + curCacheWritePolicy];
    }

}

///Using the current instance of ReSPClient, it queries ReSP for the new
///values of the metrics (i.e. CPI, frequency etc. for a processor) and returns
///this value
void CacheSubstsPolicyIf::getStats(RespClient &client, std::map<std::string, float> &toUpdateStats){
    //Here I get the stats for all the policies, the current one and all the
    //others
    std::string curCache = this->pluginName.substr(0, 1);
    std::string cacheStatus;
    client.get_probe_value(curCache + "CacheStatus", cacheStatus);
    if(cacheStatus.find("DISABLED") != std::string::npos)
        THROW_EXCEPTION("Error, the " << curCache << " cache is not enabled");

    client.get_probe_value("numMemAccesses", toUpdateStats["numMemAccesses"]);
    client.get_probe_value("numBusAccesses", toUpdateStats["numBusAccesses"]);
    client.get_probe_value(curCache + "readMissNum", toUpdateStats[curCache + "readMissNum"]);
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
    else{
        client.get_probe_value("readHitNum_icacheLRU", toUpdateStats["readHitNum_icacheLRU"]);
        client.get_probe_value("readHitNum_icacheLRR", toUpdateStats["readHitNum_icacheLRR"]);
        client.get_probe_value("readHitNum_icacheRAND", toUpdateStats["readHitNum_icacheRAND"]);
        client.get_probe_value("readMissNum_icacheLRU", toUpdateStats["readMissNum_icacheLRU"]);
        client.get_probe_value("readMissNum_icacheLRR", toUpdateStats["readMissNum_icacheLRR"]);
        client.get_probe_value("readMissNum_icacheRAND", toUpdateStats["readMissNum_icacheRAND"]);
    }
}

///Given the enumeration representing the parameter value of this plugin (i.e. the memory
///latency) it returns the corresponding memory latency
std::string CacheSubstsPolicyIf::getCachePolicy(int sizeEnum){
    bm_type::left_map::const_iterator foundVal = this->param2Enum.left.find(sizeEnum);
    if(foundVal == this->param2Enum.left.end())
        THROW_EXCEPTION("Unable to find the parameter value corresponding to " << sizeEnum);
    return foundVal->second;
}

/************************** Methods of the plugin creator factory **********************/

CacheSubstsPolicyCreator::CacheSubstsPolicyCreator(){
    plugin_creator_handler.insert(std::pair<std::string, PluginCreator *>("icacheSubstsPolicy", this));
    plugin_creator_handler.insert(std::pair<std::string, PluginCreator *>("dcacheSubstsPolicy", this));
}

CacheSubstsPolicyCreator::~CacheSubstsPolicyCreator(){
    plugin_creator_handler.erase("icacheSubstsPolicy");
    plugin_creator_handler.erase("dcacheSubstsPolicy");
}

///Creates an instance of the plugin
PluginIf * CacheSubstsPolicyCreator::create(const std::vector<std::string> &values, std::string pluginName){
    CacheSubstsPolicyIf * pluginInstance = new CacheSubstsPolicyIf(values, pluginName);
    this->instances.insert(pluginInstance);
    return pluginInstance;
}

///Destroys an instance of the plugin
void CacheSubstsPolicyCreator::destroy(PluginIf * toDestroy){
    this->instances.erase(toDestroy);
    delete dynamic_cast<CacheSubstsPolicyIf *>(toDestroy);
}
