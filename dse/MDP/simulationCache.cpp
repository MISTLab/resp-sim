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

#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "simulationCache.hpp"

#include "utils.hpp"

void SimulationCache::SystemConfig::print(std::ostream & stream) const{
    stream << this->param2Value.size() << ";"  << this->statistics.size() << ";" << this->objective2Value.size() << ";" <<  this->application << ";";
    std::map<std::string, std::string>::const_iterator paramIter, paramEnd;
    for(paramIter = this->param2Value.begin(), paramEnd = this->param2Value.end(); paramIter != paramEnd; paramIter++){
        stream << paramIter->first << "=" << paramIter->second << ";";
    }
    std::map<std::string, float>::const_iterator statsIter, statsEnd;
    for(statsIter = this->statistics.begin(), statsEnd = this->statistics.end(); statsIter != statsEnd; statsIter++){
        stream << statsIter->first << "=" << statsIter->second << ";";
    }
    std::map<std::string, double>::const_iterator metricIter, metricEnd, metricIterTemp;
    for (metricIter = this->objective2Value.begin(), metricIterTemp = metricIter, metricIterTemp++, metricEnd = this->objective2Value.end(); metricIter != metricEnd; metricIter++, metricIterTemp++){
        stream << metricIter->first << "=" << metricIter->second;
        if(metricIterTemp != metricEnd)
            stream << ";";
    }
    stream << std::endl;
}

void SimulationCache::SystemConfig::read(std::string &line){
    std::vector<std::string> lineElements;
    boost::split( lineElements, line, boost::is_any_of(";"));

    if(lineElements.size() < 4)
        THROW_EXCEPTION("Error, there should be at least 4 elements for each line, while there are " << lineElements.size());

    unsigned int numParams = boost::lexical_cast<unsigned int>(lineElements[0]);
    unsigned int numStats = boost::lexical_cast<unsigned int>(lineElements[1]);
    unsigned int numObjectives = boost::lexical_cast<unsigned int>(lineElements[2]);

    if(lineElements.size() != numParams + numStats + numObjectives + 4)
        THROW_EXCEPTION("Error, there should be " << numParams + numStats + numObjectives + 4 << " elements for each line, while there are " << lineElements.size());
    this->application = lineElements[3];

    for(unsigned int i = 0; i < numParams; i++){
        std::size_t foundEqual = lineElements[4 + i].find("=");
        if(foundEqual == std::string::npos)
            THROW_EXCEPTION("Parameter element " << lineElements[3 + i] << " does not contain the equal sign");
        this->param2Value.insert(std::pair<std::string, std::string>(lineElements[4 + i].substr(0, foundEqual), lineElements[4 + i].substr(foundEqual + 1)));
    }

    for(unsigned int i = 0; i < numStats; i++){
        std::size_t foundEqual = lineElements[4 + i + numParams].find("=");
        if(foundEqual == std::string::npos)
            THROW_EXCEPTION("Statistic element " << lineElements[4 + i + numParams] << " does not contain the equal sign");
        this->statistics.insert(std::pair<std::string, float>(lineElements[4 + i + numParams].substr(0, foundEqual), boost::lexical_cast<float>(lineElements[4 + i + numParams].substr(foundEqual + 1))));
    }

    for(unsigned int i = 0; i < numObjectives; i++){
        std::size_t foundEqual = lineElements[4 + i + numParams + numStats].find("=");
        if(foundEqual == std::string::npos)
            THROW_EXCEPTION("Objective element " << lineElements[4 + i + numParams + numStats] << " does not contain the equal sign");
        this->objective2Value.insert(std::pair<std::string, double>(lineElements[4 + i + numParams + numStats].substr(0, foundEqual), boost::lexical_cast<double>(lineElements[4 + i + numParams + numStats].substr(foundEqual + 1))));
    }
}

bool SimulationCache::SystemConfig::operator ==( const SystemConfig & other ) const{
    return this->equal(other);
}

bool SimulationCache::SystemConfig::equal( const SystemConfig & other ) const{
    std::map<std::string, std::string>::const_iterator param2ValueIter, param2ValueEnd, foundParameter;
    std::map<std::string, double>::const_iterator metric2ValueIter, metric2ValueEnd;

    if(other.application.find(this->application) == std::string::npos ) {
        return false;
    }

    for(param2ValueIter = this->param2Value.begin(), param2ValueEnd = this->param2Value.end(); param2ValueIter != param2ValueEnd; param2ValueIter++){
        foundParameter = other.param2Value.find(param2ValueIter->first);
        if(foundParameter == other.param2Value.end()) {
            return false;
        }
        if(foundParameter->second != param2ValueIter->second) {
            return false;
        }
    }

    for(metric2ValueIter = this->objective2Value.begin(), metric2ValueEnd = this->objective2Value.end(); metric2ValueIter != metric2ValueEnd; metric2ValueIter++){
        if(other.objective2Value.find(metric2ValueIter->first) == other.objective2Value.end())  {
            return false;
        }
    }

    return true;
}

bool SimulationCache::SystemConfig::equal(const std::vector<std::string> &objectiveNames,
    const std::map<std::string, std::string> &param2Value, const std::string &application) const{
    SystemConfig other;
    other.param2Value = param2Value;
    other.application = application;
    std::vector<std::string>::const_iterator objIter, objEnd;
    for(objIter = objectiveNames.begin(), objEnd = objectiveNames.end(); objIter != objEnd; objIter++)
        other.objective2Value.insert(std::pair<std::string, double>(*objIter, 0));

    return this->equal(other);
}

SimulationCache::SimulationCache(std::string cacheFileName) : cacheFileName(cacheFileName){
    // Here I have to fill in the vector with all the currently read solutions
    std::ifstream infile(cacheFileName.c_str());
    while(infile.good()){
        SystemConfig newSolution;
        std::string buffer_string;
        getline(infile, buffer_string);
        if(infile.good()){
            newSolution.read(buffer_string);
            this->cacheConfigs.push_back(newSolution);
        }
    }
    infile.close();
}

SimulationCache::~SimulationCache(){
}

/// Looks in the simulation cache for the specified solution
bool SimulationCache::find(const std::vector<std::string> &objectiveNames,
        const std::map<std::string, std::string> &param2Value, const std::string &application,
            std::map<std::string, double> &objectiveValues, std::map<std::string, float> &statistics){
    std::vector<SystemConfig>::const_iterator solIter, solEnd;

    for(solIter = this->cacheConfigs.begin(), solEnd = this->cacheConfigs.end(); solIter != solEnd; solIter++){
        if(solIter->equal(objectiveNames, param2Value, application)){
            objectiveValues.insert(solIter->objective2Value.begin(), solIter->objective2Value.end());
            statistics.insert(solIter->statistics.begin(), solIter->statistics.end());
            return true;
        }
    }

    return false;
}

/// Adds an element to the simulation cache and dumps it on the cache file
void SimulationCache::add(const std::map<std::string, double> &objective2Value,
        const std::map<std::string, std::string> &param2Value, const std::map<std::string, float> &statistics, const std::string &application){
    SystemConfig other;
    other.param2Value = param2Value;
    std::size_t fileNameBeg = application.find_last_of('/');
    if(fileNameBeg == std::string::npos)
        other.application = application;
    else
        other.application = application.substr(fileNameBeg + 1);
    other.objective2Value = objective2Value;
    other.statistics = statistics;

    // Now I open the stream and save there the solution
    std::ofstream cacheFile(this->cacheFileName.c_str(), std::ofstream::app);
    other.print(cacheFile);
    cacheFile.close();

    this->cacheConfigs.push_back(other);
}
