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

#ifndef SIMULATIONCACHE_HPP
#define SIMULATIONCACHE_HPP

#include <string>
#include <map>
#include <vector>
#include <iostream>

class SimulationCache{
    private:
        std::string cacheFileName;

        struct SystemConfig{
            std::map<std::string, double> objective2Value;
            std::map<std::string, std::string> param2Value;
            std::map<std::string, float> statistics;
            std::string application;

            void print(std::ostream & stream) const;
            void read(std::string &line);
            bool operator ==( const SystemConfig & other ) const;
            bool equal( const SystemConfig & other ) const;
            bool equal(const std::vector<std::string> &objectiveNames,
                const std::map<std::string, std::string> &param2Value, const std::string &application) const;
        };

        std::vector<SystemConfig> cacheConfigs;
    public:
        SimulationCache(std::string cacheFileName);
        ~SimulationCache();
        /// Looks in the simulation cache for the specified solution
        bool find(const std::vector<std::string> &objectiveNames, const std::map<std::string, std::string> &param2Value,
                const std::string &application, std::map<std::string, double> &objectiveValues, std::map<std::string, float> &statistics);
        /// Adds an element to the simulation cache and dumps it on the cache file
        void add(const std::map<std::string, double> &objective2Value,
                const std::map<std::string, std::string> &param2Value, const std::map<std::string, float> &statistics, const std::string &application);
};

#endif
