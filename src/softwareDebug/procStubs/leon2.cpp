/**
 * This file contains the methods necessary to access the processor registers
 * and the memory. This file is made for the leon2 functional processor model written
 * using the ArchC language
 */

#include <systemc.h>
#include <string>

 #include "leon2.H"
#include "leon2.hpp"
#include "processorInterface.hpp"

#include "utils.hpp"

#include "BreakpointManager.hpp"
#include "MySQL.hpp"


using namespace resp;

leon2Stub::leon2Stub(leon2 & processor,  unsigned int verbosityLevel) : processor(processor), verbosityLevel(verbosityLevel){}

/**
 * Returns whether the processor is big-endian.
 *
 * @return false when simulated processor is little-endian, true when big-endian.
 */
bool leon2Stub::get_ac_tgt_endian(){
    #ifdef LITTLE_ENDIAN_BO
    #ifndef NDEBUG
    if(verbosityLevel >= 2)
        std::cerr << "GDB-WRAPPER " << __PRETTY_FUNCTION__ << " Endianess conflicting with host" << std::endl;
    #endif
    return true;
    #else
    #ifdef BIG_ENDIAN_BO
    #ifndef NDEBUG
    if(verbosityLevel >= 2)
        std::cerr << "GDB-WRAPPER " << __PRETTY_FUNCTION__ << " Same Endianess of host" << std::endl;
    #endif
    return false;
    #else
    #ifndef NDEBUG
    if(verbosityLevel >= 2)
        std::cerr << "GDB-WRAPPER " << __PRETTY_FUNCTION__ << " Cannot determine host endianess" << std::endl;
    #endif
    return true;
    #endif
    #endif
}

/**
 * Stops the processor.
 */
void leon2Stub::ac_stop(){
   this->processor.stop(0);
}

/* Register Support */

/**
 * How many registers are there in the packet sent to gdb. Check GDB doc.
 *
 * \return how many registers the packet has.
 */  
unsigned int leon2Stub::nRegs(){
   return 72;
}
  
/**
 * Return register value according to GDB stub packet order.
 *
 * \param reg register number according to GDB stub packet order. Check GDB doc.
 *
 * \return register value.
 */
unsigned int leon2Stub::reg_read(int reg, MySQL *mySQL, double curSimTime){
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
            if ( ( reg >= 0 ) && ( reg < 32 ) ) 
                ev = mySQL->getDeviceEventAddr(curSimTime, this->processor.RB.sub_deviceId, reg);
            else if ( reg == 64 )
                ev = mySQL->getDeviceEventAddr(curSimTime, this->processor.Y.sub_deviceId, 0);
            else if ( reg == 65 )
                ev = mySQL->getDeviceEventAddr(curSimTime, this->processor.PSR.sub_deviceId, 0);
            else if ( reg == 66 )
                ev = mySQL->getDeviceEventAddr(curSimTime, this->processor.WIM.sub_deviceId, 0);
            else if ( reg == 67 )
                ev = mySQL->getDeviceEventAddr(curSimTime, this->processor.TBR.sub_deviceId, 0);
            else if ( reg == 68 )
                ev = mySQL->getDeviceEventAddr(curSimTime, this->processor.ac_pc.sub_deviceId, 0);
            else if ( reg == 69 )
                ev = mySQL->getDeviceEventAddr(curSimTime, this->processor.npc.sub_deviceId, 0);
            else
                return 0;
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
   /* General Purpose: G, O, L, I */
   if ( ( reg >= 0 ) && ( reg < 32 ) ) {
      return this->processor.REGS[reg];
   }
  
   /* Y, PSR, WIM, PC, NPC */
   else if ( reg == 64 )
        return this->processor.Y;
   else if ( reg == 65 ){
        return this->processor.PSR;
    }
   else if ( reg == 66 )
        return this->processor.WIM;
   else if ( reg == 67 )
        return this->processor.TBR;
   else if ( reg == 68 ){
        return this->processor.ac_pc;
    }
   else if ( reg == 69 ){
        return this->processor.npc;
    }
  
   /* Floating point, TBR, FPSR, CPSR */
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
void leon2Stub::reg_write(int reg, unsigned int value, MySQL *mySQL, double curSimTime){
    #ifdef ENABLE_MYSQL
    if(curSimTime < sc_time_stamp().to_double()){
        THROW_EXCEPTION("Modifying the past simulation status not yet allowed");
    }
    else{
     #endif
   /* General Purpose: G, O, L & I regs */
   if ( ( reg >= 0 ) && ( reg < 32 ) ) {
      this->processor.REGS[reg] = value;
   }
  
   /* Y, PSR, WIM, PC, NPC */
   else if ( reg == 64 ) this->processor.Y   = value;
   else if ( reg == 65 ) this->processor.PSR = value;
   else if ( reg == 66 ) this->processor.WIM = value;
   else if ( reg == 67 ) this->processor.TBR = value;
   else if ( reg == 68 ) this->set_ac_pc(value, mySQL, curSimTime);
   else if ( reg == 69 ) this->processor.npc = value;
    #ifdef ENABLE_MYSQL
    }
    #endif
}
  

/**
 * Writes address value to the processor ac_pc (Default Program Counter) register.
 *
 * @param value Address value to be written in the ac_pc register.
 */
void leon2Stub::set_ac_pc(unsigned int value, MySQL *mySQL, double curSimTime){
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
    this->processor.npc = value;
    this->processor.ac_pc = this->processor.npc;
    this->processor.npc += 4;
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
unsigned char leon2Stub::mem_read(unsigned int address, MySQL *mySQL, double curSimTime){
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
void leon2Stub::mem_write(unsigned int address, unsigned char byte, MySQL *mySQL, double curSimTime){
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
std::string leon2Stub::getName(){
    std::string retVal(dynamic_cast<sc_object &>(processor).name());
    return retVal;
}

#ifdef ENABLE_MYSQL
/**
  * It examines the database looking for the specified event (described by breakpoint);
  * it returns the time at which the event happened if the event happened in the
  * specified time interval,  -1 otherwise.
  */
double leon2Stub::findEvent(Breakpoint<unsigned int> &breakpoint, double startTime, double endTime, MySQL *mySQL){
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
double leon2Stub::getNextInstruction(bool forward, double simTime, MySQL *mySQL){
    return mySQL->getNextEvent(forward, this->processor.ac_pc.sub_deviceId, simTime);
}
#endif
