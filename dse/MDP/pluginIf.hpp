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

#ifndef PLUGINIF_HPP
#define PLUGINIF_HPP

/// This file contains the base class for each plugin describing how
/// each parameter is affected by the transformations. The name
/// with which the plugin registers in the plugin_handler map must
/// match the names contained in section [Parameters] of the MDP configuration file
///
/// In addition to extending the interface described in this file,
/// each plugin must declare the the plugin_handler map,
/// to create and destroy instances of the plugin.

#include <map>
#include <string>
#include <set>
#include <vector>
#include <boost/bimap.hpp>
#include "RespClient.hpp"

///Basic interface for the plugins; each plugin must
///provide the methods of this interface; in case one plugin
///does not support a method it anyway has to subclass it
///and then throw an exception
class PluginIf{

  protected:
    std::string pluginName;

  public:
    PluginIf(std::string pluginName) : pluginName(pluginName){}
    virtual ~PluginIf(){}
    ///Given a metric and the new parameter value, we obtain an
    ///estimation of the metric change from the old value to the new
    ///one
    virtual std::pair<float, float> changeValue(std::map<PluginIf*, int> &parameters, int newValue, const std::string &metric,
                const std::map<std::string, float> &centroidMap, const std::map<std::string, float> &statistics,
                const std::map<PluginIf*, std::string> &parameter_values) = 0;

    ///Given the current value of the parameter and the action which has to be
    ///applied to it, it returns the interval value of the corresponding metric.
    ///This method has more or less the same functionality of the previous one
    virtual std::pair<float, float> applyAction(std::map<PluginIf*, int> &parameters, int action, const std::string &metric,
            const std::map<std::string, float> &centroidMap, const std::map<std::string, float> &statistics,
            const std::map<PluginIf*, std::string> &parameter_values);

    ///It returns the list of possible parameter values as an enumeration
    virtual std::vector<int> getValueList() = 0;

    ///Given the enumeration representing the parameters it returns
    ///its real value as a string
    virtual std::string getParameterName(int param) = 0;

    ///Given the parameter real value as a string it returns the enumerative value that
    ///represents it
    virtual int getParameterEnum(const std::string &name) = 0;

    ///Returns a vector containing the IDs of the valid actions
    ///for this parameter
    virtual std::vector<int> getActionList(int parameter) = 0;

    ///Given an action, it returns the dual action (i.e. that action such
    ///that, if summed with curAction, it would equal a null action) if
    ///it exists, an exception in case there is none.
    virtual int getDualAction(int curAction);

    ///Given the current value of the parameter and an action
    ///it returns the value that would result from the application
    ///of that action to the parameter
    virtual int applyAction(int oldValue, int action) = 0;

    ///It computes the new value of the metrics according to
    ///the specified value and returns it
    virtual void updateStatistics(std::map<std::string, float> &curStats, int oldValue, int action, const std::map<PluginIf*, std::string> &parameter_values) = 0;

    ///Using the current instance of ReSPClient, it queries ReSP for the new
    ///values of the metrics (i.e. CPI, frequency etc. for a processor) and returns
    ///this value
    virtual void getStats(RespClient &client, std::map<std::string, float> &toUpdateStats) = 0;

    ///Specifies whether it is possible to estimate the effects of the current action or if simulation
    ///is needed
    virtual bool needsSimulation(int oldValue, int action, const std::map<std::string, float> &curStats);

    ///Function used to sort two strings in numerical order
    static bool numericSorter(const std::string &a, const std::string &b);
};

///Basic interface for the plugin manager; each plugin type must
///have its own manager, derived from this class
class PluginCreator{
  protected:
    std::set<PluginIf *> instances;
  public:
    ///Base destructor for the plugin creator; it destroys
    ///all the not yet destroyed plugin instances
    virtual ~PluginCreator(){
        std::set<PluginIf *> instancesTemp = this->instances;
        std::set<PluginIf *>::iterator instIter, instEnd;
        for(instIter = instancesTemp.begin(), instEnd = instancesTemp.end(); instIter != instEnd; instIter++){
            this->destroy(*instIter);
        }
    }
    ///Creates an instance of the plugin; the valid configurations
    ///for the parameter represented by the plugin are passed
    ///to this method.
    virtual PluginIf *create(const std::vector<std::string> &values, std::string pluginName) = 0;
    ///Destroys an instance of the plugin
    virtual void destroy(PluginIf * toDestroy){
        this->instances.erase(toDestroy);
        delete toDestroy;
    }
};

class IntegerPlugin : public PluginIf{
  protected:
    ///double ended map linking parameter values with the corresponding enumeration
    typedef boost::bimap< int, unsigned int > bm_type;
    bm_type param2Enum;

  public:
    IntegerPlugin(std::string pluginName) : PluginIf(pluginName){}
    virtual ~IntegerPlugin(){}

    ///It returns the list of possible parameter values as an enumeration
    std::vector<int> getValueList();
    ///Given the enumeration representing the parameters it returns
    ///its real value as a string
    std::string getParameterName(int param);
    ///Given the parameter real value as a string it returns the enumerative value that
    ///represents it
    int getParameterEnum(const std::string &name);
    ///Returns a vector containing the IDs of the valid actions
    ///for this parameter
    std::vector<int> getActionList(int parameter);
    ///Given the current value of the parameter and an action
    ///it returns the value that would result from the application
    ///of that action to the parameter
    int applyAction(int oldValue, int action);
};

class StringPlugin : public PluginIf{
  protected:
    ///double ended map linking parameter values with the corresponding enumeration
    typedef boost::bimap< int, std::string > bm_type;
    bm_type param2Enum;

  public:
    StringPlugin(std::string pluginName) : PluginIf(pluginName){}
    virtual ~StringPlugin(){}

    ///It returns the list of possible parameter values as an enumeration
    std::vector<int> getValueList();
    ///Given the enumeration representing the parameters it returns
    ///its real value as a string
    std::string getParameterName(int param);
    ///Given the parameter real value as a string it returns the enumerative value that
    ///represents it
    int getParameterEnum(const std::string &name);
    ///Returns a vector containing the IDs of the valid actions
    ///for this parameter
    std::vector<int> getActionList(int parameter);
    ///Given the current value of the parameter and an action
    ///it returns the value that would result from the application
    ///of that action to the parameter
    int applyAction(int oldValue, int action);
};

///Map used by the application for the management of plugins: given the
///plugin name, it manages their creation, deletion, etc.
extern std::map<std::string, PluginCreator* > plugin_creator_handler;
///Maps used to hold the plugins instances and manage them
extern std::map<std::string, PluginIf* > plugin_handler;
extern std::map<PluginIf*, std::string > rev_plugin_handler;

#endif
