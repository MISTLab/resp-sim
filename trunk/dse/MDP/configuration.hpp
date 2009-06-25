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

#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <iostream>

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

class Configuration{
  private:
    static std::string& TrimSpaces(std::string& str);
    static std::string& ToLower(std::string& str);
    bool ParseReSP(std::ifstream &stream);
    bool ParseObjectives(std::ifstream &stream, std::string & reminder);
    bool ParseParams(std::ifstream &stream, std::string & reminder);
  public:
    // Here I define the the parameters of the problem; they are loaded
    // at startup by the parameter file

    // The file containing the architecture to be simulated
    std::string archFile;
    // The path to the ReSP startSim script
    std::string respPath;
    // The application that will be simulated
    std::string application;
    // folder where the plugins for the parameters are saved
    std::string pluginFolder;
    // The port on which to start ReSP
    unsigned int respPort;
    // Specifies whether ReSP has to be executed in graphical mode or
    // in a normal terminal
    bool graphic;
    // The parameters and their values which have to be explored
    std::map<std::string, std::vector<std::string> > parameters;
    // The objective function parameters
    std::map<std::string, std::vector<std::string> > objFunParams;
    // The exploration horizon, the maximum depth of the exploration graph
    unsigned int horizon;
    // The number of restart runs which are performed before stopping the algorithm
    unsigned int numRuns;
    // Specifies the number of virtual sample that are added to each newly
    // discovered probability function in order to learn it
    unsigned int virtualSample;
    // Specifies the accuracy at which the splitting is performed
    double lambda;
    // Specifies the discount factor to be applied during the Value Iterator algorithm
    double gamma;
    // Specified the minimum reward
    double eps;
    // Maxmium number of partitions generated of a metric
    unsigned int maxBranchWidth;
    // The name of the probes holding the objectives
    std::vector<std::string> objectives;
    // Random number generator to be used during algorithm execution
    boost::minstd_rand generator;
    // Specifies the verbosity level of the algorithm
    int verbosity;
    // Specifies whether verbose execution is activated or not
    bool verbose;
    // Specifies whether the connection with ReSP is to be made permanent or
    // if a new connection has to be started for each simulation
    bool permanent;
    // The number of simulations that were performed for the solution of this problem
    unsigned int simulations_num;
    // The maximum number of vertices which can be present in the exploration graph
    unsigned int maxvertices;
    // Loads the cnfiguration from file
    bool load(const std::string &fileName);
    Configuration();
};

extern Configuration config;

#endif
