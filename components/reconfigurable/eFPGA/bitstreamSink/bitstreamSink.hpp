/**
 * Bitstream Sink, destination of the configuration bitstream, based on SimpleMemory32
 */
#ifndef BITSTREAMSINK_HPP
#define BITSTREAMSINK_HPP

#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/multi_passthrough_target_socket.h>
#include <boost/lexical_cast.hpp>
#include <string>

#include <utils.hpp>

using namespace std;
using namespace tlm;
using namespace tlm_utils;

template<typename BUSWIDTH> class bitstreamSink: public sc_module {
private:
	sc_event writeFree;
	bool busy;

public:
	multi_passthrough_target_socket<bitstreamSink, sizeof(BUSWIDTH)*8> targetSocket;

	bitstreamSink(sc_module_name module_name) : sc_module(module_name),
			targetSocket((boost::lexical_cast<std::string>(module_name) + "_targSock").c_str()) {
		this->targetSocket.register_b_transport(this, &bitstreamSink::b_transport);
//		this->targetSocket.register_get_direct_mem_ptr(this, &bitstreamSink::get_direct_mem_ptr);
//		this->targetSocket.register_transport_dbg(this, &bitstreamSink::transport_dbg);

		this->busy = false;
		end_module();
	}

	~bitstreamSink(){}

	void b_transport(int tag, tlm_generic_payload& trans, sc_time& delay){
		tlm_command cmd = trans.get_command();
		sc_dt::uint64    adr = trans.get_address();
		unsigned char*   ptr = trans.get_data_ptr();
		unsigned int     len = trans.get_data_length();
		unsigned char*   byt = trans.get_byte_enable_ptr();
		unsigned int     wid = trans.get_streaming_width();

		unsigned int block_size = len / sizeof(BUSWIDTH);
		if (len%sizeof(BUSWIDTH) != 0) block_size++;

		while( this->busy) {
			wait( this->writeFree );
		}
		this->busy = true;

		if (trans.is_write()){
//			cerr << "Sink: Received " << block_size << " words!" << endl;
			trans.set_response_status(TLM_OK_RESPONSE);
		}
		else {
			cerr << "Sink: Unknown request type!" << endl;
			trans.set_response_status(TLM_COMMAND_ERROR_RESPONSE);
		}

		this->busy = false;
		this->writeFree.notify();
	}

};

#endif /*BITSTREAMSINK_H_ */
