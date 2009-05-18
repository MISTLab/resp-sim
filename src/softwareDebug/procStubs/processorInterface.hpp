/**
 * File containing the methods which must be implemented for each processor in order to
 * correctly communicate with the GDB stub; This file takes inspiration from archc's
 * ac_gdb_interface.H
 */

#ifndef PROCESSORINTERFACE_HPP
#define PROCESSORINTERFACE_HPP

#include <string>
#include <systemc.h>

namespace resp{

template <class wordsize> class Breakpoint;
class MySQL;

template <class wordsize> class processorInterface{
  public:
  virtual ~processorInterface(){}
  /**
   * Returns whether the processor is big-endian.
   *
   * @return false when simulated processor is little-endian, true when big-endian.
   */
  virtual bool get_ac_tgt_endian() = 0;

  /**
   * Stops the processor.
   */
  virtual void ac_stop() = 0;

  /* Register Support */

  /**
   * How many registers are there in the packet sent to gdb. Check GDB doc.
   *
   * \return how many registers the packet has.
   */  
  virtual unsigned int nRegs() = 0; 
  
  /**
   * Return register value according to GDB stub packet order.
   *
   * \param reg register number according to GDB stub packet order. Check GDB doc.
   *
   * \return register value.
   */
  virtual wordsize reg_read( int reg,  MySQL *mySQL = NULL, double curSimTime = sc_time_stamp().to_double()) = 0;
  
   /**
    * Write register value according to GDB stub packet order.
    *
    * \param reg register number according to GDB stub packet order. Check GDB doc.
    * \param value what to be written.
    */
  virtual void reg_write( int reg, wordsize value,  MySQL *mySQL = NULL,  double curSimTime = sc_time_stamp().to_double() ) = 0;
  

  /**
   * Writes address value to the processor ac_pc (Default Program Counter) register.
   *
   * @param value Address value to be written in the ac_pc register.
   */
  virtual void set_ac_pc(wordsize value,  MySQL *mySQL = NULL,  double curSimTime = sc_time_stamp().to_double()) = 0;

  /* Memory Access *************************************************************/
  
  /**
   * Read memory byte.
   *
   * \param address where to get byte.
   *
   * \return byte from address.
   */
  virtual unsigned char mem_read(wordsize address,  MySQL *mySQL = NULL,  double curSimTime = sc_time_stamp().to_double()) = 0;
  
  /**
   * Write memory byte.
   *
   * \param address where to write byte.
   * \param byte what to write.
   */
  virtual void mem_write(wordsize address, unsigned char byte,  MySQL *mySQL = NULL,  double curSimTime = sc_time_stamp().to_double()) = 0;
  
  /**
    * Returns the name of the processor associated with this stub
    */
    virtual std::string getName() = 0;
    
    #ifdef ENABLE_MYSQL
    /**
      * It examines the database looking for the specified event (described by breakpoint);
      * it returns the time at which the event happened if the event happened in the
      * specified time interval,  -1 otherwise.
      */
    virtual double findEvent(Breakpoint<unsigned int> &breakpoint, double startTime, double endTime,  MySQL *mySQL) = 0;
    /**
      * It examines the database looking for the next instruction; depending on the direction
      * it could be the one just preceding or the one following
      */
    virtual double getNextInstruction(bool forward, double simTime,  MySQL *mySQL) = 0;
    #endif
};

}

#endif
