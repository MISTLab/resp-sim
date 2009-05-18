/**
* This file contains the methods necessary to access the processor registers
* and the memory. This file is made for the arm7tdmi functional and cycle accurate 
* processor models written using the ArchC language
*/

#include <systemc.h>
#include <string>

#include "arm7ca.H"
#include "arm7tdmica.hpp"
#include "processorInterface.hpp"

#include "utils.hpp"

#include "BreakpointManager.hpp"
#include "MySQL.hpp"


using namespace resp;

arm7caStub::arm7caStub(arm7ca & processor,  unsigned int verbosityLevel) : processor(processor),  verbosityLevel(verbosityLevel){}

/**
 * Returns whether the processor is big-endian.
 *
 * @return false when simulated processor is little-endian, true when big-endian.
 */
bool arm7caStub::get_ac_tgt_endian(){
    #ifdef LITTLE_ENDIAN_BO
    #ifndef NDEBUG
    if(verbosityLevel >= 2)
        std::cerr << "GDB-WRAPPER " << __PRETTY_FUNCTION__ << " Same Endianess of host" << std::endl;
    #endif
    return false;
    #else
    #ifdef BIG_ENDIAN_BO
    #ifndef NDEBUG
    if(verbosityLevel >= 2)
        std::cerr << "GDB-WRAPPER " << __PRETTY_FUNCTION__ << " Endianess conflicting with host" << std::endl;
    #endif
    return true;
    #else
    #ifndef NDEBUG
    if(verbosityLevel >= 2)
        std::cerr << "GDB-WRAPPER " << __PRETTY_FUNCTION__ << " Cannot determine host endianess" << std::endl;
    #endif
    return false;
    #endif
    #endif
}

/**
 * Stops the processor.
 */
void arm7caStub::ac_stop(){
   #ifndef NDEBUG
   if(verbosityLevel >= 2)
      std::cerr << "GDB-WRAPPER " << __PRETTY_FUNCTION__ << std::endl;
   #endif
   this->processor.stop(0);
}

/* Register Support */

/**
 * How many registers are there in the packet sent to gdb. Check GDB doc.
 *
 * \return how many registers the packet has.
 */  
unsigned int arm7caStub::nRegs(){
   #ifndef NDEBUG
   if(verbosityLevel >= 2)
      std::cerr << "GDB-WRAPPER " << __PRETTY_FUNCTION__ << ": determining number of registers" << std::endl;
   #endif
   return 26;
}
  
/**
 * Return register value according to GDB stub packet order.
 *
 * \param reg register number according to GDB stub packet order. Check GDB doc.
 *
 * \return register value.
 */
unsigned int arm7caStub::reg_read(int reg, MySQL *mySQL, double curSimTime){
    #ifdef ENABLE_MYSQL
    if(curSimTime < sc_time_stamp().to_double()){
        //Ok,  I'm trying to perform debug in the past, so I need to access the database;
        //I have to read the last value of the register before curSimTime (included)
        #ifndef NDEBUG
        if(verbosityLevel >= 2)
            std::cerr << "GDB-WRAPPER " << __PRETTY_FUNCTION__ << ": reading register " << reg << " at time " << curSimTime << std::endl;
        #endif
        try{
            SingleEvent ev;
            if(reg < 15)
                ev = mySQL->getDeviceEventAddr(curSimTime, this->processor.RB.sub_deviceId, reg);
            else if(reg == 15)
                ev = mySQL->getDeviceEventAddr(curSimTime, this->processor.ac_pc.sub_deviceId, 0);
            else
                ev = mySQL->getDeviceEventAddr(curSimTime, this->processor.PSR.sub_deviceId, 0);
            return ev.value;
        }
        catch(...){
            #ifndef NDEBUG
            std::cerr << "GDB-WRAPPER " << __PRETTY_FUNCTION__ << ": Exception occurred during the reading of register " << reg << " from the database" <<std::endl;
            #endif
           return 0;
        }
    }
    else{
    #endif
    #ifndef NDEBUG
    if(verbosityLevel >= 2)
        std::cerr << "GDB-WRAPPER " << __PRETTY_FUNCTION__ << ": reading register " << reg << std::endl;
    #endif

    if(reg < 15)
        return this->processor.RB.read(reg);
    else if(reg == 15)
        return (unsigned int)((int)this->processor.RB.read(reg) < 0 ? 0 : (int)this->processor.RB.read(reg));
    else if(reg == 25)
        return this->processor.PSR.read(0);
    else
        return 0;
    #ifdef ENABLE_MYSQL
    }
    #endif
}
  
/**
* Write register value according to GDB stub packet order.
* \param reg register number according to GDB stub packet order. Check GDB doc.
* \param value what to be written.
*/
void arm7caStub::reg_write(int reg, unsigned int value, MySQL *mySQL, double curSimTime){
    #ifdef ENABLE_MYSQL
    if(curSimTime < sc_time_stamp().to_double()){
        THROW_EXCEPTION("Modifying the past simulation status not yet allowed");
    }
    else{
     #endif
   #ifndef NDEBUG
   if(verbosityLevel >= 2)
      std::cerr << "GDB-WRAPPER " << __PRETTY_FUNCTION__ << ": writing register " << reg << " with value " << value << std::endl;
   #endif
   if(reg < 15)
      this->processor.RB.write(reg, value);
   else if(reg == 15){
      //I have to write the program counter
      this->set_ac_pc(value);
      this->processor.RB.write(reg, value + 4);
      this->processor.MAINPIPE_IF.flush();
      this->processor.MAINPIPE_ID.flush();
   }
   else if(reg == 25)
      return this->processor.PSR.write(0, value);
    #ifdef ENABLE_MYSQL
    }
    #endif
}
  

/**
 * Writes address value to the processor ac_pc (Default Program Counter) register.
 *
 * @param value Address value to be written in the ac_pc register.
 */
void arm7caStub::set_ac_pc(unsigned int value, MySQL *mySQL, double curSimTime){
    #ifdef ENABLE_MYSQL
    if(curSimTime < sc_time_stamp().to_double()){
        THROW_EXCEPTION("Modifying the past simulation status not yet allowed");
    }
    else{
     #endif
   #ifndef NDEBUG
   if(verbosityLevel >= 2)
      std::cerr << "GDB-WRAPPER " << __PRETTY_FUNCTION__ << ": writing " << value << " into PC" << std::endl;
   #endif
   this->processor.ac_pc = value;
   this->processor.RB.write(15, value + 4);
    #ifdef ENABLE_MYSQL
    }
    #endif
}

/* Memory Access *************************************************************/
  
/**
 * Read memory byte.
 *
 * \param address where to get byte.
 *
 * \return byte from address.
 */
unsigned char arm7caStub::mem_read(unsigned int address, MySQL *mySQL, double curSimTime){
    #ifdef ENABLE_MYSQL
    if(curSimTime < sc_time_stamp().to_double()){
        //Ok,  I'm trying to perform debug in the past, so I need to access the database;
        //I have to read the last value of the register before curSimTime (included)
        #ifndef NDEBUG
        if(verbosityLevel >= 2)
            std::cerr << "GDB-WRAPPER " << __PRETTY_FUNCTION__ << ": reading mem address " << address << " at time " << curSimTime << std::endl;
        #endif
        try{
            SingleEvent ev = mySQL->getDeviceEventAddr(curSimTime, this->processor.DATA_MEM_port.sub_deviceId, address);
            return ev.value;
        }
        catch(...){
            #ifndef NDEBUG
            std::cerr << "GDB-WRAPPER " << __PRETTY_FUNCTION__ << ": Exception occurred during the reading of memory address " << address << " from the database" <<std::endl;
            #endif
           return 0;
        }
    }
    else{
    #endif
   #ifndef NDEBUG
   if(verbosityLevel >= 2)
      std::cerr << "GDB-WRAPPER " << __PRETTY_FUNCTION__ << ": reading mem address " << address << std::endl;
   #endif

   return this->processor.DATA_MEM.read_byte(address);
    #ifdef ENABLE_MYSQL
    }
    #endif
}
  
/**
 * Write memory byte.
 *
 * \param address where to write byte.
 * \param byte what to write.
 */
void arm7caStub::mem_write(unsigned int address, unsigned char byte, MySQL *mySQL, double curSimTime){
    #ifdef ENABLE_MYSQL
    if(curSimTime < sc_time_stamp().to_double()){
        THROW_EXCEPTION("Modifying the past simulation status not yet allowed");
    }
    else{
     #endif
   #ifndef NDEBUG
   if(verbosityLevel >= 2)
      std::cerr << "GDB-WRAPPER " << __PRETTY_FUNCTION__ << ": writing " << byte << " into mem address " << address << std::endl;
   #endif

   this->processor.DATA_MEM.write_byte(address, byte);
    #ifdef ENABLE_MYSQL
    }
    #endif
}

/**
* Returns the name of the processor associated with this stub
*/
std::string arm7caStub::getName(){
    std::string retVal(dynamic_cast<sc_object &>(processor).name());
    return retVal;
}

#ifdef ENABLE_MYSQL
/**
  * It examines the database looking for the specified event (described by breakpoint);
  * it returns the time at which the event happened if the event happened in the
  * specified time interval,  -1 otherwise.
  */
double arm7caStub::findEvent(Breakpoint<unsigned int> &breakpoint, double startTime, double endTime, MySQL *mySQL){
    //Ok,  I have to query the database in order to determine if the memory location
    //specified by breakpoint was modified (and I stop because of a watch)
    //or if the program counter assumed this value (and I stop because of a beakpoint);
    //Note that I look for the first value among startTime and endTime (in this order,  which
    //means the closest to startTime smaller if going back,  greater if going forward)
    try{
        SingleEvent ev;
        if(breakpoint.type == Breakpoint<unsigned int>::MEM || breakpoint.type == Breakpoint<unsigned int>::HW){
            //It is a breakpoint,  i have to check the value of the program counter this->processor.ac_pc.sub_deviceId
            ev = mySQL->getDeviceEventVal(startTime, this->processor.ac_pc.sub_deviceId, breakpoint.address, 0, endTime);
        }
        else if(breakpoint.type == Breakpoint<unsigned int>::WRITE || breakpoint.type == Breakpoint<unsigned int>::ACCESS){
            //It is a breakpoint,  i have to check the value of the program counter this->processor.ac_pc.sub_deviceId
            ev = mySQL->getDeviceEventAddr(startTime, this->processor.DATA_MEM_port.sub_deviceId, breakpoint.address, endTime);
        }
        else
            std::cerr << "READ watch not yet supported by the timed debugger" << std::endl;
        return ev.time;
    }
    catch(...){
        #ifndef NDEBUG
        std::cerr << "GDB-WRAPPER " << __PRETTY_FUNCTION__ << ": Exception occurred during the check of the breakpoints from the database" <<std::endl;
        #endif
        return -1;
    }
    return -1;
}

/**
  * It examines the database looking for the next instruction; depending on the direction
  * it could be the one just preceding or the one following
  */
double arm7caStub::getNextInstruction(bool forward, double simTime, MySQL *mySQL){
    return mySQL->getNextEvent(forward, this->processor.ac_pc.sub_deviceId, simTime);
}
#endif
