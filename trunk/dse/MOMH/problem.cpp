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


/*
 *   Class representing the problem on which the MOMH library
 *   has to be applied. In our case the problem is composed by
 *   the file describing the architecture to be simulated, the
 *   parameters which have to be varied during exploration and
 *   their possible values. Finally we have a list of objectives
 *   that we wish to optimize
*/


#include "problem.hpp"

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
TDSEProblem Problem;

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
bool TDSEProblem::Load(char *FileName){
    std::ifstream infile(FileName);
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
        TDSEProblem::TrimSpaces(buffer_string);

        if(!buffer_string.empty() && buffer_string.find('#') != 0){
            std::size_t startBrack = buffer_string.find('[');
            std::size_t endBrack = buffer_string.find(']');
            if(startBrack == std::string::npos || endBrack == std::string::npos){
                std::cerr << "Error in reading category " << buffer_string << std::endl;
                return false;
            }
            std::string category = buffer_string.substr( startBrack + 1, endBrack-startBrack -1);
            if(TDSEProblem::ToLower(category) == "resp"){
                if(!this->ParseReSP(infile))
                    return false;
            }
            else if(TDSEProblem::ToLower(category) == "objectives"){
                if(!this->ParseObjectives(infile))
                    return false;
            }
            else if(TDSEProblem::ToLower(category) == "parameters"){
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
    NumberOfObjectives = this->objectives.size();
    Objectives.resize(NumberOfObjectives);
    for (int iobj = 0; iobj < NumberOfObjectives; iobj++){
        // TODO: so far the fact the objective is the minimization is hardcoded
        // we should have it specifiable from configuration line ...
        Objectives[iobj].ObjectiveType = _Min;
        Objectives[iobj].bActive = true;
    }

    /* Lets print a summary of the data */
    std::cout << "Objectives: " << std::endl;
    for (int iobj = 0; iobj < NumberOfObjectives; iobj++)
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
    std::cout << "ReSP architecture " << this->archFile << std::endl << std::endl;
    return true;
}

bool TDSEProblem::ParseObjectives(std::ifstream &stream){
    while (stream.good()){
        std::string buffer_string;
        getline(stream, buffer_string);
        TDSEProblem::TrimSpaces(buffer_string);
        if(!buffer_string.empty() && buffer_string.find('#') != 0){
            std::size_t foundEqual = buffer_string.find('=');
            if(foundEqual == std::string::npos){
                std::cerr << "Error in line \"" << buffer_string << "\" during parsing of Objectives section"  << std::endl;
                return false;
            }
            if(buffer_string.size() <= foundEqual + 1){
                std::cerr << "Error in line \"" << buffer_string << "\" during parsing of Objectives section"  << std::endl;
                return false;
            }
            std::string option = buffer_string.substr(0, foundEqual);
            TDSEProblem::TrimSpaces(option);
            std::string optionValue = buffer_string.substr(foundEqual + 1);
            TDSEProblem::TrimSpaces(optionValue);
            if(TDSEProblem::ToLower(option) == "objectivelist"){
                std::size_t commaPos = optionValue.find(',');
                while(commaPos != std::string::npos){
                    std::string objective = optionValue.substr(0, commaPos);
                    this->objectives.push_back(TDSEProblem::TrimSpaces(objective));
                    optionValue = optionValue.substr(commaPos + 1);
                    TDSEProblem::TrimSpaces(optionValue);
                    commaPos = optionValue.find(',');
                }
                this->objectives.push_back(TDSEProblem::TrimSpaces(optionValue));
                return true;
            }
            else{
                std::cerr << "Unknown option " << option << std::endl;
                return false;
            }
        }
    }
    std::cerr << "Error during parsing of the objectives" << std::endl;
    return false;
}

bool TDSEProblem::ParseReSP(std::ifstream &stream){
    int foundOptions = 0;
    while (stream.good() && foundOptions < 5){
        std::string buffer_string;
        getline(stream,buffer_string);
        TDSEProblem::TrimSpaces(buffer_string);
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
            TDSEProblem::TrimSpaces(option);
            std::string optionValue = buffer_string.substr(foundEqual + 1);
            TDSEProblem::TrimSpaces(optionValue);
            if(TDSEProblem::ToLower(option) == "architecure"){
                this->archFile = optionValue;
                foundOptions++;
            }
            else if(TDSEProblem::ToLower(option) == "localtion"){
                this->respPath = optionValue;
                foundOptions++;
            }
            else if(TDSEProblem::ToLower(option) == "application"){
                this->application = optionValue;
                foundOptions++;
            }
            else if(TDSEProblem::ToLower(option) == "port"){
                this->respPort = boost::lexical_cast<unsigned int>(optionValue);
                foundOptions++;
            }
            else if(TDSEProblem::ToLower(option) == "graphic"){
                this->graphic = boost::lexical_cast<bool>(optionValue);
                foundOptions++;
            }
            else if(TDSEProblem::ToLower(option) == "simulations_max"){
                this->simulations_max = boost::lexical_cast<unsigned int>(optionValue);
            }
            else{
                std::cerr << "Unknown option " << option << std::endl;
                return false;
            }
        }
    }
    if(foundOptions == 5)
        return true;
    else{
        std::cerr << "Error in parsing ReSP section in the configuration file: found " << foundOptions << " expected 5" << std::endl;
        return false;
    }
}

bool TDSEProblem::ParseParams(std::ifstream &stream, std::string & reminder){
    while (stream.good()){
        std::string buffer_string;
        getline(stream,buffer_string);
        TDSEProblem::TrimSpaces(buffer_string);
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
            TDSEProblem::TrimSpaces(option);
            std::string optionValue = buffer_string.substr(foundEqual + 1);
            TDSEProblem::TrimSpaces(optionValue);
            std::size_t commaPos = optionValue.find(',');
            while(commaPos != std::string::npos){
                std::string parameter = optionValue.substr(0, commaPos);
                this->parameters[option].push_back(TDSEProblem::TrimSpaces(parameter));
                optionValue = optionValue.substr(commaPos + 1);
                TDSEProblem::TrimSpaces(optionValue);
                commaPos = optionValue.find(',');
            }
            this->parameters[option].push_back(TDSEProblem::TrimSpaces(optionValue));
        }
    }
    return true;
}

std::string& TDSEProblem::ToLower(std::string& str){
    for(unsigned int i = 0; i < str.length(); i++){
        str[i] = tolower(str[i]);
    }
    return str;
}

std::string& TDSEProblem::TrimSpaces( std::string& str){
    // Trim Both leading and trailing spaces
    std::size_t startpos = str.find_first_not_of(" \t");
    std::size_t endpos = str.find_last_not_of(" \t");

    if(( std::string::npos == startpos ) || ( std::string::npos == endpos))
        str = "";
    else
        str = str.substr( startpos, endpos-startpos+1 );
    return str;
}

TDSEProblem::~TDSEProblem(){}

TDSEProblem::TDSEProblem() : graphic(true), generator(static_cast<unsigned int>(std::time(0))), verbose(false), simulations_num(0), simulations_max(0){}
