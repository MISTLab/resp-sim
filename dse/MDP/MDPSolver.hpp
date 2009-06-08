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

#ifndef MDPSOLVER_HPP
#define MDPSOLVER_HPP

#include <map>
#include <vector>
#include <string>
#include <set>

#include <boost/pool/pool_alloc.hpp>

#include "simulationCache.hpp"
#include "Graph.hpp"
#include "ProbFunction.hpp"
#include "systemConfig.hpp"
#include "ObjectiveFun.hpp"

class PluginIf;
class RespClient;

class UncertaintyExplosion: public std::runtime_error{
    public:
    UncertaintyExplosion() : std::runtime_error(""){}
    UncertaintyExplosion(const char * message) : std::runtime_error(message){}
};

/// Use boost::pool_alloc for maps
//typedef std::map<vertex_t, std::pair<std::string, int>, std::less<vertex_t> , boost::pool_allocator<std::pair<vertex_t const, std::pair<std::string,int> > > > strategy_map;
typedef std::map<vertex_t, std::pair<PluginIf *, int> > strategy_map;

///This class drives a single run of the algorithm (i.e. a run for a single
///scalarizing function). When the scalarizing funciton changes, a new
///instance of the class is created and the exploration restarted.
///Note how the non-dominated set is a static member and, as such,
///it survives class creation and destruction
class MDPSolver{
  protected:
    /// The current alpha in use
    static double current_alpha;

    ///The number of printed graphs
    unsigned int numGraphs;
    /// Map passing from statistics names to integer identifiers
    static std::map<std::string, int> global_stats;
    static std::map<int, std::string> reverse_global_stats;

    ///Exploration graph, on which the strategy is computed
    Graph *explorationGraph;
    ///The first node of the exporation graph
    vertex_t rootNode;
    ///The list of the eliminated probability Graphs
    std::set<ProbGraph *> eliminatedProbGraphs;
    ///Strategy: it is a map state, action. The state is simply
    ///a vertex of the exploration graph and the action is a
    ///pair parameter, enumerative representing the action on it
    strategy_map strategy;
    ///Instance of the objective function, used to evaluate the
    ///quality of solution
    ObjectiveFunction *objFun;

    ///Creation of the initial graph; during the creation, the
    ///virtual samples for each node are also computed
    void createExplorationGraph(SystemConfig * initialSol, std::multimap<PluginIf*, int> initForbiddenActions);
    ///Given the graph, it computes the strategy, obtaining,
    ///for each node, the actions that have to be performed
    void computeStrategy();
    ///Given the stragey, it applies it to the system; note how
    ///uncertainties might need simulation restart in case
    ///we end up in a non-expected state. DISCUSS THIS WITH GIO
    ///At the end of the application, if we reached convergence,
    ///we simply clear the forbidden actions of the last optimal
    ///configuration and restart the strategy application
    ///If we reached the horizon, we perform a simulation to
    ///determine where we are and then we restart the strategy application
    ///in case the algorithm end in a unknown state, otherwise we
    ///resume the application. Note, anyway, that we save the
    ///current simulated point so that it can be reused in
    ///later algorithm runs
    ///convergence parameter specifies whether we stopped because convergence
    ///was reached or because all the k-actions were executed; note that
    ///in case of multiple paths I consider I reached convergence if at
    ///least one path reached convergence.
    ///Note that I also return the forbidden actions for each system configuration
    std::pair<SystemConfig, std::multimap<PluginIf*, int> > applyStrategy(vertex_t initialNode, bool &convergence);
    ///Updates the set of non-dominated solutions;
    ///if the current solution is dominated by another in
    ///the set it is not added to the set. I call this
    ///method after every simulation
    static void updateNonDominated(SystemConfig &curSol);
    ///Given a point (determined by the name of each parameter and its
    ///corresponding value) it performs a simulation run and
    ///it returns the computed metric values
    static void simulatePoint(SystemConfig &curSol);
    static SystemConfig simulatePoint(const std::map<PluginIf*, int> &point);
    ///Given a metric and an interval of values in that metric's space, it partitions
    ///the space according to the accuracy lambda.
    static std::list<std::pair<float, float> > partitionMetrics(const std::string &metric,
                                                        const std::pair<float, float> &metricVal);
    void clearExplorationGraph();
    void printStrategy();
    static std::map<std::string, float> getMetricCentroid(const std::map<std::string, std::pair<float, float> > &approxMetric);
    static std::map<std::string, float> getMinMetric(const std::map<std::string, std::pair<float, float> > &approxMetric);
    SystemConfig getSystemConfig(vertex_t node);

  public:
    // Simulation cache
    static SimulationCache *simulationCache;

    // ReSP client connector
    static RespClient* client;

    ///Contains all the non-dominated configurations (i.e. the
    ///configurations on the pareto curve) found so far
    static std::vector<SystemConfig> nonDominated;
    ///Vector holding the simulated configurations, which can be used
    ///in later runs to avoid re-executing already performed
    ///simulations or to improve the precision
    static std::vector<SystemConfig> simulatedConfigs;
    ///Number of simulations so far executed
    static unsigned int numSimulations;
    static unsigned int numIIKindUncertainty;
    static unsigned int numIKindUncertainty;
    static unsigned int numExplosions;

    /// Starts the complete MDP exploration algorithm.
    void run();
    /// Constructor, it initializes to default values all the variables
    MDPSolver();
    /// Destructor: performs a cleanup of all the temporarily create variables, graphs, etc.
    ~MDPSolver();
};

#endif
