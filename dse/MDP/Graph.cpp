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

#include <map>
#include <vector>
#include <algorithm>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/config.hpp>

#include "Graph.hpp"
#include "pluginIf.hpp"
#include "utils.hpp"

#ifdef MEMORY_DEBUG
#include <mpatrol.h>
#endif

NodeWriter::NodeWriter(Graph &graph) : graph(graph){
    this->nodeInfo = boost::get(node_info_t(), this->graph);
}

void NodeWriter::operator()(std::ostream& out, const vertex_t & v) const {
    VertexInfo * vert = dynamic_cast<VertexInfo *>(this->nodeInfo[v]);
    out << "[label=\"";
    plugin_int_map::iterator paramIter, paramEnd;
    for(paramIter = vert->parameters.begin(), paramEnd = vert->parameters.end(); paramIter != paramEnd; paramIter++){
        out << rev_plugin_handler[paramIter->first] << "=" << paramIter->first->getParameterName(paramIter->second) << " ";
    }
    out << "\\n";
    metric_map::iterator metricIter, metricEnd;
    for(metricIter = vert->metrics.begin(), metricEnd = vert->metrics.end(); metricIter != metricEnd; metricIter++){
        out << metricIter->first << "=(" << metricIter->second.first << "," << metricIter->second.second << ")\\n";
    }
    out << " id=" << v <<"\"]";
}

EdgeWriter::EdgeWriter(Graph &graph) : graph(graph){
    this->edgeInfo = boost::get(edge_info_t(), this->graph);
}

void EdgeWriter::operator()(std::ostream& out, const edge_t & e) const {
    EdgeInfo * ed = dynamic_cast<EdgeInfo *>(this->edgeInfo[e]);
    out << "[label=\"" << rev_plugin_handler[ed->action.first] << "=" << ed->action.second << "\\nprob=" << ed->prob << "\\nvs=" << ed->virtualSamples << "\"]";
}

vertex_t Exploration::addGraphVertex(VertexInfo * newVertex, Graph &graph){
    //map associating an id to each node of the graph
    vertex_index_pmap_t tgIndexMap = boost::get(boost::vertex_index_t(), graph);

    //Add the vertex
    vertex_t newVertex_g = boost::add_vertex(graph);
    unsigned int &graphUniqueId = getGraphInfo(graph);
//     if(boost::num_vertices(graph) == 0){
//         graphUniqueId = 0;
//     }
    tgIndexMap[newVertex_g] = graphUniqueId;
    graphUniqueId++;

    //Setting its properties
    boost::property_map<Graph, node_info_t>::type nodeInfo = boost::get(node_info_t(), graph);
    nodeInfo[newVertex_g] = newVertex;

    return newVertex_g;
}

edge_t Exploration::addGraphEdge(EdgeInfo * newEdge, std::pair<vertex_t, vertex_t> vertices, Graph &graph){
    //First of all I have to determine the vertices which correspond to the functions which I
    //want to connect
    boost::property_map<Graph, node_info_t>::type nodeInfo = boost::get(node_info_t(), graph);

    //Now I look for the edge: in case it does not exist I create it,  otherwise I simply update its properties
    #ifndef NDEBUG
    std::pair<vertex_iterator, vertex_iterator> vertIter = boost::vertices(graph);
    if(std::find(vertIter.first, vertIter.second, vertices.first) == vertIter.second)
        THROW_EXCEPTION("Unable to find vertex " << vertices.first << " in the graph");
    if(std::find(vertIter.first, vertIter.second, vertices.second) == vertIter.second)
        THROW_EXCEPTION("Unable to find vertex " << vertices.second << " in the graph");
    #endif
    std::pair<edge_t, bool> foundEdge = boost::edge(vertices.first, vertices.second, graph);
    edge_t curEdge = foundEdge.first;
    if(!foundEdge.second){
       //Finally I make the connection...
       edge_t e;
       bool inserted;
       boost::tie(e, inserted) = boost::add_edge(vertices.first, vertices.second, newEdge, graph);
       curEdge = e;
    }
    return curEdge;
}

void Exploration::getSubtreePath(Graph &inGraph, Graph * outGraph, const vertex_t & startVertex){
    std::map<vertex_t, vertex_t> old2New;
    std::vector<edge_t> edges;
    vertex_t curVertex = startVertex;
    vertex_t oldVertex = startVertex;
    do{
        oldVertex = curVertex;
        VertexInfo * newVertInfo = new VertexInfo();
        VertexInfo &oldVertInfo = getVertexProp(curVertex, inGraph);
        newVertInfo->parameters = oldVertInfo.parameters;
        newVertInfo->curStats = oldVertInfo.curStats;
        newVertInfo->metrics = oldVertInfo.metrics;
        newVertInfo->forbiddenActions = oldVertInfo.forbiddenActions;
        newVertInfo->proDensities = oldVertInfo.proDensities;
        vertex_t newVert = Exploration::addGraphVertex(newVertInfo, *outGraph);
        old2New[curVertex] = newVert;
        if(boost::in_degree(curVertex, inGraph) > 0){
            edge_t inEdge = *(boost::in_edges(curVertex, inGraph).first);
            edges.push_back(inEdge);
            curVertex = boost::source(inEdge, inGraph);
        }
    }
    while(boost::in_degree(oldVertex, inGraph) > 0);
    //And here I add all the edges
    std::vector<edge_t>::iterator edgeIter, edgeEnd;
    for(edgeIter = edges.begin(), edgeEnd = edges.end(); edgeIter != edgeEnd; edgeIter++){
        EdgeInfo * newEInfo = new EdgeInfo();
        EdgeInfo &oldEInfo = getEdgeProp(*edgeIter, inGraph);
        newEInfo->action = oldEInfo.action;
        newEInfo->prob = oldEInfo.prob;
        newEInfo->virtualSamples = oldEInfo.virtualSamples;
        std::pair<prob_vertex_t, prob_vertex_t> newVertices;
        newVertices.first = old2New[boost::source(*edgeIter, inGraph)];
        newVertices.second = old2New[boost::target(*edgeIter, inGraph)];
        Exploration::addGraphEdge(newEInfo, newVertices, *outGraph);
    }
}
