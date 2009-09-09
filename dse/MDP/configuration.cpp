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


/*
 *   Class representing the problem on which the MOMH library
 *   has to be applied. In our case the problem is composed by
 *   the file describing the architecture to be simulated, the
 *   parameters which have to be varied during exploration and
 *   their possible values. Finally we have a list of objectives
 *   that we wish to optimize
*/


#include "configuration.hpp"

#include <boost/lexical_cast.hpp>

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <ctype.h>

#include <ctime>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#ifdef BOOST_NO_STDC_NAMESPACE
namespace std {
  using ::time;
}
#endif

//* Global variable declared external in problem.h */
Configuration config;

/// Takes in input the file containing the problems description and
/// initialized the problems class
/// The file format is:
/// [catecory]
/// name = value
///
/// For instance:
/// [ReSP]
/// architecure = FILENAME
/// localtion = PATH
/// port = PORT_NUM
/// [Objectives]
///  objectiveList = OBJ1,OBJ2
/// [Parameters]
///  parameter1 = value1,value2
///  parameter2 = value1,value2
///
/// Comments can be introduced by starting the line with #
bool Configuration::load(const std::string &fileName){
    std::ifstream infile(fileName.c_str());
    if (!infile.good()){
        return false;
    }
    std::string reminder = "";
    while (infile.good() || !reminder.empty()){
        std::string buffer_string;
        if(reminder.empty())
            getline(infile, buffer_string);
        else{
            buffer_string = reminder;
            reminder = "";
        }
        Configuration::TrimSpaces(buffer_string);

        if(!buffer_string.empty() && buffer_string.find('#') != 0){
            std::size_t startBrack = buffer_string.find('[');
            std::size_t endBrack = buffer_string.find(']');
            if(startBrack == std::string::npos || endBrack == std::string::npos){
                std::cerr << "Error in reading category " << buffer_string << std::endl;
                return false;
            }
            std::string category = buffer_string.substr( startBrack + 1, endBrack-startBrack -1);
            if(Configuration::ToLower(category) == "resp"){
                if(!this->ParseReSP(infile))
                    return false;
            }
            else if(Configuration::ToLower(category) == "algorithm"){
                if(!this->ParseObjectives(infile, reminder))
                    return false;
            }
            else if(Configuration::ToLower(category) == "parameters"){
                if(!this->ParseParams(infile, reminder))
                    return false;
            }
            else{
                std::cerr << "Unknown category " << category << std::endl;
                return false;
            }
        }
    }
    infile.close();
    //Finally I can set the objectives
    if(this->objectives.size() == 0){
        std::cerr << "Error, no objectives were specified" << std::endl;
        return false;
    }

    /* Lets print a summary of the data */
    std::cout << "Objectives: " << std::endl;
    for (unsigned int iobj = 0; iobj < this->objectives.size(); iobj++)
        std::cout << "  " << this->objectives[iobj] << std::endl;
    std::cout << "Parameters: " << std::endl;
    std::map<std::string, std::vector<std::string> >::const_iterator paramIter, paramEnd;
    for (paramIter = this->parameters.begin(), paramEnd = this->parameters.end(); paramIter != paramEnd; paramIter++){
        std::cout << "  " << paramIter->first << " = ";
        std::vector<std::string>::const_iterator parValIter, parValEnd;
        for(parValIter = paramIter->second.begin(), parValEnd = paramIter->second.end(); parValIter != parValEnd; parValIter++)
            std::cout << *parValIter << ", ";
        std::cout << std::endl;
    }
    std::cout << "Objective Function Parameters: " << std::endl;
    for (paramIter = this->objFunParams.begin(), paramEnd = this->objFunParams.end(); paramIter != paramEnd; paramIter++){
        std::cout << "  " << paramIter->first << " = ";
        std::vector<std::string>::const_iterator parValIter, parValEnd;
        for(parValIter = paramIter->second.begin(), parValEnd = paramIter->second.end(); parValIter != parValEnd; parValIter++)
            std::cout << *parValIter << ", ";
        std::cout << std::endl;
    }
    std::cout << "ReSP architecture " << this->archFile << std::endl;
    std::cout << "Plugin Folder " << this->pluginFolder << std::endl << std::endl;
    return true;
}

bool Configuration::ParseObjectives(std::ifstream &stream, std::string & reminder){
    unsigned int foundOptions = 0;
    while (stream.good()){
        std::string buffer_string;
        getline(stream, buffer_string);
        Configuration::TrimSpaces(buffer_string);
        if(!buffer_string.empty() && buffer_string.find('#') != 0){
            std::size_t foundEqual = buffer_string.find('=');
            if(foundEqual == std::string::npos){
                reminder = buffer_string;
                if(foundOptions > 6)
                    return true;
                else{
                    std::cerr << "Error in line \"" << buffer_string << "\" during parsing of Algorithm section: not enough parameters found"  << std::endl;
                    return false;
                }
            }
            if(buffer_string.size() <= foundEqual + 1){
                std::cerr << "Error in line \"" << buffer_string << "\" during parsing of Algorithm section"  << std::endl;
                return false;
            }
            std::string option = buffer_string.substr(0, foundEqual);
            Configuration::TrimSpaces(option);
            std::string optionValue = buffer_string.substr(foundEqual + 1);
            Configuration::TrimSpaces(optionValue);
            if(Configuration::ToLower(option) == "objectivelist"){
                std::size_t commaPos = optionValue.find(',');
                while(commaPos != std::string::npos){
                    std::string objective = optionValue.substr(0, commaPos);
                    this->objectives.push_back(Configuration::TrimSpaces(objective));
                    optionValue = optionValue.substr(commaPos + 1);
                    Configuration::TrimSpaces(optionValue);
                    commaPos = optionValue.find(',');
                }
                this->objectives.push_back(Configuration::TrimSpaces(optionValue));
                foundOptions++;
                //std::cerr << "objectivelist " << foundOptions << std::endl;
            }
            else if(Configuration::ToLower(option) == "horizon"){
                this->horizon = boost::lexical_cast<unsigned int>(optionValue);
                foundOptions++;
                //std::cerr << "horizon " << foundOptions << std::endl;
            }
            else if(Configuration::ToLower(option) == "numruns"){
                this->numRuns = boost::lexical_cast<unsigned int>(optionValue);
                foundOptions++;
                //std::cerr << "numRuns " << foundOptions << std::endl;
            }
            else if(Configuration::ToLower(option) == "verbosity"){
                this->verbosity = boost::lexical_cast<int>(optionValue);
            }
            else if(Configuration::ToLower(option) == "lambda"){
                this->lambda = boost::lexical_cast<double>(optionValue);
                if(this->lambda < 0 || this->lambda > 1){
                    std::cerr << "Wrong value for lambda " << this->lambda << " it should be in the [0, 1] interval"  << std::endl;
                    return false;
                }
                foundOptions++;
                //std::cerr << "lambda " << foundOptions << std::endl;
            }
            else if(Configuration::ToLower(option) == "gamma"){
                this->gamma = boost::lexical_cast<double>(optionValue);
                if(this->gamma < 0 || this->gamma > 1){
                    std::cerr << "Wrong value for gamma " << this->gamma << " it should be in the [0, 1] interval"  << std::endl;
                    return false;
                }
                foundOptions++;
                //std::cerr << "gamma " << foundOptions << std::endl;
            }
            else if(Configuration::ToLower(option) == "eps"){
                this->eps = boost::lexical_cast<double>(optionValue);
                if(this->eps < 0 || this->eps > 1){
                    std::cerr << "Wrong value for eps " << this->eps << " it should be in the [0, 1] interval"  << std::endl;
                    return false;
                }
                foundOptions++;
                //std::cerr << "eps " << foundOptions << std::endl;
            }
            else if(Configuration::ToLower(option) == "virtualsample"){
                this->virtualSample = boost::lexical_cast<unsigned int>(optionValue);
                foundOptions++;
                //std::cerr << "virtualsample " << foundOptions << std::endl;
            }
            else if(Configuration::ToLower(option) == "maxbranchwidth"){
                this->maxBranchWidth = boost::lexical_cast<unsigned int>(optionValue);
                //std::cerr << "maxBranchWidth " << foundOptions << std::endl;
            }
            else if(Configuration::ToLower(option) == "maxvertices"){
                this->maxvertices = boost::lexical_cast<unsigned int>(optionValue);
                //std::cerr << "maxvertices " << foundOptions << std::endl;
            }
            else{
                // I parse the parameters of the objective function
                std::size_t commaPos = optionValue.find(',');
                while(commaPos != std::string::npos){
                    std::string parameter = optionValue.substr(0, commaPos);
                    this->objFunParams[option].push_back(Configuration::TrimSpaces(parameter));
                    optionValue = optionValue.substr(commaPos + 1);
                    Configuration::TrimSpaces(optionValue);
                    commaPos = optionValue.find(',');
                }
                this->objFunParams[option].push_back(Configuration::TrimSpaces(optionValue));
            }
        }
    }
    return true;
}

bool Configuration::ParseReSP(std::ifstream &stream){
    int foundOptions = 0;
    while (stream.good() && foundOptions < 6){
        std::string buffer_string;
        getline(stream,buffer_string);
        Configuration::TrimSpaces(buffer_string);
        if(!buffer_string.empty() && buffer_string.find('#') != 0){
            std::size_t foundEqual = buffer_string.find('=');
            if(foundEqual == std::string::npos){
                std::cerr << "Error in line \"" << buffer_string << "\" during parsing of ReSP section"  << std::endl;
                return false;
            }
            if(buffer_string.size() <= foundEqual + 1){
                std::cerr << "Error in line \"" << buffer_string << "\" during parsing of ReSP section"  << std::endl;
                return false;
            }
            std::string option = buffer_string.substr(0, foundEqual);
            Configuration::TrimSpaces(option);
            std::string optionValue = buffer_string.substr(foundEqual + 1);
            Configuration::TrimSpaces(optionValue);
            if(Configuration::ToLower(option) == "architecure"){
                this->archFile = optionValue;
                foundOptions++;
            }
            else if(Configuration::ToLower(option) == "location"){
                this->respPath = optionValue;
                foundOptions++;
            }
            else if(Configuration::ToLower(option) == "permanent"){
                this->permanent = boost::lexical_cast<bool>(optionValue);
            }
            else if(Configuration::ToLower(option) == "application"){
                this->application = optionValue;
                foundOptions++;
            }
            else if(Configuration::ToLower(option) == "pluginfolder"){
                this->pluginFolder = optionValue;
                foundOptions++;
            }
            else if(Configuration::ToLower(option) == "port"){
                this->respPort = boost::lexical_cast<unsigned int>(optionValue);
                foundOptions++;
            }
            else if(Configuration::ToLower(option) == "graphic"){
                this->graphic = boost::lexical_cast<bool>(optionValue);
                foundOptions++;
            }
            else{
                std::cerr << "Unknown option " << option << std::endl;
                return false;
            }
        }
    }
    if(foundOptions == 6)
        return true;
    else{
        std::cerr << "Error in parsing ReSP section in the configuration file: found " << foundOptions << " expected 5" << std::endl;
        return false;
    }
}

bool Configuration::ParseParams(std::ifstream &stream, std::string & reminder){
    while (stream.good()){
        std::string buffer_string;
        getline(stream,buffer_string);
        Configuration::TrimSpaces(buffer_string);
        if(!buffer_string.empty() && buffer_string.find('#') != 0){
            std::size_t foundEqual = buffer_string.find('=');
            if(foundEqual == std::string::npos){
                reminder = buffer_string;
                return true;
            }
            if(buffer_string.size() <= foundEqual + 1){
                std::cerr << "Error in line \"" << buffer_string << "\" during parsing of Parameters section"  << std::endl;
                return false;
            }
            std::string option = buffer_string.substr(0, foundEqual);
            Configuration::TrimSpaces(option);
            std::string optionValue = buffer_string.substr(foundEqual + 1);
            Configuration::TrimSpaces(optionValue);
            std::size_t commaPos = optionValue.find(',');
            while(commaPos != std::string::npos){
                std::string parameter = optionValue.substr(0, commaPos);
                this->parameters[option].push_back(Configuration::TrimSpaces(parameter));
                optionValue = optionValue.substr(commaPos + 1);
                Configuration::TrimSpaces(optionValue);
                commaPos = optionValue.find(',');
            }
            this->parameters[option].push_back(Configuration::TrimSpaces(optionValue));
        }
    }
    return true;
}

std::string& Configuration::ToLower(std::string& str){
    for(unsigned int i = 0; i < str.length(); i++){
        str[i] = tolower(str[i]);
    }
    return str;
}

std::string& Configuration::TrimSpaces( std::string& str){
    // Trim Both leading and trailing spaces
    std::size_t startpos = str.find_first_not_of(" \t");
    std::size_t endpos = str.find_last_not_of(" \t");

    if(( std::string::npos == startpos ) || ( std::string::npos == endpos))
        str = "";
    else
        str = str.substr( startpos, endpos-startpos+1 );
    return str;
}

Configuration::Configuration() : graphic(true), generator(static_cast<unsigned int>(std::time(NULL))), verbosity(0),
                                                                        verbose(false), simulations_num(0), permanent(true){
    this->maxBranchWidth = (unsigned int)-1;
    this->maxvertices = 30000;
}
