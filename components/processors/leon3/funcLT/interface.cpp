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
#include <vector>
#include <trap_utils.hpp>

using namespace leon3_funclt_trap;
using namespace trap;
bool leon3_funclt_trap::LEON3_ABIIf::isLittleEndian() const throw(){
    return false;
}

void leon3_funclt_trap::LEON3_ABIIf::preCall() throw(){

    unsigned int newCwp = ((unsigned int)(PSR[key_CWP] - 1)) % 8;
    PSRbp = (PSR & 0xFFFFFFE0) | newCwp;
    PSR.immediateWrite(PSRbp);
    for(int i = 8; i < 32; i++){
        REGS[i].updateAlias(WINREGS[(newCwp*16 + i - 8) % (128)]);
    }
}

void leon3_funclt_trap::LEON3_ABIIf::postCall() throw(){

    unsigned int newCwp = ((unsigned int)(PSR[key_CWP] + 1)) % 8;
    PSRbp = (PSR & 0xFFFFFFE0) | newCwp;
    PSR.immediateWrite(PSRbp);
    for(int i = 8; i < 32; i++){
        REGS[i].updateAlias(WINREGS[(newCwp*16 + i - 8) % (128)]);
    }
}

void leon3_funclt_trap::LEON3_ABIIf::returnFromCall() throw(){
    PC.immediateWrite(LR + 8);
    NPC.immediateWrite(LR + 12);
}

unsigned int leon3_funclt_trap::LEON3_ABIIf::getCodeLimit(){
    return this->PROGRAM_LIMIT;
}

unsigned int leon3_funclt_trap::LEON3_ABIIf::readLR() const throw(){
    return this->LR;
}

void leon3_funclt_trap::LEON3_ABIIf::setLR( const unsigned int & newValue ) throw(){
    this->LR.immediateWrite(newValue);
}

unsigned int leon3_funclt_trap::LEON3_ABIIf::readPC() const throw(){
    return this->PC + -4;
}

void leon3_funclt_trap::LEON3_ABIIf::setPC( const unsigned int & newValue ) throw(){
    this->PC.immediateWrite(newValue);
}

unsigned int leon3_funclt_trap::LEON3_ABIIf::readSP() const throw(){
    return this->SP;
}

void leon3_funclt_trap::LEON3_ABIIf::setSP( const unsigned int & newValue ) throw(){
    this->SP.immediateWrite(newValue);
}

unsigned int leon3_funclt_trap::LEON3_ABIIf::readFP() const throw(){
    return this->FP;
}

void leon3_funclt_trap::LEON3_ABIIf::setFP( const unsigned int & newValue ) throw(){
    this->FP.immediateWrite(newValue);
}

unsigned int leon3_funclt_trap::LEON3_ABIIf::readRetVal() const throw(){
    return this->REGS[24];
}

void leon3_funclt_trap::LEON3_ABIIf::setRetVal( const unsigned int & newValue ) throw(){
    this->REGS[24].immediateWrite(newValue);
}

std::vector< unsigned int > leon3_funclt_trap::LEON3_ABIIf::readArgs() const throw(){
    std::vector< unsigned int > args;
    args.push_back(this->REGS[24]);
    args.push_back(this->REGS[25]);
    args.push_back(this->REGS[26]);
    args.push_back(this->REGS[27]);
    args.push_back(this->REGS[28]);
    args.push_back(this->REGS[29]);
    return args;
}

void leon3_funclt_trap::LEON3_ABIIf::setArgs( const std::vector< unsigned int > & \
    args ) throw(){
    if(args.size() > 6){
        THROW_EXCEPTION("ABI of processor supports up to 6 arguments: " << args.size() << \
            " given");
    }
    std::vector< unsigned int >::const_iterator argIter = args.begin(), argEnd = args.end();
    if(argIter != argEnd){
        this->REGS[24].immediateWrite(*argIter);
        argIter++;
    }
    if(argIter != argEnd){
        this->REGS[25].immediateWrite(*argIter);
        argIter++;
    }
    if(argIter != argEnd){
        this->REGS[26].immediateWrite(*argIter);
        argIter++;
    }
    if(argIter != argEnd){
        this->REGS[27].immediateWrite(*argIter);
        argIter++;
    }
    if(argIter != argEnd){
        this->REGS[28].immediateWrite(*argIter);
        argIter++;
    }
    if(argIter != argEnd){
        this->REGS[29].immediateWrite(*argIter);
        argIter++;
    }
}

unsigned int leon3_funclt_trap::LEON3_ABIIf::readGDBReg( const unsigned int & gdbId \
    ) const throw(){
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
            return REGS[15];
        break;}
        case 16:{
            return REGS[16];
        break;}
        case 17:{
            return REGS[17];
        break;}
        case 18:{
            return REGS[18];
        break;}
        case 19:{
            return REGS[19];
        break;}
        case 20:{
            return REGS[20];
        break;}
        case 21:{
            return REGS[21];
        break;}
        case 22:{
            return REGS[22];
        break;}
        case 23:{
            return REGS[23];
        break;}
        case 24:{
            return REGS[24];
        break;}
        case 25:{
            return REGS[25];
        break;}
        case 26:{
            return REGS[26];
        break;}
        case 27:{
            return REGS[27];
        break;}
        case 28:{
            return REGS[28];
        break;}
        case 29:{
            return REGS[29];
        break;}
        case 30:{
            return REGS[30];
        break;}
        case 31:{
            return REGS[31];
        break;}
        case 64:{
            return Y;
        break;}
        case 65:{
            return PSR;
        break;}
        case 66:{
            return WIM;
        break;}
        case 67:{
            return TBR;
        break;}
        case 68:{
            return PC + -4;
        break;}
        case 69:{
            return NPC;
        break;}
        default:{
            return 0;
        }
    }
}

unsigned int leon3_funclt_trap::LEON3_ABIIf::nGDBRegs() const throw(){
    return 70;
}

void leon3_funclt_trap::LEON3_ABIIf::setGDBReg( const unsigned int & newValue, const \
    unsigned int & gdbId ) throw(){
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
            REGS[16].immediateWrite(newValue);
        break;}
        case 17:{
            REGS[17].immediateWrite(newValue);
        break;}
        case 18:{
            REGS[18].immediateWrite(newValue);
        break;}
        case 19:{
            REGS[19].immediateWrite(newValue);
        break;}
        case 20:{
            REGS[20].immediateWrite(newValue);
        break;}
        case 21:{
            REGS[21].immediateWrite(newValue);
        break;}
        case 22:{
            REGS[22].immediateWrite(newValue);
        break;}
        case 23:{
            REGS[23].immediateWrite(newValue);
        break;}
        case 24:{
            REGS[24].immediateWrite(newValue);
        break;}
        case 25:{
            REGS[25].immediateWrite(newValue);
        break;}
        case 26:{
            REGS[26].immediateWrite(newValue);
        break;}
        case 27:{
            REGS[27].immediateWrite(newValue);
        break;}
        case 28:{
            REGS[28].immediateWrite(newValue);
        break;}
        case 29:{
            REGS[29].immediateWrite(newValue);
        break;}
        case 30:{
            REGS[30].immediateWrite(newValue);
        break;}
        case 31:{
            REGS[31].immediateWrite(newValue);
        break;}
        case 64:{
            Y.immediateWrite(newValue);
        break;}
        case 65:{
            PSR.immediateWrite(newValue);
        break;}
        case 66:{
            WIM.immediateWrite(newValue);
        break;}
        case 67:{
            TBR.immediateWrite(newValue);
        break;}
        case 68:{
            PC.immediateWrite(newValue);
        break;}
        case 69:{
            NPC.immediateWrite(newValue);
        break;}
        default:{
            THROW_EXCEPTION("No register corresponding to GDB id " << gdbId);
        }
    }
}

unsigned int leon3_funclt_trap::LEON3_ABIIf::readMem( const unsigned int & address ){
    return this->dataMem.read_word_dbg(address);
}

unsigned char leon3_funclt_trap::LEON3_ABIIf::readCharMem( const unsigned int & address \
    ){
    return this->dataMem.read_byte_dbg(address);
}

void leon3_funclt_trap::LEON3_ABIIf::writeMem( const unsigned int & address, unsigned \
    int datum ){
    this->dataMem.write_word_dbg(address, datum);
}

void leon3_funclt_trap::LEON3_ABIIf::writeCharMem( const unsigned int & address, \
    unsigned char datum ){
    this->dataMem.write_byte_dbg(address, datum);
}

leon3_funclt_trap::LEON3_ABIIf::~LEON3_ABIIf(){

}
leon3_funclt_trap::LEON3_ABIIf::LEON3_ABIIf( unsigned int & PROGRAM_LIMIT, MemoryInterface \
    & dataMem, Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, Reg32_2 & TBR, Reg32_3 & \
    Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 & Ybp, Reg32_3 & ASR18bp, \
    RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 * & ASR, Alias & FP, Alias \
    & LR, Alias & SP, Alias & PCR, Alias * & REGS ) : PROGRAM_LIMIT(PROGRAM_LIMIT), dataMem(dataMem), \
    PSR(PSR), WIM(WIM), TBR(TBR), Y(Y), PC(PC), NPC(NPC), PSRbp(PSRbp), Ybp(Ybp), ASR18bp(ASR18bp), \
    GLOBAL(GLOBAL), WINREGS(WINREGS), ASR(ASR), FP(FP), LR(LR), SP(SP), PCR(PCR), REGS(REGS){

}


