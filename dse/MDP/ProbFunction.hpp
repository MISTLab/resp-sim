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

 #ifndef PROBFUNCTION_HPP
 #define PROBFUNCTION_HPP

#include <string>
#include <list>
#include <vector>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/config.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/depth_first_search.hpp>

#include "utils.hpp"

#ifdef MEMORY_DEBUG
#include <mpatrol.h>
#endif

///This graph is used to express the probability and the virtual samples associated to the
///metric space after each action

struct ProbEdgeInfo{
    // The current value of the metric
    std::pair<float, float> metricVal;
    ~ProbEdgeInfo(){}
    ProbEdgeInfo * clone(){
        ProbEdgeInfo * cloned = new ProbEdgeInfo();
        cloned->metricVal = this->metricVal;
        return cloned;
    }
};

struct ProbVertexInfo{
    // The name of the metric represented by the current node: present for every node but the leaf ones
    std::string metricName;
    // The values of the metric: present for every node but the leaf ones
    list_pair_float_float metricValue;
    // Probability; note that this value is associated
    // only to leaf nodes
    float probability;
    // Number of virtual samples associated to the node
    // present only for leaf nodes
    unsigned int virtualSamples;
    ~ProbVertexInfo(){}
    ProbVertexInfo * clone(){
        ProbVertexInfo * cloned = new ProbVertexInfo();
        cloned->metricName = this->metricName;
        cloned->metricValue = this->metricValue;
        cloned->probability = this->probability;
        cloned->virtualSamples = this->virtualSamples;
        return cloned;
    }
};

struct prob_node_info_t{
   ///typedef defining node_info_t as vertex node
   typedef boost::vertex_property_tag kind;
};

struct prob_edge_info_t{
   ///typedef defining edge_info_t as edge object
   typedef boost::edge_property_tag kind;
};

struct prob_graph_info_t{
   ///typedef defining graph_info_t as graph object
   typedef boost::graph_property_tag kind;
};


///Definition of the custom property node_info.
typedef boost::property<prob_node_info_t, ProbVertexInfo * > prob_node_info_property;
///Definition of the node color property. This property is required by topological_sort function.
typedef boost::property<boost::vertex_color_t, boost::default_color_type, prob_node_info_property> ProbColor_property ;
///Definition of the node index property. This property is required by a large set of functions.
typedef boost::property<boost::vertex_index_t, std::size_t, ProbColor_property> ProbVertex_property;
///Definition of the graph property; it will hold nothing but the next unique id for the graph nodes
typedef boost::property<prob_graph_info_t, unsigned int > ProbGraph_property;

typedef boost::adjacency_list<boost::listS, boost::listS, boost::bidirectionalS,  ProbVertex_property,
                                                            boost::property<prob_edge_info_t, ProbEdgeInfo * >, ProbGraph_property > ProbGraph;

typedef boost::graph_traits<ProbGraph>::vertex_descriptor prob_vertex_t;
typedef boost::graph_traits<ProbGraph>::vertex_iterator prob_vertex_iterator;
typedef boost::graph_traits<ProbGraph>::edge_descriptor prob_edge_t;
typedef boost::graph_traits<ProbGraph>::edge_iterator prob_edge_iterator;
typedef boost::graph_traits<ProbGraph>::out_edge_iterator prob_out_edge_iterator;
typedef boost::graph_traits<ProbGraph>::in_edge_iterator prob_in_edge_iterator;
typedef boost::property_map<ProbGraph, boost::vertex_index_t>::type prob_vertex_index_pmap_t;


class ProbNodeWriter {
  public:
    ProbNodeWriter(ProbGraph &graph);
    void operator()(std::ostream& out, const prob_vertex_t & v) const;
  private:
    ProbGraph &graph;
    boost::property_map<ProbGraph, prob_node_info_t>::type nodeInfo;
};

class ProbEdgeWriter {
  public:
    ProbEdgeWriter(ProbGraph &graph);
    void operator()(std::ostream& out, const prob_edge_t & e) const;
  private:
    ProbGraph &graph;
    boost::property_map<ProbGraph, prob_edge_info_t>::type edgeInfo;
};

namespace Probability{
prob_vertex_t addProbGraphVertex(ProbVertexInfo &newVertex, ProbGraph &graph);
prob_edge_t addProbGraphEdge(ProbEdgeInfo &newEdge, std::pair<prob_vertex_t, prob_vertex_t> vertices, ProbGraph &graph);

inline unsigned int &getProbGraphInfo(ProbGraph &graph){
    return boost::get_property(graph, prob_graph_info_t());
}

inline ProbEdgeInfo & getProbEdgeProp(const prob_edge_t &edge, ProbGraph &graph){
   boost::property_map<ProbGraph, prob_edge_info_t>::type edgeInfo = boost::get(prob_edge_info_t(), graph);
   return *edgeInfo[edge];
}

inline ProbVertexInfo& getProbVertexProp(const prob_vertex_t & vertex, ProbGraph &graph){
   boost::property_map<ProbGraph, prob_node_info_t>::type nodeInfo = boost::get(prob_node_info_t(), graph);
   return *nodeInfo[vertex];
}

}

struct TreeVisitor : public boost::bfs_visitor<>{
    std::vector<prob_vertex_t> &vertices;
    std::vector<prob_edge_t> &edges;

    TreeVisitor(std::vector<prob_vertex_t> &vertices,
        std::vector<prob_edge_t> &edges) : vertices(vertices), edges(edges){}
    template <typename Edge, typename Graph>
    void tree_edge(const Edge& e, Graph& g){
        edges.push_back(e);
    }
    template <typename Vertex, typename Graph>
    void finish_vertex(const Vertex& u, Graph& g){
        vertices.push_back(u);
    }
};

struct NodeAdderVisitor : public boost::bfs_visitor<>{
    std::vector<prob_vertex_t> &vertToAdd;
    std::vector<prob_edge_t> &edges;

    NodeAdderVisitor(std::vector<prob_vertex_t> &vertToAdd,
        std::vector<prob_edge_t> &edges) : vertToAdd(vertToAdd), edges(edges){}
    template <typename Edge, typename Graph>
    void tree_edge(const Edge& e, Graph& g){
        edges.push_back(e);
    }
    template <typename Vertex, typename Graph>
    void finish_vertex(const Vertex& u, Graph& g){
        vertToAdd.push_back(u);
    }
};

struct ToRescaleSeeker : public boost::dfs_visitor<>{
    //Specifies whether the vertex which have to be rescaled was found
    bool &foundToScale;
    //The vertex with a changed number of out edges
    prob_vertex_t &vertexToChange;
    //If the number of edges decreases or not
    bool &decrease;
    //The difference between the number of edges
    unsigned int &difference;
    //The new metrics which I have to compare with the current graph: in case
    //some elements have a different number I change
    const list_float_map &newMetrics;
    ProbGraph &graph;
    ToRescaleSeeker(const list_float_map &newMetrics, ProbGraph &graph,
                            bool &foundToScale, prob_vertex_t &vertexToChange, bool &decrease, unsigned int &difference) :
                            newMetrics(newMetrics), graph(graph), foundToScale(foundToScale), vertexToChange(vertexToChange),
                                                                                    decrease(decrease), difference(difference){
        this->foundToScale = false;
    }
    template <typename Vertex, typename Graph>
    void discover_vertex(Vertex& u, Graph& g){
        if(this->foundToScale)
            return;
        ProbVertexInfo &vertexInfo = Probability::getProbVertexProp(u, graph);
        //std::cerr << "metric " << vertexInfo.metricName << std::endl;
        if(vertexInfo.metricName == ""){ // I'm in a leaf node, it doesn't represent a metric but just a probability
            //std::cerr << "leaf node, continuing" << std::endl;
            return;
        }
        list_float_map::const_iterator curMetric = this->newMetrics.find(vertexInfo.metricName);
        if(curMetric == this->newMetrics.end())
            THROW_EXCEPTION("Unable to find in the probability graph metric " << vertexInfo.metricName);
        //std::cerr << "New metric size " << curMetric->second.size() << " current vertex metric size (out edges) " << boost::out_degree(u, graph) << std::endl;
        if(boost::out_degree(u, graph) > curMetric->second.size()){
            this->foundToScale = true;
            this->vertexToChange = u;
            this->decrease = true;
            this->difference = boost::out_degree(u, graph) - curMetric->second.size();
        }
        else if(boost::out_degree(u, graph) < curMetric->second.size()){
            this->foundToScale = true;
            this->vertexToChange = u;
            this->decrease = false;
            this->difference = curMetric->second.size() - boost::out_degree(u, graph);
        }
        //std::cerr << "found " << this->foundToScale << " decrease " << this->decrease << " difference " << this->difference << std::endl;
    }
};

struct EdgeSorter{
    ProbGraph &graph;
    EdgeSorter(ProbGraph &graph) : graph(graph){}
    bool operator()(const prob_edge_t &a, const prob_edge_t &b);
};

struct LeavesSorter{
    std::map<prob_vertex_t, std::pair<float, float> > &leaf2Edge;
    LeavesSorter(std::map<prob_vertex_t, std::pair<float, float> > &leaf2Edge) : leaf2Edge(leaf2Edge){}
    bool operator()(const prob_vertex_t &a, const prob_vertex_t &b);
};

struct VertexInfo;

namespace Probability{
/// given an input graph, it performs a deep copy in an output one
void copyGraph(ProbGraph &inGraph, ProbGraph &outGraph);
/// given the outcome of the current action, it updates the probability
/// graph to reflect the number of actions (since it is possible that, for this action,
/// the number of outcomes is not the same as it was for the father)
void updateGraph(const list_float_map &newMetrics, ProbGraph &graph);
/// given the outcome of the current action, it creates the probability
/// graph to reflect the actions; a uniform distribution is used
void createUniformGraph(const list_float_map &newMetrics, ProbGraph &graph);
/// Given the value of the current metric, it updates the probability function
/// of the graph adding the virtual samples
unsigned int addVirtualSample(metric_map &currentMetric, ProbGraph &graph);
/// Given the probability graph it creates the new vertices and returns the probability which has to be associated
/// with the incoming edge. It also update the internal structure of a vertex for what concerns the
/// metric values
std::list<std::pair<VertexInfo *, float> > getNewVertices(ProbGraph &graph);
///Frees all the memory occupied by the current probability graph
void clearProbGraph(ProbGraph *graph);
std::vector<prob_vertex_t> getProbGraphLeaves(ProbGraph &graph);
prob_vertex_t getProbRoot(ProbGraph &graph);
///Eliminates the subtree starting at the specified node and it frees the memory used by it
void clearProbSubtree(prob_vertex_t root, ProbGraph &graph);
///Adds new subtrees to the probability subtree to overcome the fact that changedMetric has not more
///intervals
void addProbSubtrees(const std::string &changedMetric, unsigned int toAdd, ProbGraph &graph);
std::deque<prob_edge_t> orderProbEdges(std::pair<prob_out_edge_iterator, prob_out_edge_iterator> edges, ProbGraph &graph);
///Given a decrease in the number of edges exiting from vertex vertexToChange, I compute the corresponding remaining leaves, the ones
///that have to be eliminated and the subtrees which have to be eliminated
void getProbDecreseInfo(std::string vertexToChange, unsigned int leavesToKeep, std::map<std::string, std::deque<prob_vertex_t> > &remainingLeaves,
                std::map<std::string, std::deque<prob_vertex_t> > &eliminatedLeaves, std::vector<prob_vertex_t> &eliminatedTreeRoot, ProbGraph &graph);
///Given a change in the number of edges exiting from vertexToChange, it gets all the leaves grouping them by the same value
///of all the metrics but vertexToChange, and it orders them for increasing values of the edges exiting from vertexToChange
std::map<std::string, std::deque<prob_vertex_t> > getOrderedLeaves(std::string vertexToChange, ProbGraph &graph);
///Updates the labels of the graph with the new intervals
void updateIntervalLabels(const list_float_map &newMetrics, ProbGraph &graph);
///Checks that the graph is coherent, that the number of metrics for each node is coherent with the number
///of out edges
void checkGraphCoherency(ProbGraph &graph);
}

#endif
