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

using namespace leon3_acclt_trap;
using namespace trap;
bool leon3_acclt_trap::LEON3_ABIIf::isLittleEndian() const throw(){
    return false;
}

int leon3_acclt_trap::LEON3_ABIIf::getProcessorID() const throw(){
    return ((ASR[17] & 0xF0000000) >> 28);
}

bool leon3_acclt_trap::LEON3_ABIIf::isInstrExecuting() const throw(){
    return this->instrExecuting;
}

void leon3_acclt_trap::LEON3_ABIIf::waitInstrEnd() const throw(){
    if(this->instrExecuting){
        wait(this->instrEndEvent);
    }
}

void leon3_acclt_trap::LEON3_ABIIf::preCall() throw(){

    unsigned int newCwp = ((unsigned int)(PSR[key_CWP] - 1)) % 8;
    PSR.immediateWrite((PSR & 0xFFFFFFE0) | newCwp);

    //ABI model: we simply immediately update the alias
    for(int i = 8; i < 32; i++){
        REGS[i].updateAlias(WINREGS[(newCwp*16 + i - 8) % (128)]);
    }
}

void leon3_acclt_trap::LEON3_ABIIf::postCall() throw(){

    unsigned int newCwp = ((unsigned int)(PSR[key_CWP] + 1)) % 8;
    PSR.immediateWrite((PSR & 0xFFFFFFE0) | newCwp);

    //ABI model: we simply immediately update the alias
    for(int i = 8; i < 32; i++){
        REGS[i].updateAlias(WINREGS[(newCwp*16 + i - 8) % (128)]);
    }
}

void leon3_acclt_trap::LEON3_ABIIf::returnFromCall() throw(){
    PC.writeAll(LR + 8);
    NPC.writeAll(LR + 12);
}

bool leon3_acclt_trap::LEON3_ABIIf::isRoutineEntry( const InstructionBase * instr \
    ) throw(){
    std::vector<std::string> nextNames = this->routineEntrySequence[this->routineEntryState];
    std::vector<std::string>::const_iterator namesIter, namesEnd;
    std::string curName = instr->getInstructionName();
    for(namesIter = nextNames.begin(), namesEnd = nextNames.end(); namesIter != namesEnd; \
        namesIter++){
        if(curName == *namesIter || *namesIter == ""){
            if(this->routineEntryState == 2){
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

bool leon3_acclt_trap::LEON3_ABIIf::isRoutineExit( const InstructionBase * instr \
    ) throw(){
    std::vector<std::string> nextNames = this->routineExitSequence[this->routineExitState];
    std::vector<std::string>::const_iterator namesIter, namesEnd;
    std::string curName = instr->getInstructionName();
    for(namesIter = nextNames.begin(), namesEnd = nextNames.end(); namesIter != namesEnd; \
        namesIter++){
        if(curName == *namesIter || *namesIter == ""){
            if(this->routineExitState == 1){
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

unsigned char * leon3_acclt_trap::LEON3_ABIIf::getState() const throw(){
    unsigned char * curState = new unsigned char[696];
    unsigned char * curStateTemp = curState;
    *((unsigned int *)curStateTemp) = this->PSR;
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WIM;
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->TBR;
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->Y;
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->PC;
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->NPC;
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->GLOBAL[0];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->GLOBAL[1];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->GLOBAL[2];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->GLOBAL[3];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->GLOBAL[4];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->GLOBAL[5];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->GLOBAL[6];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->GLOBAL[7];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[0];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[1];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[2];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[3];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[4];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[5];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[6];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[7];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[8];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[9];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[10];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[11];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[12];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[13];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[14];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[15];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[16];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[17];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[18];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[19];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[20];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[21];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[22];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[23];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[24];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[25];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[26];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[27];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[28];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[29];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[30];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[31];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[32];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[33];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[34];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[35];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[36];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[37];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[38];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[39];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[40];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[41];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[42];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[43];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[44];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[45];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[46];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[47];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[48];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[49];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[50];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[51];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[52];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[53];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[54];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[55];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[56];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[57];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[58];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[59];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[60];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[61];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[62];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[63];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[64];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[65];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[66];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[67];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[68];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[69];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[70];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[71];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[72];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[73];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[74];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[75];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[76];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[77];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[78];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[79];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[80];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[81];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[82];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[83];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[84];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[85];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[86];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[87];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[88];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[89];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[90];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[91];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[92];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[93];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[94];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[95];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[96];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[97];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[98];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[99];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[100];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[101];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[102];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[103];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[104];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[105];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[106];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[107];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[108];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[109];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[110];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[111];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[112];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[113];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[114];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[115];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[116];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[117];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[118];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[119];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[120];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[121];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[122];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[123];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[124];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[125];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[126];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->WINREGS[127];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->ASR[0];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->ASR[1];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->ASR[2];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->ASR[3];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->ASR[4];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->ASR[5];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->ASR[6];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->ASR[7];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->ASR[8];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->ASR[9];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->ASR[10];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->ASR[11];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->ASR[12];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->ASR[13];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->ASR[14];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->ASR[15];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->ASR[16];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->ASR[17];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->ASR[18];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->ASR[19];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->ASR[20];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->ASR[21];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->ASR[22];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->ASR[23];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->ASR[24];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->ASR[25];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->ASR[26];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->ASR[27];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->ASR[28];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->ASR[29];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->ASR[30];
    curStateTemp += 4;
    *((unsigned int *)curStateTemp) = this->ASR[31];
    curStateTemp += 4;
    return curState;
}

void leon3_acclt_trap::LEON3_ABIIf::setState( unsigned char * state ) throw(){
    unsigned char * curStateTemp = state;
    this->PSR.writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WIM.writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->TBR.writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->Y.writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->PC.writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->NPC.writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->GLOBAL[0].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->GLOBAL[1].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->GLOBAL[2].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->GLOBAL[3].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->GLOBAL[4].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->GLOBAL[5].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->GLOBAL[6].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->GLOBAL[7].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[0].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[1].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[2].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[3].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[4].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[5].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[6].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[7].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[8].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[9].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[10].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[11].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[12].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[13].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[14].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[15].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[16].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[17].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[18].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[19].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[20].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[21].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[22].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[23].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[24].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[25].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[26].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[27].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[28].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[29].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[30].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[31].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[32].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[33].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[34].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[35].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[36].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[37].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[38].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[39].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[40].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[41].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[42].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[43].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[44].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[45].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[46].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[47].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[48].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[49].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[50].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[51].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[52].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[53].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[54].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[55].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[56].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[57].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[58].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[59].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[60].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[61].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[62].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[63].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[64].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[65].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[66].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[67].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[68].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[69].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[70].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[71].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[72].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[73].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[74].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[75].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[76].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[77].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[78].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[79].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[80].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[81].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[82].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[83].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[84].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[85].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[86].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[87].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[88].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[89].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[90].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[91].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[92].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[93].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[94].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[95].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[96].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[97].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[98].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[99].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[100].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[101].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[102].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[103].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[104].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[105].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[106].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[107].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[108].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[109].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[110].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[111].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[112].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[113].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[114].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[115].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[116].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[117].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[118].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[119].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[120].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[121].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[122].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[123].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[124].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[125].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[126].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->WINREGS[127].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->ASR[0].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->ASR[1].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->ASR[2].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->ASR[3].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->ASR[4].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->ASR[5].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->ASR[6].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->ASR[7].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->ASR[8].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->ASR[9].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->ASR[10].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->ASR[11].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->ASR[12].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->ASR[13].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->ASR[14].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->ASR[15].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->ASR[16].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->ASR[17].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->ASR[18].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->ASR[19].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->ASR[20].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->ASR[21].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->ASR[22].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->ASR[23].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->ASR[24].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->ASR[25].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->ASR[26].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->ASR[27].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->ASR[28].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->ASR[29].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->ASR[30].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
    this->ASR[31].writeAll(*((unsigned int *)curStateTemp));
    curStateTemp += 4;
}

unsigned int leon3_acclt_trap::LEON3_ABIIf::getCodeLimit(){
    return this->PROGRAM_LIMIT;
}

unsigned int leon3_acclt_trap::LEON3_ABIIf::readLR() const throw(){
    return this->LR;
}

void leon3_acclt_trap::LEON3_ABIIf::setLR( const unsigned int & newValue ) throw(){
    this->LR.writeAll(newValue);
}

unsigned int leon3_acclt_trap::LEON3_ABIIf::readPC() const throw(){
    return this->PC;
}

void leon3_acclt_trap::LEON3_ABIIf::setPC( const unsigned int & newValue ) throw(){
    this->PC.writeAll(newValue);
}

unsigned int leon3_acclt_trap::LEON3_ABIIf::readSP() const throw(){
    return this->SP;
}

void leon3_acclt_trap::LEON3_ABIIf::setSP( const unsigned int & newValue ) throw(){
    this->SP.writeAll(newValue);
}

unsigned int leon3_acclt_trap::LEON3_ABIIf::readFP() const throw(){
    return this->FP;
}

void leon3_acclt_trap::LEON3_ABIIf::setFP( const unsigned int & newValue ) throw(){
    this->FP.writeAll(newValue);
}

unsigned int leon3_acclt_trap::LEON3_ABIIf::readRetVal() const throw(){
    return this->REGS[24];
}

void leon3_acclt_trap::LEON3_ABIIf::setRetVal( const unsigned int & newValue ) throw(){
    this->REGS[24].writeAll(newValue);
}

std::vector< unsigned int > leon3_acclt_trap::LEON3_ABIIf::readArgs() const throw(){
    std::vector< unsigned int > args;
    args.push_back(this->REGS[24]);
    args.push_back(this->REGS[25]);
    args.push_back(this->REGS[26]);
    args.push_back(this->REGS[27]);
    args.push_back(this->REGS[28]);
    args.push_back(this->REGS[29]);
    return args;
}

void leon3_acclt_trap::LEON3_ABIIf::setArgs( const std::vector< unsigned int > & \
    args ) throw(){
    if(args.size() > 6){
        THROW_EXCEPTION("ABI of processor supports up to 6 arguments: " << args.size() << \
            " given");
    }
    std::vector< unsigned int >::const_iterator argIter = args.begin(), argEnd = args.end();
    if(argIter != argEnd){
        this->REGS[24].writeAll(*argIter);
        argIter++;
    }
    if(argIter != argEnd){
        this->REGS[25].writeAll(*argIter);
        argIter++;
    }
    if(argIter != argEnd){
        this->REGS[26].writeAll(*argIter);
        argIter++;
    }
    if(argIter != argEnd){
        this->REGS[27].writeAll(*argIter);
        argIter++;
    }
    if(argIter != argEnd){
        this->REGS[28].writeAll(*argIter);
        argIter++;
    }
    if(argIter != argEnd){
        this->REGS[29].writeAll(*argIter);
        argIter++;
    }
}

unsigned int leon3_acclt_trap::LEON3_ABIIf::readGDBReg( const unsigned int & gdbId \
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
            return PC;
        break;}
        case 69:{
            return NPC;
        break;}
        default:{
            return 0;
        }
    }
}

unsigned int leon3_acclt_trap::LEON3_ABIIf::nGDBRegs() const throw(){
    return 70;
}

void leon3_acclt_trap::LEON3_ABIIf::setGDBReg( const unsigned int & newValue, const \
    unsigned int & gdbId ) throw(){
    switch(gdbId){
        case 0:{
            REGS[0].writeAll(newValue);
        break;}
        case 1:{
            REGS[1].writeAll(newValue);
        break;}
        case 2:{
            REGS[2].writeAll(newValue);
        break;}
        case 3:{
            REGS[3].writeAll(newValue);
        break;}
        case 4:{
            REGS[4].writeAll(newValue);
        break;}
        case 5:{
            REGS[5].writeAll(newValue);
        break;}
        case 6:{
            REGS[6].writeAll(newValue);
        break;}
        case 7:{
            REGS[7].writeAll(newValue);
        break;}
        case 8:{
            REGS[8].writeAll(newValue);
        break;}
        case 9:{
            REGS[9].writeAll(newValue);
        break;}
        case 10:{
            REGS[10].writeAll(newValue);
        break;}
        case 11:{
            REGS[11].writeAll(newValue);
        break;}
        case 12:{
            REGS[12].writeAll(newValue);
        break;}
        case 13:{
            REGS[13].writeAll(newValue);
        break;}
        case 14:{
            REGS[14].writeAll(newValue);
        break;}
        case 15:{
            REGS[15].writeAll(newValue);
        break;}
        case 16:{
            REGS[16].writeAll(newValue);
        break;}
        case 17:{
            REGS[17].writeAll(newValue);
        break;}
        case 18:{
            REGS[18].writeAll(newValue);
        break;}
        case 19:{
            REGS[19].writeAll(newValue);
        break;}
        case 20:{
            REGS[20].writeAll(newValue);
        break;}
        case 21:{
            REGS[21].writeAll(newValue);
        break;}
        case 22:{
            REGS[22].writeAll(newValue);
        break;}
        case 23:{
            REGS[23].writeAll(newValue);
        break;}
        case 24:{
            REGS[24].writeAll(newValue);
        break;}
        case 25:{
            REGS[25].writeAll(newValue);
        break;}
        case 26:{
            REGS[26].writeAll(newValue);
        break;}
        case 27:{
            REGS[27].writeAll(newValue);
        break;}
        case 28:{
            REGS[28].writeAll(newValue);
        break;}
        case 29:{
            REGS[29].writeAll(newValue);
        break;}
        case 30:{
            REGS[30].writeAll(newValue);
        break;}
        case 31:{
            REGS[31].writeAll(newValue);
        break;}
        case 64:{
            Y.writeAll(newValue);
        break;}
        case 65:{
            PSR.writeAll(newValue);
        break;}
        case 66:{
            WIM.writeAll(newValue);
        break;}
        case 67:{
            TBR.writeAll(newValue);
        break;}
        case 68:{
            PC.writeAll(newValue);
        break;}
        case 69:{
            NPC.writeAll(newValue);
        break;}
        default:{
            THROW_EXCEPTION("No register corresponding to GDB id " << gdbId);
        }
    }
}

unsigned int leon3_acclt_trap::LEON3_ABIIf::readMem( const unsigned int & address ){
    return this->dataMem.read_word_dbg(address);
}

unsigned char leon3_acclt_trap::LEON3_ABIIf::readCharMem( const unsigned int & address ){
    return this->dataMem.read_byte_dbg(address);
}

void leon3_acclt_trap::LEON3_ABIIf::writeMem( const unsigned int & address, unsigned \
    int datum ){
    this->dataMem.write_word_dbg(address, datum);
}

void leon3_acclt_trap::LEON3_ABIIf::writeCharMem( const unsigned int & address, unsigned \
    char datum ){
    this->dataMem.write_byte_dbg(address, datum);
}

leon3_acclt_trap::LEON3_ABIIf::~LEON3_ABIIf(){

}
leon3_acclt_trap::LEON3_ABIIf::LEON3_ABIIf( unsigned int & PROGRAM_LIMIT, MemoryInterface \
    & dataMem, PipelineRegister & PSR, PipelineRegister & WIM, PipelineRegister & TBR, \
    PipelineRegister & Y, PipelineRegister & PC, PipelineRegister & NPC, PipelineRegister \
    * GLOBAL, PipelineRegister * WINREGS, PipelineRegister * ASR, Alias & FP, Alias & \
    LR, Alias & SP, Alias & PCR, Alias * REGS, bool & instrExecuting, sc_event & instrEndEvent \
    ) : PROGRAM_LIMIT(PROGRAM_LIMIT), dataMem(dataMem), PSR(PSR), WIM(WIM), TBR(TBR), \
    Y(Y), PC(PC), NPC(NPC), GLOBAL(GLOBAL), WINREGS(WINREGS), ASR(ASR), FP(FP), LR(LR), \
    SP(SP), PCR(PCR), REGS(REGS), instrExecuting(instrExecuting), instrEndEvent(instrEndEvent){
    this->routineExitState = 0;
    this->routineEntryState = 0;
    std::vector<std::string> tempVec;
    tempVec.clear();
    tempVec.push_back("CALL");
    this->routineEntrySequence.push_back(tempVec);
    tempVec.clear();
    tempVec.push_back("");
    this->routineEntrySequence.push_back(tempVec);
    tempVec.clear();
    tempVec.push_back("");
    this->routineEntrySequence.push_back(tempVec);
    tempVec.clear();
    tempVec.push_back("RESTORE_imm");
    tempVec.push_back("RESTORE_reg");
    tempVec.push_back("JUMP_imm");
    tempVec.push_back("JUMP_reg");
    this->routineExitSequence.push_back(tempVec);
    tempVec.clear();
    tempVec.push_back("JUMP_imm");
    tempVec.push_back("JUMP_reg");
    tempVec.push_back("RESTORE_imm");
    tempVec.push_back("RESTORE_reg");
    this->routineExitSequence.push_back(tempVec);
}


