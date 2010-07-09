#ifndef _NOCCOMMON_H_
#define _NOCCOMMON_H_

#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/multi_passthrough_initiator_socket.h>
#include <tlm_utils/multi_passthrough_target_socket.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <queue>
#include <map>

#include "utils.hpp"

using namespace std;
using namespace tlm;
using namespace tlm_utils;

#define DEFAULT_BUFFER_SIZE	1
#define PORTS			5
#define BYTESPERPACKET		4
#define TIMEOUT			100
#define SESSION_ID_OFFSET	1000

//#define DEBUGMODE
#define PRINTNULLGEN

// Routing tables entry
typedef struct Entry {
	unsigned int dstId;
	unsigned int outPort;
	bool active;
} RTEntry;

// Packet types definitions
enum PacketType {
	NO_TYPE=-1, HEAD=0, BODY=1, TAIL=2, DATA_PACKET=3, ACK=4, NACK=5
	//, DSR, DCR, RUPD, force routing, tail+force, routing update
};

// Packet Structure
struct Packet {
	unsigned int src_addr;
	unsigned int dst_addr;
	unsigned int session_id;
	PacketType type;
	unsigned int size;
	unsigned int flit_left;
	list<unsigned int> next_hop;
	tlm_generic_payload* payload;
	double gen_time;

	// fake packet types used in VHDL
	bool force_route;
	bool update_route;
	bool dyn_slave_deletion;
	
	Packet() {
		init((unsigned int)-1);
		force_route = false;
		update_route = false;
		dyn_slave_deletion = false;
	}
	
	void init(unsigned int z){
		make(	z,	// src
			z,	// dst
			z,	// sid
			z,	// size
			z,	// flit_left
			NULL,	// payload
			z);	// gen_time
	}
	
	void make(unsigned int src, unsigned int dst, unsigned int sid, unsigned int s, \
			unsigned int f_l, tlm_generic_payload* p_l, unsigned int g_t){
		src_addr=src; dst_addr=dst; session_id=sid; type=NO_TYPE; size=s; flit_left=f_l;
		next_hop.clear(); payload=p_l; gen_time=g_t;
	}

	inline bool operator == (const Packet& pack) const{
		return (src_addr==pack.src_addr && dst_addr==pack.dst_addr && session_id==pack.session_id && type==pack.type && flit_left==pack.flit_left &&
				payload==pack.payload && gen_time==pack.gen_time && force_route==pack.force_route && update_route==pack.update_route &&
				dyn_slave_deletion==pack.dyn_slave_deletion);
	}

	// used to identify if a packet is != -1 (and so a meaningful one)
	inline bool operator != (const int& intValue) const{
		unsigned int value = (unsigned int) intValue;
		return !(src_addr==value && dst_addr==value && session_id==value &&
			type==NO_TYPE && next_hop.size()==0 && gen_time==value);
	}

	unsigned int get_next_hop(){
		unsigned int n = next_hop.front();
		next_hop.pop_front();
		return n;
	}
};

inline ostream& operator << (ostream& os, const Packet& pack){
	switch(pack.type){
		case DATA_PACKET:
		case ACK:
			os << "Packet";
			break;
		case HEAD:
		case TAIL:
		case BODY:
		case NO_TYPE:
			os << "Flit";
			break;
		default:
			os << "Packet";
			break;
	}
	
	os	<< "[SID: " << pack.session_id << ", left/size: " << pack.flit_left << "/" \
		<< pack.size << ", " << pack.src_addr << "-->" << pack.dst_addr << ", Type: ";
	
	switch(pack.type){
		case DATA_PACKET:
			os << "D_P";
			break;
		case ACK:
			os << "A";
			break;
		case NACK:
			os << "NA";
			break;
		case NO_TYPE:
			os << "N_T";
			break;
		case HEAD:
			os << "H";
			break;
		case TAIL:
			os << "T";
			break;
		case BODY:
			os << "B";
			break;
			
		default:
			os << "X";
			break;
	}
	
	os << "]"; //", PayLoad: " << pack.payload << "]";
	return os;
}

inline void sc_trace(sc_trace_file*& tf, const Packet& pack, const string& name){
	sc_trace(tf, pack.session_id, name+".session_id");
}

inline void sc_trace(sc_trace_file*& tf, const sc_signal<Packet>& sg, const string& name){
	sc_trace(tf, sg, name+"pack_");
}

struct Session {
	unsigned int session_len;
	bool *received;

	Session(unsigned int len) {
		session_len = len;
		received = (bool*) malloc(len*sizeof(bool));
		for (unsigned int i=0; i<len; i++) received[i] = false;
	}

	void newPacket(unsigned int position) {
		if (position <= 0 || position > session_len) 
			THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Received packet " << position << " is out of range [1-" << session_len << "]" << endl);
		received[position-1] = true;
	}

	bool isComplete() {
		bool complete = true;
		for (unsigned int i=0; i<session_len; i++) complete &= received[i];
		return complete;
	}
};

#endif
