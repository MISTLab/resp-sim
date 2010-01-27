/**
 * Bitstream Sink, destination of the configuration bitstream, based on SimpleMemory32
 */
#ifndef BITSTREAMSINK_HPP
#define BITSTREAMSINK_HPP

#include <stdlib.h>
#include <string>

#include <tlm.h>
#include <systemc.h>

using namespace tlm;
using namespace std;

template <class dataType> class bitstreamSink : public sc_module,
                     public tlm_transport_if <	tlm_request<dataType, dataType>,
						tlm_response<dataType> > {

public:
	typedef tlm_request<dataType, dataType> memRequest_type;
	typedef tlm_response<dataType> memResponse_type;
	typedef tlm_transport_if<memRequest_type, memResponse_type> memIf_type;
	typedef tlm_target_port<memIf_type> memTargPort_type;

	//This is the port receiving the bitstream (uses the same types of a memory target port)
	memTargPort_type configPort;
	string name;

	sc_event writeFree;
	bool busy;

	// Constructor.
	bitstreamSink (sc_module_name n): sc_module(n), configPort( (string(n) + "_configPort").c_str() ), name(n){
		configPort(*this);
		this->busy = false;
		end_module();
	}

	//Transport methods
	memResponse_type transport(const memRequest_type& request){
		tlm_response<dataType> response;
		this->transport(request, response);
		return response;
	}

	void transport(const memRequest_type& request, memResponse_type& response){

		int block_size = request.get_block_size();

		while( this->busy  && !request.is_control_access() ) {
			wait( this->writeFree );
		}

		if( !request.is_control_access() )
			this->busy = true;
        
		if (request.get_command() == WRITE){
			//cerr << "Sink: Received " << block_size << " words!" << endl;
		}
		else {
			cerr << "Sink: Unknown request type!" << endl;
			response.get_status().set_error();
			response.get_status().set_no_response();
			this->busy = false;
			this->writeFree.notify();
			return;
		}

		response.get_status().set_ok();

		if( !request.is_control_access() ) {
			this->busy = false;
			this->writeFree.notify();
		}
	}

};

#endif /*BITSTREAMSINK_H_ */
