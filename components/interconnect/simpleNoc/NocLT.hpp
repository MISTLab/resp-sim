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
 *
 *   The following code is derived, directly or indirectly, from the SystemC
 *   source code Copyright (c) 1996-2004 by all Contributors.
 *   All Rights reserved.
 *
 *   The contents of this file are subject to the restrictions and limitations
 *   set forth in the SystemC Open Source License Version 2.4 (the "License");
 *   You may not use this file except in compliance with such restrictions and
 *   limitations. You may obtain instructions on how to receive a copy of the
 *   License at http://www.systemc.org/. Software distributed by Contributors
 *   under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 *   ANY KIND, either express or implied. See the License for the specific
 *   language governing rights and limitations under the License.
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
 *   Component developed by: Fabio Arlati arlati.fabio@gmail.com
 *
\***************************************************************************/

#ifndef NOCLT_HPP
#define NOCLT_HPP

#include "SwitchLT.hpp"

#include <boost/config.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

#include <vector>

using namespace boost;
using namespace std;
using namespace tlm;
using namespace tlm_utils;

typedef adjacency_list<vecS, vecS, undirectedS, no_property, property<edge_weight_t, int> > Graph_t;
typedef graph_traits<Graph_t>::vertex_descriptor Vertex_t;
typedef graph_traits<Graph_t>::vertex_iterator Vertex_it;
typedef graph_traits<Graph_t>::edge_descriptor Edge_t;
typedef graph_traits<Graph_t>::edge_iterator Edge_it;
typedef pair<int, int> Edge_desc;

template<typename BUSWIDTH> class NocLT: public sc_module {
private:
	// A map containing a list of switches associated to their internal tag
	typedef map<unsigned int, SwitchLT<BUSWIDTH>* > switchIdMap_t;
	typedef typename switchIdMap_t::iterator switchIdMap_iter;
	switchIdMap_t switches;
	sc_time switchLatency;
	bool locking;
	unsigned int maxSwitchQueue;

	// A utility graph used to calculate the NOC switching paths
	Graph_t networkGraph;
	property_map<Graph_t, edge_weight_t>::type graphWeightMap;

	// A map associating a set of addresses to a switch (which should be directly connected to a slave)
	map<unsigned int, addressSet > outMap;

	TopologyType topo;
	unsigned int nextSlaveToBind;
	unsigned int numMasters;
	unsigned int numSlaves;

	// The internal boundary ports of the NOC. These are used as convenience ports in order to simplify
	// the variable connection of the switches within a parametric NOC
	multi_passthrough_initiator_socket<NocLT, sizeof(BUSWIDTH)*8> intInitSocket;
	multi_passthrough_target_socket<NocLT, sizeof(BUSWIDTH)*8> intTargetSocket;

	unsigned int buildTree(unsigned int startSwitch, unsigned int endSwitch, Edge_desc* edges, int* weights, unsigned int *edgeCounter) {

		if (startSwitch==endSwitch) return 0;

		SwitchLT<BUSWIDTH>* allocSwitch;
		unsigned int nextTag = endSwitch+1;
		unsigned int numEdges;
		unsigned int extraSwitchCounter = 0;
//		cerr << "Building a level of the tree between switch #" << startSwitch << " and switch #" << endSwitch << endl;
		for (numEdges=startSwitch; numEdges <= endSwitch; numEdges+=2) {
//			cerr << "Allocating switch #" << nextTag << endl;
			allocSwitch = new SwitchLT<BUSWIDTH>(nextTag,this->switchLatency,locking,maxSwitchQueue);
			switches[nextTag] = allocSwitch;
			extraSwitchCounter++;

//			cerr << "Linking switch #" << numEdges << " to switch #" << nextTag << endl;
			switches[numEdges]->initSocketBind(switches[nextTag]->targetSocket,nextTag);
			switches[nextTag]->initSocketBind(switches[numEdges]->targetSocket,numEdges);
			edges[*edgeCounter] = Edge_desc(numEdges,nextTag);
			weights[*edgeCounter] = 1;
			(*edgeCounter)++;

			if (numEdges+1 <= endSwitch) {
//				cout << "Linking switch #" << numEdges+1 << " to switch #" << nextTag << endl;
				switches[numEdges+1]->initSocketBind(switches[nextTag]->targetSocket,nextTag);
				switches[nextTag]->initSocketBind(switches[numEdges+1]->targetSocket,numEdges+1);
				edges[*edgeCounter] = Edge_desc(numEdges+1,nextTag);
				weights[*edgeCounter] = 1;
				(*edgeCounter)++;
			}
			nextTag++;
		}
		extraSwitchCounter += buildTree(endSwitch+1,nextTag-1,edges,weights,edgeCounter);
		return extraSwitchCounter;
	}

	void makeNocMap() {
		Edge_desc* edges;
		int* weights;
		unsigned int numEdges = 0,extraSwitches = 0, edgeCounter = 0;
		unsigned int centralTag = numMasters+numSlaves+1;

		SwitchLT<BUSWIDTH>* allocSwitch;
		for (unsigned int numSwitches = 1; numSwitches <= numMasters+numSlaves; numSwitches++) {
			allocSwitch = new SwitchLT<BUSWIDTH>(numSwitches,this->switchLatency,locking,maxSwitchQueue);
			switches[numSwitches] = allocSwitch;
		}

		for (unsigned int nM = 1; nM <= numMasters; nM++) {
			intInitSocket.bind(switches[nM]->targetSocket);
		}

		for (unsigned int nS = 1; nS <= numSlaves; nS++) {
			switches[numMasters+nS]->initSocketBind(intTargetSocket,0);
		}

		switch (topo) {
		case RING:
			edges = (Edge_desc*) malloc( (numMasters+numSlaves+1) * sizeof(Edge_desc) );
			weights = (int*) malloc( (numMasters+numSlaves+1) * sizeof(int) );

			// Adding backward edges (the internal Graph in not directed, so these include forward edges)
			for (numEdges = 1; numEdges < numMasters+numSlaves; numEdges++) {
				// Adding forward edge
				switches[numEdges]->initSocketBind(switches[numEdges+1]->targetSocket,numEdges+1);
				// Addign backward edge
				switches[numEdges+1]->initSocketBind(switches[numEdges]->targetSocket,numEdges);

				// Adding edge to the internal graph
				edges[numEdges-1] = Edge_desc(numEdges,numEdges+1);
				weights[numEdges-1] = 1;
			}
			// Adding final forward edge
			switches[numMasters+numSlaves]->initSocketBind(switches[1]->targetSocket,1);
			// Adding final backward edge
			switches[1]->initSocketBind(switches[numMasters+numSlaves]->targetSocket,numMasters+numSlaves);
			// Adding final edge to the internal graph
			edges[numEdges-1] = Edge_desc(numEdges,1);
			weights[numEdges-1] = 1;
			numEdges++;
			break;
		case STAR:
			allocSwitch = new SwitchLT<BUSWIDTH>(centralTag,this->switchLatency,locking,maxSwitchQueue);
			switches[centralTag] = allocSwitch;

			edges = (Edge_desc*) malloc( (numMasters+numSlaves+1) * sizeof(Edge_desc) );
			weights = (int*) malloc( (numMasters+numSlaves+1) * sizeof(int) );

			for (numEdges = 1; numEdges < numMasters+numSlaves+1; numEdges++) {
				// Adding forward edge to the central switch
				switches[numEdges]->initSocketBind(switches[centralTag]->targetSocket,centralTag);
				// Adding backward edge from the central switch
				switches[centralTag]->initSocketBind(switches[numEdges]->targetSocket,numEdges);

				// Adding edge to the internal graph
				edges[numEdges-1] = Edge_desc(numEdges,centralTag);
				weights[numEdges-1] = 1;
			}
			extraSwitches++;
			break;
		case MESHFC:
			numEdges = 0;
			for (unsigned int i=1; i<numMasters+numSlaves; i++) numEdges+=i;
			edges = (Edge_desc*) malloc( numEdges * sizeof(Edge_desc) );
			weights = (int*) malloc( numEdges * sizeof(int) );

			for (numEdges = 1; numEdges < numMasters+numSlaves+1; numEdges++) {
				for (unsigned int numEdges2 = numEdges; numEdges2 < numMasters+numSlaves+1; numEdges2++) {
					// If the switches have different tags... (we don't want to insert autoloops)
					if (numEdges==numEdges2) continue;

					// Adding forward edge between each couple of switches
					switches[numEdges]->initSocketBind(switches[numEdges2]->targetSocket,numEdges2);
					// Adding backward edge between each couple of switches
					switches[numEdges2]->initSocketBind(switches[numEdges]->targetSocket,numEdges);

					// Adding edge to the internal graph
					edges[edgeCounter] = Edge_desc(numEdges,numEdges2);
					weights[edgeCounter] = 1;
					edgeCounter++;
				}
			}
			numEdges = edgeCounter;
			break;
		case TREE:
			edgeCounter = numMasters+numSlaves;
			if (edgeCounter%2 != 0) edgeCounter++;
			edges = (Edge_desc*) malloc( 2 * (edgeCounter-1) * sizeof(Edge_desc) );
			weights = (int*) malloc( 2 * (edgeCounter-1) * sizeof(int) );
			numEdges = 0;
			extraSwitches = buildTree(1,numMasters+numSlaves,edges,weights,&numEdges);
			break;
		default:
			THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": The given topology is unknown, NOC cannot be created");
		}
		networkGraph = Graph_t(edges, edges + numEdges, weights, numMasters+numSlaves+extraSwitches);
		graphWeightMap = get(edge_weight, networkGraph);

		resetMaps();
	}

	// This method adds to each switch the routing paths concerning the given slave device
	void updateMaps(unsigned int slave) {
		vector<Vertex_t> p(num_vertices(networkGraph));
		vector<int> d(num_vertices(networkGraph));
		Vertex_it vi, vend;
		Vertex_t s;

		// First, we check if the requested slave is bound to a set of addresses
		if (outMap.count(slave) == 0) {}
//			cerr << "No binding for output port attached to " << slave << endl;
		else {
			// Then, if it is, we calculate for each switch the shortest path to the slave
			s = vertex(slave, networkGraph);
			dijkstra_shortest_paths(networkGraph, s, predecessor_map(&p[0]).distance_map(&d[0]));

			// Finally, for each switch we have to update the internal routing map
			for (tie(vi, vend) = vertices(networkGraph); vi != vend; ++vi) {
				// Vertex 0 is not used in our graph...
				if (*vi == 0) continue;
				if (d[*vi]!=0)
					switches[*vi]->addRoutingPath(outMap[slave].first, outMap[slave].second, p[*vi]);
				else
					// If the graph distance is 0, then we're processing the slave component itself
					// In this case, the request should be forwarded to 0 (which is the outbound port)
					switches[*vi]->addRoutingPath(outMap[slave].first, outMap[slave].second, 0);
//				cerr << "Distance(" << *vi << ") = " << d[*vi] << ", ";
//				cerr << "Parent(" << *vi << ") = " << p[*vi] << endl;
			}
		}
	}

	// This method recalculates the routing maps for each switch starting from scratch
	void resetMaps() {
		// First, we delete the routing map of each switch
		for (typename switchIdMap_t::iterator sIiter = switches.begin(); sIiter != switches.end(); sIiter++) {
			(sIiter->second)->clearRoutingMap();
		}

		// Then, we rebuild the maps processing each one of the switch connected to a slave device
		for (unsigned int i = numMasters+1; i <= numMasters+numSlaves; i++) {
			updateMaps(i);
		}
	}

public:
	// These ports are the external boudnary ports, on which the devices are connected
	// All of their transactions are forwarded to the internal ports, which are going
	// to furtherly forward the requests according to the routing algorithm
	multi_passthrough_target_socket<NocLT, sizeof(BUSWIDTH)*8> targetSocket;
	multi_passthrough_initiator_socket<NocLT, sizeof(BUSWIDTH)*8> initiatorSocket;

	unsigned int numAccesses;
	unsigned int numWords;

	NocLT(sc_module_name module_name, unsigned int nM, unsigned int nS, TopologyType t, sc_time lat = SC_ZERO_TIME, bool isLocking = true, unsigned int maxQueue = 100): sc_module(module_name),
			targetSocket((boost::lexical_cast<std::string>(module_name) + "_ExtTargSock").c_str()), numMasters(nM), numSlaves(nS), topo(t),
			initiatorSocket((boost::lexical_cast<std::string>(module_name) + "_ExtInitSock").c_str()), nextSlaveToBind(nM+1), switchLatency(lat),
			intTargetSocket((boost::lexical_cast<std::string>(module_name) + "_IntTargSock").c_str()), locking(isLocking), maxSwitchQueue(maxQueue),
			intInitSocket((boost::lexical_cast<std::string>(module_name) + "_IntInitSock").c_str()) {
		// Registering the transports used to forward requests between external and internal ports
		this->targetSocket.register_b_transport(this, &NocLT::b_t1);
		this->intTargetSocket.register_b_transport(this, &NocLT::b_t2);
		this->targetSocket.register_get_direct_mem_ptr(this, &NocLT::get_direct_mem_ptr);
		this->intTargetSocket.register_get_direct_mem_ptr(this, &NocLT::get_direct_mem_ptr);
		this->targetSocket.register_transport_dbg(this, &NocLT::t_dbg1);
		this->intTargetSocket.register_transport_dbg(this, &NocLT::t_dbg2);

		numAccesses = 0;
		numWords = 0;

		// Calling the function that creates the internal NOC connections and support variables (boost graph, etc.)
		this->makeNocMap();
		end_module();
	}

	void b_t1(int tag, tlm_generic_payload& trans, sc_time& delay){
//		cerr << "Request entering NOC" << endl;

		unsigned int len = trans.get_data_length();
		unsigned int words = len / sizeof(BUSWIDTH);
		this->numAccesses++;
		this->numWords+=words;

		intInitSocket[tag]->b_transport(trans, delay);
		trans.set_dmi_allowed(false);			// Disables DMI in order to insert the NOC latency for each transaction
	}

	void b_t2(int tag, tlm_generic_payload& trans, sc_time& delay){
		initiatorSocket[tag]->b_transport(trans, delay);

//		cerr << "Request exiting NOC" << endl;
	}

	bool get_direct_mem_ptr(int tag, tlm_generic_payload& trans, tlm_dmi& dmi_data){
		cout << "DMI not supported for NOC transactions! DMI requested by " << tag << endl;
		return false;
	}

	unsigned int t_dbg1(int tag, tlm::tlm_generic_payload& trans) {
//		cerr << "DBG Request entering NOC" << endl;
		return intInitSocket[tag]->transport_dbg(trans);
	}

	unsigned int t_dbg2(int tag, tlm::tlm_generic_payload& trans) {
		unsigned int retVal = initiatorSocket[tag]->transport_dbg(trans);
//		cerr << "DBG Request exiting NOC" << endl;
		return retVal;
	}

	void addBinding(sc_dt::uint64 startAddress, sc_dt::uint64 endAddress) {
		if (startAddress > endAddress || (nextSlaveToBind-numMasters) > numSlaves) {cout << "Error!" << endl; return;}
		this->outMap[this->nextSlaveToBind].first = startAddress;
		this->outMap[this->nextSlaveToBind].second = endAddress;
		this->updateMaps(this->nextSlaveToBind);
//		this->resetMaps();
		this->nextSlaveToBind++;
	}

	void printAccesses() {
		cout << "The entire NOC had " << numAccesses << " accesses, for a total amount of " << numWords << " words" << endl;
		switchIdMap_iter swIter;
		for (swIter = switches.begin(); swIter != switches.end(); swIter++) {
			(swIter->second)->printAccesses();
		}
	}

};

#endif
