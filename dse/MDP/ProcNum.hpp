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

#ifndef PROCNUM_HPP
#define PROCNUM_HPP

/// This file contains the class representing the parameters number of
/// processors.

#include <map>
#include <string>
#include <set>
#include <vector>
#include "RespClient.hpp"

#include "pluginIf.hpp"

#ifdef MEMORY_DEBUG
#include <mpatrol.h>
#endif

///This parameter uses the average load as statistic.
///The only action that can be performed is an increment/decrement
///of the number of processor according to the possible numbers specified at configuration
///time in the configurtion file.
class ProcNumIf : public IntegerPlugin{
  protected:
    SimpleProcessor p_model;

  public:
    ProcNumIf(const std::vector<std::string> &values, std::string pluginName);

    ///Given a metric and the new parameter value, we obtain an
    ///estimation of the metric change from the old value to the new
    ///one
    std::pair<float, float> changeValue(plugin_int_map &parameters, int newValue, const std::string &metric,
                            const float_map &centroidMap, const float_map &statistics,
                                                                    const std::map<PluginIf *, std::string> &parameter_values);

    ///It computes the new value of the metrics according to
    ///the specified value and returns it
    void updateStatistics(float_map &curStats, int oldValue, int action, const std::map<PluginIf *, std::string> &parameter_values);

    ///Using the current instance of ReSPClient, it queries ReSP for the new
    ///values of the metrics (i.e. CPI, frequency etc. for a processor) and returns
    ///this value
    void getStats(RespClient &client, float_map &toUpdateStats);
    unsigned int getProcNum(int numberEnum);
};

class ProcNumCreator : public PluginCreator{
  public:
    ProcNumCreator();
    ~ProcNumCreator();
    ///Creates an instance of the plugin; the valid configurations
    ///for the parameter represented by the plugin are passed
    ///to this method
    PluginIf *create(const std::vector<std::string> &values, std::string pluginName);
    ///Destroys an instance of the plugin
    void destroy(PluginIf * toDestroy);
};

#endif
