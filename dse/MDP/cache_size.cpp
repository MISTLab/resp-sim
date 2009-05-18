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
#include <set>
#include <vector>
#include <cmath>
#include "RespClient.hpp"

#include "cache_size.hpp"
#include "pluginIf.hpp"

#include "utils.hpp"

#ifdef MEMORY_DEBUG
#include <mpatrol.h>
#endif

///Creator variable: this variable is initialized when the dll
///is loaded; this action also automatically registers the plugin
///in the application
static CacheSizeCreator creator;

CacheSizeIf::CacheSizeIf(const std::vector<std::string> &values, std::string pluginName) : IntegerPlugin(pluginName), model(){
    int numInserted = 0;
    unsigned int prevVal = 0;
    std::vector<std::string>::const_iterator valueIter, valueEnd;
    for(valueIter = values.begin(), valueEnd = values.end(); valueIter != valueEnd; valueIter++){
        if(prevVal != 0 && prevVal*2 != boost::lexical_cast<unsigned int>(*valueIter)){
            THROW_EXCEPTION("Values for the cache size can only be in consecutive powers of two: " << prevVal << " and " << *valueIter << " are invalid!!");
        }
        prevVal = boost::lexical_cast<unsigned int>(*valueIter);
        this->param2Enum.insert(bm_type::value_type(numInserted, boost::lexical_cast<unsigned int>(*valueIter)));
        numInserted++;
    }
}

///Given a metric and the new parameter value, we obtain an
///estimation of the metric change from the old value to the new
///one
std::pair<float, float> CacheSizeIf::changeValue(plugin_int_map &parameters, int newValue, const std::string &metric,
                                    const float_map &centroidMap, const float_map &statistics,
                                                                            const std::map<PluginIf *, std::string> &parameter_values){
    int oldValue = parameters[this];
    std::pair<float, float> retVal;

    unsigned int readHitNum = 0;
    unsigned int writeHitNum = 0;
    unsigned int readMissNum = 0;
    unsigned int writeMissNum = 0;
    if(this->pluginName[0] == 'd'){
        readHitNum = (unsigned int)statistics.find("dreadHitNum")->second;
        writeHitNum = (unsigned int)statistics.find("dwriteHitNum")->second;
        readMissNum = (unsigned int)statistics.find("dreadMissNum")->second;
        writeMissNum = (unsigned int)statistics.find("dwriteMissNum")->second;
    }
    else{
        readHitNum = (unsigned int)statistics.find("ireadHitNum")->second;
        writeHitNum = (unsigned int)statistics.find("iwriteHitNum")->second;
        readMissNum = (unsigned int)statistics.find("ireadMissNum")->second;
        writeMissNum = (unsigned int)statistics.find("iwriteMissNum")->second;
    }

    unsigned int oldSize = this->getCacheSize(oldValue);
    unsigned int newSize = this->getCacheSize(newValue);

    if(centroidMap.find("execTime") == centroidMap.end())
        THROW_EXCEPTION("Unable to find the metric execTime");
    if(centroidMap.find("energy") == centroidMap.end())
        THROW_EXCEPTION("Unable to find the metric energy");

    #ifndef NDEBUG
    if(oldSize == newSize)
        THROW_EXCEPTION("After action the values for the old and new cache size are still the same");
    #endif

    double execTime = centroidMap.find("execTime")->second;

    double ratio = (double) (readHitNum+writeHitNum+readMissNum+writeMissNum)/(double) (readHitNum+writeHitNum);

    double bestTime = execTime*(1.0/ratio);
    double worstTime = execTime*ratio;
    double oldEnergy = 0;
    double newEnergyRatio = 0;

     if(metric == "execTime"){
//          cout << "Worst: " << worstTime << " Exec: " << execTime << " Best: " << bestTime << endl;
        if(oldSize > newSize){
            // Decrease the size:
            // Lower bound, execution time is not affected
            // Upper bound, execution time increases: all hits become misses
            retVal.first = execTime*0.9;
            retVal.second = worstTime*1.1;
        }
        else{
            // Increase the size:
            // Lower bound, execution time decreases: all misses become hits
            // Upper bound, execution time is unaffected
            retVal.first = bestTime*0.9;
            retVal.second = execTime*1.1;
        }
    }
    else if(metric == "energy"){
        double curMetricCentroid = centroidMap.find("energy")->second;

        // Get new energy values from ReSP
        this->model.set_parameter("size",oldSize );
        this->model.set_probe("execution_time",0);
        this->model.set_probe("readHitNum", readHitNum );
        this->model.set_probe("writeHitNum", writeHitNum );
        this->model.set_probe("readMissNum", readMissNum );
        this->model.set_probe("writeMissNum", writeMissNum );
        this->model.update_parameters();
        oldEnergy = model.get_energy();

        // New cache size, same parameters
        this->model.set_parameter("size",newSize );
        this->model.update_parameters();
        newEnergyRatio = oldEnergy/model.get_energy();

        // New cache size no misses
        this->model.set_probe("readHitNum", readHitNum+readMissNum );
        this->model.set_probe("writeHitNum", writeHitNum+writeMissNum );
        this->model.set_probe("readMissNum", 0 );
        this->model.set_probe("writeMissNum", 0 );
        this->model.update_parameters();
        double bestEnergy = model.get_energy();

        // New cache size no hits
        this->model.set_probe("readHitNum", 0 );
        this->model.set_probe("writeHitNum", 0 );
        this->model.set_probe("readMissNum", readHitNum+readMissNum );
        this->model.set_probe("writeMissNum", writeHitNum+writeMissNum );
        this->model.update_parameters();
        double worstEnergy = model.get_energy();

        if( bestEnergy > worstEnergy ) {
            double temp = worstEnergy;
            worstEnergy = bestEnergy;
            bestEnergy = temp;
        }
        
//         cout << "Old         size: " << oldSize << " New: " << newSize <<  endl;
//         cout << "Total Energy: " << curMetricCentroid <<  endl;
//         cout << "Old energy: " << oldEnergy << " New: " << model.get_energy() <<  endl;
//         cout << "New ratio: " << newEnergyRatio << endl;
     
        if(oldSize > newSize){
            if( worstEnergy > oldEnergy ) {
                retVal.first = curMetricCentroid;
                retVal.second = curMetricCentroid*(worstEnergy/oldEnergy);
            } else {
                retVal.first = curMetricCentroid*(worstEnergy/oldEnergy);
                retVal.second = curMetricCentroid*ratio;
            }

            //if( newEnergyRatio < 1.0 ) retVal.first = retVal.first*newEnergyRatio;
            
        }
        else{
            // Increase the size:
            if( bestEnergy < oldEnergy ) {
                retVal.first = curMetricCentroid*(bestEnergy/oldEnergy);
                retVal.second = curMetricCentroid;
            } else {
                retVal.first = curMetricCentroid*(1.0/ratio);
                retVal.second = curMetricCentroid*(bestEnergy/oldEnergy);
            }

            //if( newEnergyRatio > 1.0 ) retVal.second = retVal.second*newEnergyRatio;

        }

    }
    else
        THROW_EXCEPTION(metric << " Unrecognized metric");

    if( retVal.second < 0 || retVal.first < 0 )
        THROW_EXCEPTION( this->pluginName+" has negative value ("+boost::lexical_cast<std::string>(retVal.first)+","+boost::lexical_cast<std::string>(retVal.second)+")");

    if( retVal.second < retVal.first ) {
        cout << "Old size: " << oldSize << " New: " << newSize <<  endl;
        cout << "Old energy: " << oldEnergy << " New: " << model.get_energy() <<  endl;
        cout << "New ratio: " << newEnergyRatio << endl;
        THROW_EXCEPTION( this->pluginName+" has inverse values ("+boost::lexical_cast<std::string>(retVal.first)+","+boost::lexical_cast<std::string>(retVal.second)+")");
    }
    return retVal;
}

///It computes the new value of the metrics according to
///the specified value and returns it
void CacheSizeIf::updateStatistics(float_map &curStats, int oldValue, int action, const std::map<PluginIf *, std::string> &parameter_values){
    std::string k = this->pluginName.substr(0, 1);

    if(action == 0){ // Increase
        curStats[k+"readMissNum"] = round(curStats[k+"readMissNum"]/2);
        curStats[k+"writeMissNum"] = round(curStats[k+"writeMissNum"]/2);
        curStats[k+"readHitNum"] += curStats[k+"readMissNum"];
        curStats[k+"writeHitNum"] += curStats[k+"writeMissNum"];
        curStats["numBusAccesses"] -= curStats[k+"readMissNum"]+curStats[k+"writeMissNum"];
        curStats["numMemAccesses"] -= curStats[k+"readMissNum"]+curStats[k+"writeMissNum"];
        if(k == "d"){
            curStats["readMissNum_dcacheLRU_TH"] = round(curStats["readMissNum_dcacheLRU_TH"]/2);
            curStats["readMissNum_dcacheLRU_THA"] = round(curStats["readMissNum_dcacheLRU_THA"]/2);
            curStats["readMissNum_dcacheLRU_B"] = round(curStats["readMissNum_dcacheLRU_B"]/2);
            curStats["readMissNum_dcacheLRR_TH"] = round(curStats["readMissNum_dcacheLRR_TH"]/2);
            curStats["readMissNum_dcacheLRR_THA"] = round(curStats["readMissNum_dcacheLRR_THA"]/2);
            curStats["readMissNum_dcacheLRR_B"] = round(curStats["readMissNum_dcacheLRR_B"]/2);
            curStats["readMissNum_dcacheRAND_TH"] = round(curStats["readMissNum_dcacheRAND_TH"]/2);
            curStats["readMissNum_dcacheRAND_THA"] = round(curStats["readMissNum_dcacheRAND_THA"]/2);
            curStats["readMissNum_dcacheRAND_B"] = round(curStats["readMissNum_dcacheRAND_B"]/2);
            curStats["writeMissNum_dcacheLRU_TH"] = round(curStats["writeMissNum_dcacheLRU_TH"]/2);
            curStats["writeMissNum_dcacheLRU_THA"] = round(curStats["writeMissNum_dcacheLRU_THA"]/2);
            curStats["writeMissNum_dcacheLRU_B"] = round(curStats["writeMissNum_dcacheLRU_B"]/2);
            curStats["writeMissNum_dcacheLRR_TH"] = round(curStats["writeMissNum_dcacheLRR_TH"]/2);
            curStats["writeMissNum_dcacheLRR_THA"] = round(curStats["writeMissNum_dcacheLRR_THA"]/2);
            curStats["writeMissNum_dcacheLRR_B"] = round(curStats["writeMissNum_dcacheLRR_B"]/2);
            curStats["writeMissNum_dcacheRAND_TH"] = round(curStats["writeMissNum_dcacheRAND_TH"]/2);
            curStats["writeMissNum_dcacheRAND_THA"] = round(curStats["writeMissNum_dcacheRAND_THA"]/2);
            curStats["writeMissNum_dcacheRAND_B"] = round(curStats["writeMissNum_dcacheRAND_B"]/2);
            curStats["writeHitNum_dcacheLRU_TH"] += curStats["writeMissNum_dcacheLRU_TH"];
            curStats["writeHitNum_dcacheLRU_THA"] += curStats["writeMissNum_dcacheLRU_THA"];
            curStats["writeHitNum_dcacheLRU_B"] += curStats["writeMissNum_dcacheLRU_B"];
            curStats["writeHitNum_dcacheLRR_TH"] += curStats["writeMissNum_dcacheLRR_TH"];
            curStats["writeHitNum_dcacheLRR_THA"] += curStats["writeMissNum_dcacheLRR_THA"];
            curStats["writeHitNum_dcacheLRR_B"] += curStats["writeMissNum_dcacheLRR_B"];
            curStats["writeHitNum_dcacheRAND_TH"] += curStats["writeMissNum_dcacheRAND_TH"];
            curStats["writeHitNum_dcacheRAND_THA"] += curStats["writeMissNum_dcacheRAND_THA"];
            curStats["writeHitNum_dcacheRAND_B"] += curStats["writeMissNum_dcacheRAND_B"];
            curStats["readHitNum_dcacheLRU_TH"] += curStats["readMissNum_dcacheLRU_TH"];
            curStats["readHitNum_dcacheLRU_THA"] += curStats["readMissNum_dcacheLRU_THA"];
            curStats["readHitNum_dcacheLRU_B"] += curStats["readMissNum_dcacheLRU_B"];
            curStats["readHitNum_dcacheLRR_TH"] += curStats["readMissNum_dcacheLRR_TH"];
            curStats["readHitNum_dcacheLRR_THA"] += curStats["readMissNum_dcacheLRR_THA"];
            curStats["readHitNum_dcacheLRR_B"] += curStats["readMissNum_dcacheLRR_B"];
            curStats["readHitNum_dcacheRAND_TH"] += curStats["readMissNum_dcacheRAND_TH"];
            curStats["readHitNum_dcacheRAND_THA"] += curStats["readMissNum_dcacheRAND_THA"];
            curStats["readHitNum_dcacheRAND_B"] += curStats["readMissNum_dcacheRAND_B"];
        }
        else{
            curStats["readMissNum_icacheLRU"] = round(curStats["readMissNum_icacheLRU"]/2);
            curStats["readMissNum_icacheLRR"] = round(curStats["readMissNum_icacheLRR"]/2);
            curStats["readMissNum_icacheRAND"] = round(curStats["readMissNum_icacheRAND"]/2);
            curStats["readHitNum_icacheRAND"] += curStats["readMissNum_icacheRAND"];
            curStats["readHitNum_icacheLRU"] += curStats["readMissNum_icacheLRU"];
            curStats["readHitNum_icacheLRR"] += curStats["readMissNum_icacheLRR"];
        }
    } else if (action == 1) { //Decrease
        curStats[k+"readHitNum"] = round(curStats[k+"readHitNum"]/2);
        curStats[k+"writeHitNum"] = round(curStats[k+"writeHitNum"]/2);
        curStats[k+"readMissNum"] += curStats[k+"readHitNum"];
        curStats[k+"writeMissNum"] += curStats[k+"writeHitNum"];
        curStats["numBusAccesses"] += curStats[k+"readHitNum"]+curStats[k+"writeHitNum"];
        curStats["numMemAccesses"] += curStats[k+"readHitNum"]+curStats[k+"writeHitNum"];
        if(k == "d"){
            curStats["writeHitNum_dcacheLRU_TH"] = round(curStats["writeHitNum_dcacheLRU_TH"]/2);
            curStats["writeHitNum_dcacheLRU_THA"] = round(curStats["writeHitNum_dcacheLRU_THA"]/2);
            curStats["writeHitNum_dcacheLRU_B"] = round(curStats["writeHitNum_dcacheLRU_B"]/2);
            curStats["writeHitNum_dcacheLRR_TH"] = round(curStats["writeHitNum_dcacheLRR_TH"]/2);
            curStats["writeHitNum_dcacheLRR_THA"] = round(curStats["writeHitNum_dcacheLRR_THA"]/2);
            curStats["writeHitNum_dcacheLRR_B"] = round(curStats["writeHitNum_dcacheLRR_B"]/2);
            curStats["writeHitNum_dcacheRAND_TH"] = round(curStats["writeHitNum_dcacheRAND_TH"]/2);
            curStats["writeHitNum_dcacheRAND_THA"] = round(curStats["writeHitNum_dcacheRAND_THA"]/2);
            curStats["writeHitNum_dcacheRAND_B"] = round(curStats["writeHitNum_dcacheRAND_B"]/2);
            curStats["readHitNum_dcacheLRU_TH"] = round(curStats["readHitNum_dcacheLRU_TH"]/2);
            curStats["readHitNum_dcacheLRU_THA"] = round(curStats["readHitNum_dcacheLRU_THA"]/2);
            curStats["readHitNum_dcacheLRU_B"] = round(curStats["readHitNum_dcacheLRU_B"]/2);
            curStats["readHitNum_dcacheLRR_TH"] = round(curStats["readHitNum_dcacheLRR_TH"]/2);
            curStats["readHitNum_dcacheLRR_THA"] = round(curStats["readHitNum_dcacheLRR_THA"]/2);
            curStats["readHitNum_dcacheLRR_B"] = round(curStats["readHitNum_dcacheLRR_B"]/2);
            curStats["readHitNum_dcacheRAND_TH"] = round(curStats["readHitNum_dcacheRAND_TH"]/2);
            curStats["readHitNum_dcacheRAND_THA"] = round(curStats["readHitNum_dcacheRAND_THA"]/2);
            curStats["readHitNum_dcacheRAND_B"] = round(curStats["readHitNum_dcacheRAND_B"]/2);
            curStats["readMissNum_dcacheLRU_TH"] += curStats["readHitNum_dcacheLRU_TH"];
            curStats["readMissNum_dcacheLRU_THA"] += curStats["readHitNum_dcacheLRU_THA"];
            curStats["readMissNum_dcacheLRU_B"] += curStats["readHitNum_dcacheLRU_B"];
            curStats["readMissNum_dcacheLRR_TH"] += curStats["readHitNum_dcacheLRR_TH"];
            curStats["readMissNum_dcacheLRR_THA"] += curStats["readHitNum_dcacheLRR_THA"];
            curStats["readMissNum_dcacheLRR_B"] += curStats["readHitNum_dcacheLRR_B"];
            curStats["readMissNum_dcacheRAND_TH"] += curStats["readHitNum_dcacheRAND_TH"];
            curStats["readMissNum_dcacheRAND_THA"] += curStats["readHitNum_dcacheRAND_THA"];
            curStats["readMissNum_dcacheRAND_B"] += curStats["readHitNum_dcacheRAND_B"];
            curStats["writeMissNum_dcacheLRU_TH"] += curStats["writeHitNum_dcacheLRU_TH"];
            curStats["writeMissNum_dcacheLRU_THA"] += curStats["writeHitNum_dcacheLRU_THA"];
            curStats["writeMissNum_dcacheLRU_B"] += curStats["writeHitNum_dcacheLRU_B"];
            curStats["writeMissNum_dcacheLRR_TH"] += curStats["writeHitNum_dcacheLRR_TH"];
            curStats["writeMissNum_dcacheLRR_THA"] += curStats["writeHitNum_dcacheLRR_THA"];
            curStats["writeMissNum_dcacheLRR_B"] += curStats["writeHitNum_dcacheLRR_B"];
            curStats["writeMissNum_dcacheRAND_TH"] += curStats["writeHitNum_dcacheRAND_TH"];
            curStats["writeMissNum_dcacheRAND_THA"] += curStats["writeHitNum_dcacheRAND_THA"];
            curStats["writeMissNum_dcacheRAND_B"] += curStats["writeHitNum_dcacheRAND_B"];
        }
        else{
            curStats["readHitNum_icacheRAND"] = round(curStats["readHitNum_icacheRAND"]/2);
            curStats["readHitNum_icacheLRU"] = round(curStats["readHitNum_icacheLRU"]/2);
            curStats["readHitNum_icacheLRR"] = round(curStats["readHitNum_icacheLRR"]/2);
            curStats["readMissNum_icacheLRU"] += curStats["readHitNum_icacheLRU"];
            curStats["readMissNum_icacheLRR"] += curStats["readHitNum_icacheLRR"];
            curStats["readMissNum_icacheRAND"] += curStats["readHitNum_icacheRAND"];
        }
    }
    // I also have to go through all the other statistics and update them
}

///Using the current instance of ReSPClient, it queries ReSP for the new
///values of the metrics (i.e. CPI, frequency etc. for a processor) and returns
///this value
void CacheSizeIf::getStats(RespClient &client, float_map &toUpdateStats){
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
    else{
        client.get_probe_value("readHitNum_icacheRAND", toUpdateStats["readHitNum_icacheRAND"]);
        client.get_probe_value("readMissNum_icacheLRU", toUpdateStats["readMissNum_icacheLRU"]);
        client.get_probe_value("readMissNum_icacheLRR", toUpdateStats["readMissNum_icacheLRR"]);
        client.get_probe_value("readMissNum_icacheRAND", toUpdateStats["readMissNum_icacheRAND"]);
        client.get_probe_value("readHitNum_icacheLRU", toUpdateStats["readHitNum_icacheLRU"]);
        client.get_probe_value("readHitNum_icacheLRR", toUpdateStats["readHitNum_icacheLRR"]);
    }
    client.get_probe_value("numBusAccesses", toUpdateStats["numBusAccesses"]);
    client.get_probe_value("numMemAccesses", toUpdateStats["numMemAccesses"]);
}

///Given the enumeration representing the parameter value of this plugin (i.e. the memory
///latency) it returns the corresponding memory latency
unsigned int CacheSizeIf::getCacheSize(int sizeEnum){
    bm_type::left_map::const_iterator foundVal = this->param2Enum.left.find(sizeEnum);
    if(foundVal == this->param2Enum.left.end())
        THROW_EXCEPTION("Unable to find the parameter value corresponding to " << sizeEnum);
    return foundVal->second;
}

/************************** Methods of the plugin creator factory **********************/

CacheSizeCreator::CacheSizeCreator(){
    plugin_creator_handler.insert(std::pair<std::string, PluginCreator *>("d_cache_size", this));
    plugin_creator_handler.insert(std::pair<std::string, PluginCreator *>("i_cache_size", this));
}

CacheSizeCreator::~CacheSizeCreator(){
    plugin_creator_handler.erase("d_cache_size");
    plugin_creator_handler.erase("i_cache_size");
}

///Creates an instance of the plugin
PluginIf * CacheSizeCreator::create(const std::vector<std::string> &values, std::string pluginName){
    CacheSizeIf * pluginInstance = new CacheSizeIf(values, pluginName);
    this->instances.insert(pluginInstance);
    return pluginInstance;
}

///Destroys an instance of the plugin
void CacheSizeCreator::destroy(PluginIf * toDestroy){
    this->instances.erase(toDestroy);
    delete dynamic_cast<CacheSizeIf *>(toDestroy);
}
