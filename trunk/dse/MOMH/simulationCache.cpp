#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>

#include "simulationCache.hpp"

#include "utils.hpp"

void SimulationCache::SystemConfig::print(std::ostream & stream) const{
    stream << this->param2Value.size() << ";" << this->objective2Value.size() << ";" <<  this->application << ";";
    std::map<std::string, std::string>::const_iterator paramIter, paramEnd;
    for(paramIter = this->param2Value.begin(), paramEnd = this->param2Value.end(); paramIter != paramEnd; paramIter++){
        stream << paramIter->first << "=" << paramIter->second << ";";
    }
    std::map<std::string, double>::const_iterator metricIter, metricEnd, metricIterTemp;
    for (metricIter = this->objective2Value.begin(), metricIterTemp = metricIter, metricIterTemp++, metricEnd = this->objective2Value.end(); metricIter != metricEnd; metricIter++, metricIterTemp++){
        stream << metricIter->first << "=" << metricIter->second;
        if(metricIterTemp != metricEnd)
            stream << ";";
    }
    stream << std::endl;
}

void SimulationCache::SystemConfig::read(std::string line){
    std::vector<std::string> lineElements;
    boost::split( lineElements, line, boost::is_any_of(";"));
    if(lineElements.size() < 3)
        THROW_EXCEPTION("Error, there should be at least 3 elements for each line, while there are " << lineElements.size() << " -- line content: " << line);
    unsigned int numParams = boost::lexical_cast<unsigned int>(lineElements[0]);
    unsigned int numObjectives = boost::lexical_cast<unsigned int>(lineElements[1]);
    if(lineElements.size() != numParams + numObjectives + 3)
        THROW_EXCEPTION("Error, there should be " << numParams + numObjectives + 3 << " elements for each line, while there are " << lineElements.size());
    this->application = lineElements[2];
    for(unsigned int i = 0; i < numParams; i++){
        std::size_t foundEqual = lineElements[3 + i].find("=");
        if(foundEqual == std::string::npos)
            THROW_EXCEPTION("Parameter element " << lineElements[3 + i] << " does not contain the equal sign");
        this->param2Value.insert(std::pair<std::string, std::string>(lineElements[3 + i].substr(0, foundEqual), lineElements[3 + i].substr(foundEqual + 1)));
    }
    for(unsigned int i = 0; i < numObjectives; i++){
        std::size_t foundEqual = lineElements[3 + i + numParams].find("=");
        if(foundEqual == std::string::npos)
            THROW_EXCEPTION("Objective element " << lineElements[3 + i + numParams] << " does not contain the equal sign");
        this->objective2Value.insert(std::pair<std::string, double>(lineElements[3 + i + numParams].substr(0, foundEqual), boost::lexical_cast<double>(lineElements[3 + i + numParams].substr(foundEqual + 1))));
    }
}

bool SimulationCache::SystemConfig::operator ==( const SystemConfig & other ) const{
    return this->equal(other);
}

bool SimulationCache::SystemConfig::equal( const SystemConfig & other ) const{
    std::map<std::string, std::string>::const_iterator param2ValueIter, param2ValueEnd, foundParameter;
    std::map<std::string, double>::const_iterator metric2ValueIter, metric2ValueEnd;

    if(other.application.find(this->application) == std::string::npos && this->application.find(other.application) == std::string::npos){
        //std::cerr << "this->application=" << this->application << " other.application=" << other.application << std::endl;
        return false;
    }

    for(param2ValueIter = this->param2Value.begin(), param2ValueEnd = this->param2Value.end(); param2ValueIter != param2ValueEnd; param2ValueIter++){
        foundParameter = other.param2Value.find(param2ValueIter->first);
        if(foundParameter == other.param2Value.end()){
            //std::cerr << "Parameter " << foundParameter->first << " not found" << std::endl;
            return false;
        }
        if(foundParameter->second != param2ValueIter->second){
            //std::cerr << "Parameter " << foundParameter->first << " not equal" << std::endl;
            return false;
        }
    }

    for(metric2ValueIter = this->objective2Value.begin(), metric2ValueEnd = this->objective2Value.end(); metric2ValueIter != metric2ValueEnd; metric2ValueIter++){
        if(other.objective2Value.find(metric2ValueIter->first) == other.objective2Value.end()){
            //std::cerr << "Not found metric " << metric2ValueIter->first << std::endl;
            return false;
        }
    }

    return true;
}

bool SimulationCache::SystemConfig::equal(const std::vector<std::string> &objectiveNames,
    const std::map<std::string, std::string> &param2Value, const std::string &application) const{
    SystemConfig other;
    other.param2Value = param2Value;
    std::size_t fileNameBeg = application.find_last_of('/');
    if(fileNameBeg == std::string::npos)
        other.application = application;
    else
        other.application = application.substr(fileNameBeg + 1);
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

/// Adds an element to the simulation cache and dumps it on the cache file
void SimulationCache::add(const std::vector<std::string> &objectiveNames, const std::vector<double> &objectiveValues,
        const std::map<std::string, std::string> &param2Value, const std::string &application){
    SystemConfig other;
    other.param2Value = param2Value;
    std::size_t fileNameBeg = application.find_last_of('/');
    if(fileNameBeg == std::string::npos)
        other.application = application;
    else
        other.application = application.substr(fileNameBeg + 1);
    std::vector<std::string>::const_iterator objNIter, objNEnd;
    std::vector<double>::const_iterator objIter, objEnd;
    for(objNIter = objectiveNames.begin(), objNEnd = objectiveNames.end(), objIter = objectiveValues.begin(), objEnd = objectiveValues.end();
                                                                                objNIter != objNEnd && objIter != objEnd; objNIter++, objIter++)
        other.objective2Value.insert(std::pair<std::string, double>(*objNIter, *objIter));

    // Now I open the stream and save there the solution
    std::ofstream cacheFile(this->cacheFileName.c_str(), std::ofstream::app);
    other.print(cacheFile);
    cacheFile.close();

    this->cacheConfigs.push_back(other);
}

/// Looks in the simulation cache for the specified solution
std::map<std::string, double> SimulationCache::find(const std::vector<std::string> &objectiveNames,
        const std::map<std::string, std::string> &param2Value, const std::string &application){
    std::vector<SystemConfig>::const_iterator solIter, solEnd;
    for(solIter = this->cacheConfigs.begin(), solEnd = this->cacheConfigs.end(); solIter != solEnd; solIter++){
        if(solIter->equal(objectiveNames, param2Value, application))
            return solIter->objective2Value;
    }
    std::map<std::string, double> notFound;
    return notFound;
}

/// Adds an element to the simulation cache and dumps it on the cache file
void SimulationCache::add(const std::map<std::string, double> &objective2Value,
        const std::map<std::string, std::string> &param2Value, const std::string &application){
    SystemConfig other;
    other.param2Value = param2Value;
    std::size_t fileNameBeg = application.find_last_of('/');
    if(fileNameBeg == std::string::npos)
        other.application = application;
    else
        other.application = application.substr(fileNameBeg + 1);
    other.objective2Value = objective2Value;

    // Now I open the stream and save there the solution
    std::ofstream cacheFile(this->cacheFileName.c_str(), std::ofstream::app);
    other.print(cacheFile);
    cacheFile.close();

    this->cacheConfigs.push_back(other);
}
