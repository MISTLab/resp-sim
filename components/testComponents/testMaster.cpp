#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/simple_target_socket.h>
#include <boost/lexical_cast.hpp>
#include <string>

#include "testMaster.hpp"

testMaster::testMaster(sc_module_name name) : sc_module(name), initSocket((boost::lexical_cast<std::string>(name) + "_port").c_str()){
    SC_THREAD(sendChars);
    srand((unsigned)time(0));
    end_module();
}

void testMaster::sendChars(){
    unsigned char datum = (unsigned char)(rand()%255);
    tlm::tlm_generic_payload trans;
    sc_time delay;
    trans.set_address(address);
    trans.set_write();
    trans.set_data_ptr(&datum);
    trans.set_data_length(sizeof(datum));
    trans.set_byte_enable_ptr(0);
    trans.set_dmi_allowed(false);
    trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
    this->initSocket->b_transport(trans, delay);

    if(trans.is_response_error()){
        std::cerr << "testMaster: Error response from the TLM port" << std::endl;
    }
}
