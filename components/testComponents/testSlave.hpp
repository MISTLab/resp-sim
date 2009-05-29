#ifndef TEST_SLAVE
#define TEST_SLAVE

#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/simple_target_socket.h>
#include <boost/lexical_cast.hpp>
#include <string>

class testSlave: public sc_module{
    public:
    sc_signal<int> i;
    tlm_utils::simple_target_socket< testSlave, 8 > targetSocket;

    testSlave(sc_module_name name);

    void b_transport(tlm::tlm_generic_payload& trans, sc_time& delay);
};

#endif
