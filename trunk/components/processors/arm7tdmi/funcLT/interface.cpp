/***************************************************************************\
 *
 *   
 *            ___        ___           ___           ___
 *           /  /\      /  /\         /  /\         /  /\
 *          /  /:/     /  /::\       /  /::\       /  /::\
 *         /  /:/     /  /:/\:\     /  /:/\:\     /  /:/\:\
 *        /  /:/     /  /:/~/:/    /  /:/~/::\   /  /:/~/:/
 *       /  /::\    /__/:/ /:/___ /__/:/ /:/\:\ /__/:/ /:/
 *      /__/:/\:\   \  \:\/:::::/ \  \:\/:/__\/ \  \:\/:/
 *      \__\/  \:\   \  \::/~~~~   \  \::/       \  \::/
 *           \  \:\   \  \:\        \  \:\        \  \:\
 *            \  \ \   \  \:\        \  \:\        \  \:\
 *             \__\/    \__\/         \__\/         \__\/
 *   
 *
 *
 *   
 *   This file is part of TRAP.
 *   
 *   TRAP is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *   
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *   
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *   or see <http://www.gnu.org/licenses/>.
 *   
 *
 *
 *   (c) Luca Fossati, fossati@elet.polimi.it
 *
\***************************************************************************/



#include <interface.hpp>
#include <ABIIf.hpp>
#include <memory.hpp>
#include <registers.hpp>
#include <alias.hpp>
#include <systemc.h>
#include <vector>
#include <string>
#include <instructionBase.hpp>
#include <trap_utils.hpp>

using namespace arm7tdmi_funclt_trap;
using namespace trap;
bool arm7tdmi_funclt_trap::ARM7TDMI_ABIIf::isLittleEndian() const throw(){
    return true;
}

bool arm7tdmi_funclt_trap::ARM7TDMI_ABIIf::isInstrExecuting() const throw(){
    return this->instrExecuting;
}

void arm7tdmi_funclt_trap::ARM7TDMI_ABIIf::waitInstrEnd() const throw(){
    if(this->instrExecuting){
        wait(this->instrEndEvent);
    }
}

bool arm7tdmi_funclt_trap::ARM7TDMI_ABIIf::isRoutineEntry( const InstructionBase \
    * instr ) throw(){
    std::vector<std::string> nextNames = this->routineEntrySequence[this->routineEntryState];
    std::vector<std::string>::const_iterator namesIter, namesEnd;
    std::string curName = instr->getInstructionName();
    for(namesIter = nextNames.begin(), namesEnd = nextNames.end(); namesIter != namesEnd; \
        namesIter++){
        if(curName == *namesIter || *namesIter == ""){
            if(this->routineEntryState == -1){
                this->routineEntryState = 0;
                return true;
            }
            this->routineEntryState++;
            return false;
        }
    }
    this->routineEntryState = 0;
    return false;
}

bool arm7tdmi_funclt_trap::ARM7TDMI_ABIIf::isRoutineExit( const InstructionBase * \
    instr ) throw(){
    std::vector<std::string> nextNames = this->routineExitSequence[this->routineExitState];
    std::vector<std::string>::const_iterator namesIter, namesEnd;
    std::string curName = instr->getInstructionName();
    for(namesIter = nextNames.begin(), namesEnd = nextNames.end(); namesIter != namesEnd; \
        namesIter++){
        if(curName == *namesIter || *namesIter == ""){
            if(this->routineExitState == -1){
                this->routineExitState = 0;
                return true;
            }
            this->routineExitState++;
            return false;
        }
    }
    this->routineExitState = 0;
    return false;
}

unsigned char * arm7tdmi_funclt_trap::ARM7TDMI_ABIIf::getState() const throw(){
    unsigned char * curState = new unsigned char[148];
    unsigned char * curStateTemp = curState;
    *((unsigned int *)curStateTemp) = this->CPSR.readNewValue();
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->MP_ID.readNewValue();
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->RB[0].readNewValue();
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->RB[1].readNewValue();
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->RB[2].readNewValue();
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->RB[3].readNewValue();
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->RB[4].readNewValue();
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->RB[5].readNewValue();
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->RB[6].readNewValue();
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->RB[7].readNewValue();
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->RB[8].readNewValue();
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->RB[9].readNewValue();
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->RB[10].readNewValue();
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->RB[11].readNewValue();
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->RB[12].readNewValue();
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->RB[13].readNewValue();
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->RB[14].readNewValue();
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->RB[15].readNewValue();
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->RB[16].readNewValue();
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->RB[17].readNewValue();
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->RB[18].readNewValue();
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->RB[19].readNewValue();
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->RB[20].readNewValue();
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->RB[21].readNewValue();
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->RB[22].readNewValue();
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->RB[23].readNewValue();
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->RB[24].readNewValue();
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->RB[25].readNewValue();
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->RB[26].readNewValue();
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->RB[27].readNewValue();
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->RB[28].readNewValue();
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->RB[29].readNewValue();
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->SPSR[0].readNewValue();
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->SPSR[1].readNewValue();
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->SPSR[2].readNewValue();
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->SPSR[3].readNewValue();
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->SPSR[4].readNewValue();
    curStateTemp += 4;
    return curState;
}

void arm7tdmi_funclt_trap::ARM7TDMI_ABIIf::setState( unsigned char * state ) throw(){
    unsigned char * curStateTemp = state;
    this->CPSR.immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->MP_ID.immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->RB[0].immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->RB[1].immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->RB[2].immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->RB[3].immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->RB[4].immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->RB[5].immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->RB[6].immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->RB[7].immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->RB[8].immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->RB[9].immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->RB[10].immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->RB[11].immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->RB[12].immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->RB[13].immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->RB[14].immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->RB[15].immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->RB[16].immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->RB[17].immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->RB[18].immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->RB[19].immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->RB[20].immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->RB[21].immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->RB[22].immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->RB[23].immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->RB[24].immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->RB[25].immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->RB[26].immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->RB[27].immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->RB[28].immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->RB[29].immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->SPSR[0].immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->SPSR[1].immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->SPSR[2].immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->SPSR[3].immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->SPSR[4].immediateWrite(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
}

unsigned int arm7tdmi_funclt_trap::ARM7TDMI_ABIIf::getCodeLimit(){
    return this->PROGRAM_LIMIT;
}

unsigned int arm7tdmi_funclt_trap::ARM7TDMI_ABIIf::readLR() const throw(){
    return this->LINKR;
}

void arm7tdmi_funclt_trap::ARM7TDMI_ABIIf::setLR( const unsigned int & newValue ) \
    throw(){
    this->LINKR.immediateWrite(newValue);
}

unsigned int arm7tdmi_funclt_trap::ARM7TDMI_ABIIf::readPC() const throw(){
    return this->PC + -4;
}

void arm7tdmi_funclt_trap::ARM7TDMI_ABIIf::setPC( const unsigned int & newValue ) \
    throw(){
    this->PC.immediateWrite(newValue);
}

unsigned int arm7tdmi_funclt_trap::ARM7TDMI_ABIIf::readSP() const throw(){
    return this->SPTR;
}

void arm7tdmi_funclt_trap::ARM7TDMI_ABIIf::setSP( const unsigned int & newValue ) \
    throw(){
    this->SPTR.immediateWrite(newValue);
}

unsigned int arm7tdmi_funclt_trap::ARM7TDMI_ABIIf::readFP() const throw(){
    return this->FP;
}

void arm7tdmi_funclt_trap::ARM7TDMI_ABIIf::setFP( const unsigned int & newValue ) \
    throw(){
    this->FP.immediateWrite(newValue);
}

unsigned int arm7tdmi_funclt_trap::ARM7TDMI_ABIIf::readRetVal() const throw(){
    return this->REGS[0];
}

void arm7tdmi_funclt_trap::ARM7TDMI_ABIIf::setRetVal( const unsigned int & newValue \
    ) throw(){
    this->REGS[0].immediateWrite(newValue);
}

std::vector< unsigned int > arm7tdmi_funclt_trap::ARM7TDMI_ABIIf::readArgs() const \
    throw(){
    std::vector< unsigned int > args;
    args.push_back(this->REGS[0]);
    args.push_back(this->REGS[1]);
    args.push_back(this->REGS[2]);
    args.push_back(this->REGS[3]);
    return args;
}

void arm7tdmi_funclt_trap::ARM7TDMI_ABIIf::setArgs( const std::vector< unsigned int \
    > & args ) throw(){
    if(args.size() > 4){
        THROW_EXCEPTION("ABI of processor supports up to 4 arguments: " << args.size() << \
            " given");
    }
    std::vector< unsigned int >::const_iterator argIter = args.begin(), argEnd = args.end();
    if(argIter != argEnd){
        this->REGS[0].immediateWrite(*argIter);
        argIter++;
    }
    if(argIter != argEnd){
        this->REGS[1].immediateWrite(*argIter);
        argIter++;
    }
    if(argIter != argEnd){
        this->REGS[2].immediateWrite(*argIter);
        argIter++;
    }
    if(argIter != argEnd){
        this->REGS[3].immediateWrite(*argIter);
        argIter++;
    }
}

unsigned int arm7tdmi_funclt_trap::ARM7TDMI_ABIIf::readGDBReg( const unsigned int \
    & gdbId ) const throw(){
    switch(gdbId){
        case 0:{
            return REGS[0];
        break;}
        case 1:{
            return REGS[1];
        break;}
        case 2:{
            return REGS[2];
        break;}
        case 3:{
            return REGS[3];
        break;}
        case 4:{
            return REGS[4];
        break;}
        case 5:{
            return REGS[5];
        break;}
        case 6:{
            return REGS[6];
        break;}
        case 7:{
            return REGS[7];
        break;}
        case 8:{
            return REGS[8];
        break;}
        case 9:{
            return REGS[9];
        break;}
        case 10:{
            return REGS[10];
        break;}
        case 11:{
            return REGS[11];
        break;}
        case 12:{
            return REGS[12];
        break;}
        case 13:{
            return REGS[13];
        break;}
        case 14:{
            return REGS[14];
        break;}
        case 15:{
            return REGS[15] + -4;
        break;}
        case 16:{
            return CPSR;
        break;}
        default:{
            return 0;
        }
    }
}

unsigned int arm7tdmi_funclt_trap::ARM7TDMI_ABIIf::nGDBRegs() const throw(){
    return 17;
}

void arm7tdmi_funclt_trap::ARM7TDMI_ABIIf::setGDBReg( const unsigned int & newValue, \
    const unsigned int & gdbId ) throw(){
    switch(gdbId){
        case 0:{
            REGS[0].immediateWrite(newValue);
        break;}
        case 1:{
            REGS[1].immediateWrite(newValue);
        break;}
        case 2:{
            REGS[2].immediateWrite(newValue);
        break;}
        case 3:{
            REGS[3].immediateWrite(newValue);
        break;}
        case 4:{
            REGS[4].immediateWrite(newValue);
        break;}
        case 5:{
            REGS[5].immediateWrite(newValue);
        break;}
        case 6:{
            REGS[6].immediateWrite(newValue);
        break;}
        case 7:{
            REGS[7].immediateWrite(newValue);
        break;}
        case 8:{
            REGS[8].immediateWrite(newValue);
        break;}
        case 9:{
            REGS[9].immediateWrite(newValue);
        break;}
        case 10:{
            REGS[10].immediateWrite(newValue);
        break;}
        case 11:{
            REGS[11].immediateWrite(newValue);
        break;}
        case 12:{
            REGS[12].immediateWrite(newValue);
        break;}
        case 13:{
            REGS[13].immediateWrite(newValue);
        break;}
        case 14:{
            REGS[14].immediateWrite(newValue);
        break;}
        case 15:{
            REGS[15].immediateWrite(newValue);
        break;}
        case 16:{
            CPSR.immediateWrite(newValue);
        break;}
        default:{
            THROW_EXCEPTION("No register corresponding to GDB id " << gdbId);
        }
    }
}

unsigned int arm7tdmi_funclt_trap::ARM7TDMI_ABIIf::readMem( const unsigned int & \
    address ){
    return this->dataMem.read_word_dbg(address);
}

unsigned char arm7tdmi_funclt_trap::ARM7TDMI_ABIIf::readCharMem( const unsigned int \
    & address ){
    return this->dataMem.read_byte_dbg(address);
}

void arm7tdmi_funclt_trap::ARM7TDMI_ABIIf::writeMem( const unsigned int & address, \
    unsigned int datum ){
    this->dataMem.write_word_dbg(address, datum);
}

void arm7tdmi_funclt_trap::ARM7TDMI_ABIIf::writeCharMem( const unsigned int & address, \
    unsigned char datum ){
    this->dataMem.write_byte_dbg(address, datum);
}

arm7tdmi_funclt_trap::ARM7TDMI_ABIIf::~ARM7TDMI_ABIIf(){

}
arm7tdmi_funclt_trap::ARM7TDMI_ABIIf::ARM7TDMI_ABIIf( unsigned int & PROGRAM_LIMIT, \
    MemoryInterface & dataMem, Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 \
    * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias & LR_IRQ, \
    Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, bool & instrExecuting, \
    sc_event & instrEndEvent ) : PROGRAM_LIMIT(PROGRAM_LIMIT), dataMem(dataMem), CPSR(CPSR), \
    MP_ID(MP_ID), RB(RB), SPSR(SPSR), FP(FP), SPTR(SPTR), LINKR(LINKR), SP_IRQ(SP_IRQ), \
    LR_IRQ(LR_IRQ), SP_FIQ(SP_FIQ), LR_FIQ(LR_FIQ), PC(PC), REGS(REGS), instrExecuting(instrExecuting), \
    instrEndEvent(instrEndEvent){
    this->routineExitState = 0;
    this->routineEntryState = 0;
    std::vector<std::string> tempVec;
}


