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
#include <boost/lexical_cast.hpp>

#include "pluginIf.hpp"

std::map<std::string, PluginCreator* > plugin_creator_handler;
std::map<std::string, PluginIf* > plugin_handler;
std::map<PluginIf*, std::string > rev_plugin_handler;

///Function used to sort two strings in numerical order
bool PluginIf::numericSorter(const std::string &a, const std::string &b){
    return (boost::lexical_cast<double>(a) < boost::lexical_cast<double>(b));
}

///Given the current value of the parameter and the action which has to be
///applied to it, it returns the interval value of the corresponding metric.
///This method has more or less the same functionality of the previous one
std::pair<float, float> PluginIf::applyAction(std::map<PluginIf*, int> &parameters, int action, const std::string &metric,
                                const std::map<std::string, float> &centroidMap, const std::map<std::string, float> &statistics,
                                                                    const std::map<PluginIf *, std::string> &parameter_values){
    return this->changeValue(parameters, this->applyAction(parameters[this], action), metric, centroidMap, statistics, parameter_values);
}

///Given an action, it returns the dual action (i.e. that action such
///that, if summed with curAction, it would equal a null action) if
///it exists, an exception in case there is none.
int PluginIf::getDualAction(int curAction){
    return (1 - curAction);
}

///Specifies whether it is possible to estimate the effects of the current action or if simulation
///is needed
bool PluginIf::needsSimulation(int oldValue, int action, const std::map<std::string, float> &curStats){
    return false;
}

/************************************Integer Plugin base class*************************************/
///It returns the list of possible parameter values as an enumeration
std::vector<int> IntegerPlugin::getValueList(){
    std::vector<int> retVal;
    bm_type::left_map::const_iterator paramEnumIter, paramEnumEnd;
    for(paramEnumIter = this->param2Enum.left.begin(),
            paramEnumEnd = this->param2Enum.left.end(); paramEnumIter != paramEnumEnd; paramEnumIter++){
        retVal.push_back(paramEnumIter->first);
    }
    return retVal;
}

///Given the enumeration representing the parameters it returns
///its real value as a string
std::string IntegerPlugin::getParameterName(int param){
    bm_type::left_map::const_iterator foundVal = this->param2Enum.left.find(param);
    if(foundVal == this->param2Enum.left.end())
        THROW_EXCEPTION("Unable to find the parameter value corresponding to " << param);
    return boost::lexical_cast<std::string>(foundVal->second);
}

///Given the parameter real value as a string it returns the enumerative value that
///represents it
int IntegerPlugin::getParameterEnum(const std::string &name){
    bm_type::right_map::const_iterator foundVal = this->param2Enum.right.find(boost::lexical_cast<unsigned int>(name));
    if(foundVal == this->param2Enum.right.end())
        THROW_EXCEPTION("Unable to find the parameter enumeration corresponding to " << name);
    return foundVal->second;
}


///Returns a vector containing the IDs of the valid actions
///for this parameter
std::vector<int> IntegerPlugin::getActionList(int parameter){
    std::vector<int> retVal;
    if(parameter != this->param2Enum.size() -1)
        retVal.push_back(0); //increment
    if(parameter != 0)
        retVal.push_back(1); //decrement
    return retVal;
}


///Given the current value of the parameter and an action
///it returns the value that would result from the application
///of that action to the parameter
int IntegerPlugin::applyAction(int oldValue, int action){
    bm_type::left_map::const_iterator foundVal;
    switch(action){
        case 0: // Number Increment
            foundVal = this->param2Enum.left.find(oldValue + 1);
            if(foundVal == this->param2Enum.left.end())
                return oldValue;
            else
                return oldValue + 1;
        break;
        case 1: // Number Decrement
            foundVal = this->param2Enum.left.find(oldValue - 1);
            if(foundVal == this->param2Enum.left.end())
                return oldValue;
            else
                return oldValue - 1;
        break;
        default:
            THROW_EXCEPTION("Unknown action: " << action);
        break;
    }
}

/************************************String Plugin base class*************************************/
///It returns the list of possible parameter values as an enumeration
std::vector<int> StringPlugin::getValueList(){
    std::vector<int> retVal;
    bm_type::left_map::const_iterator paramEnumIter, paramEnumEnd;
    for(paramEnumIter = this->param2Enum.left.begin(),
            paramEnumEnd = this->param2Enum.left.end(); paramEnumIter != paramEnumEnd; paramEnumIter++){
        retVal.push_back(paramEnumIter->first);
    }
    return retVal;
}

///Given the enumeration representing the parameters it returns
///its real value as a string
std::string StringPlugin::getParameterName(int param){
    bm_type::left_map::const_iterator foundVal = this->param2Enum.left.find(param);
    if(foundVal == this->param2Enum.left.end())
        THROW_EXCEPTION("Unable to find the parameter value corresponding to " << param);
    return boost::lexical_cast<std::string>(foundVal->second);
}

///Given the parameter real value as a string it returns the enumerative value that
///represents it
int StringPlugin::getParameterEnum(const std::string &name){
    bm_type::right_map::const_iterator foundVal = this->param2Enum.right.find(name);
    if(foundVal == this->param2Enum.right.end())
        THROW_EXCEPTION("Unable to find the parameter enumeration corresponding to " << name);
    return foundVal->second;
}


///Returns a vector containing the IDs of the valid actions
///for this parameter
std::vector<int> StringPlugin::getActionList(int parameter){
    std::vector<int> retVal;
    if(parameter != this->param2Enum.size() -1)
        retVal.push_back(0); //increment
    if(parameter != 0)
        retVal.push_back(1); //decrement
    return retVal;
}


///Given the current value of the parameter and an action
///it returns the value that would result from the application
///of that action to the parameter
int StringPlugin::applyAction(int oldValue, int action){
    bm_type::left_map::const_iterator foundVal;
    switch(action){
        case 0: // Number Increment
            foundVal = this->param2Enum.left.find(oldValue + 1);
            if(foundVal == this->param2Enum.left.end())
                return oldValue;
            else
                return oldValue + 1;
        break;
        case 1: // Number Decrement
            foundVal = this->param2Enum.left.find(oldValue - 1);
            if(foundVal == this->param2Enum.left.end())
                return oldValue;
            else
                return oldValue - 1;
        break;
        default:
            THROW_EXCEPTION("Unknown action: " << action);
        break;
    }
}
