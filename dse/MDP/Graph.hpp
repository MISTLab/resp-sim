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


#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <map>
#include <string>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/config.hpp>

#include "ProbFunction.hpp"

#include "pluginIf.hpp"

// Redifine to use different allocators
typedef std::map<std::pair<PluginIf *, int>, ProbGraph *> density_map;

///This graph is used to express the exploration tree; in particular we associate to each
///node: <parameter configuration, metric values, probability density>. While the
///first two are maps name, value (the parameter conf is expresses as an integer; in case
///it represents a non-numeric value, the integer is the enumeration of this non-
///numeric value), the third is a map: action, list<doubles>: for each action
///we map the probability density.
///Associated to each node we also have a list of actions multimap(param, action>): this is
///the list of forbidden actions: for every action that we perform, we augment this list.
///
///Associated to each edge we have the probability of ending in that edge (i.e.
///the corresponding double value of the probabilty function of the source node)
///and the action associated to the edge. An action (this is valid also for the
///edge) is identified by the parameter it refers to and an enumerative
///describing the action (the enumerative is specific for each action)

struct EdgeInfo{
    // The action associated with the edge; it is represented
    // by the name of the changed parameter and the id
    // of the action
    std::pair<PluginIf*, int> action;
    // Probability of executing the specified action and ending
    // in the target state
    float prob;
    // The corresponding virtual samples
    unsigned int virtualSamples;
    virtual ~EdgeInfo(){}
};

struct VertexInfo{

    // The int is not directly the value of the parameter,
    // but an enumeration; refer to the specific parameter plugin
    // for the decoding of this value
    std::map<PluginIf*, int> parameters;
    // For every plugin, this variables holds the value of the statistics
    // for the plugin itself. Note that the application of an action
    // modifies the statistics themselves
    std::map<int, float> curStats;
    // Estimated metric intervals; in case the two estimated boundaries
    // are equal it means that we have determined a precise metric value
    std::map<std::string, std::pair<float, float> > metrics;
    // For each parameters it contains a list of the forbidden
    // actions; again the action is represented by an enumeration
    // which can be resolved by the corresponding plugin
    std::multimap<PluginIf*, int> forbiddenActions;
    // probability density functions; there is a graph which
    // repsents the probability density: note that I have a different
    // probability function for each action
    density_map proDensities;
    virtual ~VertexInfo(){}
};

struct node_info_t{
   ///typedef defining node_info_t as vertex node
   typedef boost::vertex_property_tag kind;
};

struct edge_info_t{
   ///typedef defining edge_info_t as edge object
   typedef boost::edge_property_tag kind;
};

struct graph_info_t{
   ///typedef defining graph_info_t as graph object
   typedef boost::graph_property_tag kind;
};


///Definition of the custom property node_info.
typedef boost::property<node_info_t, VertexInfo * > node_info_property;
///Definition of the node color property. This property is required by topological_sort function.
typedef boost::property<boost::vertex_color_t, boost::default_color_type, node_info_property> Color_property ;
///Definition of the node index property. This property is required by a large set of functions.
typedef boost::property<boost::vertex_index_t, std::size_t, Color_property> Vertex_property;
///Definition of the graph property; it will hold nothing but the next unique id for the graph nodes
typedef boost::property<graph_info_t, unsigned int > Graph_property;

typedef boost::adjacency_list<boost::listS, boost::listS, boost::bidirectionalS,  Vertex_property,
                                                            boost::property<edge_info_t, EdgeInfo * >, Graph_property > Graph;

typedef boost::graph_traits<Graph>::vertex_descriptor vertex_t;
typedef boost::graph_traits<Graph>::vertex_iterator vertex_iterator;
typedef boost::graph_traits<Graph>::edge_descriptor edge_t;
typedef boost::graph_traits<Graph>::edge_iterator edge_iterator;
typedef boost::graph_traits<Graph>::out_edge_iterator out_edge_iterator;
typedef boost::graph_traits<Graph>::in_edge_iterator in_edge_iterator;
typedef boost::property_map<Graph, boost::vertex_index_t>::type vertex_index_pmap_t;

class NodeWriter {
  public:
    NodeWriter(Graph &graph);
    void operator()(std::ostream& out, const vertex_t & v) const;
  private:
    Graph &graph;
    boost::property_map<Graph, node_info_t>::type nodeInfo;
};

class EdgeWriter {
  public:
    EdgeWriter(Graph &graph);
    void operator()(std::ostream& out, const edge_t & e) const;
  private:
    Graph &graph;
    boost::property_map<Graph, edge_info_t>::type edgeInfo;
};

namespace Exploration{

vertex_t addGraphVertex(VertexInfo * newVertex, Graph &graph);
edge_t addGraphEdge(EdgeInfo * newEdge, std::pair<vertex_t, vertex_t> vertices, Graph &graph);

inline unsigned int &getGraphInfo(Graph &graph){
    return boost::get_property(graph, graph_info_t());
}

inline EdgeInfo & getEdgeProp(const edge_t &edge, Graph &graph){
   boost::property_map<Graph, edge_info_t>::type edgeInfo = boost::get(edge_info_t(), graph);
   return *edgeInfo[edge];
}

inline VertexInfo& getVertexProp(const vertex_t & vertex, Graph &graph){
   boost::property_map<Graph, node_info_t>::type nodeInfo = boost::get(node_info_t(), graph);
   return *nodeInfo[vertex];
}

void getSubtreePath(Graph &inGraph, Graph * outGraph, const vertex_t & startVertex);
}

#endif
