#ifndef __PACKETNOC_H__
#define __PACKETNOC_H__

#include "nocCommon.hpp"
#include "Switch.hpp"
#include "slavePE.hpp"
#include "masterPE.hpp"

// Necessary to compile with BoostGraph 1.42
#define BOOST_NO_0X_HDR_INITIALIZER_LIST

#include "tinyxml_libs/tinyxml.hpp"
#include <boost/config.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

using namespace boost;

typedef adjacency_list<vecS, vecS, directedS, no_property, property<edge_weight_t, int> > Graph_t;
typedef graph_traits<Graph_t>::vertex_descriptor Vertex_t;
typedef graph_traits<Graph_t>::vertex_iterator Vertex_it;
typedef graph_traits<Graph_t>::edge_descriptor Edge_t;
typedef graph_traits<Graph_t>::edge_iterator Edge_it;
typedef pair<int, int> Edge_desc;

class packetNoc: public sc_module {
private:
	vector<masterPE*> ms_list;
	unsigned int masters;
	vector<Switch*> sw_list;
	unsigned int switches;
	vector<slavePE*> sl_list;
	unsigned int slaves;
	list< sc_signal<Packet>* > signal_list;
	sc_clock clock;

	// A map associating a set of addresses to a slavePE
	forwardMap_t forwardMap;

	Graph_t networkGraph;
	property_map<Graph_t, edge_weight_t>::type graphWeightMap;

	// Utility function for NOC construction
	masterPE* getMasterWithId(unsigned int dst_id);
	slavePE* getSlaveWithId(unsigned int dst_id);
	Switch* getSwitchWithId(unsigned int dst_id);
	void loadElementsFromXml(TiXmlHandle rootH);
	void loadBindingsFromXml(TiXmlHandle rootH);
	void completeSCBindings();
	void makeLRTs();

	// The internal boundary ports of the NOC. These are used as convenience ports in order to simplify
	// the variable connection of the switches within a parametric NOC
	multi_passthrough_initiator_socket<packetNoc, 32> intInitSocket;
	multi_passthrough_target_socket<packetNoc, 32> intTargetSocket;

public:
	// These ports are the external boundary ports, on which the devices are connected
	// All of their transactions are forwarded to the internal ports, which are going
	// to furtherly forward the requests according to the routing algorithm
	multi_passthrough_target_socket<packetNoc, 32> targetSocket;
	multi_passthrough_initiator_socket<packetNoc, 32> initiatorSocket;
	void b_t1(int tag, tlm_generic_payload& trans, sc_time& delay);
	void b_t2(int tag, tlm_generic_payload& trans, sc_time& delay);
	bool get_direct_mem_ptr(int tag, tlm_generic_payload& trans, tlm_dmi& dmi_data);
	unsigned int t_dbg1(int tag, tlm::tlm_generic_payload& trans);
	unsigned int t_dbg2(int tag, tlm::tlm_generic_payload& trans);

	unsigned int numAccesses;
	unsigned int numWords;

	packetNoc(sc_module_name module_name, string fileName, sc_time cycleLatency);
	~packetNoc();
	void addBinding(unsigned int spe, sc_dt::uint64 startAddress, sc_dt::uint64 endAddress);
	void printBindings();
	void printGRT();
	void printStats();
	unsigned int getFlitsIn(unsigned int elId);
	unsigned int getFlitsOut(unsigned int elId);
	unsigned int getTimeouts(unsigned int masterId);
	unsigned int getAllTimeouts();
	unsigned int getDropped(unsigned int switchId);
	unsigned int getBufferSize(unsigned int switchId);
	unsigned int getAllDropped();
	void changeTimeout(unsigned int masterId, sc_time tO);
	void changeAllTimeouts(sc_time tO);
	void changeBufferSize(unsigned int switchId, unsigned int size);
	void changeAllBufferSizes(unsigned int size);
	void changePath(unsigned int switchId, unsigned int destinationId, unsigned int nextHop);
	
	std::vector<unsigned int> getSwitchIds();
};

#endif

