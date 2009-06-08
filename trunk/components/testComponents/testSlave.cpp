#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/simple_target_socket.h>
#include <boost/lexical_cast.hpp>
#include <string>

#include "testSlave.hpp"

testSlave::testSlave(sc_module_name name) : sc_module(name), targetSocket((boost::lexical_cast<std::string>(name) + "_port").c_str()){
    this->targetSocket.register_b_transport(this, &testSlave::b_transport);
    end_module();
}

void testSlave::b_transport(tlm::tlm_generic_payload& trans, sc_time& delay){
    tlm::tlm_command cmd = trans.get_command();
    sc_dt::uint64    adr = trans.get_address();
    unsigned char*   ptr = trans.get_data_ptr();
    unsigned int     len = trans.get_data_length();
    unsigned char*   byt = trans.get_byte_enable_ptr();
    unsigned int     wid = trans.get_streaming_width();

    if(trans.get_command() == tlm::TLM_READ_COMMAND){
        std::cerr << "Error, the read request is not currently supported by testSlave" << std::endl;
    }
    else if(cmd == tlm::TLM_WRITE_COMMAND){
        std::cout << "testSlave: received character --> " << *ptr << std::endl;
    }

    trans.set_response_status(tlm::TLM_OK_RESPONSE);
}
