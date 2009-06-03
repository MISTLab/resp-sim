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

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/config.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/graph/graphviz.hpp>

#include <list>
#include <vector>
#include <algorithm>
#include <cmath>

#include <iostream>
#include <fstream>

#include "utils.hpp"

#include "Graph.hpp"
#include "configuration.hpp"
#include "ProbFunction.hpp"

bool EdgeSorter::operator()(const prob_edge_t &a, const prob_edge_t &b){
    return Probability::getProbEdgeProp(a, this->graph).metricVal.second < Probability::getProbEdgeProp(b, this->graph).metricVal.second;
}

bool LeavesSorter::operator()(const prob_vertex_t &a, const prob_vertex_t &b){
    return this->leaf2Edge[a].first < this->leaf2Edge[b].first;
}

ProbNodeWriter::ProbNodeWriter(ProbGraph &graph) : graph(graph){
    this->nodeInfo = boost::get(prob_node_info_t(), this->graph);
}

void ProbNodeWriter::operator()(std::ostream& out, const prob_vertex_t & v) const {
    ProbVertexInfo * vert = dynamic_cast<ProbVertexInfo *>(this->nodeInfo[v]);
    out << "[label=\"";
    if(vert->metricName != ""){
        out << vert->metricName << "\\n" << vert;
    }
    else{
        out << vert->probability << "\\n" << vert->virtualSamples;
    }
    out << "\"]";
}

ProbEdgeWriter::ProbEdgeWriter(ProbGraph &graph) : graph(graph){
    this->edgeInfo = boost::get(prob_edge_info_t(), this->graph);
}

void ProbEdgeWriter::operator()(std::ostream& out, const prob_edge_t & e) const {
    ProbEdgeInfo * ed = dynamic_cast<ProbEdgeInfo *>(this->edgeInfo[e]);
    out << "[label=\"" << ed->metricVal.first << "\\n" << ed->metricVal.second << "\"]";
}

prob_vertex_t Probability::addProbGraphVertex(ProbVertexInfo &newVertex, ProbGraph &graph){
    //map associating an id to each node of the graph
    prob_vertex_index_pmap_t tgIndexMap = boost::get(boost::vertex_index_t(), graph);

    //Add the vertex
    prob_vertex_t newVertex_g = boost::add_vertex(graph);
    unsigned int &graphUniqueId = getProbGraphInfo(graph);
//     if(boost::num_vertices(graph) == 0){
//         graphUniqueId = 0;
//     }
    tgIndexMap[newVertex_g] = graphUniqueId;
    graphUniqueId++;

    //Setting its properties
    boost::property_map<ProbGraph, prob_node_info_t>::type nodeInfo = boost::get(prob_node_info_t(), graph);
    nodeInfo[newVertex_g] = &newVertex;

    return newVertex_g;
}

prob_edge_t Probability::addProbGraphEdge(ProbEdgeInfo &newEdge, std::pair<prob_vertex_t, prob_vertex_t> vertices, ProbGraph &graph){
    //First of all I have to determine the vertices which correspond to the functions which I
    //want to connect
    boost::property_map<ProbGraph, prob_node_info_t>::type nodeInfo = boost::get(prob_node_info_t(), graph);

    //Now I look for the edge: in case it does not exist I create it,  otherwise I simply update its properties
    std::pair<prob_vertex_iterator, prob_vertex_iterator> vertIter = boost::vertices(graph);
    #ifndef NDEBUG
    if(std::find(vertIter.first, vertIter.second, vertices.first) == vertIter.second)
        THROW_EXCEPTION("Unable to find vertex " << vertices.first << " in the graph");
    if(std::find(vertIter.first, vertIter.second, vertices.second) == vertIter.second)
        THROW_EXCEPTION("Unable to find vertex " << vertices.second << " in the graph");
    #endif
    std::pair<prob_edge_t, bool> foundEdge = boost::edge(vertices.first, vertices.second, graph);
    prob_edge_t curEdge = foundEdge.first;
    if(!foundEdge.second){
       //Finally I make the connection...
       prob_edge_t e;
       bool inserted;
       boost::tie(e, inserted) = boost::add_edge(vertices.first, vertices.second, &newEdge, graph);
       curEdge = e;
    }
    return curEdge;
}

/// given an input graph, it performs a deep copy in an output one
void Probability::copyGraph(ProbGraph &inGraph, ProbGraph &outGraph){
    std::map<prob_vertex_t, prob_vertex_t> old2New;
    //Lets create the new vertices
    prob_vertex_iterator allvIter, allvEnd;
    for(boost::tie(allvIter, allvEnd) = boost::vertices(inGraph); allvIter != allvEnd; allvIter++){
        ProbVertexInfo * newVertInfo = new ProbVertexInfo();
        newVertInfo->metricName = getProbVertexProp(*allvIter, inGraph).metricName;
        newVertInfo->metricValue = getProbVertexProp(*allvIter, inGraph).metricValue;
        newVertInfo->probability = getProbVertexProp(*allvIter, inGraph).probability;
        newVertInfo->virtualSamples = getProbVertexProp(*allvIter, inGraph).virtualSamples;
        old2New[*allvIter] = addProbGraphVertex(*newVertInfo, outGraph);
    }
    //Finally lets add the edges
    prob_edge_iterator alleIter, alleEnd;
    for(boost::tie(alleIter, alleEnd) = boost::edges(inGraph); alleIter != alleEnd; alleIter++){
        ProbEdgeInfo * newEdgeInfo = new ProbEdgeInfo();
        newEdgeInfo->metricVal = getProbEdgeProp(*alleIter, inGraph).metricVal;
        std::pair<prob_vertex_t, prob_vertex_t> newVertices;
        newVertices.first = old2New[boost::source(*alleIter, inGraph)];
        newVertices.second = old2New[boost::target(*alleIter, inGraph)];
        addProbGraphEdge(*newEdgeInfo, newVertices, outGraph);
    }
}

/// given the outcome of the current action, it updates the probability
/// graph to reflect the number of actions (since it is possible that, for this action,
/// the number of outcomes is not the same as it was for the father)
void Probability::updateGraph(const std::map<std::string, std::list<std::pair<float, float> > > &newMetrics, ProbGraph &graph){
    //The idea is to examine the tree in a depth first fashion: as soon as I find a
    //node (metric) with a different number of out edges (metrics) from the new ones,
    //I have to rescale the whole tree. I then restart examioning the new tree, until
    //the number of out edges coincides with the new metrics.
    //In order to rescale we the remaining leaves so that the value of all the metrics
    //is the same for all of them but for the metric whose intervals have changed: in order
    //to do this we start from the leaves and go up until we hit the changed metric. We
    //concatenate the strings of the encountered metrics, and use them as index in the
    //map. For each map I order the leaves in order to the metric values
    // then we use two cases:
    // 1) we have less intervals: we take n = oldNum/newNum. z = 1, I take z%
    //    samples from first interval and k=n-z% from the second, k=k-1% from third
    //    until k-1 is positive; then z = 1-k, and I go on for the assigment to the second new
    //    interval...
    // 2) we have more interval: we take n = oldNum/newNum; z = n, tot = 0. I take z from first, tot += z;
    //    if n -z > 0 I take n-z from second, tot = n-z. if 1 - tot < n, z = 1 - tot, else z = n. I then
    //    restart with new interval.
    // note that each time I round the number of virtual samples (they are integers).
    // I then restart the algorithm from the beginning; when there are no more changes I simply
    // update the probability values based on the new virtual samples.

//     std::cerr << "inside UPDATE Prob Graph" << std::endl;
//     std::cerr << "NEW METRICS:" << std::endl;
//     std::map<std::string, std::list<std::pair<double, double> > >::const_iterator dumpMetricIter, dumpMetricEnd;
//     for(dumpMetricIter = newMetrics.begin(), dumpMetricEnd = newMetrics.end(); dumpMetricIter != dumpMetricEnd; dumpMetricIter++){
//         std::cerr << dumpMetricIter->first << " -- " << dumpMetricIter->second.size() << " intervals" << std::endl;
//     }

/*    std::ofstream graphOut4("BeforeCoherentProbGraph.dot");
    boost::write_graphviz(graphOut4, graph, ProbNodeWriter(graph), ProbEdgeWriter(graph));
    graphOut4.close();
    Probability::checkGraphCoherency(graph);
    std::ofstream graphOut3("AfterCoherentProbGraph.dot");
    boost::write_graphviz(graphOut3, graph, ProbNodeWriter(graph), ProbEdgeWriter(graph));
    graphOut3.close();*/
    bool repeat = false;
    do{
        repeat = false;
        bool foundToScale = false;
        prob_vertex_t vertexToChange;
        bool decrease = false;
        unsigned int difference = 0;
        ToRescaleSeeker vis(newMetrics, graph, foundToScale, vertexToChange, decrease, difference);
/*        std::cerr << "going to perform depth first search" << std::endl;
        std::ofstream graphOut1("BeforeSearchedProbGraph.dot");
        boost::write_graphviz(graphOut1, graph, ProbNodeWriter(graph), ProbEdgeWriter(graph));
        graphOut1.close();*/
        boost::depth_first_search(graph, visitor(vis));
//         std::ofstream graphOut("SearchedProbGraph.dot");
//         boost::write_graphviz(graphOut, graph, ProbNodeWriter(graph), ProbEdgeWriter(graph));
//         graphOut.close();

        //std::cerr << "performed depth first search- found --> " << vis.foundToScale << std::endl;
        if(foundToScale){
            //std::cerr << "We need to perform a scaling on vertex " << getProbVertexProp(vis.vertexToChange, graph).metricName << " we have new " << newMetrics.find(getProbVertexProp(vertexToChange, graph).metricName)->second.size() << " vertices" << std::endl;
            repeat = true;
            if(decrease){
                //std::cerr << "decrese in interval num: difference " << difference << std::endl;
                //The number of intervals decreased
                std::map<std::string, std::deque<prob_vertex_t> > remainingLeaves;
                std::map<std::string, std::deque<prob_vertex_t> > eliminatedLeaves;
                std::vector<prob_vertex_t> eliminatedTreeRoot;
                std::string curMetricName = Probability::getProbVertexProp(vertexToChange, graph).metricName;
                Probability::getProbDecreseInfo(curMetricName, newMetrics.find(curMetricName)->second.size(),
                                                    remainingLeaves, eliminatedLeaves, eliminatedTreeRoot, graph);
                //Now I have to redistribute the samples
                std::map<std::string, std::deque<prob_vertex_t> >::iterator leavesIter, leavesEnd;
                //std::cerr << "decreasing, managing " << remainingLeaves.size() << " remaining functions" << std::endl;
                for(leavesIter = remainingLeaves.begin(), leavesEnd = remainingLeaves.end(); leavesIter != leavesEnd; leavesIter++){
                    //std::cerr << "managing remaining leaves" << std::endl;
                    std::deque<unsigned int> oldVs;
                    std::deque<prob_vertex_t>::iterator vertexIter, vertexEnd;
                    for(vertexIter = leavesIter->second.begin(), vertexEnd = leavesIter->second.end(); vertexIter != vertexEnd; vertexIter++){
                        oldVs.push_back(Probability::getProbVertexProp(*vertexIter, graph).virtualSamples);
                    }
                    for(vertexIter = eliminatedLeaves[leavesIter->first].begin(), vertexEnd = eliminatedLeaves[leavesIter->first].end(); vertexIter != vertexEnd; vertexIter++){
                        //std::cerr << "managing eliminated leaves" << std::endl;
                        oldVs.push_back(getProbVertexProp(*vertexIter, graph).virtualSamples);
                    }
                    //Ok, I have the old virtual samples, lets redistribute them on remainingLeaves
                    const double remFrac = (double)(leavesIter->second.size() + eliminatedLeaves[leavesIter->first].size())/(double)leavesIter->second.size(); //n
                    double curSlotFrac = 1;
                    double currentlyTaken = 0;
                    int curSlot = 0;
                    std::deque<prob_vertex_t>::iterator curLeaf = leavesIter->second.begin();
                    //std::cerr << "decrease, computing the final new probabilities, I have to take " << remFrac << " from each old node; examining metric " << leavesIter->first << std::endl;
                    while(curLeaf != leavesIter->second.end()){
                        double curAdded = 0;
                        double tempVs = 0;
                        double totalSlot = 0;
                        while(curAdded < remFrac){
                            tempVs += (double)oldVs[curSlot]*(double)curSlotFrac;
                            totalSlot += curSlotFrac;
                            if(totalSlot >= 1){
                                totalSlot = 0;
                                curSlot++;
                            }
                            curAdded += curSlotFrac;
                            if(remFrac - curAdded < 1)
                                curSlotFrac = (remFrac - curAdded);
                            else
                                curSlotFrac = 1;
                        }
                        Probability::getProbVertexProp(*curLeaf, graph).virtualSamples = (unsigned int)round(tempVs);
                        //std::cerr << "Associated " << getProbVertexProp(*curLeaf, graph).virtualSamples << " vs " << std::endl;
                        curLeaf++;
                    }
                }
                //Now I can delete the subtrees starting at eliminatedTreeRoot
                //std::cerr << "eliminating " << eliminatedTreeRoot.size() << " subtrees" << std::endl;
                std::vector<prob_vertex_t>::iterator eliminatedIter, eliminatedEnd;
                for(eliminatedIter = eliminatedTreeRoot.begin(), eliminatedEnd = eliminatedTreeRoot.end(); eliminatedIter != eliminatedEnd; eliminatedIter++){
                    Probability::clearProbSubtree(*eliminatedIter, graph);
                }
                //std::cerr << "eliminated" << std::endl;
            }
            else{
                //std::cerr << "increse in interval num: difference " << difference << std::endl;
                //The number of intervals increased
                //Now I have to create new samples
                Probability::addProbSubtrees(Probability::getProbVertexProp(vertexToChange, graph).metricName, difference, graph);
/*                std::ofstream graphOut5("AfteraddProbSubtrees.dot");
                boost::write_graphviz(graphOut5, graph, ProbNodeWriter(graph), ProbEdgeWriter(graph));
                graphOut5.close();*/
                std::map<std::string, std::deque<prob_vertex_t> > allLeaves = Probability::getOrderedLeaves(Probability::getProbVertexProp(vertexToChange, graph).metricName, graph);
                //And compute their value
                std::map<std::string, std::deque<prob_vertex_t> >::iterator leavesIter, leavesEnd;
                //std::cerr << "increase, managing " << allLeaves.size() << " total functions" << std::endl;
                for(leavesIter = allLeaves.begin(), leavesEnd = allLeaves.end(); leavesIter != leavesEnd; leavesIter++){
                    std::deque<unsigned int> oldVs;
                    unsigned int min = (unsigned int)-1;
                    std::deque<prob_vertex_t>::iterator vertexIter, vertexEnd;
                    for(vertexIter = leavesIter->second.begin(), vertexEnd = leavesIter->second.end(); vertexIter != vertexEnd; vertexIter++){
                        oldVs.push_back(Probability::getProbVertexProp(*vertexIter, graph).virtualSamples);
                        if(Probability::getProbVertexProp(*vertexIter, graph).virtualSamples < min)
                            min = Probability::getProbVertexProp(*vertexIter, graph).virtualSamples;
                    }
                    //For each element of the map I have to add vis.numNew nodes
                    std::deque<prob_vertex_t> newLeaves;
                    for(vertexIter = leavesIter->second.begin(), vertexEnd = leavesIter->second.end(); vertexIter != vertexEnd; vertexIter++){
                        newLeaves.push_back(*vertexIter);
                    }
                    //Ok, I have the old virtual samples, lets redistribute them on newLeaves
                    const double remFrac = (double)(leavesIter->second.size() - difference)/(double)(leavesIter->second.size()); //n
                    double curSlotFrac = remFrac;
                    double alreadyTaken = 0;
                    int curSlot = 0;
                    if(remFrac*min < 1){
                        //Now, first of all, I have to make sure that there are enough virtual samples not to create approximation errors
                        std::deque<unsigned int>::iterator oldVsIter, oldVsEnd;
                        for(oldVsIter = oldVs.begin(), oldVsEnd = oldVs.end(); oldVsIter != oldVsEnd; oldVsIter++){
                            *oldVsIter = (unsigned int)round(*oldVsIter*(min/remFrac));
                        }
                    }
                    std::deque<prob_vertex_t>::iterator curLeaf = newLeaves.begin();
                    //std::cerr << "increase, computing the final new probabilities, I have to take " << remFrac << " from each old node examining metric " << leavesIter->first << std::endl;
                    while(curLeaf != newLeaves.end()){
                        double tempVS = oldVs[curSlot]*curSlotFrac;
                        alreadyTaken += curSlotFrac;
                        if(alreadyTaken >= 1){
                            alreadyTaken = 0;
                            curSlot++;
                            if(curSlotFrac < remFrac){
                                tempVS += oldVs[curSlot]*(remFrac - curSlotFrac);
                            }
                        }
                        else{
                            if(curSlotFrac < remFrac){
                                if((alreadyTaken + remFrac - curSlotFrac) >= 1){
                                    double toTake = 1 - alreadyTaken;
                                    tempVS += oldVs[curSlot]*toTake;
                                    curSlot++;
                                    curSlotFrac = remFrac - curSlotFrac - toTake;
                                    tempVS += oldVs[curSlot]*curSlotFrac;
                                    alreadyTaken = curSlotFrac;
                                }
                                else{
                                    tempVS += oldVs[curSlot]*(remFrac - curSlotFrac);
                                    alreadyTaken += remFrac - curSlotFrac;
                                }
                            }
                        }
                        if(alreadyTaken + remFrac >= 1)
                            curSlotFrac = 1 - alreadyTaken;
                        else
                            curSlotFrac = remFrac;
                        Probability::getProbVertexProp(*curLeaf, graph).virtualSamples = (unsigned int)round(tempVS);
                        if(Probability::getProbVertexProp(*curLeaf, graph).virtualSamples < 1)
                            Probability::getProbVertexProp(*curLeaf, graph).virtualSamples = 1;
                        //std::cerr << "Associated " << getProbVertexProp(*curLeaf, graph).virtualSamples << " vs " << std::endl;
                        curLeaf++;
                    }
                }
            }
        }
        //std::cerr << "next loop" << std::endl;
    }while(repeat);
    //std::cerr << "Completed rescaling" << std::endl;
    //Now I have the right number of nodes; the problem is that they are still associated with
    //the old intervals; I need to go over them and associate them to the new intervals
    Probability::updateIntervalLabels(newMetrics, graph);
    //std::cerr << "Updated the intervals values" << std::endl;
    // Ok, finally I can go over the graph and update the probability on the basis of
    // the new virtual sample values
    std::vector<prob_vertex_t> totalLeaves = getProbGraphLeaves(graph);
    unsigned int totalVs = 0;
    std::vector<prob_vertex_t>::iterator leafIter, leafEnd;
    for(leafIter = totalLeaves.begin(), leafEnd = totalLeaves.end(); leafIter != leafEnd; leafIter++){
        totalVs += Probability::getProbVertexProp(*leafIter, graph).virtualSamples;
    }
    for(leafIter = totalLeaves.begin(), leafEnd = totalLeaves.end(); leafIter != leafEnd; leafIter++){
        Probability::getProbVertexProp(*leafIter, graph).probability = (float)Probability::getProbVertexProp(*leafIter, graph).virtualSamples/(float)totalVs;
        //std::cerr << "probability " << getProbVertexProp(*leafIter, graph).probability << " vs " << getProbVertexProp(*leafIter, graph).virtualSamples << std::endl;
    }
    Probability::checkGraphCoherency(graph);
    //std::cerr << "Updated the probability values" << std::endl;
}

/// given the outcome of the current action, it creates the probability
/// graph to reflect the actions; a uniform distribution is used
void Probability::createUniformGraph(const std::map<std::string, std::list<std::pair<float, float> > > &newMetrics, ProbGraph &graph){
    //I start creating the root node
    //std::cerr << "Creating uniform probability density" << std::endl;
    std::vector<prob_vertex_t> curLeaves;
    ProbVertexInfo * newVertInfo = new ProbVertexInfo();
    curLeaves.push_back(addProbGraphVertex(*newVertInfo, graph));
    std::map<std::string, std::list<std::pair<float, float> > >::const_iterator metricIter, metricEnd;
    for(metricIter = newMetrics.begin(), metricEnd = newMetrics.end(); metricIter != metricEnd; metricIter++){
        //std::cerr << "Examining metric " << metricIter->first << std::endl;
        std::vector<prob_vertex_t> newLeaves;
        //Now I have to examine all the current leaf nodes and for each of them create the successors
        std::vector<prob_vertex_t>::iterator leafIter, leafEnd;
        for(leafIter = curLeaves.begin(), leafEnd = curLeaves.end(); leafIter != leafEnd; leafIter++){
            //std::cerr << "Setting value for metric " << metricIter->first << std::endl;
            Probability::getProbVertexProp(*leafIter, graph).metricName = metricIter->first;
            Probability::getProbVertexProp(*leafIter, graph).metricValue = metricIter->second;
            std::list<std::pair<float, float> >::const_iterator interIter, interEnd;
            for(interIter = metricIter->second.begin(), interEnd = metricIter->second.end(); interIter != interEnd; interIter++){
                //std::cerr << "Creating prob node " << interIter->first << " - " << interIter->second << std::endl;
                ProbVertexInfo * newVertInfo = new ProbVertexInfo();
                prob_vertex_t newVert = Probability::addProbGraphVertex(*newVertInfo, graph);
                newLeaves.push_back(newVert);
                ProbEdgeInfo * newEdgeInfo = new ProbEdgeInfo();
                newEdgeInfo->metricVal = *interIter;
                std::pair<prob_vertex_t, prob_vertex_t> newVertices;
                newVertices.first = *leafIter;
                newVertices.second = newVert;
                Probability::addProbGraphEdge(*newEdgeInfo, newVertices, graph);
            }
        }
        curLeaves = newLeaves;
    }
    //Ok, I take the remaining leaves and associate the probabilities to them; so far I use uniform
    //probabilities since I have no other information.
    std::vector<prob_vertex_t>::iterator leafIter, leafEnd;
    for(leafIter = curLeaves.begin(), leafEnd = curLeaves.end(); leafIter != leafEnd; leafIter++){
        Probability::getProbVertexProp(*leafIter, graph).probability = (float)1/(float)curLeaves.size();
        Probability::getProbVertexProp(*leafIter, graph).virtualSamples = config.virtualSample;
    }
}

/// Given the value of the current metric, it updates the probability function
/// of the graph adding the virtual samples
unsigned int Probability::addVirtualSample(std::map<std::string, std::pair<float, float> > &currentMetric, ProbGraph &graph){
    //int usedMem = mallinfo().arena;
    //Starting from the root node I have to go down along the edges following currentMetric
    //until I get to a leaf: then I add the virtual sample to that leaf.
    //I then take all the leaves and rescale the probabability values based on the new number
    //of virtual samples
    prob_vertex_t curVertex = Probability::getProbRoot(graph);
    while(boost::out_degree(curVertex, graph) > 0){
        //I have to examine all the out edges to find the correct one
        std::string curMetricName = Probability::getProbVertexProp(curVertex, graph).metricName;
        prob_out_edge_iterator outEIter, outEEnd;
        for(boost::tie(outEIter, outEEnd) = boost::out_edges(curVertex, graph); outEIter != outEEnd; outEIter++){
            if(currentMetric[curMetricName] == Probability::getProbEdgeProp(*outEIter, graph).metricVal){
                curVertex = boost::target(*outEIter, graph);
                break;
            }
        }
    }
//     if(mallinfo().arena - usedMem > 0){
//         std::cerr << "first loop " << std::dec << mallinfo().arena/1024 << " difference " << (mallinfo().arena - usedMem)/1024 << std::endl;
//         usedMem = mallinfo().arena;
//     }

    Probability::getProbVertexProp(curVertex, graph).virtualSamples += config.virtualSample;
/*    if(mallinfo().arena - usedMem > 0){
        std::cerr << "got vs " << std::dec << mallinfo().arena/1024 << " difference " << (mallinfo().arena - usedMem)/1024 << std::endl;
        usedMem = mallinfo().arena;
    }*/
    //Now I rescale the probability on the new number of virtual samples
    std::vector<prob_vertex_t> leafVertices = getProbGraphLeaves(graph);
/*    if(mallinfo().arena - usedMem > 0){
        std::cerr << "got leaves " << std::dec << mallinfo().arena/1024 << " difference " << (mallinfo().arena - usedMem)/1024 << std::endl;
        usedMem = mallinfo().arena;
    }*/
    std::vector<prob_vertex_t>::iterator leafIter, leafEnd;
    unsigned int totVirtualSamples = 0;
    for(leafIter = leafVertices.begin(), leafEnd = leafVertices.end(); leafIter != leafEnd; leafIter++){
        totVirtualSamples += Probability::getProbVertexProp(*leafIter, graph).virtualSamples;
    }
/*    if(mallinfo().arena - usedMem > 0){
        std::cerr << "second loop " << std::dec << mallinfo().arena/1024 << " difference " << (mallinfo().arena - usedMem)/1024 << std::endl;
        usedMem = mallinfo().arena;
    }*/
    //std::cerr << "probabilities ";
    for(leafIter = leafVertices.begin(), leafEnd = leafVertices.end(); leafIter != leafEnd; leafIter++){
        Probability::getProbVertexProp(*leafIter, graph).probability = ((float)Probability::getProbVertexProp(*leafIter, graph).virtualSamples)/(float)totVirtualSamples;
        //std::cerr << getProbVertexProp(*leafIter, graph).probability << " ";
    }
/*    if(mallinfo().arena - usedMem > 0){
        std::cerr << "last loop " << std::dec << mallinfo().arena/1024 << " difference " << (mallinfo().arena - usedMem)/1024 << std::endl;
        usedMem = mallinfo().arena;
    }*/
    //std::cerr << std::endl;
    return Probability::getProbVertexProp(curVertex, graph).virtualSamples;
}

/// Given the probability graph it creates the new vertices and returns the probability which has to be associated
/// with the incoming edge. It also update the internal structure of a vertex for what concerns the
/// metric values
std::list<std::pair<VertexInfo *, float> > Probability::getNewVertices(ProbGraph &graph){
    std::vector<prob_vertex_t> leafVertices = Probability::getProbGraphLeaves(graph);
    // Now, for each leaf vertex, I go up to the root: the path corresponds to the
    // metric value for the vertex
    std::list<std::pair<VertexInfo *, float> > retVal;
    std::vector<prob_vertex_t>::iterator leafIter, leafEnd;
    for(leafIter = leafVertices.begin(), leafEnd = leafVertices.end(); leafIter != leafEnd; leafIter++){
        std::pair<VertexInfo *, float> curVertex;
        curVertex.second = Probability::getProbVertexProp(*leafIter, graph).probability;
        curVertex.first = new VertexInfo();
        //now I start going up in the chain
        prob_vertex_t curVert = *leafIter;
        while(boost::in_degree(curVert, graph) > 0){
            #ifndef NDEBUG
            if(boost::in_degree(curVert, graph) != 1){
                std::ofstream graphOut("ErrorProbGraph.dot");
                boost::write_graphviz(graphOut, graph, ProbNodeWriter(graph), ProbEdgeWriter(graph));
                graphOut.close();
                THROW_EXCEPTION("The probability graph contains a vertex with more than one father");
            }
            #endif
            prob_edge_t inEdge = *(boost::in_edges(curVert, graph).first);
            prob_vertex_t father = boost::source(inEdge, graph);
            curVertex.first->metrics[Probability::getProbVertexProp(father, graph).metricName] = Probability::getProbEdgeProp(inEdge, graph).metricVal;
            curVert = father;
        }
        retVal.push_back(curVertex);
    }
    return retVal;
}

///Frees all the memory occupied by the current probability graph
void Probability::clearProbGraph(ProbGraph *graph){
    //I simply have to go over all vevrtices and edges and delete the properties associated to them
    //then I delete the graph itself
    prob_edge_iterator alleIter, alleEnd;
    for(boost::tie(alleIter, alleEnd) = boost::edges(*graph); alleIter != alleEnd; alleIter++){
        delete &(Probability::getProbEdgeProp(*alleIter, *graph));
    }
    std::vector<prob_vertex_t> toEliminate(boost::vertices(*graph).first, boost::vertices(*graph).second);
    std::vector<prob_vertex_t>::iterator allVertIter, allVertEnd;
    for(allVertIter = toEliminate.begin(), allVertEnd = toEliminate.end(); allVertIter != allVertEnd; allVertIter++){
        delete &(Probability::getProbVertexProp(*allVertIter, *graph));
        boost::clear_vertex(*allVertIter, *graph);
        boost::remove_vertex(*allVertIter, *graph);
    }

    delete graph;
}

std::vector<prob_vertex_t> Probability::getProbGraphLeaves(ProbGraph &graph){
    std::vector<prob_vertex_t> leafVertices;
    prob_vertex_iterator allVertIter, allVertEnd;
    for(boost::tie(allVertIter, allVertEnd) = boost::vertices(graph); allVertIter != allVertEnd; allVertIter++){
        if(boost::out_degree(*allVertIter, graph) == 0)
            leafVertices.push_back(*allVertIter);
    }
    return leafVertices;
}

///Eliminates the subtree starting at the specified node and it frees the memory used by it
void Probability::clearProbSubtree(prob_vertex_t root, ProbGraph &graph){
    //I simply have to perform a breadth first search from root and delete
    //the properties associated to the nodes and to the edges
    //Finally I remove the encountered nodes
    if(std::find(boost::vertices(graph).first, boost::vertices(graph).second, root) == boost::vertices(graph).second){
        std::ofstream graphOut("ErrorProbGraph.dot");
        boost::write_graphviz(graphOut, graph, ProbNodeWriter(graph), ProbEdgeWriter(graph));
        graphOut.close();
        THROW_EXCEPTION("Unable to find root vertex");
    }

    std::vector<prob_vertex_t> vertices;
    std::vector<prob_edge_t> edges;
    TreeVisitor vis(vertices, edges);
    //std::cerr << "performing bfs" << std::endl;
    boost::breadth_first_search(graph, root, visitor(vis));
    //std::cerr << "performed bfs" << std::endl;
    std::vector<prob_edge_t>::iterator eIter, eEnd;
    for(eIter = edges.begin(), eEnd = edges.end(); eIter != eEnd; eIter++){
        delete &(getProbEdgeProp(*eIter, graph));
    }
    //std::cerr << "deleted edges prop" << std::endl;
    std::vector<prob_vertex_t>::iterator vertIter, vertEnd;
    for(vertIter = vertices.begin(), vertEnd = vertices.end(); vertIter != vertEnd; vertIter++){
        boost::clear_vertex(*vertIter, graph);
        delete &(getProbVertexProp(*vertIter, graph));
        boost::remove_vertex(*vertIter, graph);
    }
    //I now have to remap all the vertex indices to take into account the eliminated
    //vertex
    prob_vertex_index_pmap_t tgIndexMap = boost::get(boost::vertex_index_t(), graph);
    vertex_iterator vIter, v_end;
    unsigned int index = 0;
    for (boost::tie(vIter, v_end) = boost::vertices(graph); vIter != v_end; vIter++, index++){
      tgIndexMap[*vIter] = index;
    }
    Probability::getProbGraphInfo(graph) = index;

    //std::cerr << "deleted vertex prop" << std::endl;
}

///Adds new subtrees to the probability subtree to overcome the fact that changedMetric have more
///intervals
void Probability::addProbSubtrees(const std::string &changedMetric, unsigned int toAdd, ProbGraph &graph){
    //I have to get all the nodes already existing representing changedMetric;
    //std::cerr << "I have to add " << toAdd << " subtrees" << std::endl;
    std::vector<prob_vertex_t> curMetricVert;
    prob_vertex_iterator allVertIter, allVertEnd;
    for(boost::tie(allVertIter, allVertEnd) = boost::vertices(graph); allVertIter != allVertEnd; allVertIter++){
        if(Probability::getProbVertexProp(*allVertIter, graph).metricName == changedMetric){
            curMetricVert.push_back(*allVertIter);
        }
    }

    std::vector<prob_vertex_t>::iterator vertIter, vertEnd;
    // Now I check that all the found subtree have the same number of outedges
//     int numOutEdges = boost::out_degree(curMetricVert[0], graph);
//     for(vertIter = curMetricVert.begin(), vertEnd = curMetricVert.end(); vertIter != vertEnd; vertIter++){
//         if(boost::out_degree(*vertIter, graph) != numOutEdges){
//             std::ofstream graphOut("ErrorProbGraph.dot");
//             boost::write_graphviz(graphOut, graph, ProbNodeWriter(graph), ProbEdgeWriter(graph));
//             graphOut.close();
//             THROW_EXCEPTION("Error: nodes associated to metric " << changedMetric << " have different out degree");
//         }
//     }

    //std::cerr << "found " << curMetricVert.size() << " roots for the new subtrees" << std::endl;
    //Now I take one of the subtrees starting from these nodes and replicate it
    //on the new metric values
    prob_vertex_t subTreeRoot = boost::target(*(boost::out_edges(curMetricVert[0], graph).first), graph);
    std::vector<prob_edge_t> edges;
    std::vector<prob_vertex_t> vertToAdd;
    NodeAdderVisitor nodeAdder(vertToAdd, edges);
    boost::breadth_first_search(graph, subTreeRoot, visitor(nodeAdder));
    for(vertIter = curMetricVert.begin(), vertEnd = curMetricVert.end(); vertIter != vertEnd; vertIter++){
        //Lets add all the necessary subtrees
        for(unsigned int i = 0; i < toAdd; i++){
            std::map<prob_vertex_t, prob_vertex_t> old2New;
            //std::cerr << "added nodes" << std::endl;
            std::vector<prob_vertex_t>::iterator vertToAddIter, vertToAddEnd;
            for(vertToAddIter = vertToAdd.begin(), vertToAddEnd = vertToAdd.end(); vertToAddIter != vertToAddEnd; vertToAddIter++){
                ProbVertexInfo * newVertInfo = getProbVertexProp(*vertToAddIter, graph).clone();
                prob_vertex_t newVert = addProbGraphVertex(*newVertInfo, graph);
                old2New[*vertToAddIter] = newVert;
            }
            //std::cerr << "New subtree root: " << old2New[subTreeRoot] << " old " << subTreeRoot << std::endl;
            //and finally I add all the edges
            if(boost::out_degree(*vertIter, graph) == 0){
                std::ofstream graphOut("ErrorProbGraph.dot");
                boost::write_graphviz(graphOut, graph, ProbNodeWriter(graph), ProbEdgeWriter(graph));
                graphOut.close();
                THROW_EXCEPTION("Error: node associated to metric " << changedMetric << " has not output edges");
            }
            ProbEdgeInfo * newEdgeInfo = new ProbEdgeInfo();
            //I now have to increment the interval of the previous edge, since the graph cannot have overlapping metric intervals
            std::deque<prob_edge_t> orderedEdges = orderProbEdges(boost::out_edges(*vertIter, graph), graph);
            //std::cerr << "ordered probability edges: found " << orderedEdges.size() << " metric (" << getProbEdgeProp(orderedEdges.back(), graph).metricVal.first << "," << getProbEdgeProp(orderedEdges.back(), graph).metricVal.second << ")" << std::endl;
            newEdgeInfo->metricVal.first = getProbEdgeProp(orderedEdges.back(), graph).metricVal.second*1.1;
            newEdgeInfo->metricVal.second = newEdgeInfo->metricVal.first*1.1;
            //std::cerr << "set new edge info" << std::endl;
            std::pair<prob_vertex_t, prob_vertex_t> newEdge(*vertIter, old2New[subTreeRoot]);
            addProbGraphEdge(*newEdgeInfo, newEdge, graph);
            //std::cerr << "adding edge" << std::endl;
            Probability::getProbVertexProp(*vertIter, graph).metricValue.push_back(std::pair<float, float>(newEdgeInfo->metricVal.first, newEdgeInfo->metricVal.first));
            std::vector<prob_edge_t>::iterator edgeToAddIter, edgeToAddEnd;
            for(edgeToAddIter = edges.begin(), edgeToAddEnd = edges.end(); edgeToAddIter != edgeToAddEnd; edgeToAddIter++){
                ProbEdgeInfo * clonedEdgeInfo = Probability::getProbEdgeProp(*edgeToAddIter, graph).clone();
                //std::cerr << "new edge starting from: " << old2New[boost::source(*edgeToAddIter, graph)] << " old " << boost::source(*edgeToAddIter, graph) << std::endl;
                std::pair<prob_vertex_t, prob_vertex_t> clonedEdge(old2New[boost::source(*edgeToAddIter, graph)], old2New[boost::target(*edgeToAddIter, graph)]);
                Probability::addProbGraphEdge(*clonedEdgeInfo, clonedEdge, graph);
            }
            //std::cerr << "added edges" << std::endl;
        }
    }
}

prob_vertex_t Probability::getProbRoot(ProbGraph &graph){
    prob_vertex_iterator allVertIter, allVertEnd;
    for(boost::tie(allVertIter, allVertEnd) = boost::vertices(graph); allVertIter != allVertEnd; allVertIter++){
        if(boost::in_degree(*allVertIter, graph) == 0)
            return *allVertIter;
    }
    std::ofstream graphOut("ErrorProbGraph.dot");
    boost::write_graphviz(graphOut, graph, ProbNodeWriter(graph), ProbEdgeWriter(graph));
    graphOut.close();
    THROW_EXCEPTION("No root node present in the current probability tree");
    return NULL;
}

std::deque<prob_edge_t> Probability::orderProbEdges(std::pair<prob_out_edge_iterator, prob_out_edge_iterator> edges, ProbGraph &graph){
    //std::cerr << "inside orderProbEdges" << std::endl;
    std::deque<prob_edge_t> orderedEdges;
    for(; edges.first != edges.second; edges.first++){
        orderedEdges.push_back(*edges.first);
    }
    //std::cerr << "added all the edges to the list" << std::endl;
    std::sort(orderedEdges.begin(), orderedEdges.end(), EdgeSorter(graph));
    //std::cerr << "ordered all the edges" << std::endl;
    return orderedEdges;
}

///Given a decrease in the number of edges exiting from vertex vertexToChange, I compute the corresponding remaining leaves, the ones
///that have to be eliminated and the subtrees which have to be eliminated. All of them are ordered
void Probability::getProbDecreseInfo(std::string vertexToChange, unsigned int leavesToKeep, std::map<std::string, std::deque<prob_vertex_t> > &remainingLeaves,
                std::map<std::string, std::deque<prob_vertex_t> > &eliminatedLeaves, std::vector<prob_vertex_t> &eliminatedTreeRoot, ProbGraph &graph){
    std::map<prob_vertex_t, std::pair<float, float> > leaf2Edge;
    std::map<prob_vertex_t, prob_vertex_t> leaf2Subtree;
    std::map<std::string, std::deque<prob_vertex_t> > funToLeaves;
    //I have to start from the leaves and compute the backward path
    std::vector<prob_vertex_t> leaves = getProbGraphLeaves(graph);
    std::vector<prob_vertex_t>::iterator leafIter, leafEnd;
    //std::cerr << "examining " << leaves.size() << " leaves; total vertices " << boost::num_vertices(graph) << std::endl;
    for(leafIter = leaves.begin(), leafEnd = leaves.end(); leafIter != leafEnd; leafIter++){
        std::string tempFunction = "";
        prob_vertex_t cutVertex = *leafIter;
        while(boost::in_degree(cutVertex, graph) > 0){
            prob_edge_t inComingEdge = *(boost::in_edges(cutVertex, graph).first);
            cutVertex = boost::source(inComingEdge, graph);
            if(Probability::getProbVertexProp(cutVertex, graph).metricName != vertexToChange){
                tempFunction += Probability::getProbVertexProp(cutVertex, graph).metricName +
                        boost::lexical_cast<std::string>(Probability::getProbEdgeProp(inComingEdge, graph).metricVal.first) +
                        "_"  + boost::lexical_cast<std::string>(Probability::getProbEdgeProp(inComingEdge, graph).metricVal.second);
            }
            else{
                if(boost::out_degree(cutVertex, graph) != Probability::getProbVertexProp(cutVertex, graph).metricValue.size()){
                    std::ofstream graphOut("ErrorProbGraph.dot");
                    boost::write_graphviz(graphOut, graph, ProbNodeWriter(graph), ProbEdgeWriter(graph));
                    graphOut.close();
                    THROW_EXCEPTION("Vertex " << Probability::getProbVertexProp(cutVertex, graph).metricName << " has associate a metric with " << Probability::getProbVertexProp(cutVertex, graph).metricValue.size() << " intervals, but it has only " << boost::out_degree(cutVertex, graph) << " out edges");
                }
                if(leaf2Subtree.find(*leafIter) != leaf2Subtree.end()){
                    std::ofstream graphOut("ErrorProbGraph.dot");
                    boost::write_graphviz(graphOut, graph, ProbNodeWriter(graph), ProbEdgeWriter(graph));
                    graphOut.close();
                    THROW_EXCEPTION("Error, I have already added a subtree for leaf " << *leafIter);
                }
                leaf2Edge[*leafIter] = Probability::getProbEdgeProp(inComingEdge, graph).metricVal;
                leaf2Subtree[*leafIter] = boost::target(inComingEdge, graph);
            }
        }
        funToLeaves[tempFunction].push_back(*leafIter);
        tempFunction = "";
    }
    //Now we should order the leaves for each element of the funToLeaves map
    std::map<std::string, std::deque<prob_vertex_t> >::iterator funsIter, funsEnd;
    for(funsIter = funToLeaves.begin(), funsEnd = funToLeaves.end(); funsIter != funsEnd; funsIter++){
        //std::cerr << funsIter->first << " num leaves " << funsIter->second.size() << " keeping " << leavesToKeep << std::endl;
        LeavesSorter sorter(leaf2Edge);
        std::sort(funsIter->second.begin(), funsIter->second.end(), sorter);
    }
    //Finally I have to fill the elements to return from the function
    for(funsIter = funToLeaves.begin(), funsEnd = funToLeaves.end(); funsIter != funsEnd; funsIter++){
        std::deque<prob_vertex_t>::iterator leavesIter, leavesEnd;
        for(leavesIter = funsIter->second.begin(), leavesEnd = funsIter->second.end();
                                                    leavesIter != leavesEnd; leavesIter++){
            if(remainingLeaves[funsIter->first].size() < leavesToKeep){
                //std::cerr << "adding remaining leaf " << *leavesIter << std::endl;
                remainingLeaves[funsIter->first].push_back(*leavesIter);
            }
            else{
                //std::cerr << "Adding subtree to eliminate " << *leavesIter << std::endl;
                eliminatedLeaves[funsIter->first].push_back(*leavesIter);
                if(std::find(eliminatedTreeRoot.begin(), eliminatedTreeRoot.end(), leaf2Subtree[*leavesIter]) == eliminatedTreeRoot.end())
                    eliminatedTreeRoot.push_back(leaf2Subtree[*leavesIter]);
            }
        }
    }
}

///Given a change in the number of edges exiting from vertexToChange, it gets all the leaves grouping them by the same value
///of all the metrics but vertexToChange, and it orders them for increasing values of the edges exiting from vertexToChange
std::map<std::string, std::deque<prob_vertex_t> > Probability::getOrderedLeaves(std::string vertexToChange, ProbGraph &graph){
    std::map<prob_vertex_t, std::pair<float, float> > leaf2Edge;
    std::map<std::string, std::deque<prob_vertex_t> > funToLeaves;
    std::string tempFunction;
    //std::cerr << "getting ordered leaves for metric " << vertexToChange << std::endl;
    //I have to start from the leaves and compute the backward path
    std::vector<prob_vertex_t> leaves = Probability::getProbGraphLeaves(graph);
    //std::cerr << "We have " << leaves.size() << " leaves" << std::endl;
    std::vector<prob_vertex_t>::iterator leafIter, leafEnd;
    for(leafIter = leaves.begin(), leafEnd = leaves.end(); leafIter != leafEnd; leafIter++){
        prob_vertex_t cutVertex = *leafIter;
        while(boost::in_degree(cutVertex, graph) > 0){
            prob_edge_t inComingEdge = *(boost::in_edges(cutVertex, graph).first);
            cutVertex = boost::source(inComingEdge, graph);
            if(Probability::getProbVertexProp(cutVertex, graph).metricName != vertexToChange)
                tempFunction += Probability::getProbVertexProp(cutVertex, graph).metricName +
                        boost::lexical_cast<std::string>(Probability::getProbEdgeProp(inComingEdge, graph).metricVal.first) +
                        "_"  + boost::lexical_cast<std::string>(Probability::getProbEdgeProp(inComingEdge, graph).metricVal.second);
            else{
                leaf2Edge[*leafIter] = Probability::getProbEdgeProp(inComingEdge, graph).metricVal;
            }
        }
        funToLeaves[tempFunction].push_back(*leafIter);
        tempFunction = "";
    }
    //std::cerr << "ordering leaves" << std::endl;
    //Now we should order the leaves for each element of the funToLeaves map
    std::map<std::string, std::deque<prob_vertex_t> >::iterator funsIter, funsEnd;
    for(funsIter = funToLeaves.begin(), funsEnd = funToLeaves.end(); funsIter != funsEnd; funsIter++){
        LeavesSorter sorter(leaf2Edge);
        std::sort(funsIter->second.begin(), funsIter->second.end(), sorter);
        //std::cerr << "Function " << funsIter->first  << " - " << funsIter->second.size() << " elements" << std::endl;
    }
    return funToLeaves;
}

///Updates the labels of the graph with the new intervals
void Probability::updateIntervalLabels(const std::map<std::string, std::list<std::pair<float, float> > > &newMetrics, ProbGraph &graph){
    //std::cerr << "updating interval labels" << std::endl;
    prob_vertex_iterator allVertIter, allVertEnd;
    for(boost::tie(allVertIter, allVertEnd) = boost::vertices(graph); allVertIter != allVertEnd; allVertIter++){
        std::string curMetricName = Probability::getProbVertexProp(*allVertIter, graph).metricName;
        //std::cerr << "updating metric " << curMetricName << std::endl;
        std::map<std::string, std::list<std::pair<float, float> > >::const_iterator foundInterval = newMetrics.find(curMetricName);
        if(foundInterval == newMetrics.end()){
            if(curMetricName != ""){
                std::ofstream graphOut("ErrorProbGraph.dot");
                boost::write_graphviz(graphOut, graph, ProbNodeWriter(graph), ProbEdgeWriter(graph));
                graphOut.close();
                THROW_EXCEPTION("Unable to find metric " << curMetricName << " in the probability graph");
            }
            else
                continue;
        }
        //std::cerr << "present " << getProbVertexProp(*allVertIter, graph).metricValue.size() << " old metric values" << std::endl;
        Probability::getProbVertexProp(*allVertIter, graph).metricValue = foundInterval->second;
        //std::cerr << "ordering " << boost::out_degree(*allVertIter, graph) << " probability edges" << std::endl;
        std::deque<prob_edge_t> orderedOutEdges = Probability::orderProbEdges(boost::out_edges(*allVertIter, graph), graph);
        //std::cerr << "orderdered " << orderedOutEdges.size() << " probability edges" << std::endl;
        Probability::getProbVertexProp(*allVertIter, graph).metricValue.sort();
        //std::cerr << "oredered " << getProbVertexProp(*allVertIter, graph).metricValue.size() << " metric values" << std::endl;
        if(orderedOutEdges.size() != getProbVertexProp(*allVertIter, graph).metricValue.size()){
            std::ofstream graphOut("ErrorProbGraph.dot");
            boost::write_graphviz(graphOut, graph, ProbNodeWriter(graph), ProbEdgeWriter(graph));
            graphOut.close();
            THROW_EXCEPTION("The number of edges " << orderedOutEdges.size() << " is different from the metric width " << getProbVertexProp(*allVertIter, graph).metricValue.size());
        }
        unsigned int computedOutEdge = 0;
        std::list<std::pair<float, float> >::iterator outEdgeIter, outEdgeEnd;
        for(outEdgeIter = getProbVertexProp(*allVertIter, graph).metricValue.begin(),
            outEdgeEnd = Probability::getProbVertexProp(*allVertIter, graph).metricValue.end(); outEdgeIter != outEdgeEnd; outEdgeIter++){
            //std::cerr << "Dealing with values (" << outEdgeIter->first << "," << outEdgeIter->second << ")" << std::endl;
            Probability::getProbEdgeProp(orderedOutEdges[computedOutEdge], graph).metricVal = *outEdgeIter;
            computedOutEdge++;
        }
    }
}

///Checks that the graph is coherent, that the number of metrics for each node is coherent with the number
///of out edges
void Probability::checkGraphCoherency(ProbGraph &graph){
    prob_vertex_iterator allVertIter, allVertEnd;
    for(boost::tie(allVertIter, allVertEnd) = boost::vertices(graph); allVertIter != allVertEnd; allVertIter++){
        if(Probability::getProbVertexProp(*allVertIter, graph).metricValue.size() != boost::out_degree(*allVertIter, graph)){
            std::ofstream graphOut("ErrorProbGraph.dot");
            boost::write_graphviz(graphOut, graph, ProbNodeWriter(graph), ProbEdgeWriter(graph));
            graphOut.close();
            THROW_EXCEPTION("Vertex " << Probability::getProbVertexProp(*allVertIter, graph).metricName << " has associate a metric with " << Probability::getProbVertexProp(*allVertIter, graph).metricValue.size() << " intervals, but it has only " << boost::out_degree(*allVertIter, graph));
        }
    }
    // I also have to check that the metrics are contiguous and non overlapping
    std::vector<prob_vertex_t> leafVertices = Probability::getProbGraphLeaves(graph);
    // Now, for each leaf vertex, I go up to the root: the path corresponds to the
    // metric value for the vertex
    double totalProb = 0;
    std::map<std::string, std::set<std::pair<float, float> > > metricVals;
    std::vector<prob_vertex_t>::iterator leafIter, leafEnd;
    for(leafIter = leafVertices.begin(), leafEnd = leafVertices.end(); leafIter != leafEnd; leafIter++){
        totalProb += Probability::getProbVertexProp(*leafIter, graph).probability;
        //now I start going up in the chain
        prob_vertex_t curVert = *leafIter;
        while(boost::in_degree(curVert, graph) > 0){
            #ifndef NDEBUG
            if(boost::in_degree(curVert, graph) != 1){
                std::ofstream graphOut("ErrorProbGraph.dot");
                boost::write_graphviz(graphOut, graph, ProbNodeWriter(graph), ProbEdgeWriter(graph));
                graphOut.close();
                THROW_EXCEPTION("The probability graph contains a vertex with more than one father");
            }
            #endif
            prob_edge_t inEdge = *(boost::in_edges(curVert, graph).first);
            prob_vertex_t father = boost::source(inEdge, graph);
            metricVals[Probability::getProbVertexProp(father, graph).metricName].insert(Probability::getProbEdgeProp(inEdge, graph).metricVal);
            curVert = father;
        }
    }

    if(totalProb > 1.01 || totalProb < 0.99){
        std::ofstream graphOut("ErrorProbGraph.dot");
        boost::write_graphviz(graphOut, graph, ProbNodeWriter(graph), ProbEdgeWriter(graph));
        graphOut.close();
        THROW_EXCEPTION("The probability graph has wrong Probability: the sum is " << totalProb);

    }

    // Now I have to read the metricVals map in order to determine if there are overlappings or if
    // there are holes
    std::map<std::string, std::set<std::pair<float, float> > >::iterator metricValIter, metricValEnd;
    for(metricValIter = metricVals.begin(), metricValEnd = metricVals.end(); metricValIter != metricValEnd; metricValIter++){
        std::set<std::pair<float, float> >::iterator curMetricIter, curMetricEnd, curMetricNext;
        for(curMetricIter = metricValIter->second.begin(), curMetricEnd = metricValIter->second.end(), curMetricNext = curMetricIter, curMetricNext++;
                                                                                        curMetricNext != curMetricEnd; curMetricIter++, curMetricNext++){
            if( abs(curMetricIter->second - curMetricNext->first) > curMetricNext->first*config.eps){
                std::ofstream graphOut("ErrorProbGraph.dot");
                boost::write_graphviz(graphOut, graph, ProbNodeWriter(graph), ProbEdgeWriter(graph));
                graphOut.close();
                std::cout << curMetricIter->second << " " << curMetricNext->first << " " << (double) curMetricIter->second - (double) curMetricNext->first << std::endl;
                THROW_EXCEPTION("The probability graph has wrong metric values for metric " << metricValIter->first);
            }
        }
    }
}
