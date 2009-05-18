/**
 * This file contains the methods necessary to access the processor registers
 * and the memory. This file is made for the leon2 functional processor model written
 * using the ArchC language
 */

#ifndef LEON2_HPP
#define LEON2_HPP

#include <string>
#include <systemc.h>

#include "processorInterface.hpp"
#include "leon2.H"

namespace resp{

template <class wordsize> class BreakpointManager;

class leon2Stub : public processorInterface<unsigned int>{
  private:
  leon2 & processor;
  unsigned int verbosityLevel;
  public:
  leon2Stub(leon2 & processor,  unsigned int verbosityLevel = 1);
  /**
   * Returns whether the processor is big-endian.
   *
   * @return false when simulated processor is little-endian, true when big-endian.
   */
  bool get_ac_tgt_endian();

  /**
   * Stops the processor.
   */
  void ac_stop();

  /* Register Support */

  /**
   * How many registers are there in the packet sent to gdb. Check GDB doc.
   *
   * \return how many registers the packet has.
   */  
  unsigned int nRegs(); 
  
  /**
   * Return register value according to GDB stub packet order.
   *
   * \param reg register number according to GDB stub packet order. Check GDB doc.
   *
   * \return register value.
   */
  unsigned int reg_read(int reg,  MySQL *mySQL = NULL,  double curSimTime = sc_time_stamp().to_double());
  
   /**
    * Write register value according to GDB stub packet order.
    *
    * \param reg register number according to GDB stub packet order. Check GDB doc.
    * \param value what to be written.
    */
  void reg_write(int reg, unsigned int value, MySQL *mySQL = NULL, double curSimTime = sc_time_stamp().to_double());
  

  /**
   * Writes address value to the processor ac_pc (Default Program Counter) register.
   *
   * @param value Address value to be written in the ac_pc register.
   */
  void set_ac_pc(unsigned int value,  MySQL *mySQL = NULL, double curSimTime = sc_time_stamp().to_double());

  /* Memory Access *************************************************************/
  
  /**
   * Read memory byte.
   *
   * \param address where to get byte.
   *
   * \return byte from address.
   */
  unsigned char mem_read(unsigned int address,  MySQL *mySQL = NULL,  double curSimTime = sc_time_stamp().to_double());
  
  /**
   * Write memory byte.
   *
   * \param address where to write byte.
   * \param byte what to write.
   */
  void mem_write(unsigned int address, unsigned char byte,  MySQL *mySQL = NULL,  double curSimTime = sc_time_stamp().to_double());
  /**
    * Returns the name of the processor associated with this stub
    */
    std::string getName();

    #ifdef ENABLE_MYSQL
    /**
      * It examines the database looking for the specified event (described by breakpoint);
      * it returns the time at which the event happened if the event happened in the
      * specified time interval,  -1 otherwise.
      */
    double findEvent(Breakpoint<unsigned int> &breakpoint, double startTime, double endTime,  MySQL *mySQL);
    /**
      * It examines the database looking for the next instruction; depending on the direction
      * it could be the one just preceding or the one following
      */
    double getNextInstruction(bool forward, double simTime,  MySQL *mySQL);
    #endif
    
};

}

#endif
