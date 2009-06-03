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

#include "cache.hpp"

class CacheWritePolicyIf : public StringPlugin{
  protected:
    ECacti model;
  public:
    CacheWritePolicyIf(const std::vector<std::string> &values, std::string pluginName);

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

    ///Returns a vector containing the IDs of the valid actions
    ///for this parameter
    std::vector<int> getActionList(int parameter);
};

class CacheWritePolicyCreator : public PluginCreator{
  public:
    CacheWritePolicyCreator();
    ~CacheWritePolicyCreator();
    ///Creates an instance of the plugin; the valid configurations
    ///for the parameter represented by the plugin are passed
    ///to this method
    PluginIf *create(const std::vector<std::string> &values, std::string pluginName);
    ///Destroys an instance of the plugin
    void destroy(PluginIf * toDestroy);
};

#endif
