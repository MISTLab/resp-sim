

#ifndef MYTIMER_HPP
#define MYTIMER_HPP

#include <iostream>

#include <systemc.h>
#include <utils.hpp>
#include <boost/lexical_cast.hpp>
#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>


//Now some stuff for generating random numbers
// #include <ctime>
// #include <boost/random/linear_congruential.hpp>
// #include <boost/random/uniform_int.hpp>
// #include <boost/random/uniform_real.hpp>
// #include <boost/random/variate_generator.hpp>


#ifdef BOOST_NO_STDC_NAMESPACE
namespace std {
  using ::time;
}
#endif


///Define the positions of all the registers in the timer internal address space
// #define TIMER_LOAD 0 //< regsister for loading the timer initial value
// #define TIMER_VALUE 1 //< current timer value
// #define TIMER_CONTROL 2 //< timer control register
// #define TIMER_CLEAR 3 //< Clear interrupt register

class MyTimer : public sc_module{
private:
  sc_time latency;
  int lastIrq;

  // Random number generator to be used during algorithm execution
  //   boost::minstd_rand generator;
  
  ///Defines the fields of the control register
  //    struct control_reg_type{
  //      bool enable;
  //      bool periodic;
  //      unsigned char prescale;
  //    };


  ///Keeps track of the current value of the timer
  int currentValue;
  ///Keeps track of the value initially loaded into the timer
  //int loadValue;
  ///Control register of the timer
  //control_reg_type controlReg;
  ///Buffer counter used to implement prescaling facilities
  // unsigned char prescaleCounter;
  //    unsigned int verbosityLevel;


  //    void initTimer();

  //    int serveReadReq(int addr);
  //    void serveWriteReq(int addr, int datum);


public:
  

//master
  tlm_utils::simple_initiator_socket< MyTimer, 32 > initSocket;
  //slave
  tlm_utils::simple_target_socket< MyTimer, 32 > targetSocket;
   
  SC_HAS_PROCESS(MyTimer);


  MyTimer(sc_module_name name, sc_time latency) : 
    sc_module(name), 
    targetSocket(("timer_target_" + boost::lexical_cast<std::string>(name)).c_str()),
    latency(latency){
     

    this->currentValue=0;
    this->targetSocket.register_b_transport(this, &MyTimer::b_transport);
    SC_THREAD(countTime);     
    end_module();
  }

  
  void b_transport(tlm::tlm_generic_payload& trans, sc_time& delay){
    if(this->lastIrq < 0){
      THROW_EXCEPTION("Error, lowering an interrupt which hasn't been raised yet!!");
    }

  
    tlm::tlm_command cmd = trans.get_command();
    sc_dt::uint64    adr = trans.get_address();
    unsigned char*   ptr = trans.get_data_ptr();
  
    if(trans.get_command() == tlm::TLM_READ_COMMAND){
      THROW_EXCEPTION("Error, the read request is not currently supported by external PINs");
    }else if(cmd == tlm::TLM_WRITE_COMMAND){
 
      if(this->lastIrq != adr){
	THROW_EXCEPTION("Error, lowering interrupt " << std::hex << std::showbase << (unsigned int)adr << " while " << std::hex << std::showbase << this->lastIrq << " was raised");
      }else{
	//finally I can really lower the interrupt: I send 0 on
	//the initSocked
	unsigned char data = 0;
	trans.set_data_ptr(&data);
	trans.set_dmi_allowed(false);
	trans.set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );
	sc_time delay;


	this->initSocket->b_transport(trans, delay);
 
	if(trans.is_response_error()){
	  std::string errorStr("Error in b_transport of PIN, response status = " + trans.get_response_string());
	  SC_REPORT_ERROR("TLM-2", errorStr.c_str());
	}
      
	this->lastIrq = -1;
      }
    }
  
  
    trans.set_response_status(tlm::TLM_OK_RESPONSE);
  }






  ///Counts the time, incrementing the clock counts at every cycle
  void countTime(){
    while(true){


      if(this->lastIrq == -1){

      
	unsigned char data = 1;
	tlm::tlm_generic_payload trans;
	//boost::uniform_int<> degen_dist(0x1, 0xe);
	// boost::variate_generator<boost::minstd_rand&, boost::uniform_int<> > deg(this->generator, degen_dist);
	this->lastIrq = this->currentValue;
	  
	this->currentValue++;

	std::cerr << "Sending out IRQ id=" << std::hex << std::showbase << this->lastIrq << std::endl;
	trans.set_address(this->lastIrq);
	trans.set_data_ptr(&data);
	trans.set_data_length(0);
	trans.set_byte_enable_ptr(0);
	trans.set_dmi_allowed(false);
	trans.set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );
	sc_time delay;
	this->initSocket->b_transport(trans, delay);
	if(trans.is_response_error()){
	  std::string errorStr("Error in generateIrq, response status = " + trans.get_response_string());
	  SC_REPORT_ERROR("TLM-2", errorStr.c_str());
	}
      }
      wait(this->latency);
    }
  }



   

  //tlm::tlm_transport_if<initSocket, targetSocket> timerIf_type;





  //void countTime();
};




#endif
