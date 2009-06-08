#ifndef TEST_MASTER
#define TEST_MASTER

#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <boost/lexical_cast.hpp>
#include <string>

class testMaster: public sc_module{
    public:
    tlm_utils::simple_initiator_socket< testMaster, 8 > initSocket;

    SC_HAS_PROCESS(testMaster);
    testMaster(sc_module_name name);

    void sendChars();
};

#endif
