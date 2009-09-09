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
#include <vector>
#include <string>
#include <list>
#include <set>
#include <deque>

#include <iostream>
#include <fstream>

#include <boost/graph/graphviz.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/graph/topological_sort.hpp>

#include <exception>
#include <stdexcept>

#include "RespClient.hpp"

#include "simulationCache.hpp"
#include "systemConfig.hpp"
#include "configuration.hpp"
#include "pluginIf.hpp"
#include "MDPSolver.hpp"
#include "ProbFunction.hpp"
#include "ObjectiveFun.hpp"

#include "map_defs.hpp"
#include "utils.hpp"

unsigned int MDPSolver::numSimulations = 0;
unsigned int MDPSolver::numIIKindUncertainty = 0;
unsigned int MDPSolver::numIKindUncertainty = 0;
unsigned int MDPSolver::numExplosions = 0;
std::vector<SystemConfig> MDPSolver::simulatedConfigs;
std::vector<SystemConfig> MDPSolver::nonDominated;
double MDPSolver::current_alpha;
SimulationCache *MDPSolver::simulationCache = NULL;

std::map<std::string, int> MDPSolver::global_stats;
std::map<int, std::string> MDPSolver::reverse_global_stats;

RespClient* MDPSolver::client = NULL;

///Creation of the initial graph; during the creation, the
///virtual samples for each node are also computed.
///In case an initial solution is provided, the creation of the
///graph starts from there. Otherwise we randomly select a point,
///simulate it and start the creation of the graph
void MDPSolver::createExplorationGraph(SystemConfig * initialSol, std::multimap<PluginIf*, int> initForbiddenActions){
    // This map keeps track of the probability graphs currently in use in the system: when
    // the count reaches 0 it means that the associated graph can be eliminated from the system
    //std::map<ProbGraph *, int, std::less<ProbGraph *>, boost::pool_allocator<std::pair<ProbGraph * const, int> > > refCountMap;
    std::map<ProbGraph *, int> refCountMap;
    unsigned int eliminatedGraphs = 0;

    //int usedMem = 0;

    if(initialSol == NULL){
        //Lets create a random solution; I simply have to go
        //over the different parameters and choose one of them
        //randomly
        //std::cerr << "simulating inital empty solution" << std::endl;
        initialSol = new SystemConfig();
        std::map<std::string, std::vector<std::string> >::iterator paramsIter, paramsEnd;
        for(paramsIter = config.parameters.begin(), paramsEnd = config.parameters.end(); paramsIter != paramsEnd; paramsIter++){
            //std::cerr << " plugin has " << paramsIter->second.size() << " parameters" << std::endl;
            boost::uniform_int<> degen_dist(0, paramsIter->second.size() - 1);
            boost::variate_generator<boost::minstd_rand&, boost::uniform_int<> > deg(config.generator, degen_dist);
            std::string choseParamValue = paramsIter->second[deg()];
            initialSol->param2Value[paramsIter->first] = plugin_handler[paramsIter->first]->getParameterEnum(choseParamValue);
            //std::cerr << "chosen parameter " << choseParamValue << "(" << initialSol->param2Value[paramsIter->first] << ") for plugin " << paramsIter->first << std::endl;
        }
//         //std::cerr << "Simulating initial configuration" << std::endl;
/*        initialSol->param2Value["bus_latencyNS"] = plugin_handler["bus_latencyNS"]->getParameterEnum("10");
        initialSol->param2Value["i_cache_size"] = plugin_handler["i_cache_size"]->getParameterEnum("32768");
        initialSol->param2Value["icacheSubstsPolicy"] = plugin_handler["icacheSubstsPolicy"]->getParameterEnum("UniversalCache32.UniversalCache32.RANDOM");
        initialSol->param2Value["memLatencyNS"] = plugin_handler["memLatencyNS"]->getParameterEnum("10");
        initialSol->param2Value["procFreqMHz"] = plugin_handler["procFreqMHz"]->getParameterEnum("500");
        initialSol->param2Value["procNum"] = plugin_handler["procNum"]->getParameterEnum("8");*/
        MDPSolver::simulatePoint(*initialSol);
    }
    //std::cerr << "simulated" << std::endl;
    // Ok, now we have the first vertex, we can procede with the creation of the graph;
    // for this we procede breadth first: we apply all the possible actions and create
    // all the child nodes; then, for each child note we repeat the same. This until
    // we have reached the event horizon. Note that, as we proceede with the exploration,
    // the number of possible actions reduces, since we are adding the dual actions
    // of the performed ones to the list of the forbidden actions. In this step we
    // also compute the transition functions by modifying the density functions
    // associated to each node and, consequently, the probability associated
    // to each edge.
    unsigned int probGraphsNum = 0;
    std::vector<vertex_t> leafVert;
    this->explorationGraph = new Graph();
    VertexInfo * initVert = new VertexInfo();
    //Lets initialize the properties of the first vertex
    std::map<std::string, int>::iterator initParamsIter, initParamsEnd;
    for(initParamsIter = initialSol->param2Value.begin(), initParamsEnd = initialSol->param2Value.end(); initParamsIter != initParamsEnd; initParamsIter++){
        initVert->parameters[plugin_handler[initParamsIter->first]] = initParamsIter->second;
    }
    std::map<std::string, double>::iterator metricIter, metricEnd;
    for(metricIter = initialSol->metric2Value.begin(), metricEnd = initialSol->metric2Value.end(); metricIter != metricEnd; metricIter++){
        //std::cerr << "setting initial value " << metricIter->second << " for metric " << metricIter->first << std::endl;
        initVert->metrics[metricIter->first] = std::pair<float, float>((float)metricIter->second, (float)metricIter->second);
    }
    initVert->curStats = initialSol->param2Stats;
    initVert->forbiddenActions = initForbiddenActions;
    //std::cerr << "there are " << initVert->forbiddenActions.size() << " forbidden actions" << std::endl;
    this->rootNode = Exploration::addGraphVertex(initVert, *this->explorationGraph);
    leafVert.push_back(this->rootNode);

    // Tree depth counter
    unsigned int curDepth = 0;

    // Node number counter
    unsigned int counter = 1;

    //std::cerr << "entering in the main loop. horizon=" << config.horizon << " size of vertices=" << leafVert.size() << std::endl;
    // If the horizon has been reached or there are too many nodes, quit
    while(curDepth < config.horizon && leafVert.size() > 0 && counter < config.maxvertices ){
        curDepth++;
        //std::cerr << "entered in the main loop" << std::endl;

        std::cerr << "Level " << curDepth << " nodes " << counter << std::endl; // << " Memory: " << std::dec << mallinfo().arena/1024


        //For each leaft vertex I apply all possible actions of each plugin and compute the corresponding transition functions
        std::vector<vertex_t> leafTemp;
        std::vector<vertex_t>::iterator leafIter, leafEnd;
        for(leafIter = leafVert.begin(), leafEnd = leafVert.end(); leafIter != leafEnd; leafIter++){
            //std::cerr << std::endl << " ** ** processing leaf ** ** " << std::endl;
            std::multimap<PluginIf*, int> &forbiddenActions = Exploration::getVertexProp(*leafIter, *this->explorationGraph).forbiddenActions;
            std::map<PluginIf*, int> &parameters = Exploration::getVertexProp(*leafIter, *this->explorationGraph).parameters;
            std::map<std::string, std::pair<float, float> > &metrics = Exploration::getVertexProp(*leafIter, *this->explorationGraph).metrics;
            density_map &proDensities = Exploration::getVertexProp(*leafIter, *this->explorationGraph).proDensities;
            std::map<int, float> &curStats = Exploration::getVertexProp(*leafIter, *this->explorationGraph).curStats;
            //std::cerr << "now there are " << forbiddenActions.size() << " forbidden actions" << std::endl;

//             if( counter % 60000 > 49000 && counter % 60000 < 50000 )
//                 cout << "Num Nodes: " << counter << endl;

            std::map<std::string, PluginIf* >::iterator pluginIter, pluginEnd;
            for(pluginIter = plugin_handler.begin(), pluginEnd = plugin_handler.end(); pluginIter != pluginEnd; pluginIter++){
                std::vector<int> actions = pluginIter->second->getActionList(parameters[pluginIter->second]);
                //std::cerr << "got " << actions.size() << " actions for plugin " << pluginIter->first << std::endl;
                std::vector<int>::iterator actionIter, actionEnd;
                for(actionIter = actions.begin(), actionEnd  = actions.end(); actionIter != actionEnd; actionIter++){
                    //std::cerr << "       ACTION  " << *actionIter << std::endl;
                    //I check that this action if not forbidden:
                    std::pair<std::multimap<PluginIf*, int>::iterator, std::multimap<PluginIf*, int>::iterator> foundAction = forbiddenActions.equal_range(pluginIter->second);
                    std::multimap<PluginIf*, int>::iterator it;
                    bool skip = false;
                    for(it = foundAction.first; it != foundAction.second; it++){
                        if(it->second == *actionIter)
                            skip = true;
                    }
                    if(skip){
                        //std::cerr << "skipping action " << *actionIter << std::endl;
                        continue;
                    }
                    //std::cerr << "executing apply action 1" << std::endl;
                    int currentParameter = pluginIter->second->applyAction(parameters[pluginIter->second], *actionIter);
//                     std::cerr << "plugin " << pluginIter->first << " old parameter value " << pluginIter->second->getParameterName(parameters[pluginIter->first]) << " new parameter value " << pluginIter->second->getParameterName(currentParameter) << std::endl;
                    std::map<std::string, float> currentStats;
                    std::map<std::string, float> curStats_plugin;
                    for( std::map<int, float>::iterator it = curStats.begin() ; it != curStats.end(); it++) {
                        curStats_plugin[this->reverse_global_stats[it->first]] = it->second;
                    }


                    //First lets actually modify all the metrics for the current action
                    std::map<std::string, std::list<std::pair<float, float> > > partitionedMetrics;
                    //First of all I check if the current plugin is not able to make estimations: in this case
                    //we resort to simulation
                    if(pluginIter->second->needsSimulation(parameters[pluginIter->second], *actionIter, curStats_plugin)){
                        // ok, a simulation is needed, I perform it
                        std::map<PluginIf*, int> newPoint;
                        std::map<PluginIf*, int>::iterator newPointIter, newPointEnd;
                        for(newPointIter = parameters.begin(), newPointEnd = parameters.end(); newPointIter != newPointEnd; newPointIter++){
                            if(newPointIter->first == pluginIter->second){
                                newPoint[newPointIter->first] = currentParameter;
                            }
                            else{
                                newPoint[newPointIter->first] = newPointIter->second;
                            }
                        }
                        std::cerr << "Simulating because plugin cannot estimate" << std::endl;
                        MDPSolver::simulatePoint(newPoint);
                    }
                    //Ok, lets see now if the current configuration has already been simulated: in case I directly used
                    //the resulting metrics, without resorting to estimation
                    bool found = false;
                    std::vector<SystemConfig>::iterator simConfigIter, simConfigEnd;
                    for(simConfigIter = MDPSolver::simulatedConfigs.begin(), simConfigEnd = MDPSolver::simulatedConfigs.end();
                                                                                    simConfigIter != simConfigEnd; simConfigIter++){
                        found = false;
                        std::map<std::string, int>::iterator param2ValIter, param2ValEnd;
                        for(param2ValIter = simConfigIter->param2Value.begin(), param2ValEnd = simConfigIter->param2Value.end();
                                                                                        param2ValIter != param2ValEnd; param2ValIter++){
                            int value = 0;
                            if(param2ValIter->first == pluginIter->first)
                                value = currentParameter;
                            else
                                value = parameters[plugin_handler[param2ValIter->first]];
                            if(param2ValIter->second != value){
                                found = false;
                                break;
                            }
                            else
                                found = true;
                        }
                        if(found){
                            //I simply get the metrics and statistics from the saved solution
                            std::vector<std::string>::iterator metricsIter, metricsEnd;
                            for(metricsIter = config.objectives.begin(), metricsEnd = config.objectives.end(); metricsIter != metricsEnd; metricsIter++){
                                partitionedMetrics[*metricsIter].push_back(std::pair<float, float>(simConfigIter->metric2Value[*metricsIter], simConfigIter->metric2Value[*metricsIter]));
                            }

                            for( std::map<int, float>::iterator it = simConfigIter->param2Stats.begin() ; it != simConfigIter->param2Stats.end(); it++) {
                                currentStats[this->reverse_global_stats[it->first]] = it->second;
                            }
                            //std::cerr << "found simulated config" << std::endl;
                            break;
                        }
                    }
                    if(!found){
                        //Not found statistic, resorting to estimation
                        //std::cerr << "not found simulated config" << std::endl;
                        std::map<std::string, float> centroidMap;
                        std::map<PluginIf*, std::string> parameter_values;
                        for( std::map<std::string, PluginIf*>::iterator it = plugin_handler.begin() ; it != plugin_handler.end() ; it++ ){
                            parameter_values[it->second] = it->second->getParameterName(parameters[it->second]);
                        }

                        std::vector<std::string>::iterator metricsIter, metricsEnd;
                        for(metricsIter = config.objectives.begin(), metricsEnd = config.objectives.end(); metricsIter != metricsEnd; metricsIter++){
                            centroidMap[*metricsIter] = (metrics[*metricsIter].first + metrics[*metricsIter].second)/2;
                        }
                        for(metricsIter = config.objectives.begin(), metricsEnd = config.objectives.end(); metricsIter != metricsEnd; metricsIter++){
                            //std::cerr << "executing apply action 2" << std::endl;
                            try{
                                partitionedMetrics[*metricsIter] = MDPSolver::partitionMetrics(*metricsIter,
                                            pluginIter->second->applyAction(parameters, *actionIter , *metricsIter,
                                            centroidMap, curStats_plugin, parameter_values));
                            }
                            catch(...){
                                // In order to make the output understandable, I simply print the path in the tree that from the root node
                                // drove us here
                                Graph * subGraph = new Graph();
                                Exploration::getSubtreePath(*this->explorationGraph, subGraph, *leafIter);
                                std::ofstream graphOutL("LocalErrorGraph.dot");
                                boost::write_graphviz(graphOutL,  *subGraph,  NodeWriter(*subGraph),  EdgeWriter(*subGraph));
                                graphOutL.close();
                                std::ofstream graphOutG("GlobalErrorGraph.dot");
                                boost::write_graphviz(graphOutG,  *this->explorationGraph,  NodeWriter(*this->explorationGraph),  EdgeWriter(*this->explorationGraph));
                                graphOutG.close();
                                throw;
                            }
                            //std::cerr << "Analyzing metric " << *metricsIter << std::endl;
                        }
                        currentStats = curStats_plugin;
                        pluginIter->second->updateStatistics(currentStats, parameters[pluginIter->second], *actionIter, parameter_values);
                    }

                    // Here I have to create the new vertices for the graph; I also have to compute their probability functions using the
                    // Probability graph.
                    // the algorithm works like this:
                    // -1-: Updted the probability graph of the father corresponding to this action using the new metrics (since it is possible that the number of states created by the action changed)
                    // -2-: using the father's probability graph create the new vertices and compute the probability for the edge connecting the father with the new nodes
                    // -3-: Copy the probability graph of the father corresponding to this action
                    // -4-: copy it in each vertex and update it adding the virtual samples
                    // -5-: update the new node's forbidden actions, etc.
                    // -6-: add the new node to the list of leaf nodes

                    // **** 1 ****//
                    // First of all I check that I have already encountered the current action; if it is the case I update the
                    // probability graph, otherwise I have to create a new one
/*                    if(usedMem < 0)
                        ::exit(0);*/
                    density_map::iterator foundProb = proDensities.find(std::pair<PluginIf *, int>(pluginIter->second, *actionIter));
                    if(foundProb == proDensities.end()){
                        //std::cerr << "creating uniform probability graph" << std::endl;
/*                        std::cerr << "Before creating new graph " << std::dec << mallinfo().arena/1024 << " difference " << (mallinfo().arena - usedMem)/1024 << std::endl;
                        usedMem = mallinfo().arena;*/
                        ProbGraph *graph = new ProbGraph();
                        refCountMap[graph] = 1;
                        Probability::createUniformGraph(partitionedMetrics, *graph);
                        proDensities[std::pair<PluginIf *, int>(pluginIter->second, *actionIter)] = graph;
                        if(config.verbose && config.verbosity > 1){
                            std::ofstream graphOut(("probGraph_" + pluginIter->first + "_" + boost::lexical_cast<std::string>(*actionIter) + "_" + boost::lexical_cast<std::string>(probGraphsNum) + ".dot").c_str());
                            boost::write_graphviz(graphOut, *graph, ProbNodeWriter(*graph), ProbEdgeWriter(*graph));
                            graphOut.close();
                            probGraphsNum++;
                        }
/*                        std::cerr << "after creating new graph " << std::dec << mallinfo().arena/1024 << " difference " << (mallinfo().arena - usedMem)/1024 << std::endl;
                        usedMem = mallinfo().arena;*/
                    }
                    else{
                        if(config.verbose && config.verbosity > 1){
                            std::ofstream graphOut(("BEFOREprobGraph_" + pluginIter->first + "_" + boost::lexical_cast<std::string>(*actionIter) + "_" + boost::lexical_cast<std::string>(probGraphsNum) + ".dot").c_str());
                            boost::write_graphviz(graphOut, *(foundProb->second), ProbNodeWriter(*(foundProb->second)), ProbEdgeWriter(*(foundProb->second)));
                            graphOut.close();
                        }
                        //std::cerr << "updating the probability graph " << probGraphsNum << " for action " << *actionIter << std::endl;
                        //std::cerr << "updating " << foundProb->second << std::endl;
/*                        std::cerr << "Beofre update " << std::dec << mallinfo().arena/1024 << " difference " << (mallinfo().arena - usedMem)/1024 << std::endl;
                        usedMem = mallinfo().arena;*/
                        Probability::updateGraph(partitionedMetrics, *(foundProb->second));
/*                        std::cerr << "After update " << std::dec << mallinfo().arena/1024 << " difference " << (mallinfo().arena - usedMem)/1024 << std::endl;
                        usedMem = mallinfo().arena;*/
                        //std::cerr << "updated the probability graph " << probGraphsNum << " for action " << *actionIter << std::endl;
                        if(config.verbose && config.verbosity > 1){
                            std::ofstream graphOut(("AFTERprobGraph_" + pluginIter->first + "_" + boost::lexical_cast<std::string>(*actionIter) + "_" + boost::lexical_cast<std::string>(probGraphsNum) + ".dot").c_str());
                            boost::write_graphviz(graphOut, *(foundProb->second), ProbNodeWriter(*(foundProb->second)), ProbEdgeWriter(*(foundProb->second)));
                            graphOut.close();
                            probGraphsNum++;
                        }
                    }

                    // **** 2,3,4,5,6 ****//
/*                    if(mallinfo().arena - usedMem > 0){
                        std::cerr << "Getting new vertices " << std::dec << mallinfo().arena/1024 << " difference " << (mallinfo().arena - usedMem)/1024 << std::endl;
                        usedMem = mallinfo().arena;
                    }*/
                    ProbGraph &curGraph = *(proDensities[std::pair<PluginIf *, int>(pluginIter->second, *actionIter)]);
                    refCountMap[proDensities[std::pair<PluginIf *, int>(pluginIter->second, *actionIter)]]--;
                    std::list<std::pair<VertexInfo *, float> > newVertices = Probability::getNewVertices(curGraph);
                    std::list<std::pair<VertexInfo *, float> >::iterator newVertIter, newVertEnd;
                    //std::cerr << "Created " << newVertices.size() << " new nodes" << std::endl;
//                     if(mallinfo().arena - usedMem > 0){
//                         std::cerr << "Staring iterating on new vertices " << std::dec << mallinfo().arena/1024 << " difference " << (mallinfo().arena - usedMem)/1024 << std::endl;
//                         usedMem = mallinfo().arena;
//                     }
                    for(newVertIter = newVertices.begin(), newVertEnd = newVertices.end(); newVertIter != newVertEnd; newVertIter++){
//                         if(mallinfo().arena - usedMem > 0){
//                             std::cerr << "Before Copy " << std::dec << mallinfo().arena/1024 << " difference " << (mallinfo().arena - usedMem)/1024 << std::endl;
//                             usedMem = mallinfo().arena;
//                         }
                        ProbGraph *graph = new ProbGraph();
                        refCountMap[graph] = 1;
                        Probability::copyGraph(curGraph, *graph);
//                         if(mallinfo().arena - usedMem > 0){
//                             std::cerr << "After Copy " << std::dec << mallinfo().arena/1024 << " difference " << (mallinfo().arena - usedMem)/1024 << std::endl;
//                             std::cerr << "Graph size " << boost::num_vertices(*graph) << endl;
//                             usedMem = mallinfo().arena;
//                         }
                        unsigned int vs = Probability::addVirtualSample(newVertIter->first->metrics, *graph);
//                         if(mallinfo().arena - usedMem > 0){
//                             std::cerr << "After vs " << std::dec << mallinfo().arena/1024 << " difference " << (mallinfo().arena - usedMem)/1024 << std::endl;
//                             usedMem = mallinfo().arena;
//                         }
//                         std::cerr << "Adding " << vs << " virtual samples to metric: ";
//                         std::map<std::string, std::pair<float, float> >::const_iterator metrTempIter, metrTempEnd;
//                         for(metrTempIter = newVertIter->first->metrics.begin(), metrTempEnd = newVertIter->first->metrics.end(); metrTempIter != metrTempEnd; metrTempIter++){
//                             std::cerr << metrTempIter->first << "(" << metrTempIter->second.first << ", " << metrTempIter->second.second << ") -- ";
//                         }
//                         std::cerr << std::endl;
                        newVertIter->first->forbiddenActions = forbiddenActions;
                        try{
                            int dualAction = pluginIter->second->getDualAction(*actionIter);
                            foundAction = forbiddenActions.equal_range(pluginIter->second);
                            skip = false;
                            for(it = foundAction.first; it != foundAction.second; it++){
                                if(it->second == dualAction)
                                    skip = true;
                            }
                            if(!skip){
                                //std::cerr << "getting new forbidden action " << pluginIter->second->getDualAction(*actionIter) << " for plugin " << pluginIter->first << std::endl;
                                newVertIter->first->forbiddenActions.insert(std::pair<PluginIf *, int>(pluginIter->second, dualAction));
                            }
                        }
                        catch(...){;}
                        //Update parameters
                        newVertIter->first->parameters = parameters;
                        newVertIter->first->parameters[pluginIter->second] = currentParameter;
                        //Update probability densities
                        // TODO: is it OK like this or I have to perform a deep copy?
                        newVertIter->first->proDensities = proDensities;
                        newVertIter->first->proDensities[std::pair<PluginIf *, int>(pluginIter->second, *actionIter)] = graph;
                        density_map::const_iterator probGraphIter, probGraphEnd;
                        for(probGraphIter = proDensities.begin(), probGraphEnd = proDensities.end(); probGraphIter != probGraphEnd; probGraphIter++){
                            if(probGraphIter->first != std::pair<PluginIf *, int>(pluginIter->second, *actionIter)){
                                refCountMap[probGraphIter->second]++;
                            }
                        }
//                         if(mallinfo().arena - usedMem > 0){
//                             std::cerr << "Before augmenting graph " << std::dec << mallinfo().arena/1024 << " difference " << (mallinfo().arena - usedMem)/1024 << std::endl;
//                             usedMem = mallinfo().arena;
//                         }
                        // Update the statistics


                        //newVertIter->first->curStats = currentStats;
                        for( std::map<std::string, float>::iterator it = currentStats.begin() ; it != currentStats.end() ; it++ ) {
                            newVertIter->first->curStats[this->global_stats[it->first]] = it->second;
                        }

                        //Here we add the newly created vertex to the graph
                        vertex_t newVert = Exploration::addGraphVertex(newVertIter->first, *this->explorationGraph);
                        //Lets start creating the edge
                        EdgeInfo * eInfo = new EdgeInfo();
                        eInfo->action.first = pluginIter->second;
                        eInfo->action.second = *actionIter;
                        //std::cerr << "Setting edge probability " << newVertIter->second << " vs " << vs << std::endl;
                        eInfo->prob = newVertIter->second;
                        eInfo->virtualSamples = vs;
                        edge_t newEdge = Exploration::addGraphEdge(eInfo, std::pair<vertex_t, vertex_t>(*leafIter, newVert), *this->explorationGraph);
                        //finally we schedule the vertex to be examined later
                        leafTemp.push_back(newVert); counter++;
//                         if(mallinfo().arena - usedMem > 0){
//                             std::cerr << "After augmenting graph " << std::dec << mallinfo().arena/1024 << " difference " << (mallinfo().arena - usedMem)/1024 << std::endl;
//                             usedMem = mallinfo().arena;
//                         }
                    }
//                     if(mallinfo().arena - usedMem > 0){
//                         std::cerr << "After Vertices creation " << std::dec << mallinfo().arena/1024 << " difference " << (mallinfo().arena - usedMem)/1024 << std::endl;
//                         usedMem = mallinfo().arena;
//                     }
                }
            }
            //Ok, now I have completed examing the current vertex, I will not come back to it anymore, so I can delete
            //the unused parts, thus freeing memory (I can erase statistics, metrics and probability densities)
            //In order to be sure I first check that the current vertex have out_edges
            if(boost::out_degree(*leafIter, *this->explorationGraph) > 0){
                forbiddenActions.clear();
                proDensities.clear();
                curStats.clear();
            }
        }
        std::vector<ProbGraph *> probGraphToErase;
        std::map<ProbGraph *, int>::iterator refCountMapIter, refCountMapEnd;
        for(refCountMapIter = refCountMap.begin(), refCountMapEnd = refCountMap.end(); refCountMapIter != refCountMapEnd; refCountMapIter++){
            if(refCountMapIter->second <= 0){
                //std::cerr << "deleting " << refCountMapIter->first << std::endl;
                Probability::clearProbGraph(refCountMapIter->first);
                this->eliminatedProbGraphs.insert(refCountMapIter->first);
                eliminatedGraphs++;
                //std::cerr << "deleted " << refCountMapIter->first << std::endl;
                probGraphToErase.push_back(refCountMapIter->first);
            }
        }
        //std::cerr << "Clearing map" << std::endl;
        std::vector<ProbGraph *>::iterator toEraseIter, toEraseEnd;
        for(toEraseIter = probGraphToErase.begin(), toEraseEnd = probGraphToErase.end(); toEraseIter != toEraseEnd; toEraseIter++){
            refCountMap.erase(*toEraseIter);
        }
        //std::cerr << "Cleared map" << std::endl;
        leafVert = leafTemp;
    }
    if(config.verbose){
        std::cout << "Exploration graph composed of " << counter << " vertices" << std::endl;
        std::cout << "Eliminated " << eliminatedGraphs << " graphs during the exploration" << std::endl << std::endl;
    }
}

///Given a metric and an interval of values in that metric's space, it partitions
///the space according to the accuracy lambda.
std::list<std::pair<float, float> > MDPSolver::partitionMetrics(const std::string &metric,
                                                        const std::pair<float, float> &metricVal){
    //Rember to sort the list on the boundaries from the smallest to the
    //biggest; I split so that all the intervals are smaller than
    //lambda*Max and that there are no more intervals than maxBranchWidth
    //std::cerr << "going to partition interval " << metricVal.first << " - " << metricVal.second << std::endl;
    if(metricVal.first > metricVal.second){
        THROW_EXCEPTION("In all the metric intervals it is expected that the first interval is smaller than the second, while for metric " << metric << " we have interval (" << metricVal.first << "," << metricVal.second << ")");
    }
    if(metricVal.first < 0 || metricVal.second < 0){
        THROW_EXCEPTION("Metric " << metric << " we have interval (" << metricVal.first << "," << metricVal.second << ") is negative!");
    }
    std::list<std::pair<float, float> > splitted;
    double startInter = metricVal.first;

    double maxInter = (abs(metricVal.second+metricVal.first)/(float) 2)*config.lambda;
    if((abs(metricVal.second) - abs(metricVal.first))/maxInter > config.maxBranchWidth){
        maxInter = (abs(metricVal.second) - abs(metricVal.first))/(double)config.maxBranchWidth;
    }
    //std::cerr << "maximum interval " << maxInter << std::endl;
    while(startInter < metricVal.second){
        double nextInter = startInter + maxInter;
        if(nextInter >= metricVal.second)
            nextInter = metricVal.second;
        splitted.push_back(std::pair<float, float>((float)startInter, (float)nextInter));
        startInter = nextInter;
    }
    if(splitted.size() == 0)
        splitted.push_back(metricVal);
    //std::cerr << "Created " << splitted.size() << " itnervals" << std::endl;
    return splitted;
}

///Given the graph, it computes the strategy, obtaining,
///for each node, the actions that have to be performed
void MDPSolver::computeStrategy(){
    this->strategy.clear();
    // the algorithm is simple: I have to iterate on
    // all the nodes and evaluate, for each of them, all
    // the possible out-edges. The fastest way to reach
    // convergence is to examine the vertices in reverse
    // topological order:
    // -1-: I set V(s) = 0 for each node s
    // -2-: order the nodes in reverse topological order
    // -3-: starting from first nodes (i.e. the leaves)
    //      * Apply all the possible actions (i.e. follow the out edges and compute
    //        the expected return for each of them) and choose the
    //        best one, the one with the highest return; update V(s) for this
    //        state with that return if the improvement with the previous
    //        return is greater that eps
    // -4-: procede until there are updates to at least one V(s)
    std::map<vertex_t, double> cumulativeReturn; // V(s)
    vertex_iterator allVertIter, allVertEnd;
    for(boost::tie(allVertIter, allVertEnd) = boost::vertices(*this->explorationGraph); allVertIter != allVertEnd; allVertIter++){
        cumulativeReturn[*allVertIter] = 0;
    }
    std::deque<vertex_t> topoVert;
    //std::cerr << "topological sorting" << std::endl;
    boost::topological_sort(*this->explorationGraph, std::front_inserter(topoVert));
    //std::cerr << "topological sorted" << std::endl;
    // Now I start the main loop
    bool keepLoop = true;
    std::deque<vertex_t>::iterator topoVertIter, topoVertEnd;
    while(keepLoop){
        //std::cerr << "there are " << topoVert.size() << " vertices in the graph" << std::endl;
        keepLoop = false;
        for(topoVertIter = topoVert.begin(), topoVertEnd = topoVert.end(); topoVertIter != topoVertEnd; topoVertIter++){
            //If I'm considering a leaf I can skip it
            //std::cerr << "examining " << Exploration::getVertexProp(*topoVertIter, *this->explorationGraph).parameters["procNum"] << std::endl;
            if(boost::out_degree(*topoVertIter, *this->explorationGraph) == 0)
                continue;
            //Now I iterate on all the out_edges to find the best action
            edge_t bestEdge;
            double expReturn = 0;
            out_edge_iterator outEIter, outEEnd;
            for(boost::tie(outEIter, outEEnd) = boost::out_edges(*topoVertIter, *this->explorationGraph); outEIter != outEEnd; outEIter++){
                double curProb = Exploration::getEdgeProp(*outEIter, *this->explorationGraph).prob;
                //std::cerr << "computed transition probability " << curProb << std::endl;
                vertex_t nextVertex = boost::target(*outEIter, *this->explorationGraph);
                std::map<std::string, float> sourceMetrics = MDPSolver::getMetricCentroid(Exploration::getVertexProp(*topoVertIter, *this->explorationGraph).metrics);
                std::map<std::string, float> destMetrics = MDPSolver::getMetricCentroid(Exploration::getVertexProp(nextVertex, *this->explorationGraph).metrics);
                double reward = this->objFun->estimate(sourceMetrics) - this->objFun->estimate(destMetrics);
                //std::cerr << "computed reward " << reward << std::endl;
                //std::cerr << "reward " << reward << " src: " << this->objFun->estimate(sourceMetrics) << " dst: "<< this->objFun->estimate(destMetrics) << std::endl;
                double curReturn = curProb*(reward + config.gamma*cumulativeReturn[nextVertex]);
                //std::cerr << "return " << curReturn << std::endl;
                if(curReturn > expReturn){
                    expReturn = curReturn;
                    bestEdge = *outEIter;
                    //std::cerr << "Setting return for vertex " << Exploration::getVertexProp(*topoVertIter, *this->explorationGraph).parameters["procNum"] << std::endl;
                }
            }
            //std::cerr << "updating the strategy" << std::endl;
            //std::cerr << "cumulative Return: " << cumulativeReturn[*topoVertIter] << " expected return " << expReturn << std::endl;
            //Now I can update the strategy
            if(expReturn > cumulativeReturn[*topoVertIter]*(1+config.eps)){
                //std::cerr << "Setting strategy for vertex " << Exploration::getVertexProp(*topoVertIter, *this->explorationGraph).parameters["procNum"] << std::endl;
                std::pair<PluginIf *, int> bestAction = Exploration::getEdgeProp(bestEdge, *this->explorationGraph).action;
                keepLoop = true;
                this->strategy[*topoVertIter] = bestAction;
                cumulativeReturn[*topoVertIter] = expReturn;
                //std::cerr << "new strategy" << std::endl;
            }
            //std::cerr << "updated the strategy" << std::endl;
        }
    }
}

///Given the stragey, it applies it to the system; note how
///uncertainties might need simulation restart in case
///we end up in a non-expected state; as explained below we
///restart everything in this case.
///At the end of the application, if we reached convergence,
///we simply clear the forbidden actions of the last optimal
///configuration and restart the strategy application
///If we reached the horizon, we perform a simulation to
///determine where we are and then we restart the strategy application
///in case the algorithm end in a unknown state, otherwise we
///resume the application. Note, anyway, that we save the
///current simulated point so that it can be reused in
///later algorithm runs
///Note that I also return the forbidden actions for each system configuration
std::pair<SystemConfig, std::multimap<PluginIf*, int> > MDPSolver::applyStrategy(vertex_t initialNode, bool &convergence){
    // Start from the initialNode and execute the corresponding
    // action, as mapped in the strategy: this means that I look
    // in all the out-edges of the node to find all the ones that are
    // tagged with the current action: if the target states are
    // mapped to the same action in the strategy ok, I continue the
    // exploration from them; otherwise I simulate and solve the
    // uncertainty.
    // If during simulation I see that none of the states satisfies
    // the result, I have to restart everything (an exception is raised)
    // When strategy application end a simulation run is executed on
    // each of the optimal found solutions. I return the result
    // of these runs
    std::vector<std::pair<SystemConfig, std::multimap<PluginIf*, int> > > retVal;
    double maxMetric = 0;
    double gain = 1;
    std::set<vertex_t> curNodes;
    curNodes.insert(initialNode);
    std::set<std::pair<PluginIf *, int> > toApplyStrategy;
    std::set<vertex_t>::iterator curNodesIter, curNodesEnd;
    for(curNodesIter = curNodes.begin(), curNodesEnd = curNodes.end(); curNodesIter != curNodesEnd; curNodesIter++){
        strategy_map::iterator foundStrategy = this->strategy.find(*curNodesIter);
        if(foundStrategy != this->strategy.end())
            toApplyStrategy.insert(foundStrategy->second);
    }
    while(toApplyStrategy.size() > 0 && gain > maxMetric*config.eps){
        gain = 0;
        std::set<vertex_t> nextNodes;
        if(toApplyStrategy.size() > 1){
            //std::cerr << "Found an uncertainty of II kind" << std::endl;
            //Uncertainty of II kind: we need to simulate and see where we are and consequently correct curNodes
            //std::cerr << "Simulating II kind uncertainty" << std::endl;
            if(config.verbose){
                std::cout << "Uncertainty of II kind: simulating" << std::endl;
            }
            MDPSolver::numIIKindUncertainty++;
            SystemConfig simulatedPoint = MDPSolver::simulatePoint(Exploration::getVertexProp(*curNodes.begin(), *this->explorationGraph).parameters);
            //I also need to check that we are in one of the states of curNodes, otherwise there has been
            //an explosion and I signal it with an exception
            bool found = false;
            vertex_t realNode;
            for(curNodesIter = curNodes.begin(), curNodesEnd = curNodes.end(); curNodesIter != curNodesEnd && !found; curNodesIter++){
                std::map<std::string, std::pair<float, float> > &metrics = Exploration::getVertexProp(*curNodesIter, *this->explorationGraph).metrics;
                std::map<std::string, std::pair<float, float> >::iterator metricIter, metricEnd;
                unsigned int numFound = 0;
                for(metricIter = metrics.begin(), metricEnd = metrics.end(); metricIter != metricEnd; metricIter++){
                    if(simulatedPoint.metric2Value[metricIter->first] >= metricIter->second.first &&
                        simulatedPoint.metric2Value[metricIter->first] <= metricIter->second.second){
                        numFound++;
                    }
                }
                if(numFound == metrics.size()){
                    found = true;
                    realNode = *curNodesIter;
                    break;
                }
            }
            if(!found){
                int curVertNum = 0;
                if(config.verbose){
                    std::cout << "Uncertainty Explosion: dumping graphs" << std::endl;
                }
                for(curNodesIter = curNodes.begin(), curNodesEnd = curNodes.end(); curNodesIter != curNodesEnd && !found; curNodesIter++){
                    //I print the local graphs of the node which caused the uncertainty explosion
                    Graph * subGraph = new Graph();
                    Exploration::getSubtreePath(*this->explorationGraph, subGraph, *curNodesIter);
                    std::ofstream graphOutL(("ExplosionGraph_" + boost::lexical_cast<std::string>(MDPSolver::numExplosions) + "_" + boost::lexical_cast<std::string>(curVertNum) + "_.dot").c_str());
                    boost::write_graphviz(graphOutL,  *subGraph,  NodeWriter(*subGraph),  EdgeWriter(*subGraph));
                    graphOutL.close();
                    delete subGraph;
                    curVertNum++;
                }
                MDPSolver::numExplosions++;
                throw UncertaintyExplosion();
            }
            curNodes.clear();
            curNodes.insert(realNode);
        }
        else if(curNodes.size() > 1){
            MDPSolver::numIKindUncertainty++;
        }

        //Now I actually apply the strategy to the nodes
        for(curNodesIter = curNodes.begin(), curNodesEnd = curNodes.end(); curNodesIter != curNodesEnd; curNodesIter++){
            //std::cout << "applying action to: ";
/*            std::map<std::string, int> parameters = Exploration::getVertexProp(*curNodesIter, *this->explorationGraph).parameters;
            std::map<std::string, int>::const_iterator parametersIter, parametersEnd;
            for(parametersIter = parameters.begin(), parametersEnd = parameters.end(); parametersIter != parametersEnd; parametersIter++){
                std::cout << parametersIter->first << " = " << parametersIter->second << ", ";
            }
            std::cout << endl;*/
            out_edge_iterator oe, oeEnd;
            for(boost::tie(oe, oeEnd) = boost::out_edges(*curNodesIter, *this->explorationGraph); oe != oeEnd; oe++){
                std::pair<PluginIf *, int> &action = Exploration::getEdgeProp(*oe, *this->explorationGraph).action;
                if(action == this->strategy[*curNodesIter]){
                    //ok, I have found one of the next states
                    vertex_t nextNode = boost::target(*oe, *this->explorationGraph);

                    //std::cout << "result of action application: ";
//                     parameters = Exploration::getVertexProp(nextNode, *this->explorationGraph).parameters;
//                     for(parametersIter = parameters.begin(), parametersEnd = parameters.end(); parametersIter != parametersEnd; parametersIter++){
//                         std::cout << parametersIter->first << " = " << parametersIter->second << ", ";
//                     }
//                     std::cout << endl;

                    //Now I compute the gain given by the execution of this action
                    std::map<std::string, float> sourceMetrics = MDPSolver::getMetricCentroid(Exploration::getVertexProp(*curNodesIter, *this->explorationGraph).metrics);
                    std::map<std::string, float> destMetrics = MDPSolver::getMetricCentroid(Exploration::getVertexProp(nextNode, *this->explorationGraph).metrics);
                    double curGain = this->objFun->estimate(sourceMetrics) - this->objFun->estimate(destMetrics);
                    //std::cout << "curGain=" << curGain << " sourceSpeed=" << sourceMetrics["execTime"] << " destSpeed=" << destMetrics["execTime"] <<  std::endl;
                    if(curGain > gain){
                        gain = curGain;
                        maxMetric = this->objFun->estimate(destMetrics);
                    }
                    nextNodes.insert(nextNode);
                }
            }
        }
        //std::cout << "gain=" << gain << " maxMetric=" << maxMetric <<  std::endl;
        curNodes = nextNodes;
        //std::cout << "obtained processors --> " << Exploration::getVertexProp(*curNodes.begin(), *this->explorationGraph).parameters["procNum"] << std::endl;
        toApplyStrategy.clear();
        for(curNodesIter = curNodes.begin(), curNodesEnd = curNodes.end(); curNodesIter != curNodesEnd; curNodesIter++){
            strategy_map::iterator foundStrategy = this->strategy.find(*curNodesIter);
            if(foundStrategy != this->strategy.end())
                toApplyStrategy.insert(foundStrategy->second);
        }
        //std::cerr << "Found " << toApplyStrategy.size() << " actions to be applied at the next run" << std::endl;
    }
    // Ok, I have a ended the exploration of the current path: lets see if I stopped because of convergence
    // or because I finished the actions
    if(gain < maxMetric*config.eps || boost::num_vertices(*this->explorationGraph) <= 1) {
        std::cout << "Converged with gain: " << gain << " wrt " << maxMetric*config.eps << std::endl;
        convergence = true;
    } else
        convergence = false;
    std::pair<SystemConfig, std::multimap<PluginIf*, int> > curSystemConfig;
    // Lets now simulate the obtained point
    //std::cout << "END obtained processors --> " << Exploration::getVertexProp(*curNodes.begin(), *this->explorationGraph).parameters["procNum"] << std::endl;
    //std::cerr << "Simulating at the end of the strategy application" << std::endl;
    if(config.verbose){
        std::cout << "Ended Strategy Application: Simulating best point obtained so far" << std::endl;
    }
    curSystemConfig.first = MDPSolver::simulatePoint(Exploration::getVertexProp(*curNodes.begin(), *this->explorationGraph).parameters);
    curSystemConfig.second = Exploration::getVertexProp(*curNodes.begin(), *this->explorationGraph).forbiddenActions;
    //std::cerr << "Completed the strategy application: correctly executed the simulation" << std::endl;
    Graph * subGraph = new Graph();
    Exploration::getSubtreePath(*this->explorationGraph, subGraph, *curNodes.begin());
    std::ofstream graphOutL(("explorationPath" + boost::lexical_cast<std::string>(this->numGraphs) + ".dot").c_str());
    boost::write_graphviz(graphOutL,  *subGraph,  NodeWriter(*subGraph),  EdgeWriter(*subGraph));
    graphOutL.close();
    edge_iterator eIter, eEnd;
    for(boost::tie(eIter, eEnd) = boost::edges(*subGraph); eIter != eEnd; eIter++){
        delete &Exploration::getEdgeProp(*eIter, *subGraph);
    }
    std::pair<vertex_iterator, vertex_iterator> vertIter;
    std::vector<vertex_t> toDelete;
    for(vertIter = boost::vertices(*subGraph); vertIter.first != vertIter.second; vertIter.first++){
        delete &Exploration::getVertexProp(*vertIter.first, *subGraph);
        toDelete.push_back(*vertIter.first);
    }
    std::vector<vertex_t>::iterator toDeleteIter, toDeleteEnd;
    for(toDeleteIter = toDelete.begin(), toDeleteEnd = toDelete.end(); toDeleteIter != toDeleteEnd; toDeleteIter++){
        boost::clear_vertex(*toDeleteIter, *this->explorationGraph);
        boost::remove_vertex(*toDeleteIter, *this->explorationGraph);
    }

    delete subGraph;

    return curSystemConfig;
}

///Updates the set of non-dominated solutions;
///if the current solution is dominated by another in
///the set it is not added to the set. I call this
///method after every simulation
void MDPSolver::updateNonDominated(SystemConfig &curSol){
    std::vector<SystemConfig>::iterator nonDomIter, nonDomEnd;
    for(nonDomIter = MDPSolver::nonDominated.begin(), nonDomEnd = MDPSolver::nonDominated.end(); nonDomIter != nonDomEnd; nonDomIter++){
        if(nonDomIter->dominates(curSol))
            return;
    }
    //Now, before adding the solution I have to check if there
    //are solutions which are dominated by the current one
    std::vector<SystemConfig> newSolutions;
    for(nonDomIter = MDPSolver::nonDominated.begin(), nonDomEnd = MDPSolver::nonDominated.end(); nonDomIter != nonDomEnd; nonDomIter++){
        if(!curSol.dominates(*nonDomIter))
            newSolutions.push_back(*nonDomIter);
        else
            nonDomIter->dominated = false;
    }
    MDPSolver::nonDominated = newSolutions;
    curSol.dominated = true;
    MDPSolver::nonDominated.push_back(curSol);
}

///Given a solution, it takes its parameter configuration,
///executes a simulation run of the corresponding configuration
///and updates the other fields of curSol with the simulation
///result
void MDPSolver::simulatePoint(SystemConfig &curSol){
    // First of all I check the the current system configuration hasn't been already
    // simulated: in this case I avoid re-simulating it
    bool found = false;
    //std::cerr << "goiung to simulate:" << std::endl;
    std::vector<SystemConfig>::iterator simConfigIter, simConfigEnd;
    for(simConfigIter = MDPSolver::simulatedConfigs.begin(), simConfigEnd = MDPSolver::simulatedConfigs.end();
                                                                    simConfigIter != simConfigEnd; simConfigIter++){
        found = false;
        std::map<std::string, int>::iterator param2ValIter, param2ValEnd;
        for(param2ValIter = simConfigIter->param2Value.begin(), param2ValEnd = simConfigIter->param2Value.end();
                                                                        param2ValIter != param2ValEnd; param2ValIter++){
            //std::cerr << param2ValIter->first << " = " << curSol.param2Value[param2ValIter->first] << " name=" << plugin_handler[param2ValIter->first]->getParameterName(curSol.param2Value[param2ValIter->first]) << std::endl;
            if(param2ValIter->second != curSol.param2Value[param2ValIter->first]){
                found = false;
                break;
            }
            else{
                found = true;
            }
        }
        if(found){
            //I simply get the metrics and statistics from the saved solution
            curSol.metric2Value = simConfigIter->metric2Value;
            curSol.param2Stats = simConfigIter->param2Stats;
            curSol.alpha = current_alpha;
            return;
        }
    }

    //std::cerr << "Actually simulating, point not found" << std::endl;
    MDPSolver::numSimulations++;

    // Here I determine if there is really the need for simulations of if the solution is already present in the simulation cache
    std::map<std::string, std::string> param2ValString;
    std::map<std::string, int>::const_iterator solIter, solEnd;
    if(MDPSolver::simulationCache != NULL){
        for(solIter = curSol.param2Value.begin(), solEnd = curSol.param2Value.end(); solIter != solEnd; solIter++){
            param2ValString.insert(std::pair<std::string, std::string>(solIter->first, plugin_handler[solIter->first]->getParameterName(solIter->second)));
        }

        std::map<std::string, float> statsMap;
        if(MDPSolver::simulationCache->find(config.objectives, param2ValString, config.application, curSol.metric2Value, statsMap)){
            // I found the solution, I just have to correctly update the statistics
            std::map<std::string, float>::iterator statsIter, statsEnd;
            for(statsIter = statsMap.begin(), statsEnd = statsMap.end(); statsIter != statsEnd; statsIter++){
                unsigned int value = hash_fun_char(statsIter->first.c_str());
                global_stats[statsIter->first] = value;
                reverse_global_stats[value] = statsIter->first;
                curSol.param2Stats.insert(std::pair<int, float>(value, statsIter->second));
            }

            if(config.verbose){
                std::cout << curSol << std::endl;
            }

            curSol.alpha = current_alpha;

            SystemConfig sysConfCopy(curSol);
            MDPSolver::simulatedConfigs.push_back(sysConfCopy);
            MDPSolver::updateNonDominated(curSol);
            return;
        }
    }

    if(!client->load_architecture(config.archFile)){
        std::string respMex = client->getResponseMessage();
        client->quit();
        THROW_EXCEPTION("Error during loading of the architectural file " << respMex);
    }
    if(config.verbose){
        std::cout << "Performing simulation -- Parameters: " << std::endl;
    }

    for(solIter = curSol.param2Value.begin(), solEnd = curSol.param2Value.end(); solIter != solEnd; solIter++){
        if(config.verbose){
            std::cout << "    " << solIter->first << " = " << plugin_handler[solIter->first]->getParameterName(solIter->second) << std::endl;
        }
        if(!client->set_variable_value(solIter->first, plugin_handler[solIter->first]->getParameterName(solIter->second))){
            std::string respMex = client->getResponseMessage();
            client->quit();
            THROW_EXCEPTION("Error during setting variable " << solIter->first << " - " << respMex);
        }
    }
    std::vector<std::string> setUpParams;
    setUpParams.push_back(config.application);
    std::string respMex;
    if(client->callMethod("setUp", respMex, setUpParams) != ""){
         respMex = client->getResponseMessage();
        //getchar();
        client->quit();
        THROW_EXCEPTION("Error during call of SetUp method with application " << config.application << " - " << respMex);
    }
    if(!client->run_simulation()){
        std::string respMex = client->getResponseMessage();
        client->quit();
        THROW_EXCEPTION("Error during simulation start - " << respMex);
    }
    client->wait_sim_end();
    //Finally I update the value of the objectives;
    //the objectives are simply variables in the global namespace
    //of which I have to read the values
    if(client->callMethod("getStats", respMex) != ""){
        respMex = client->getResponseMessage();
        client->quit();
        THROW_EXCEPTION("Error during call of getStats method - " << respMex);
    }
    if(config.verbose){
        std::cout << "Resulting metrics:" << std::endl;
    }
    for (unsigned int iobj = 0; iobj < config.objectives.size(); iobj++){
        client->get_probe_value(config.objectives[iobj], curSol.metric2Value[config.objectives[iobj]]);
        if(config.verbose){
            std::cout << "    " << config.objectives[iobj] << " = " << curSol.metric2Value[config.objectives[iobj]] << std::endl;
        }
    }
    // Finally I have to update the metrics of each plugin and save the just simulated solution
    std::map<std::string, float> realStatsTemp;
    std::map<std::string, PluginIf* >::const_iterator pluginIter, pluginEnd;
    for(pluginIter = plugin_handler.begin(), pluginEnd = plugin_handler.end(); pluginIter != pluginEnd; pluginIter++){
        //std::cerr << "Setting statistics for plugin " << pluginIter->first << std::endl;
        std::map<std::string, float> temp;
        pluginIter->second->getStats(*client, temp);
        // Put the values in curSol.param2Stats and the global statistics
        for( std::map<std::string, float>::iterator it = temp.begin() ; it != temp.end() ; it++ ) {
            unsigned int value = hash_fun_char(it->first.c_str());
            global_stats[it->first] = value;
            reverse_global_stats[value] = it->first;
            curSol.param2Stats[value] = it->second;
            realStatsTemp[it->first] = it->second;
        }
    }

    // Ok, since I am here, this is a new, never simulated before solution: I add it to the solution cache
    // and to the solution file
    if(MDPSolver::simulationCache != NULL){
        MDPSolver::simulationCache->add(curSol.metric2Value, param2ValString, realStatsTemp, config.application);
    }

    if(config.permanent){
        if(!client->reset()){
            std::string resetError;
            try{
                resetError = client->getResponseMessage();
                client->quit();
            }
            catch(...){}
            std::cerr << "Error in reseting ReSP client: " << resetError << " ... restarting ReSP" << std::endl;
            client->reconnect();
        }

        if(client->getUsedMemory() > 1024*1024*1024){
            std::cerr << "Reconnecting since ReSP was using too much memory" << std::endl;
            client->reconnect();
        }
    }
    else{
        client->reconnect();
    }

    curSol.alpha = current_alpha;

    SystemConfig sysConfCopy(curSol);
    MDPSolver::simulatedConfigs.push_back(sysConfCopy);
    MDPSolver::updateNonDominated(curSol);
}

///Given a point (determined by the name of each parameter and its
///corresponding value) it performs a simulation run and
///it returns the computed metric values
SystemConfig MDPSolver::simulatePoint(const std::map<PluginIf*, int> &point){
    //std::cerr << "simulating single point" << std::endl;
    SystemConfig curSol;
    std::map<PluginIf*, int>::const_iterator curPointIter, curPointEnd;
    for(curPointIter = point.begin(), curPointEnd = point.end(); curPointIter != curPointEnd; curPointIter++){
        curSol.param2Value[rev_plugin_handler[curPointIter->first]] = curPointIter->second;
    }
    MDPSolver::simulatePoint(curSol);
    return curSol;
}

/// Starts the complete MDP exploration algorithm.
void MDPSolver::run(){
    // The algorithm is composed of several steps:
    // -- Creation of the initial graph, containing the estimates
    //    for the actions outcome, the transition probabilities, etc.
    // -- Computation of the strategy on the graph using the value
    //    iteration algorithm; the strategy consists in a mapping
    //    state, action
    // -- Application of the strategy; during the application I
    //    might need to, in case we go nuts, to completely restart
    //    the algorithm (but this time using the info of the simulations
    //    executed so far)
    // * At the end of these steps we need to check for convergency and,
    //   eventually, restart the whole algoritm (from the computation
    //   of the exploration graph) using last nodes as the initial ones;
    //   we keep, in one case, the forbidden actions, while in the
    //   other one we do not.
    // * Done this we change the objective function and restart with
    //   everything

    //TODO -- TODO -- I start by computing all the possible combination of the parameters of the
    //objective function
    //TODO: So far I hardcoded the fact that our objective function only has parameter alpha
    std::pair<SystemConfig, std::multimap<PluginIf*, int> > bestPoint;
    for(unsigned int j = 0; j < config.objFunParams["alpha"].size(); j++){
        //std::cerr << std::endl << "   RESTARTING WITH A NEW OBJECTIVE FUNCTION" << std::endl << std::endl;
        std::map<std::string, std::string> curObjParam;
        curObjParam["alpha"] = config.objFunParams["alpha"][j];
        current_alpha = boost::lexical_cast<double>(curObjParam["alpha"]);
        this->objFun = new ObjectiveFunction(curObjParam);
        bool converged = false;
        bool restart = false;
        if(config.verbose){
            std::cout << "Analyzing objective function with parameter: " << curObjParam["alpha"] << std::endl;
            if(j > 0){
                std::cout << "Restarting from solution: " << std::endl;
                std::cout << bestPoint.first << std::endl;
            }
        }
        do{
            restart = false;
            if(j == 0)
                this->createExplorationGraph(NULL, std::multimap<PluginIf*, int>());
            else
                this->createExplorationGraph(&(bestPoint.first), std::multimap<PluginIf*, int>());
            if(config.verbose){
                std::ofstream graphOut(("explorationGraph" + boost::lexical_cast<std::string>(this->numGraphs) + ".dot").c_str());
                boost::write_graphviz(graphOut,  *this->explorationGraph,  NodeWriter(*this->explorationGraph),  EdgeWriter(*this->explorationGraph));
                graphOut.close();
                this->numGraphs++;
            }
            //std::cerr << std::endl << std::endl << "computing strategy" << std::endl;
            this->computeStrategy();
            if(config.verbose && config.verbosity > 0){
                // Here I print the computed strategy in the form of a graph
                this->printStrategy();
            }
            //std::cerr << "computed strategy" << std::endl;
            converged = false;
            try{
                //std::cerr << std::endl << std::endl << "applying the strategy" << std::endl;
                bestPoint = this->applyStrategy(this->rootNode, converged);
                //std::cerr << "applied the strategy" << std::endl;
                restart = false;
            }
            catch(UncertaintyExplosion &etc){
                //There has been an uncertainty of the second kind and none of the estimated
                //boundaries were correct: I have to completely restart from scratch to I free
                //the create graph (but I keep the simulated points)
                //std::cerr << "Error in the strategy application, I need to continue" << std::endl;
                this->clearExplorationGraph();
                restart = true;
                continue;
            }
            unsigned int numRuns = 0;
            while(!restart && numRuns < config.numRuns && !converged){
                //Ok, now, if convergence was not reached (i.e. we stopped because the horizon
                // was hit) resume from where we stopped
                //I also check that the best point found so far is different from the root node of
                //the previous graph: in case it is not, it is useless to repeat the algorithm, since
                //I would obtain exactly the same result
                if(bestPoint.first == MDPSolver::getSystemConfig(this->rootNode)){
                    if(config.verbose) {
                        std::cout << "No restart performed since the best point was the root of the exploration graph" << std::endl;
                    }
                    converged = false;
                    break;
                }
                this->clearExplorationGraph();
                if(config.verbose)
                    std::cout << "Optimality restart: the horizon was reached" << std::endl;
                this->createExplorationGraph(&(bestPoint.first), bestPoint.second);
                if(config.verbose){
                    std::ofstream graphOut(("explorationGraph" + boost::lexical_cast<std::string>(this->numGraphs) + ".dot").c_str());
                    boost::write_graphviz(graphOut,  *this->explorationGraph,  NodeWriter(*this->explorationGraph),  EdgeWriter(*this->explorationGraph));
                    graphOut.close();
                    this->numGraphs++;
                }
                this->computeStrategy();
                if(config.verbose && config.verbosity > 0){
                    // Here I print the computed strategy in the form of a graph
                    this->printStrategy();
                }
                converged = false;
                try{
                    //std::cerr << "Applying last computed strategy" << std::endl;
                    bestPoint = this->applyStrategy(this->rootNode, converged);
                    restart = false;
                }
                catch(UncertaintyExplosion &etc){
                    restart = true;
                    //I again have to clear the exploration graph and the related temporary information
                    this->clearExplorationGraph();
                    continue;
                }
                std::cout << "Run " << numRuns++ << std::endl;
            }
            if(restart)
                continue;
            //Ok, now I have to restart to make sure that we actually reached an optimal
            //configuration: I have to free the current exploration graph and restart the
            //exploration from the best points that were reached
            //std::cerr << std::endl << std::endl << "ENDED: optimality restart" << std::endl;
            this->clearExplorationGraph();
            if(converged){
                if(config.verbose)
                    std::cout << "Optimality restart: convergence was reached" << std::endl;
                this->createExplorationGraph(&(bestPoint.first), std::multimap<PluginIf*, int>());
                if(config.verbose){
                    std::ofstream graphOut(("explorationGraph" + boost::lexical_cast<std::string>(this->numGraphs) + ".dot").c_str());
                    boost::write_graphviz(graphOut,  *this->explorationGraph,  NodeWriter(*this->explorationGraph),  EdgeWriter(*this->explorationGraph));
                    graphOut.close();
                    this->numGraphs++;
                }
                this->computeStrategy();
                if(config.verbose && config.verbosity > 0){
                    // Here I print the computed strategy in the form of a graph
                    this->printStrategy();
                }
                converged = false;
                try{
                    //std::cerr << "Applying last computed strategy" << std::endl;
                    this->applyStrategy(this->rootNode, converged);
                    restart = false;
                }
                catch(UncertaintyExplosion &etc){
                    restart = true;
                    //I again have to clear the exploration graph and the related temporary information
                    this->clearExplorationGraph();
                    continue;
                }
                if(config.verbose){
                    if(converged){
                        std::cout << "Ended restart run: convergence was reached" << std::endl;
                    }
                    else{
                        std::cout << "Ended restart run: the horizon was reached" << std::endl;
                        std::cout << "Performed " << numRuns << " runs" << std::endl;
                    }
                }
                //I clear the exploration graph and prepare for a new run
                this->clearExplorationGraph();
            }
            else{
                if(config.verbose){
                    std::cout << "No final restart performed, since convergence was not reached: no need to enable opposite actions" << std::endl;
                }
            }
        }while(restart);
        delete this->objFun;
        this->objFun = NULL;
    }
    //std::cerr << std::endl << "**** COMPLETELY ENDED THE EXPLORATION ALGORITHM ****" << std::endl;
}

///Clears the memory occupied by the exploration graph and all the related information
void MDPSolver::clearExplorationGraph(){
    if(this->explorationGraph != NULL){
        // I simply go over all the nodes of the exploration graph and delete the
        // graphs associated to the probability densities; I keep in a set the pointers
        // already deleted, this way avoiding double deletions of the same object
        //std::cerr << "deleting vertices" << std::endl;
        edge_iterator eIter, eEnd;
        for(boost::tie(eIter, eEnd) = boost::edges(*this->explorationGraph); eIter != eEnd; eIter++){
            delete &Exploration::getEdgeProp(*eIter, *this->explorationGraph);
        }
        std::set<ProbGraph *> examinedProbs = this->eliminatedProbGraphs;
        std::pair<vertex_iterator, vertex_iterator> vertIter;
        std::vector<vertex_t> toDelete;
        for(vertIter = boost::vertices(*this->explorationGraph); vertIter.first != vertIter.second; vertIter.first++){
            density_map::iterator probIter, probEnd;
            for(probIter = Exploration::getVertexProp(*vertIter.first, *this->explorationGraph).proDensities.begin(),
                    probEnd = Exploration::getVertexProp(*vertIter.first, *this->explorationGraph).proDensities.end(); probIter != probEnd; probIter++){
                if(examinedProbs.find(probIter->second) == examinedProbs.end()){
                    examinedProbs.insert(probIter->second);
                    Probability::clearProbGraph(probIter->second);
                }
            }
            toDelete.push_back(*vertIter.first);
        }
        std::vector<vertex_t>::iterator toDeleteIter, toDeleteEnd;
        for(toDeleteIter = toDelete.begin(), toDeleteEnd = toDelete.end(); toDeleteIter != toDeleteEnd; toDeleteIter++){
            delete &Exploration::getVertexProp(*toDeleteIter, *this->explorationGraph);
            boost::clear_vertex(*toDeleteIter, *this->explorationGraph);
            boost::remove_vertex(*toDeleteIter, *this->explorationGraph);
        }
        //Now it is time to delete the properties associated to the edges
        //std::cerr << "deleting edges" << std::endl;
        delete this->explorationGraph;
        this->explorationGraph = NULL;
        //Since the graph does not exist anymore, I also have to clear the computed strategy
        this->strategy.clear();
        this->eliminatedProbGraphs.clear();
    }
}

std::map<std::string, float> MDPSolver::getMetricCentroid(const std::map<std::string, std::pair<float, float> > &approxMetric){
    std::map<std::string, float> retVal;
    std::map<std::string, std::pair<float, float> >::const_iterator iter, iterEnd;
    for(iter = approxMetric.begin(), iterEnd = approxMetric.end(); iter != iterEnd; iter++){
        retVal[iter->first] = (iter->second.first + iter->second.second)/2;
    }
    return retVal;
}

std::map<std::string, float> MDPSolver::getMinMetric(const std::map<std::string, std::pair<float, float> > &approxMetric){
    std::map<std::string, float> retVal;
    std::map<std::string, std::pair<float, float> >::const_iterator iter, iterEnd;
    for(iter = approxMetric.begin(), iterEnd = approxMetric.end(); iter != iterEnd; iter++){
        retVal[iter->first] = iter->second.first ;
    }
    return retVal;
}

void MDPSolver::printStrategy(){
    strategy_map::const_iterator strategyIter, strageyEnd;
    for(strategyIter = this->strategy.begin(), strageyEnd = this->strategy.end(); strategyIter != strageyEnd; strategyIter++){
        std::map<PluginIf*, int> parameters = Exploration::getVertexProp(strategyIter->first, *this->explorationGraph).parameters;
        std::map<std::string, std::pair<float, float> > metrics = Exploration::getVertexProp(strategyIter->first, *this->explorationGraph).metrics;
        std::map<PluginIf*, int>::const_iterator parametersIter, parametersEnd;
        for(parametersIter = parameters.begin(), parametersEnd = parameters.end(); parametersIter != parametersEnd; parametersIter++){
            std::cout << rev_plugin_handler[parametersIter->first] << " = " << parametersIter->second << ", ";
        }
        std::cout << std::endl;
        std::map<std::string, std::pair<float, float> >::const_iterator metricsIter, metricsEnd;
        for(metricsIter = metrics.begin(), metricsEnd = metrics.end(); metricsIter != metricsEnd; metricsIter++){
            std::cout << metricsIter->first << " = (" << metricsIter->second.first << "," << metricsIter->second.second << ") - ";
        }
        std::cout << std::endl;
        std::cout << "Action: " << rev_plugin_handler[strategyIter->second.first] << "=" << strategyIter->second.second << std::endl << std::endl;
    }
}

SystemConfig MDPSolver::getSystemConfig(vertex_t node){
    SystemConfig curSol;
    std::map<PluginIf*, int>::const_iterator curPointIter, curPointEnd;
    for(curPointIter = Exploration::getVertexProp(node, *this->explorationGraph).parameters.begin(),
            curPointEnd = Exploration::getVertexProp(node, *this->explorationGraph).parameters.end(); curPointIter != curPointEnd; curPointIter++){
        curSol.param2Value[rev_plugin_handler[curPointIter->first]] = curPointIter->second;
    }
    std::map<std::string, std::pair<float, float> >::const_iterator metricsIter, metricsEnd;
    for(metricsIter = Exploration::getVertexProp(node, *this->explorationGraph).metrics.begin(), metricsEnd = Exploration::getVertexProp(node, *this->explorationGraph).metrics.end();
                        metricsIter != metricsEnd; metricsIter++){
        curSol.metric2Value[metricsIter->first] = (metricsIter->second.first + metricsIter->second.second)/2;
    }
    curSol.param2Stats = Exploration::getVertexProp(node, *this->explorationGraph).curStats;
    return curSol;
}

/// Constructor, it initializes to default values all the variables
MDPSolver::MDPSolver() {
//     client = new RespClient(config.respPort, "localhost", config.respPath, config.graphic);
    this->numGraphs = 0;
    this->explorationGraph = NULL;
    this->objFun = NULL;
}

/// Destructor: performs a cleanup of all the temporarily create variables, graphs, etc.
MDPSolver::~MDPSolver(){
    this->clearExplorationGraph();
    if(this->objFun != NULL)
        delete this->objFun;
}
