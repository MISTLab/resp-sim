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


/*
 *   Class representing the problem on which the MOMH library
 *   has to be applied. In our case the problem is composed by
 *   the file describing the architecture to be simulated, the
 *   parameters which have to be varied during exploration and
 *   their possible values. Finally we have a list of objectives
 *   that we wish to optimize
*/

#ifndef TDSEPROBLEM_HPP
#define TDSEPROBLEM_HPP

#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <iostream>

#include <momh/problem.h>

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

#include "simulationCache.hpp"

/** Instance of the multiple objective knapsack problem */
class TDSEProblem: public TProblem{
    /** TDSESolution is granted full access to memebers of TDSEProblem */
    friend class TDSESolution;
  private:
    bool ParseReSP(std::ifstream &stream);
    bool ParseObjectives(std::ifstream &stream);
    bool ParseParams(std::ifstream &stream, std::string & reminder);
  protected:
    // Here I define the the parameters of the problem; they are loaded
    // at startup by the parameter file

    // The file containing the architecture to be simulated
    std::string archFile;
    // Random generator for the boost library
    boost::minstd_rand generator;
  public:
    SimulationCache * simulationCache;
    // The application that will be simulated
    std::string application;
    // The parameters and their values which have to be explored
    std::map<std::string, std::vector<std::string> > parameters;
    // The name of the probes holding the objectives
    std::vector<std::string> objectives;

        // The path to the ReSP startSim script
    std::string respPath;
    // The port on which to start ReSP
    unsigned int respPort;
    // Specifies whether ReSP has to be executed in graphical mode or
    // in a normal terminal
    bool graphic;
    // Specifies whether verbose execution is activated or not
    bool verbose;
    // The number of simulations that were performed for the solution of this problem
    unsigned int simulations_num;
    // The maximum number of simulations to be performed
    unsigned int simulations_max;
    /** Loads the instance data from a file with the given name
    *
    *    Returns true if the instance is read successfully */
    bool Load(char *FileName);
    static std::string& TrimSpaces(std::string& str);
    static std::string& ToLower(std::string& str);
    TDSEProblem();
    ~TDSEProblem();
};

/** Declaration of external Problem variable (defined in TMOKPProblem.cpp)
*    makes it accessible to all modules that include this header */
extern TDSEProblem Problem;

#endif
