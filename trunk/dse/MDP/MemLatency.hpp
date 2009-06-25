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

#ifndef PROCNUM_HPP
#define PROCNUM_HPP

/// This file contains the class representing the parameters number of
/// processors.

#include "pluginIf.hpp"

///This parameter uses as statistic the number of accesses made to memory. Given
///this the only action that can be performed is an increment/decrement
///of the memory latency according to the possible latencies specified at configuration
///time in the configurtion file.
///This means:
///The configuration file has latencies 1,5,10,20
///action increment moves from 1 to 5, from 5 to 10 etc.
///action decrement moves from 10 to 5, 5 to 1 etc.
class MemLatencyIf : public IntegerPlugin{
  public:
    MemLatencyIf(const std::vector<std::string> &values, std::string pluginName);

    ///Given a metric and the new parameter value, we obtain an
    ///estimation of the metric change from the old value to the new
    ///one
    std::pair<float, float> changeValue(std::map<PluginIf*, int> &parameters, int newValue, const std::string &metric,
                            const std::map<std::string, float> &centroidMap, const std::map<std::string, float> &statistics,
                                                                    const std::map<PluginIf *, std::string> &parameter_values);

    ///It computes the new value of the metrics according to
    ///the specified value and returns it
    void updateStatistics(std::map<std::string, float> &curStats, int oldValue, int action, const std::map<PluginIf *, std::string> &parameter_values);

    ///Using the current instance of ReSPClient, it queries ReSP for the new
    ///values of the metrics (i.e. CPI, frequency etc. for a processor) and returns
    ///this value
    void getStats(RespClient &client, std::map<std::string, float> &toUpdateStats);

    ///Given the enumeration representing the parameter value of this plugin (i.e. the memory
    ///latency) it returns the corresponding memory latency
    unsigned int getMemoryLatency(int latencyEnum);
};

class MemLatencyCreator : public PluginCreator{
  public:
    MemLatencyCreator();
    ~MemLatencyCreator();
    ///Creates an instance of the plugin; the valid configurations
    ///for the parameter represented by the plugin are passed
    ///to this method
    PluginIf *create(const std::vector<std::string> &values, std::string pluginName);
    ///Destroys an instance of the plugin
    void destroy(PluginIf * toDestroy);
};

#endif
