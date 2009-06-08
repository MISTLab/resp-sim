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



#include <string>
#include <customExceptions.hpp>
#include <trap_utils.hpp>
#include <registers.hpp>
#include <alias.hpp>
#include <externalPorts.hpp>
#include <externalPins.hpp>
#include <instructions.hpp>
#include <sstream>
#include <systemc.h>

using namespace leon3_funclt_trap;
bool leon3_funclt_trap::Instruction::IncrementRegWindow(){
    {
        unsigned int newCwp;

        newCwp = ((unsigned int)(PSR[key_CWP] + 1)) % NUM_REG_WIN;
        if(((0x01 << (newCwp)) & WIM) != 0){
            return false;
        }
        PSRbp = (PSR & 0xFFFFFFE0) | newCwp;
        PSR.immediateWrite(PSRbp);
        for(int i = 8; i < 32; i++){
            REGS[i].updateAlias(WINREGS[(newCwp*16 + i - 8) % (16*NUM_REG_WIN)]);
        }
        return true;
    }
}

bool leon3_funclt_trap::Instruction::DecrementRegWindow(){
    {
        unsigned int newCwp;

        newCwp = ((unsigned int)(PSR[key_CWP] - 1)) % NUM_REG_WIN;
        if(((0x01 << (newCwp)) & WIM) != 0){
            return false;
        }
        PSRbp = (PSR & 0xFFFFFFE0) | newCwp;
        PSR.immediateWrite(PSRbp);
        for(int i = 8; i < 32; i++){
            REGS[i].updateAlias(WINREGS[(newCwp*16 + i - 8) % (16*NUM_REG_WIN)]);
        }
        return true;
    }
}

int leon3_funclt_trap::Instruction::SignExtend( unsigned int bitSeq, unsigned int \
    bitSeq_length ) const throw(){

    if((bitSeq & (1 << (bitSeq_length - 1))) != 0)
    bitSeq |= (((unsigned int)0xFFFFFFFF) << bitSeq_length);
    return bitSeq;
}

void leon3_funclt_trap::Instruction::RaiseException( unsigned int exceptionId, unsigned \
    int customTrapOffset ){

    if(PSR[key_ET] == 0){
        if(exceptionId < IRQ_LEV_15){
            // I print a core dump and then I signal an error: an exception happened while
            // exceptions were disabled in the processor core
            THROW_EXCEPTION("Exception " << exceptionId << " happened while the PSR[ET] = 0; \
                PC = " << std::hex << std::showbase << PC << std::endl << "Instruction " << getMnemonic());
        }
    }
    else{
        unsigned int curPSR = PSR;
        curPSR = (curPSR & 0xffffffbf) | (PSR[key_S] << 6);
        curPSR = (curPSR & 0xffffff7f) | 0x00000080;
        curPSR &= 0xffffffdf;
        unsigned int newCwp = ((unsigned int)(PSR[key_CWP] - 1)) % NUM_REG_WIN;
        for(int i = 8; i < 32; i++){
            REGS[i].updateAlias(WINREGS[(newCwp*16 + i - 8) % (16*NUM_REG_WIN)]);
        }

        curPSR = (curPSR & 0xffffffe0) + newCwp;
        PSR = curPSR;
        PSRbp = curPSR;
        #ifdef ACC_MODEL
        REGS[17] = PC;
        REGS[18] = NPC;
        #else
        REGS[17] = PC - 12;
        REGS[18] = NPC - 4;
        #endif
        switch(exceptionId){
            case RESET:{
            }break;
            case DATA_STORE_ERROR:{
                TBR[key_TT] = 0x2b;
            }break;
            case INSTR_ACCESS_MMU_MISS:{
                TBR[key_TT] = 0x3c;
            }break;
            case INSTR_ACCESS_ERROR:{
                TBR[key_TT] = 0x21;
            }break;
            case R_REGISTER_ACCESS_ERROR:{
                TBR[key_TT] = 0x20;
            }break;
            case INSTR_ACCESS_EXC:{
                TBR[key_TT] = 0x01;
            }break;
            case PRIVILEDGE_INSTR:{
                TBR[key_TT] = 0x03;
            }break;
            case ILLEGAL_INSTR:{
                TBR[key_TT] = 0x02;
            }break;
            case FP_DISABLED:{
                TBR[key_TT] = 0x04;
            }break;
            case CP_DISABLED:{
                TBR[key_TT] = 0x24;
            }break;
            case UNIMPL_FLUSH:{
                TBR[key_TT] = 0x25;
            }break;
            case WATCHPOINT_DETECTED:{
                TBR[key_TT] = 0x0b;
            }break;
            case WINDOW_OVERFLOW:{
                TBR[key_TT] = 0x05;
            }break;
            case WINDOW_UNDERFLOW:{
                TBR[key_TT] = 0x06;
            }break;
            case MEM_ADDR_NOT_ALIGNED:{
                TBR[key_TT] = 0x07;
            }break;
            case FP_EXCEPTION:{
                TBR[key_TT] = 0x08;
            }break;
            case CP_EXCEPTION:{
                TBR[key_TT] = 0x28;
            }break;
            case DATA_ACCESS_ERROR:{
                TBR[key_TT] = 0x29;
            }break;
            case DATA_ACCESS_MMU_MISS:{
                TBR[key_TT] = 0x2c;
            }break;
            case DATA_ACCESS_EXC:{
                TBR[key_TT] = 0x09;
            }break;
            case TAG_OVERFLOW:{
                TBR[key_TT] = 0x0a;
            }break;
            case DIV_ZERO:{
                TBR[key_TT] = 0x2a;
            }break;
            case TRAP_INSTRUCTION:{
                TBR[key_TT] = 0x80 + customTrapOffset;
            }break;
            case IRQ_LEV_15:{
                TBR[key_TT] = 0x1f;
            }break;
            case IRQ_LEV_14:{
                TBR[key_TT] = 0x1e;
            }break;
            case IRQ_LEV_13:{
                TBR[key_TT] = 0x1d;
            }break;
            case IRQ_LEV_12:{
                TBR[key_TT] = 0x1c;
            }break;
            case IRQ_LEV_11:{
                TBR[key_TT] = 0x1b;
            }break;
            case IRQ_LEV_10:{
                TBR[key_TT] = 0x1a;
            }break;
            case IRQ_LEV_9:{
                TBR[key_TT] = 0x19;
            }break;
            case IRQ_LEV_8:{
                TBR[key_TT] = 0x18;
            }break;
            case IRQ_LEV_7:{
                TBR[key_TT] = 0x17;
            }break;
            case IRQ_LEV_6:{
                TBR[key_TT] = 0x16;
            }break;
            case IRQ_LEV_5:{
                TBR[key_TT] = 0x15;
            }break;
            case IRQ_LEV_4:{
                TBR[key_TT] = 0x14;
            }break;
            case IRQ_LEV_3:{
                TBR[key_TT] = 0x13;
            }break;
            case IRQ_LEV_2:{
                TBR[key_TT] = 0x12;
            }break;
            case IRQ_LEV_1:{
                TBR[key_TT] = 0x11;
            }break;
            case IMPL_DEP_EXC:{
                TBR[key_TT] = 0x60 + customTrapOffset;
            }break;
            default:{
            }break;
        }
        if(exceptionId == RESET){
            // I have to jump to address 0 and restart execution
            PC = 0;
            NPC = 4;
        }
        else{
            // I have to jump to the address contained in the TBR register
            PC = TBR;
            NPC = TBR + 4;
        }
        flush();
        annull();
    }
}

leon3_funclt_trap::Instruction::Instruction( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : PSR(PSR), WIM(WIM), \
    TBR(TBR), Y(Y), PC(PC), NPC(NPC), PSRbp(PSRbp), Ybp(Ybp), ASR18bp(ASR18bp), GLOBAL(GLOBAL), \
    WINREGS(WINREGS), ASR(ASR), FP(FP), LR(LR), SP(SP), PCR(PCR), REGS(REGS), instrMem(instrMem), \
    dataMem(dataMem), irqAck(irqAck), NUM_REG_WIN(8){
    this->totalInstrCycles = 0;
}

leon3_funclt_trap::Instruction::~Instruction(){

}
leon3_funclt_trap::IncrementPC_op::~IncrementPC_op(){

}
leon3_funclt_trap::IncrementPC_op::IncrementPC_op( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::WB_plain_op::~WB_plain_op(){

}
leon3_funclt_trap::WB_plain_op::WB_plain_op( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::ICC_writeLogic_op::~ICC_writeLogic_op(){

}
leon3_funclt_trap::ICC_writeLogic_op::ICC_writeLogic_op( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::WB_icc_op::~WB_icc_op(){

}
leon3_funclt_trap::WB_icc_op::WB_icc_op( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & \
    WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::ICC_writeTSub_op::~ICC_writeTSub_op(){

}
leon3_funclt_trap::ICC_writeTSub_op::ICC_writeTSub_op( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::ICC_writeDiv_op::~ICC_writeDiv_op(){

}
leon3_funclt_trap::ICC_writeDiv_op::ICC_writeDiv_op( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::ICC_writeAdd_op::~ICC_writeAdd_op(){

}
leon3_funclt_trap::ICC_writeAdd_op::ICC_writeAdd_op( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::ICC_writeSub_op::~ICC_writeSub_op(){

}
leon3_funclt_trap::ICC_writeSub_op::ICC_writeSub_op( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::WB_yicc_op::~WB_yicc_op(){

}
leon3_funclt_trap::WB_yicc_op::WB_yicc_op( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::WB_yasr_op::~WB_yasr_op(){

}
leon3_funclt_trap::WB_yasr_op::WB_yasr_op( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::WB_y_op::~WB_y_op(){

}
leon3_funclt_trap::WB_y_op::WB_y_op( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck){

}

leon3_funclt_trap::ICC_writeTAdd_op::~ICC_writeTAdd_op(){

}
leon3_funclt_trap::ICC_writeTAdd_op::ICC_writeTAdd_op( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::ICC_writeTVSub_op::~ICC_writeTVSub_op(){

}
leon3_funclt_trap::ICC_writeTVSub_op::ICC_writeTVSub_op( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::WB_icctv_op::~WB_icctv_op(){

}
leon3_funclt_trap::WB_icctv_op::WB_icctv_op( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::ICC_writeTVAdd_op::~ICC_writeTVAdd_op(){

}
leon3_funclt_trap::ICC_writeTVAdd_op::ICC_writeTVAdd_op( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

unsigned int leon3_funclt_trap::InvalidInstr::behavior(){
    THROW_EXCEPTION("Unknown Instruction at PC: " << this->PC-4);
    return 0;
}

Instruction * leon3_funclt_trap::InvalidInstr::replicate() const throw(){
    return new InvalidInstr(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

void leon3_funclt_trap::InvalidInstr::setParams( const unsigned int & bitString ) \
    throw(){

}

std::string leon3_funclt_trap::InvalidInstr::getInstructionName(){
    return "InvalidInstruction";
}

std::string leon3_funclt_trap::InvalidInstr::getMnemonic(){
    return "invalid";
}

leon3_funclt_trap::InvalidInstr::InvalidInstr( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::InvalidInstr::~InvalidInstr(){

}
unsigned int leon3_funclt_trap::READasr::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    asr_temp = ASR[asr];

    rd = asr_temp;
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::READasr::replicate() const throw(){
    return new READasr(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::READasr::getInstructionName(){
    return "READasr";
}

void leon3_funclt_trap::READasr::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->asr = (bitString & 0x7c000) >> 14;
}

std::string leon3_funclt_trap::READasr::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "rd asr ";
    oss << this->asr;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::READasr::READasr( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::READasr::~READasr(){

}
unsigned int leon3_funclt_trap::WRITEY_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    result = rs1 ^ rs2;

    Ybp = result;

    Y = result;
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::WRITEY_reg::replicate() const throw(){
    return new WRITEY_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::WRITEY_reg::getInstructionName(){
    return "WRITEY_reg";
}

void leon3_funclt_trap::WRITEY_reg::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::WRITEY_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "wr r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " y";
    return oss.str();
}

leon3_funclt_trap::WRITEY_reg::WRITEY_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::WRITEY_reg::~WRITEY_reg(){

}
unsigned int leon3_funclt_trap::XNOR_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    result = rs1_op ^ ~rs2_op;
    this->WB_plain(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::XNOR_reg::replicate() const throw(){
    return new XNOR_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::XNOR_reg::getInstructionName(){
    return "XNOR_reg";
}

void leon3_funclt_trap::XNOR_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::XNOR_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "xnor r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::XNOR_reg::XNOR_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), WB_plain_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, \
    GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::XNOR_reg::~XNOR_reg(){

}
unsigned int leon3_funclt_trap::ANDNcc_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    result = rs1_op & ~rs2_op;
    this->ICC_writeLogic(this->result);
    this->WB_icc(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::ANDNcc_reg::replicate() const throw(){
    return new ANDNcc_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::ANDNcc_reg::getInstructionName(){
    return "ANDNcc_reg";
}

void leon3_funclt_trap::ANDNcc_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::ANDNcc_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "andncc r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::ANDNcc_reg::ANDNcc_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), ICC_writeLogic_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_icc_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, \
    LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, \
    NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::ANDNcc_reg::~ANDNcc_reg(){

}
unsigned int leon3_funclt_trap::LDSB_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    address = rs1 + SignExtend(simm13, 13);

    readValue = SignExtend(dataMem.read_byte(address), 8);

    rd = readValue;
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::LDSB_imm::replicate() const throw(){
    return new LDSB_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::LDSB_imm::getInstructionName(){
    return "LDSB_imm";
}

void leon3_funclt_trap::LDSB_imm::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::LDSB_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "ldsb r";
    oss << this->rs1_bit;
    oss << "+";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::LDSB_imm::LDSB_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::LDSB_imm::~LDSB_imm(){

}
unsigned int leon3_funclt_trap::WRITEpsr_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    // Note how we filter writes to EF and EC fields since we do not
    // have neither a co-processor nor the FPU
    result = ((rs1 ^ SignExtend(simm13, 13)) & 0x00FFCFFF) | 0xF3000000;
    supervisorException = (PSR[key_S] == 0);
    illegalCWP = (result & 0x0000001f) >= NUM_REG_WIN;

    if(!(supervisorException || illegalCWP)){
        PSRbp = result;
    }

    if(supervisorException){
        RaiseException(PRIVILEDGE_INSTR);
    }
    if(illegalCWP){
        RaiseException(ILLEGAL_INSTR);
    }

    PSR = result;
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::WRITEpsr_imm::replicate() const throw(){
    return new WRITEpsr_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::WRITEpsr_imm::getInstructionName(){
    return "WRITEpsr_imm";
}

void leon3_funclt_trap::WRITEpsr_imm::setParams( const unsigned int & bitString ) \
    throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rd = (bitString & 0x3e000000) >> 25;
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::WRITEpsr_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "wr r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " psr";
    return oss.str();
}

leon3_funclt_trap::WRITEpsr_imm::WRITEpsr_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::WRITEpsr_imm::~WRITEpsr_imm(){

}
unsigned int leon3_funclt_trap::READy::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    y_temp = Y;

    rd = y_temp;
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::READy::replicate() const throw(){
    return new READy(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::READy::getInstructionName(){
    return "READy";
}

void leon3_funclt_trap::READy::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
}

std::string leon3_funclt_trap::READy::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "rd ";
    oss << "y";
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::READy::READy( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, Reg32_2 \
    & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 & \
    Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::READy::~READy(){

}
unsigned int leon3_funclt_trap::XNORcc_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    result = rs1_op ^ ~rs2_op;
    this->ICC_writeLogic(this->result);
    this->WB_icc(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::XNORcc_reg::replicate() const throw(){
    return new XNORcc_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::XNORcc_reg::getInstructionName(){
    return "XNORcc_reg";
}

void leon3_funclt_trap::XNORcc_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::XNORcc_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "xnorcc r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::XNORcc_reg::XNORcc_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), ICC_writeLogic_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_icc_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, \
    LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, \
    NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::XNORcc_reg::~XNORcc_reg(){

}
unsigned int leon3_funclt_trap::READpsr::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    psr_temp = PSR;
    supervisor = (psr_temp & 0x00000080) != 0;

    if(!supervisor){
        RaiseException(PRIVILEDGE_INSTR);
    }

    rd = psr_temp;
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::READpsr::replicate() const throw(){
    return new READpsr(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::READpsr::getInstructionName(){
    return "READpsr";
}

void leon3_funclt_trap::READpsr::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->asr = (bitString & 0x7c000) >> 14;
}

std::string leon3_funclt_trap::READpsr::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "rd ";
    oss << "psr r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::READpsr::READpsr( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::READpsr::~READpsr(){

}
unsigned int leon3_funclt_trap::ANDN_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;

    result = rs1_op & ~(SignExtend(simm13, 13));
    this->WB_plain(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::ANDN_imm::replicate() const throw(){
    return new ANDN_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::ANDN_imm::getInstructionName(){
    return "ANDN_imm";
}

void leon3_funclt_trap::ANDN_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::ANDN_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "andn r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::ANDN_imm::ANDN_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), WB_plain_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, \
    GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::ANDN_imm::~ANDN_imm(){

}
unsigned int leon3_funclt_trap::ANDcc_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    result = rs1_op & rs2_op;
    this->ICC_writeLogic(this->result);
    this->WB_icc(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::ANDcc_reg::replicate() const throw(){
    return new ANDcc_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::ANDcc_reg::getInstructionName(){
    return "ANDcc_reg";
}

void leon3_funclt_trap::ANDcc_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::ANDcc_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "andcc r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::ANDcc_reg::ANDcc_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & \
    WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), ICC_writeLogic_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_icc_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, \
    LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, \
    NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::ANDcc_reg::~ANDcc_reg(){

}
unsigned int leon3_funclt_trap::TSUBcc_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = SignExtend(simm13, 13);

    result = rs1_op - rs2_op;
    temp_V = ((unsigned int)((rs1_op & (~rs2_op) & (~result)) | ((~rs1_op) & rs2_op & \
        result))) >> 31;
    if(!temp_V && (((rs1_op | rs2_op) & 0x00000003) != 0)){
        temp_V = 1;
    }
    this->ICC_writeTSub(this->result, this->temp_V, this->rs1_op, this->rs2_op);
    this->WB_icc(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::TSUBcc_imm::replicate() const throw(){
    return new TSUBcc_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::TSUBcc_imm::getInstructionName(){
    return "TSUBcc_imm";
}

void leon3_funclt_trap::TSUBcc_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::TSUBcc_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "tsubcc r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::TSUBcc_imm::TSUBcc_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), ICC_writeTSub_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_icc_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, \
    LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, \
    NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::TSUBcc_imm::~TSUBcc_imm(){

}
unsigned int leon3_funclt_trap::LDSBA_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    address = rs1 + rs2;
    supervisor = PSR[key_S];

    if(!supervisor){
        flush();
    }

    if(!supervisor){
        flush();
    }
    else{
        readValue = SignExtend(dataMem.read_byte(address), 8);
    }

    if(!supervisor){
        RaiseException(PRIVILEDGE_INSTR);
    }

    rd = readValue;
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::LDSBA_reg::replicate() const throw(){
    return new LDSBA_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::LDSBA_reg::getInstructionName(){
    return "LDSBA_reg";
}

void leon3_funclt_trap::LDSBA_reg::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->asi = (bitString & 0x1fe0) >> 5;
}

std::string leon3_funclt_trap::LDSBA_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "ldba r";
    oss << this->rs1_bit;
    oss << "+r";
    oss << this->rs2_bit;
    oss << " ";
    oss << this->asi;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::LDSBA_reg::LDSBA_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & \
    WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::LDSBA_reg::~LDSBA_reg(){

}
unsigned int leon3_funclt_trap::LDUH_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    address = rs1 + SignExtend(simm13, 13);

    notAligned = (address & 0x00000001) != 0;
    if(notAligned){
        flush();
    }

    readValue = dataMem.read_half(address);

    if(notAligned){
        RaiseException(MEM_ADDR_NOT_ALIGNED);
    }

    rd = readValue;
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::LDUH_imm::replicate() const throw(){
    return new LDUH_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::LDUH_imm::getInstructionName(){
    return "LDUH_imm";
}

void leon3_funclt_trap::LDUH_imm::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::LDUH_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "lduh r";
    oss << this->rs1_bit;
    oss << "+";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::LDUH_imm::LDUH_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::LDUH_imm::~LDUH_imm(){

}
unsigned int leon3_funclt_trap::STA_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    address = rs1 + rs2;
    toWrite = rd;
    supervisor = PSR[key_S];

    notAligned = (address & 0x00000003) != 0;
    if(!supervisor || notAligned){
        flush();
    }

    if(supervisor || !notAligned){
        dataMem.write_word(address, toWrite);
    }
    else{
        flush();
    }

    if(!supervisor){
        RaiseException(PRIVILEDGE_INSTR);
    }
    if(notAligned){
        RaiseException(MEM_ADDR_NOT_ALIGNED);
    }
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::STA_reg::replicate() const throw(){
    return new STA_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::STA_reg::getInstructionName(){
    return "STA_reg";
}

void leon3_funclt_trap::STA_reg::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->asi = (bitString & 0x1fe0) >> 5;
}

std::string leon3_funclt_trap::STA_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "sta r";
    oss << this->rd_bit;
    oss << " r";
    oss << this->rs1_bit;
    oss << "+r";
    oss << this->rs2_bit;
    oss << " ";
    oss << this->asi;
    return oss.str();
}

leon3_funclt_trap::STA_reg::STA_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::STA_reg::~STA_reg(){

}
unsigned int leon3_funclt_trap::ORN_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    result = rs1_op | ~rs2_op;
    this->WB_plain(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::ORN_reg::replicate() const throw(){
    return new ORN_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::ORN_reg::getInstructionName(){
    return "ORN_reg";
}

void leon3_funclt_trap::ORN_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::ORN_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "orn r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::ORN_reg::ORN_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), WB_plain_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, \
    GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::ORN_reg::~ORN_reg(){

}
unsigned int leon3_funclt_trap::LDSHA_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    address = rs1 + rs2;
    supervisor = PSR[key_S];

    notAligned = (address & 0x00000001) != 0;
    if(notAligned || !supervisor){
        flush();
    }

    if(notAligned || !supervisor){
        flush();
    }
    else{
        readValue = SignExtend(dataMem.read_half(address), 16);
    }

    if(!supervisor){
        RaiseException(PRIVILEDGE_INSTR);
    }
    if(notAligned){
        RaiseException(MEM_ADDR_NOT_ALIGNED);
    }

    rd = readValue;
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::LDSHA_reg::replicate() const throw(){
    return new LDSHA_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::LDSHA_reg::getInstructionName(){
    return "LDSHA_reg";
}

void leon3_funclt_trap::LDSHA_reg::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->asi = (bitString & 0x1fe0) >> 5;
}

std::string leon3_funclt_trap::LDSHA_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "ldsha r";
    oss << this->rs1_bit;
    oss << "+r";
    oss << this->rs2_bit;
    oss << " ";
    oss << this->asi;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::LDSHA_reg::LDSHA_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & \
    WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::LDSHA_reg::~LDSHA_reg(){

}
unsigned int leon3_funclt_trap::STBA_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    address = rs1 + rs2;
    toWrite = (unsigned char)(rd & 0x000000FF);
    supervisor = PSR[key_S];

    if(!supervisor){
        flush();
    }

    if(supervisor){
        dataMem.write_byte(address, toWrite);
    }
    else{
        flush();
    }

    if(!supervisor){
        RaiseException(PRIVILEDGE_INSTR);
    }
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::STBA_reg::replicate() const throw(){
    return new STBA_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::STBA_reg::getInstructionName(){
    return "STBA_reg";
}

void leon3_funclt_trap::STBA_reg::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->asi = (bitString & 0x1fe0) >> 5;
}

std::string leon3_funclt_trap::STBA_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "stba r";
    oss << this->rd_bit;
    oss << " r";
    oss << this->rs1_bit;
    oss << "+r";
    oss << this->rs2_bit;
    oss << " ";
    oss << this->asi;
    return oss.str();
}

leon3_funclt_trap::STBA_reg::STBA_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::STBA_reg::~STBA_reg(){

}
unsigned int leon3_funclt_trap::ST_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    address = rs1 + SignExtend(simm13, 13);
    toWrite = rd;

    notAligned = (address & 0x00000003) != 0;
    if(notAligned){
        flush();
    }

    if(!notAligned){
        dataMem.write_word(address, toWrite);
    }
    else{
        flush();
    }

    if(notAligned){
        RaiseException(MEM_ADDR_NOT_ALIGNED);
    }
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::ST_imm::replicate() const throw(){
    return new ST_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::ST_imm::getInstructionName(){
    return "ST_imm";
}

void leon3_funclt_trap::ST_imm::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::ST_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "st r";
    oss << this->rd_bit;
    oss << " r";
    oss << this->rs1_bit;
    oss << "+";
    oss << this->simm13;
    return oss.str();
}

leon3_funclt_trap::ST_imm::ST_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::ST_imm::~ST_imm(){

}
unsigned int leon3_funclt_trap::READtbr::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    tbr_temp = TBR;
    supervisor = PSR[key_S];

    if(!supervisor){
        RaiseException(PRIVILEDGE_INSTR);
    }

    rd = tbr_temp;
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::READtbr::replicate() const throw(){
    return new READtbr(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::READtbr::getInstructionName(){
    return "READtbr";
}

void leon3_funclt_trap::READtbr::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->asr = (bitString & 0x7c000) >> 14;
}

std::string leon3_funclt_trap::READtbr::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "rd ";
    oss << "tbr r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::READtbr::READtbr( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::READtbr::~READtbr(){

}
unsigned int leon3_funclt_trap::UDIVcc_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = SignExtend(simm13, 13);

    exception = rs2_op == 0;
    if(!exception){
        unsigned long long res64 = ((unsigned long long)((((unsigned long long)Ybp) << 32) \
            | (unsigned long long)rs1_op))/(unsigned long long)rs2_op;
        temp_V = (res64 & 0xFFFFFFFF00000000LL) != 0;
        if(temp_V){
            result = 0xFFFFFFFF;
        }
        else{
            result = (unsigned int)(res64 & 0x00000000FFFFFFFFLL);
        }
    }
    this->ICC_writeDiv(this->result, this->temp_V, this->rs1_op, this->rs2_op);

    if(exception){
        RaiseException(DIV_ZERO);
    }
    this->WB_icc(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::UDIVcc_imm::replicate() const throw(){
    return new UDIVcc_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::UDIVcc_imm::getInstructionName(){
    return "UDIVcc_imm";
}

void leon3_funclt_trap::UDIVcc_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::UDIVcc_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "udivcc";
    oss << " r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::UDIVcc_imm::UDIVcc_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), ICC_writeDiv_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, \
    FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), WB_icc_op(PSR, WIM, TBR, Y, PC, \
    NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::UDIVcc_imm::~UDIVcc_imm(){

}
unsigned int leon3_funclt_trap::SWAPA_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    address = rs1 + rs2;
    toWrite = rd;
    supervisor = PSR[key_S];

    if(!supervisor){
        RaiseException(PRIVILEDGE_INSTR);
    }
    if(notAligned){
        RaiseException(MEM_ADDR_NOT_ALIGNED);
    }

    if(!supervisor || notAligned){
        flush();
    }
    else{
        readValue = dataMem.read_word(address);
        dataMem.write_word(address, toWrite);
    }

    notAligned = (address & 0x00000003) != 0;
    if(!supervisor || notAligned){
        flush();
    }

    rd = readValue;
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::SWAPA_reg::replicate() const throw(){
    return new SWAPA_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::SWAPA_reg::getInstructionName(){
    return "SWAPA_reg";
}

void leon3_funclt_trap::SWAPA_reg::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->asi = (bitString & 0x1fe0) >> 5;
}

std::string leon3_funclt_trap::SWAPA_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "swapa r";
    oss << this->rs1_bit;
    oss << "+r";
    oss << this->rs2_bit;
    oss << " ";
    oss << this->asi;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::SWAPA_reg::SWAPA_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & \
    WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::SWAPA_reg::~SWAPA_reg(){

}
unsigned int leon3_funclt_trap::ADDXcc_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = SignExtend(simm13, 13);

    result = rs1_op + rs2_op + PSRbp[key_ICC_c];
    this->ICC_writeAdd(this->result, this->rs1_op, this->rs2_op);
    this->WB_icc(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::ADDXcc_imm::replicate() const throw(){
    return new ADDXcc_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::ADDXcc_imm::getInstructionName(){
    return "ADDXcc_imm";
}

void leon3_funclt_trap::ADDXcc_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::ADDXcc_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "addxcc r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::ADDXcc_imm::ADDXcc_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), ICC_writeAdd_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_icc_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, \
    LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, \
    NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::ADDXcc_imm::~ADDXcc_imm(){

}
unsigned int leon3_funclt_trap::STB_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    address = rs1 + SignExtend(simm13, 13);
    toWrite = (unsigned char)(rd & 0x000000FF);

    dataMem.write_byte(address, toWrite);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::STB_imm::replicate() const throw(){
    return new STB_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::STB_imm::getInstructionName(){
    return "STB_imm";
}

void leon3_funclt_trap::STB_imm::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::STB_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "stb r";
    oss << this->rd_bit;
    oss << " r";
    oss << this->rs1_bit;
    oss << "+";
    oss << this->simm13;
    return oss.str();
}

leon3_funclt_trap::STB_imm::STB_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::STB_imm::~STB_imm(){

}
unsigned int leon3_funclt_trap::SUBXcc_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = SignExtend(simm13, 13);

    result = rs1_op - rs2_op - PSRbp[key_ICC_c];
    this->ICC_writeSub(this->result, this->rs1_op, this->rs2_op);
    this->WB_icc(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::SUBXcc_imm::replicate() const throw(){
    return new SUBXcc_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::SUBXcc_imm::getInstructionName(){
    return "SUBXcc_imm";
}

void leon3_funclt_trap::SUBXcc_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::SUBXcc_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "subxcc r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::SUBXcc_imm::SUBXcc_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), ICC_writeSub_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_icc_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, \
    LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, \
    NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::SUBXcc_imm::~SUBXcc_imm(){

}
unsigned int leon3_funclt_trap::STH_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    address = rs1 + rs2;
    toWrite = (unsigned short int)(rd & 0x0000FFFF);

    notAligned = (address & 0x00000001) != 0;
    if(notAligned){
        flush();
    }

    if(!notAligned){
        dataMem.write_half(address, toWrite);
    }
    else{
        flush();
    }

    if(notAligned){
        RaiseException(MEM_ADDR_NOT_ALIGNED);
    }
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::STH_reg::replicate() const throw(){
    return new STH_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::STH_reg::getInstructionName(){
    return "STH_reg";
}

void leon3_funclt_trap::STH_reg::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->asi = (bitString & 0x1fe0) >> 5;
}

std::string leon3_funclt_trap::STH_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "sth r";
    oss << this->rd_bit;
    oss << " r";
    oss << this->rs1_bit;
    oss << "+r";
    oss << this->rs2_bit;
    return oss.str();
}

leon3_funclt_trap::STH_reg::STH_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::STH_reg::~STH_reg(){

}
unsigned int leon3_funclt_trap::SRL_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;

    result = ((unsigned int)rs1_op) >> simm13;
    this->WB_plain(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::SRL_imm::replicate() const throw(){
    return new SRL_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::SRL_imm::getInstructionName(){
    return "SRL_imm";
}

void leon3_funclt_trap::SRL_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::SRL_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "srl r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::SRL_imm::SRL_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), WB_plain_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, \
    GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::SRL_imm::~SRL_imm(){

}
unsigned int leon3_funclt_trap::WRITEasr_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    result = rs1 ^ SignExtend(simm13, 13);

    if(rd == 18){
        ASR18bp = result;
    }

    ASR[rd] = result;
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::WRITEasr_imm::replicate() const throw(){
    return new WRITEasr_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::WRITEasr_imm::getInstructionName(){
    return "WRITEasr_imm";
}

void leon3_funclt_trap::WRITEasr_imm::setParams( const unsigned int & bitString ) \
    throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rd = (bitString & 0x3e000000) >> 25;
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::WRITEasr_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "wr r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " asr";
    oss << this->rd;
    return oss.str();
}

leon3_funclt_trap::WRITEasr_imm::WRITEasr_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::WRITEasr_imm::~WRITEasr_imm(){

}
unsigned int leon3_funclt_trap::UMULcc_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    unsigned long long resultTemp = (unsigned long long)(((unsigned long long)((unsigned \
        int)rs1_op))*((unsigned long long)((unsigned int)rs2_op)));
    Ybp = resultTemp >> 32;
    result = resultTemp & 0x00000000FFFFFFFF;
    this->ICC_writeLogic(this->result);
    this->WB_yicc(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::UMULcc_reg::replicate() const throw(){
    return new UMULcc_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::UMULcc_reg::getInstructionName(){
    return "UMULcc_reg";
}

void leon3_funclt_trap::UMULcc_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::UMULcc_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "umulcc r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::UMULcc_reg::UMULcc_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), ICC_writeLogic_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_yicc_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, \
    FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, \
    Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::UMULcc_reg::~UMULcc_reg(){

}
unsigned int leon3_funclt_trap::LDSTUB_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    address = rs1 + rs2;

    readValue = dataMem.read_byte(address);
    dataMem.write_byte(address, 0xff);

    rd = readValue;
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::LDSTUB_reg::replicate() const throw(){
    return new LDSTUB_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::LDSTUB_reg::getInstructionName(){
    return "LDSTUB_reg";
}

void leon3_funclt_trap::LDSTUB_reg::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->asi = (bitString & 0x1fe0) >> 5;
}

std::string leon3_funclt_trap::LDSTUB_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "ldastub r";
    oss << this->rs1_bit;
    oss << "+r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::LDSTUB_reg::LDSTUB_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::LDSTUB_reg::~LDSTUB_reg(){

}
unsigned int leon3_funclt_trap::XOR_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;

    result = rs1_op ^ SignExtend(simm13, 13);
    this->WB_plain(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::XOR_imm::replicate() const throw(){
    return new XOR_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::XOR_imm::getInstructionName(){
    return "XOR_imm";
}

void leon3_funclt_trap::XOR_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::XOR_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "xor r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::XOR_imm::XOR_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), WB_plain_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, \
    GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::XOR_imm::~XOR_imm(){

}
unsigned int leon3_funclt_trap::SMAC_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    int resultTemp = ((int)SignExtend(rs1_op & 0x0000ffff, 16))*((int)SignExtend(rs2_op \
        & 0x0000ffff, 16));
    long long resultAcc = ((((long long)(Ybp & 0x000000ff)) << 32) | (int)ASR18bp) + \
        resultTemp;
    Ybp = (resultAcc & 0x000000ff00000000LL) >> 32;
    ASR18bp = resultAcc & 0x00000000FFFFFFFFLL;
    result = resultAcc & 0x00000000FFFFFFFFLL;
    this->WB_yasr(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::SMAC_reg::replicate() const throw(){
    return new SMAC_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::SMAC_reg::getInstructionName(){
    return "SMAC_reg";
}

void leon3_funclt_trap::SMAC_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::SMAC_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "smac r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::SMAC_reg::SMAC_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), WB_yasr_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, \
    GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::SMAC_reg::~SMAC_reg(){

}
unsigned int leon3_funclt_trap::WRITEasr_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    result = rs1 ^ rs2;

    if(rd == 18){
        ASR18bp = result;
    }

    ASR[rd] = result;
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::WRITEasr_reg::replicate() const throw(){
    return new WRITEasr_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::WRITEasr_reg::getInstructionName(){
    return "WRITEasr_reg";
}

void leon3_funclt_trap::WRITEasr_reg::setParams( const unsigned int & bitString ) \
    throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
    this->rd = (bitString & 0x3e000000) >> 25;
}

std::string leon3_funclt_trap::WRITEasr_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "wr r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " asr";
    oss << this->rd;
    return oss.str();
}

leon3_funclt_trap::WRITEasr_reg::WRITEasr_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::WRITEasr_reg::~WRITEasr_reg(){

}
unsigned int leon3_funclt_trap::LD_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    address = rs1 + rs2;

    notAligned = (address & 0x00000003) != 0;
    if(notAligned){
        flush();
    }

    readValue = dataMem.read_word(address);

    if(notAligned){
        RaiseException(MEM_ADDR_NOT_ALIGNED);
    }

    rd = readValue;
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::LD_reg::replicate() const throw(){
    return new LD_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::LD_reg::getInstructionName(){
    return "LD_reg";
}

void leon3_funclt_trap::LD_reg::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->asi = (bitString & 0x1fe0) >> 5;
}

std::string leon3_funclt_trap::LD_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "ld r";
    oss << this->rs1_bit;
    oss << "+r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::LD_reg::LD_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::LD_reg::~LD_reg(){

}
unsigned int leon3_funclt_trap::ST_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    address = rs1 + rs2;
    toWrite = rd;

    notAligned = (address & 0x00000003) != 0;
    if(notAligned){
        flush();
    }

    if(!notAligned){
        dataMem.write_word(address, toWrite);
    }
    else{
        flush();
    }

    if(notAligned){
        RaiseException(MEM_ADDR_NOT_ALIGNED);
    }
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::ST_reg::replicate() const throw(){
    return new ST_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::ST_reg::getInstructionName(){
    return "ST_reg";
}

void leon3_funclt_trap::ST_reg::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->asi = (bitString & 0x1fe0) >> 5;
}

std::string leon3_funclt_trap::ST_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "st r";
    oss << this->rd_bit;
    oss << " r";
    oss << this->rs1_bit;
    oss << "+r";
    oss << this->rs2_bit;
    return oss.str();
}

leon3_funclt_trap::ST_reg::ST_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::ST_reg::~ST_reg(){

}
unsigned int leon3_funclt_trap::SUBcc_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    result = rs1_op - rs2_op;
    this->ICC_writeSub(this->result, this->rs1_op, this->rs2_op);
    this->WB_icc(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::SUBcc_reg::replicate() const throw(){
    return new SUBcc_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::SUBcc_reg::getInstructionName(){
    return "SUBcc_reg";
}

void leon3_funclt_trap::SUBcc_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::SUBcc_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "subcc r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::SUBcc_reg::SUBcc_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & \
    WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), ICC_writeSub_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_icc_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, \
    LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, \
    NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::SUBcc_reg::~SUBcc_reg(){

}
unsigned int leon3_funclt_trap::LDD_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    #ifdef ACC_MODEL
    REGS[rd_bit | 0x1].lock();
    #endif

    address = rs1 + rs2;

    notAligned = (address & 0x00000007) != 0;
    if(notAligned){
        #ifdef ACC_MODEL
        REGS[rd_bit | 0x1].unlock();
        #endif
        flush();
    }

    readValue = dataMem.read_dword(address);

    if(notAligned){
        RaiseException(MEM_ADDR_NOT_ALIGNED);
    }

    if(rd_bit % 2 == 0){
        rd = (unsigned int)(readValue & 0x00000000FFFFFFFFLL);
        REGS[rd_bit + 1] = (unsigned int)((readValue >> 32) & 0x00000000FFFFFFFFLL);
        #ifdef ACC_MODEL
        REGS[rd_bit + 1].unlock();
        #endif
    }
    else{
        REGS[rd_bit - 1] = (unsigned int)(readValue & 0x00000000FFFFFFFFLL);
        rd = (unsigned int)((readValue >> 32) & 0x00000000FFFFFFFFLL);
        #ifdef ACC_MODEL
        REGS[rd_bit - 1].unlock();
        #endif
    }
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::LDD_reg::replicate() const throw(){
    return new LDD_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::LDD_reg::getInstructionName(){
    return "LDD_reg";
}

void leon3_funclt_trap::LDD_reg::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->asi = (bitString & 0x1fe0) >> 5;
}

std::string leon3_funclt_trap::LDD_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "ldd r";
    oss << this->rs1_bit;
    oss << "+r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::LDD_reg::LDD_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::LDD_reg::~LDD_reg(){

}
unsigned int leon3_funclt_trap::ADDcc_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = SignExtend(simm13, 13);

    result = rs1_op + rs2_op;
    this->ICC_writeAdd(this->result, this->rs1_op, this->rs2_op);
    this->WB_icc(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::ADDcc_imm::replicate() const throw(){
    return new ADDcc_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::ADDcc_imm::getInstructionName(){
    return "ADDcc_imm";
}

void leon3_funclt_trap::ADDcc_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::ADDcc_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "addcc r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::ADDcc_imm::ADDcc_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & \
    WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), ICC_writeAdd_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_icc_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, \
    LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, \
    NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::ADDcc_imm::~ADDcc_imm(){

}
unsigned int leon3_funclt_trap::LDUH_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    address = rs1 + rs2;

    notAligned = (address & 0x00000001) != 0;
    if(notAligned){
        flush();
    }

    readValue = dataMem.read_half(address);

    if(notAligned){
        RaiseException(MEM_ADDR_NOT_ALIGNED);
    }

    rd = readValue;
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::LDUH_reg::replicate() const throw(){
    return new LDUH_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::LDUH_reg::getInstructionName(){
    return "LDUH_reg";
}

void leon3_funclt_trap::LDUH_reg::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->asi = (bitString & 0x1fe0) >> 5;
}

std::string leon3_funclt_trap::LDUH_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "lduh r";
    oss << this->rs1_bit;
    oss << "+r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::LDUH_reg::LDUH_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::LDUH_reg::~LDUH_reg(){

}
unsigned int leon3_funclt_trap::SRL_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    result = ((unsigned int)rs1_op) >> (rs2_op & 0x0000001f);
    this->WB_plain(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::SRL_reg::replicate() const throw(){
    return new SRL_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::SRL_reg::getInstructionName(){
    return "SRL_reg";
}

void leon3_funclt_trap::SRL_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::SRL_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "srl r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::SRL_reg::SRL_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), WB_plain_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, \
    GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::SRL_reg::~SRL_reg(){

}
unsigned int leon3_funclt_trap::SAVE_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = SignExtend(simm13, 13);

    result = rs1_op + rs2_op;
    okNewWin = DecrementRegWindow();

    if(!okNewWin){
        RaiseException(WINDOW_OVERFLOW);
    }

    if(okNewWin){
        rd = result;
    }
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::SAVE_imm::replicate() const throw(){
    return new SAVE_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::SAVE_imm::getInstructionName(){
    return "SAVE_imm";
}

void leon3_funclt_trap::SAVE_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::SAVE_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "save";
    oss << " r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::SAVE_imm::SAVE_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::SAVE_imm::~SAVE_imm(){

}
unsigned int leon3_funclt_trap::MULScc_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    unsigned int yNew = (((unsigned int)Ybp) >> 1) | (rs1_op << 31);
    rs1_op = ((PSRbp[key_ICC_n] ^ PSRbp[key_ICC_v]) << 31) | (((unsigned int)rs1_op) >> 1);
    result = rs1_op;
    if((Ybp & 0x00000001) != 0){
        result += rs2_op;
    }
    else{
        rs2_op = 0;
    }
    Ybp = yNew;
    this->ICC_writeAdd(this->result, this->rs1_op, this->rs2_op);
    this->WB_yicc(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::MULScc_reg::replicate() const throw(){
    return new MULScc_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::MULScc_reg::getInstructionName(){
    return "MULScc_reg";
}

void leon3_funclt_trap::MULScc_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::MULScc_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "mulscc r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::MULScc_reg::MULScc_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), ICC_writeAdd_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_yicc_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, \
    FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, \
    Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::MULScc_reg::~MULScc_reg(){

}
unsigned int leon3_funclt_trap::OR_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;

    result = rs1_op | SignExtend(simm13, 13);
    this->WB_plain(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::OR_imm::replicate() const throw(){
    return new OR_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::OR_imm::getInstructionName(){
    return "OR_imm";
}

void leon3_funclt_trap::OR_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::OR_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "or r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::OR_imm::OR_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), WB_plain_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, \
    GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::OR_imm::~OR_imm(){

}
unsigned int leon3_funclt_trap::STD_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    address = rs1 + SignExtend(simm13, 13);
    if(rd_bit % 2 == 0){
        toWrite = rd | (((unsigned long long)REGS[rd_bit + 1]) << 32);
    }
    else{
        toWrite = REGS[rd_bit - 1] | (((unsigned long long)rd) << 32);
    }

    notAligned = (address & 0x00000007) != 0;
    if(notAligned){
        flush();
    }

    if(!notAligned){
        dataMem.write_dword(address, toWrite);
    }
    else{
        flush();
    }

    if(notAligned){
        RaiseException(MEM_ADDR_NOT_ALIGNED);
    }
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::STD_imm::replicate() const throw(){
    return new STD_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::STD_imm::getInstructionName(){
    return "STD_imm";
}

void leon3_funclt_trap::STD_imm::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::STD_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "std r";
    oss << this->rd_bit;
    oss << " r";
    oss << this->rs1_bit;
    oss << "+";
    oss << this->simm13;
    return oss.str();
}

leon3_funclt_trap::STD_imm::STD_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::STD_imm::~STD_imm(){

}
unsigned int leon3_funclt_trap::SUBXcc_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    result = rs1_op - rs2_op - PSRbp[key_ICC_c];
    this->ICC_writeSub(this->result, this->rs1_op, this->rs2_op);
    this->WB_icc(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::SUBXcc_reg::replicate() const throw(){
    return new SUBXcc_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::SUBXcc_reg::getInstructionName(){
    return "SUBXcc_reg";
}

void leon3_funclt_trap::SUBXcc_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::SUBXcc_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "subxcc r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::SUBXcc_reg::SUBXcc_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), ICC_writeSub_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_icc_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, \
    LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, \
    NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::SUBXcc_reg::~SUBXcc_reg(){

}
unsigned int leon3_funclt_trap::ADDX_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = SignExtend(simm13, 13);

    result = rs1_op + rs2_op + PSRbp[key_ICC_c];
    this->WB_plain(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::ADDX_imm::replicate() const throw(){
    return new ADDX_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::ADDX_imm::getInstructionName(){
    return "ADDX_imm";
}

void leon3_funclt_trap::ADDX_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::ADDX_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "addx r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::ADDX_imm::ADDX_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), WB_plain_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, \
    GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::ADDX_imm::~ADDX_imm(){

}
unsigned int leon3_funclt_trap::SWAP_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    address = rs1 + SignExtend(simm13, 13);
    toWrite = rd;

    if(notAligned){
        RaiseException(MEM_ADDR_NOT_ALIGNED);
    }

    if(notAligned){
        flush();
    }
    else{
        readValue = dataMem.read_word(address);
        dataMem.write_word(address, toWrite);
    }

    notAligned = (address & 0x00000003) != 0;
    if(notAligned){
        flush();
    }

    rd = readValue;
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::SWAP_imm::replicate() const throw(){
    return new SWAP_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::SWAP_imm::getInstructionName(){
    return "SWAP_imm";
}

void leon3_funclt_trap::SWAP_imm::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::SWAP_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "swap r";
    oss << this->rs1_bit;
    oss << "+";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::SWAP_imm::SWAP_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::SWAP_imm::~SWAP_imm(){

}
unsigned int leon3_funclt_trap::UMUL_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    unsigned long long resultTemp = (unsigned long long)(((unsigned long long)((unsigned \
        int)rs1_op))*((unsigned long long)((unsigned int)rs2_op)));
    Ybp = resultTemp >> 32;
    result = resultTemp & 0x00000000FFFFFFFF;
    this->WB_y(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::UMUL_reg::replicate() const throw(){
    return new UMUL_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::UMUL_reg::getInstructionName(){
    return "UMUL_reg";
}

void leon3_funclt_trap::UMUL_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::UMUL_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "umul r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::UMUL_reg::UMUL_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), WB_y_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, \
    GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::UMUL_reg::~UMUL_reg(){

}
unsigned int leon3_funclt_trap::WRITEY_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    result = rs1 ^ SignExtend(simm13, 13);

    Ybp = result;

    Y = result;
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::WRITEY_imm::replicate() const throw(){
    return new WRITEY_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::WRITEY_imm::getInstructionName(){
    return "WRITEY_imm";
}

void leon3_funclt_trap::WRITEY_imm::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::WRITEY_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "wr r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " y";
    return oss.str();
}

leon3_funclt_trap::WRITEY_imm::WRITEY_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::WRITEY_imm::~WRITEY_imm(){

}
unsigned int leon3_funclt_trap::AND_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    result = rs1_op & rs2_op;
    this->WB_plain(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::AND_reg::replicate() const throw(){
    return new AND_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::AND_reg::getInstructionName(){
    return "AND_reg";
}

void leon3_funclt_trap::AND_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::AND_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "and r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::AND_reg::AND_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), WB_plain_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, \
    GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::AND_reg::~AND_reg(){

}
unsigned int leon3_funclt_trap::FLUSH_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::FLUSH_imm::replicate() const throw(){
    return new FLUSH_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::FLUSH_imm::getInstructionName(){
    return "FLUSH_imm";
}

void leon3_funclt_trap::FLUSH_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::FLUSH_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "flush r";
    oss << this->rs1_bit;
    oss << "+";
    oss << this->simm13;
    return oss.str();
}

leon3_funclt_trap::FLUSH_imm::FLUSH_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & \
    WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::FLUSH_imm::~FLUSH_imm(){

}
unsigned int leon3_funclt_trap::SRA_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    result = ((int)rs1_op) >> (rs2_op & 0x0000001f);
    this->WB_plain(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::SRA_reg::replicate() const throw(){
    return new SRA_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::SRA_reg::getInstructionName(){
    return "SRA_reg";
}

void leon3_funclt_trap::SRA_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::SRA_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "sra r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::SRA_reg::SRA_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), WB_plain_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, \
    GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::SRA_reg::~SRA_reg(){

}
unsigned int leon3_funclt_trap::STH_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    address = rs1 + SignExtend(simm13, 13);
    toWrite = (unsigned short int)(rd & 0x0000FFFF);

    notAligned = (address & 0x00000001) != 0;
    if(notAligned){
        flush();
    }

    if(!notAligned){
        dataMem.write_half(address, toWrite);
    }
    else{
        flush();
    }

    if(notAligned){
        RaiseException(MEM_ADDR_NOT_ALIGNED);
    }
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::STH_imm::replicate() const throw(){
    return new STH_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::STH_imm::getInstructionName(){
    return "STH_imm";
}

void leon3_funclt_trap::STH_imm::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::STH_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "sth r";
    oss << this->rd_bit;
    oss << " r";
    oss << this->rs1_bit;
    oss << "+";
    oss << this->simm13;
    return oss.str();
}

leon3_funclt_trap::STH_imm::STH_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::STH_imm::~STH_imm(){

}
unsigned int leon3_funclt_trap::WRITEwim_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    result = rs1 ^ SignExtend(simm13, 13);
    raiseException = (PSR[key_S] == 0);

    if(raiseException){
        RaiseException(PRIVILEDGE_INSTR);
    }

    if(!raiseException){
        WIM = result & ((unsigned int)0xFFFFFFFF >> (32 - NUM_REG_WIN));
    }
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::WRITEwim_imm::replicate() const throw(){
    return new WRITEwim_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::WRITEwim_imm::getInstructionName(){
    return "WRITEwim_imm";
}

void leon3_funclt_trap::WRITEwim_imm::setParams( const unsigned int & bitString ) \
    throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rd = (bitString & 0x3e000000) >> 25;
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::WRITEwim_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "wr r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " wim";
    return oss.str();
}

leon3_funclt_trap::WRITEwim_imm::WRITEwim_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::WRITEwim_imm::~WRITEwim_imm(){

}
unsigned int leon3_funclt_trap::LDD_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    #ifdef ACC_MODEL
    REGS[rd_bit | 0x1].lock();
    #endif

    address = rs1 + SignExtend(simm13, 13);

    notAligned = (address & 0x00000007) != 0;
    if(notAligned){
        #ifdef ACC_MODEL
        REGS[rd_bit | 0x1].unlock();
        #endif
        flush();
    }

    readValue = dataMem.read_dword(address);

    if(notAligned){
        RaiseException(MEM_ADDR_NOT_ALIGNED);
    }

    if(rd_bit % 2 == 0){
        rd = (unsigned int)(readValue & 0x00000000FFFFFFFFLL);
        REGS[rd_bit + 1] = (unsigned int)((readValue >> 32) & 0x00000000FFFFFFFFLL);
        #ifdef ACC_MODEL
        REGS[rd_bit + 1].unlock();
        #endif
    }
    else{
        REGS[rd_bit - 1] = (unsigned int)(readValue & 0x00000000FFFFFFFFLL);
        rd = (unsigned int)((readValue >> 32) & 0x00000000FFFFFFFFLL);
        #ifdef ACC_MODEL
        REGS[rd_bit - 1].unlock();
        #endif
    }
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::LDD_imm::replicate() const throw(){
    return new LDD_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::LDD_imm::getInstructionName(){
    return "LDD_imm";
}

void leon3_funclt_trap::LDD_imm::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::LDD_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "ldd r";
    oss << this->rs1_bit;
    oss << "+";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::LDD_imm::LDD_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::LDD_imm::~LDD_imm(){

}
unsigned int leon3_funclt_trap::SLL_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;

    result = rs1_op << simm13;
    this->WB_plain(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::SLL_imm::replicate() const throw(){
    return new SLL_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::SLL_imm::getInstructionName(){
    return "SLL_imm";
}

void leon3_funclt_trap::SLL_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::SLL_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "sll r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::SLL_imm::SLL_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), WB_plain_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, \
    GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::SLL_imm::~SLL_imm(){

}
unsigned int leon3_funclt_trap::LDUHA_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    address = rs1 + rs2;
    supervisor = PSR[key_S];

    notAligned = (address & 0x00000001) != 0;
    if(notAligned || !supervisor){
        flush();
    }

    if(notAligned || !supervisor){
        flush();
    }
    else{
        readValue = dataMem.read_half(address);
    }

    if(!supervisor){
        RaiseException(PRIVILEDGE_INSTR);
    }
    if(notAligned){
        RaiseException(MEM_ADDR_NOT_ALIGNED);
    }

    rd = readValue;
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::LDUHA_reg::replicate() const throw(){
    return new LDUHA_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::LDUHA_reg::getInstructionName(){
    return "LDUHA_reg";
}

void leon3_funclt_trap::LDUHA_reg::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->asi = (bitString & 0x1fe0) >> 5;
}

std::string leon3_funclt_trap::LDUHA_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "lduha r";
    oss << this->rs1_bit;
    oss << "+r";
    oss << this->rs2_bit;
    oss << " ";
    oss << this->asi;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::LDUHA_reg::LDUHA_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & \
    WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::LDUHA_reg::~LDUHA_reg(){

}
unsigned int leon3_funclt_trap::TADDcc_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    result = rs1_op + rs2_op;
    temp_V = ((unsigned int)((rs1_op & rs2_op & (~result)) | ((~rs1_op) & (~rs2_op) & \
        result))) >> 31;
    if(!temp_V && (((rs1_op | rs2_op) & 0x00000003) != 0)){
        temp_V = 1;
    }
    this->ICC_writeTAdd(this->result, this->temp_V, this->rs1_op, this->rs2_op);
    this->WB_icc(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::TADDcc_reg::replicate() const throw(){
    return new TADDcc_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::TADDcc_reg::getInstructionName(){
    return "TADDcc_reg";
}

void leon3_funclt_trap::TADDcc_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::TADDcc_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "taddcc r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::TADDcc_reg::TADDcc_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), ICC_writeTAdd_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_icc_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, \
    LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, \
    NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::TADDcc_reg::~TADDcc_reg(){

}
unsigned int leon3_funclt_trap::TADDcc_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = SignExtend(simm13, 13);

    result = rs1_op + rs2_op;
    temp_V = ((unsigned int)((rs1_op & rs2_op & (~result)) | ((~rs1_op) & (~rs2_op) & \
        result))) >> 31;
    if(!temp_V && (((rs1_op | rs2_op) & 0x00000003) != 0)){
        temp_V = 1;
    }
    this->ICC_writeTAdd(this->result, this->temp_V, this->rs1_op, this->rs2_op);
    this->WB_icc(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::TADDcc_imm::replicate() const throw(){
    return new TADDcc_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::TADDcc_imm::getInstructionName(){
    return "TADDcc_imm";
}

void leon3_funclt_trap::TADDcc_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::TADDcc_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "taddcc r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::TADDcc_imm::TADDcc_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), ICC_writeTAdd_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_icc_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, \
    LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, \
    NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::TADDcc_imm::~TADDcc_imm(){

}
unsigned int leon3_funclt_trap::SDIV_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = SignExtend(simm13, 13);

    exception = rs2_op == 0;
    if(!exception){
        long long res64 = ((long long)((((unsigned long long)Ybp) << 32) | (unsigned long \
            long)rs1_op))/((long long)((int)rs2_op));
        temp_V = (res64 & 0xFFFFFFFF80000000LL) != 0 && (res64 & 0xFFFFFFFF80000000LL) != \
            0xFFFFFFFF80000000LL;
        if(temp_V){
            if(res64 > 0){
                result = 0x7FFFFFFF;
            }
            else{
                result = 0x80000000;
            }
        }
        else{
            result = (unsigned int)(res64 & 0x00000000FFFFFFFFLL);
        }
    }

    if(exception){
        RaiseException(DIV_ZERO);
    }
    this->WB_plain(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::SDIV_imm::replicate() const throw(){
    return new SDIV_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::SDIV_imm::getInstructionName(){
    return "SDIV_imm";
}

void leon3_funclt_trap::SDIV_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::SDIV_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "sdiv";
    oss << " r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::SDIV_imm::SDIV_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_plain_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, \
    FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::SDIV_imm::~SDIV_imm(){

}
unsigned int leon3_funclt_trap::TSUBccTV_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = SignExtend(simm13, 13);

    result = rs1_op - rs2_op;
    temp_V = ((unsigned int)((rs1_op & (~rs2_op) & (~result)) | ((~rs1_op) & rs2_op & \
        result))) >> 31;
    if(!temp_V && (((rs1_op | rs2_op) & 0x00000003) != 0)){
        temp_V = 1;
    }
    this->ICC_writeTVSub(this->result, this->temp_V, this->rs1_op, this->rs2_op);

    if(temp_V){
        RaiseException(TAG_OVERFLOW);
    }
    this->WB_icctv(this->rd, this->rd_bit, this->result, this->temp_V);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::TSUBccTV_imm::replicate() const throw(){
    return new TSUBccTV_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::TSUBccTV_imm::getInstructionName(){
    return "TSUBccTV_imm";
}

void leon3_funclt_trap::TSUBccTV_imm::setParams( const unsigned int & bitString ) \
    throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::TSUBccTV_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "tsubcctv r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::TSUBccTV_imm::TSUBccTV_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), ICC_writeTVSub_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_icctv_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, \
    FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, \
    Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::TSUBccTV_imm::~TSUBccTV_imm(){

}
unsigned int leon3_funclt_trap::FLUSH_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::FLUSH_reg::replicate() const throw(){
    return new FLUSH_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::FLUSH_reg::getInstructionName(){
    return "FLUSH_reg";
}

void leon3_funclt_trap::FLUSH_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::FLUSH_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "flush r";
    oss << this->rs1_bit;
    oss << "+r";
    oss << this->rs2_bit;
    return oss.str();
}

leon3_funclt_trap::FLUSH_reg::FLUSH_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & \
    WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::FLUSH_reg::~FLUSH_reg(){

}
unsigned int leon3_funclt_trap::ORNcc_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    result = rs1_op | ~rs2_op;
    this->ICC_writeLogic(this->result);
    this->WB_icc(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::ORNcc_reg::replicate() const throw(){
    return new ORNcc_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::ORNcc_reg::getInstructionName(){
    return "ORNcc_reg";
}

void leon3_funclt_trap::ORNcc_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::ORNcc_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "orncc r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::ORNcc_reg::ORNcc_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & \
    WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), ICC_writeLogic_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_icc_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, \
    LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, \
    NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::ORNcc_reg::~ORNcc_reg(){

}
unsigned int leon3_funclt_trap::RETT_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = SignExtend(simm13, 13);
    exceptionEnabled = PSR[key_ET];
    supervisor = PSR[key_S];

    targetAddr = rs1_op + rs2_op;
    newCwp = ((unsigned int)(PSR[key_CWP] + 1)) % NUM_REG_WIN;

    if(exceptionEnabled){
        if(supervisor){
            RaiseException(ILLEGAL_INSTR);
        }
        else{
            RaiseException(PRIVILEDGE_INSTR);
        }
    }
    else if(!supervisor || ((0x01 << (newCwp)) & WIM) != 0 || (targetAddr & 0x00000003) \
        != 0){
        THROW_EXCEPTION("Invalid processor mode during execution of the RETT instruction \
            - supervisor: " << supervisor << " newCwp: " << std::hex << std::showbase << newCwp \
            << " targetAddr: " << std::hex << std::showbase << targetAddr);
    }
    for(int i = 8; i < 32; i++){
        REGS[i].updateAlias(WINREGS[(newCwp*16 + i - 8) % (16*NUM_REG_WIN)]);
    }

    PSRbp = (PSR & 0xFFFFFF40) | (newCwp | 0x20 | (PSR[key_PS] << 7));
    PSR.immediateWrite(PSRbp);
    PC = NPC;
    NPC = targetAddr;
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::RETT_imm::replicate() const throw(){
    return new RETT_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::RETT_imm::getInstructionName(){
    return "RETT_imm";
}

void leon3_funclt_trap::RETT_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::RETT_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "rett r";
    oss << this->rs1_bit;
    oss << "+";
    oss << this->simm13;
    return oss.str();
}

leon3_funclt_trap::RETT_imm::RETT_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::RETT_imm::~RETT_imm(){

}
unsigned int leon3_funclt_trap::SDIVcc_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    exception = rs2_op == 0;
    if(!exception){
        long long res64 = ((long long)((((unsigned long long)Ybp) << 32) | (unsigned long \
            long)rs1_op))/((long long)((int)rs2_op));
        temp_V = (res64 & 0xFFFFFFFF80000000LL) != 0 && (res64 & 0xFFFFFFFF80000000LL) != \
            0xFFFFFFFF80000000LL;
        if(temp_V){
            if(res64 > 0){
                result = 0x7FFFFFFF;
            }
            else{
                result = 0x80000000;
            }
        }
        else{
            result = (unsigned int)(res64 & 0x00000000FFFFFFFFLL);
        }
    }
    this->ICC_writeDiv(this->result, this->temp_V, this->rs1_op, this->rs2_op);

    if(exception){
        RaiseException(DIV_ZERO);
    }
    this->WB_icc(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::SDIVcc_reg::replicate() const throw(){
    return new SDIVcc_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::SDIVcc_reg::getInstructionName(){
    return "SDIVcc_reg";
}

void leon3_funclt_trap::SDIVcc_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::SDIVcc_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "sdivcc";
    oss << " r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::SDIVcc_reg::SDIVcc_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), ICC_writeDiv_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, \
    FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), WB_icc_op(PSR, WIM, TBR, Y, PC, \
    NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::SDIVcc_reg::~SDIVcc_reg(){

}
unsigned int leon3_funclt_trap::ADD_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    result = rs1_op + rs2_op;
    this->WB_plain(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::ADD_reg::replicate() const throw(){
    return new ADD_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::ADD_reg::getInstructionName(){
    return "ADD_reg";
}

void leon3_funclt_trap::ADD_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::ADD_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "add r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::ADD_reg::ADD_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), WB_plain_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, \
    GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::ADD_reg::~ADD_reg(){

}
unsigned int leon3_funclt_trap::TRAP_imm::behavior(){
    this->totalInstrCycles = 0;

    // All the other non-special situations
    raiseException = (cond == 0x8) ||
    ((cond == 0x9) && PSRbp[key_ICC_z] == 0) ||
    ((cond == 0x1) && PSRbp[key_ICC_z] != 0) ||
    ((cond == 0xa) && (PSRbp[key_ICC_z] == 0) && (PSRbp[key_ICC_n] == PSRbp[key_ICC_v])) ||
    ((cond == 0x2) && ((PSRbp[key_ICC_z] != 0) || (PSRbp[key_ICC_n] != PSRbp[key_ICC_v]))) ||
    ((cond == 0xb) && PSRbp[key_ICC_n] == PSRbp[key_ICC_v]) ||
    ((cond == 0x3) && PSRbp[key_ICC_n] != PSRbp[key_ICC_v]) ||
    ((cond == 0xc) && (PSRbp[key_ICC_c] + PSRbp[key_ICC_z]) == 0) ||
    ((cond == 0x4) && (PSRbp[key_ICC_c] + PSRbp[key_ICC_z]) > 0) ||
    ((cond == 0xd) && PSRbp[key_ICC_c] == 0) ||
    ((cond == 0x5) && PSRbp[key_ICC_c] != 0) ||
    ((cond == 0xe) && PSRbp[key_ICC_n] == 0) ||
    ((cond == 0x6) && PSRbp[key_ICC_n] != 0) ||
    ((cond == 0xf) && PSRbp[key_ICC_v] == 0) ||
    ((cond == 0x7) && PSRbp[key_ICC_v] != 0);

    if(raiseException){
        RaiseException(TRAP_INSTRUCTION, (rs1 + SignExtend(imm7, 7)) & 0x0000007F);
    }
    #ifndef ACC_MODEL
    else{
        PC = NPC;
        NPC += 4;
    }
    #endif
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::TRAP_imm::replicate() const throw(){
    return new TRAP_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::TRAP_imm::getInstructionName(){
    return "TRAP_imm";
}

void leon3_funclt_trap::TRAP_imm::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->reserved1 = (bitString & 0x20000000) >> 29;
    this->cond = (bitString & 0x1e000000) >> 25;
    this->reserved2 = (bitString & 0x1f80) >> 7;
    this->imm7 = (bitString & 0x7f);
}

std::string leon3_funclt_trap::TRAP_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "t";
    switch(this->cond){
        case 0:{
            oss << "n";
        break;}
        case 1:{
            oss << "e";
        break;}
        case 2:{
            oss << "le";
        break;}
        case 3:{
            oss << "l";
        break;}
        case 4:{
            oss << "leu";
        break;}
        case 6:{
            oss << "neg";
        break;}
        case 7:{
            oss << "vs";
        break;}
        case 8:{
            oss << "a";
        break;}
        case 9:{
            oss << "ne";
        break;}
        case 10:{
            oss << "cs";
        break;}
        case 11:{
            oss << "ge";
        break;}
        case 12:{
            oss << "gu";
        break;}
        case 13:{
            oss << "cc";
        break;}
        case 14:{
            oss << "pos";
        break;}
        case 15:{
            oss << "vc";
        break;}
        default:
        break;
    }
    oss << " r";
    oss << this->rs1_bit;
    oss << "+";
    oss << this->imm7;
    return oss.str();
}

leon3_funclt_trap::TRAP_imm::TRAP_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck){

}

leon3_funclt_trap::TRAP_imm::~TRAP_imm(){

}
unsigned int leon3_funclt_trap::WRITEtbr_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    result = rs1 ^ SignExtend(simm13, 13);
    raiseException = (PSR[key_S] == 0);

    if(raiseException){
        RaiseException(PRIVILEDGE_INSTR);
    }

    if(!raiseException){
        TBR |= (result & 0xFFFFF000);
    }
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::WRITEtbr_imm::replicate() const throw(){
    return new WRITEtbr_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::WRITEtbr_imm::getInstructionName(){
    return "WRITEtbr_imm";
}

void leon3_funclt_trap::WRITEtbr_imm::setParams( const unsigned int & bitString ) \
    throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rd = (bitString & 0x3e000000) >> 25;
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::WRITEtbr_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "wr r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " tbr";
    return oss.str();
}

leon3_funclt_trap::WRITEtbr_imm::WRITEtbr_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::WRITEtbr_imm::~WRITEtbr_imm(){

}
unsigned int leon3_funclt_trap::LDUB_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    address = rs1 + rs2;

    readValue = dataMem.read_byte(address);

    rd = readValue;
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::LDUB_reg::replicate() const throw(){
    return new LDUB_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::LDUB_reg::getInstructionName(){
    return "LDUB_reg";
}

void leon3_funclt_trap::LDUB_reg::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->asi = (bitString & 0x1fe0) >> 5;
}

std::string leon3_funclt_trap::LDUB_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "ldub r";
    oss << this->rs1_bit;
    oss << "+r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::LDUB_reg::LDUB_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::LDUB_reg::~LDUB_reg(){

}
unsigned int leon3_funclt_trap::RESTORE_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    result = rs1_op + rs2_op;
    okNewWin = IncrementRegWindow();

    if(!okNewWin){
        RaiseException(WINDOW_UNDERFLOW);
    }

    if(okNewWin){
        rd = result;
    }
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::RESTORE_reg::replicate() const throw(){
    return new RESTORE_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::RESTORE_reg::getInstructionName(){
    return "RESTORE_reg";
}

void leon3_funclt_trap::RESTORE_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::RESTORE_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "restore";
    oss << " r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::RESTORE_reg::RESTORE_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::RESTORE_reg::~RESTORE_reg(){

}
unsigned int leon3_funclt_trap::ADDXcc_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    result = rs1_op + rs2_op + PSRbp[key_ICC_c];
    this->ICC_writeAdd(this->result, this->rs1_op, this->rs2_op);
    this->WB_icc(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::ADDXcc_reg::replicate() const throw(){
    return new ADDXcc_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::ADDXcc_reg::getInstructionName(){
    return "ADDXcc_reg";
}

void leon3_funclt_trap::ADDXcc_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::ADDXcc_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "addxcc r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::ADDXcc_reg::ADDXcc_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), ICC_writeAdd_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_icc_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, \
    LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, \
    NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::ADDXcc_reg::~ADDXcc_reg(){

}
unsigned int leon3_funclt_trap::STB_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    address = rs1 + rs2;
    toWrite = (unsigned char)(rd & 0x000000FF);

    dataMem.write_byte(address, toWrite);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::STB_reg::replicate() const throw(){
    return new STB_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::STB_reg::getInstructionName(){
    return "STB_reg";
}

void leon3_funclt_trap::STB_reg::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->asi = (bitString & 0x1fe0) >> 5;
}

std::string leon3_funclt_trap::STB_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "stb r";
    oss << this->rd_bit;
    oss << " r";
    oss << this->rs1_bit;
    oss << "+r";
    oss << this->rs2_bit;
    return oss.str();
}

leon3_funclt_trap::STB_reg::STB_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::STB_reg::~STB_reg(){

}
unsigned int leon3_funclt_trap::AND_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;

    result = rs1_op & SignExtend(simm13, 13);
    this->WB_plain(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::AND_imm::replicate() const throw(){
    return new AND_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::AND_imm::getInstructionName(){
    return "AND_imm";
}

void leon3_funclt_trap::AND_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::AND_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "and r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::AND_imm::AND_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), WB_plain_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, \
    GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::AND_imm::~AND_imm(){

}
unsigned int leon3_funclt_trap::SMUL_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = SignExtend(simm13, 13);

    long long resultTemp = (long long)(((long long)((int)rs1_op))*((long long)((int)rs2_op)));
    Ybp = ((unsigned long long)resultTemp) >> 32;
    result = resultTemp & 0x00000000FFFFFFFF;
    this->WB_y(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::SMUL_imm::replicate() const throw(){
    return new SMUL_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::SMUL_imm::getInstructionName(){
    return "SMUL_imm";
}

void leon3_funclt_trap::SMUL_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::SMUL_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "smul r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::SMUL_imm::SMUL_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), WB_y_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, \
    GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::SMUL_imm::~SMUL_imm(){

}
unsigned int leon3_funclt_trap::ADD_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = SignExtend(simm13, 13);

    result = rs1_op + rs2_op;
    this->WB_plain(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::ADD_imm::replicate() const throw(){
    return new ADD_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::ADD_imm::getInstructionName(){
    return "ADD_imm";
}

void leon3_funclt_trap::ADD_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::ADD_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "add r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::ADD_imm::ADD_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), WB_plain_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, \
    GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::ADD_imm::~ADD_imm(){

}
unsigned int leon3_funclt_trap::UMUL_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = SignExtend(simm13, 13);

    unsigned long long resultTemp = (unsigned long long)(((unsigned long long)((unsigned \
        int)rs1_op))*((unsigned long long)((unsigned int)rs2_op)));
    Ybp = resultTemp >> 32;
    result = resultTemp & 0x00000000FFFFFFFF;
    this->WB_y(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::UMUL_imm::replicate() const throw(){
    return new UMUL_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::UMUL_imm::getInstructionName(){
    return "UMUL_imm";
}

void leon3_funclt_trap::UMUL_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::UMUL_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "umul r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::UMUL_imm::UMUL_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), WB_y_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, \
    GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::UMUL_imm::~UMUL_imm(){

}
unsigned int leon3_funclt_trap::READwim::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    wim_temp = WIM;
    supervisor = PSR[key_S];

    if(!supervisor){
        RaiseException(PRIVILEDGE_INSTR);
    }

    rd = wim_temp;
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::READwim::replicate() const throw(){
    return new READwim(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::READwim::getInstructionName(){
    return "READwim";
}

void leon3_funclt_trap::READwim::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->asr = (bitString & 0x7c000) >> 14;
}

std::string leon3_funclt_trap::READwim::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "rd ";
    oss << "wim r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::READwim::READwim( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::READwim::~READwim(){

}
unsigned int leon3_funclt_trap::LDSTUB_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    address = rs1 + SignExtend(simm13, 13);

    readValue = dataMem.read_byte(address);
    dataMem.write_byte(address, 0xff);

    rd = readValue;
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::LDSTUB_imm::replicate() const throw(){
    return new LDSTUB_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::LDSTUB_imm::getInstructionName(){
    return "LDSTUB_imm";
}

void leon3_funclt_trap::LDSTUB_imm::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::LDSTUB_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "ldastub r";
    oss << this->rs1_bit;
    oss << "+";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::LDSTUB_imm::LDSTUB_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::LDSTUB_imm::~LDSTUB_imm(){

}
unsigned int leon3_funclt_trap::SMAC_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = SignExtend(simm13, 13);

    int resultTemp = ((int)SignExtend(rs1_op & 0x0000ffff, 16))*((int)SignExtend(rs2_op \
        & 0x0000ffff, 16));
    long long resultAcc = ((((long long)(Ybp & 0x000000ff)) << 32) | (int)ASR18bp) + \
        resultTemp;
    Ybp = (resultAcc & 0x000000ff00000000LL) >> 32;
    ASR18bp = resultAcc & 0x00000000FFFFFFFFLL;
    result = resultAcc & 0x00000000FFFFFFFFLL;
    this->WB_yasr(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::SMAC_imm::replicate() const throw(){
    return new SMAC_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::SMAC_imm::getInstructionName(){
    return "SMAC_imm";
}

void leon3_funclt_trap::SMAC_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::SMAC_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "smac r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::SMAC_imm::SMAC_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), WB_yasr_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, \
    GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::SMAC_imm::~SMAC_imm(){

}
unsigned int leon3_funclt_trap::LDSB_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    address = rs1 + rs2;

    readValue = SignExtend(dataMem.read_byte(address), 8);

    rd = readValue;
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::LDSB_reg::replicate() const throw(){
    return new LDSB_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::LDSB_reg::getInstructionName(){
    return "LDSB_reg";
}

void leon3_funclt_trap::LDSB_reg::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->asi = (bitString & 0x1fe0) >> 5;
}

std::string leon3_funclt_trap::LDSB_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "ldsb r";
    oss << this->rs1_bit;
    oss << "+r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::LDSB_reg::LDSB_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::LDSB_reg::~LDSB_reg(){

}
unsigned int leon3_funclt_trap::ANDN_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    result = rs1_op & ~rs2_op;
    this->WB_plain(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::ANDN_reg::replicate() const throw(){
    return new ANDN_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::ANDN_reg::getInstructionName(){
    return "ANDN_reg";
}

void leon3_funclt_trap::ANDN_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::ANDN_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "andn r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::ANDN_reg::ANDN_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), WB_plain_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, \
    GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::ANDN_reg::~ANDN_reg(){

}
unsigned int leon3_funclt_trap::TSUBccTV_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    result = rs1_op - rs2_op;
    temp_V = ((unsigned int)((rs1_op & (~rs2_op) & (~result)) | ((~rs1_op) & rs2_op & \
        result))) >> 31;
    if(!temp_V && (((rs1_op | rs2_op) & 0x00000003) != 0)){
        temp_V = 1;
    }
    this->ICC_writeTVSub(this->result, this->temp_V, this->rs1_op, this->rs2_op);

    if(temp_V){
        RaiseException(TAG_OVERFLOW);
    }
    this->WB_icctv(this->rd, this->rd_bit, this->result, this->temp_V);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::TSUBccTV_reg::replicate() const throw(){
    return new TSUBccTV_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::TSUBccTV_reg::getInstructionName(){
    return "TSUBccTV_reg";
}

void leon3_funclt_trap::TSUBccTV_reg::setParams( const unsigned int & bitString ) \
    throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::TSUBccTV_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "tsubcctv r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::TSUBccTV_reg::TSUBccTV_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), ICC_writeTVSub_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_icctv_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, \
    FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, \
    Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::TSUBccTV_reg::~TSUBccTV_reg(){

}
unsigned int leon3_funclt_trap::SETHI::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    result = 0xfffffc00 & (imm22 << 10);
    this->WB_plain(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::SETHI::replicate() const throw(){
    return new SETHI(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::SETHI::getInstructionName(){
    return "SETHI";
}

void leon3_funclt_trap::SETHI::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->imm22 = (bitString & 0x3fffff);
}

std::string leon3_funclt_trap::SETHI::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "sethi ";
    oss << this->imm22;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::SETHI::SETHI( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, Reg32_2 \
    & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 & \
    Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), WB_plain_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, \
    GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::SETHI::~SETHI(){

}
unsigned int leon3_funclt_trap::SRA_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;

    result = ((int)rs1_op) >> simm13;
    this->WB_plain(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::SRA_imm::replicate() const throw(){
    return new SRA_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::SRA_imm::getInstructionName(){
    return "SRA_imm";
}

void leon3_funclt_trap::SRA_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::SRA_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "sra r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::SRA_imm::SRA_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), WB_plain_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, \
    GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::SRA_imm::~SRA_imm(){

}
unsigned int leon3_funclt_trap::LDSH_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    address = rs1 + rs2;

    notAligned = (address & 0x00000001) != 0;
    if(notAligned){
        flush();
    }

    if(!notAligned){
        readValue = SignExtend(dataMem.read_half(address), 16);
    }
    else{
        flush();
    }

    if(notAligned){
        RaiseException(MEM_ADDR_NOT_ALIGNED);
    }

    rd = readValue;
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::LDSH_reg::replicate() const throw(){
    return new LDSH_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::LDSH_reg::getInstructionName(){
    return "LDSH_reg";
}

void leon3_funclt_trap::LDSH_reg::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->asi = (bitString & 0x1fe0) >> 5;
}

std::string leon3_funclt_trap::LDSH_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "ldsh r";
    oss << this->rs1_bit;
    oss << "+r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::LDSH_reg::LDSH_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::LDSH_reg::~LDSH_reg(){

}
unsigned int leon3_funclt_trap::UDIVcc_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    exception = rs2_op == 0;
    if(!exception){
        unsigned long long res64 = ((unsigned long long)((((unsigned long long)Ybp) << 32) \
            | (unsigned long long)rs1_op))/(unsigned long long)rs2_op;
        temp_V = (res64 & 0xFFFFFFFF00000000LL) != 0;
        if(temp_V){
            result = 0xFFFFFFFF;
        }
        else{
            result = (unsigned int)(res64 & 0x00000000FFFFFFFFLL);
        }
    }
    this->ICC_writeDiv(this->result, this->temp_V, this->rs1_op, this->rs2_op);

    if(exception){
        RaiseException(DIV_ZERO);
    }
    this->WB_icc(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::UDIVcc_reg::replicate() const throw(){
    return new UDIVcc_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::UDIVcc_reg::getInstructionName(){
    return "UDIVcc_reg";
}

void leon3_funclt_trap::UDIVcc_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::UDIVcc_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "udivcc";
    oss << " r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::UDIVcc_reg::UDIVcc_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), ICC_writeDiv_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, \
    FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), WB_icc_op(PSR, WIM, TBR, Y, PC, \
    NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::UDIVcc_reg::~UDIVcc_reg(){

}
unsigned int leon3_funclt_trap::ORN_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;

    result = rs1_op | ~(SignExtend(simm13, 13));
    this->WB_plain(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::ORN_imm::replicate() const throw(){
    return new ORN_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::ORN_imm::getInstructionName(){
    return "ORN_imm";
}

void leon3_funclt_trap::ORN_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::ORN_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "orn r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::ORN_imm::ORN_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), WB_plain_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, \
    GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::ORN_imm::~ORN_imm(){

}
unsigned int leon3_funclt_trap::STD_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    address = rs1 + rs2;
    if(rd_bit % 2 == 0){
        toWrite = rd | (((unsigned long long)REGS[rd_bit + 1]) << 32);
    }
    else{
        toWrite = REGS[rd_bit - 1] | (((unsigned long long)rd) << 32);
    }

    notAligned = (address & 0x00000007) != 0;
    if(notAligned){
        flush();
    }

    if(!notAligned){
        dataMem.write_dword(address, toWrite);
    }
    else{
        flush();
    }

    if(notAligned){
        RaiseException(MEM_ADDR_NOT_ALIGNED);
    }
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::STD_reg::replicate() const throw(){
    return new STD_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::STD_reg::getInstructionName(){
    return "STD_reg";
}

void leon3_funclt_trap::STD_reg::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->asi = (bitString & 0x1fe0) >> 5;
}

std::string leon3_funclt_trap::STD_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "std r";
    oss << this->rd_bit;
    oss << " r";
    oss << this->rs1_bit;
    oss << "+r";
    oss << this->rs2_bit;
    return oss.str();
}

leon3_funclt_trap::STD_reg::STD_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::STD_reg::~STD_reg(){

}
unsigned int leon3_funclt_trap::ANDNcc_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;

    result = rs1_op & ~(SignExtend(simm13, 13));
    this->ICC_writeLogic(this->result);
    this->WB_icc(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::ANDNcc_imm::replicate() const throw(){
    return new ANDNcc_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::ANDNcc_imm::getInstructionName(){
    return "ANDNcc_imm";
}

void leon3_funclt_trap::ANDNcc_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::ANDNcc_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "andncc r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::ANDNcc_imm::ANDNcc_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), ICC_writeLogic_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_icc_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, \
    LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, \
    NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::ANDNcc_imm::~ANDNcc_imm(){

}
unsigned int leon3_funclt_trap::TADDccTV_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = SignExtend(simm13, 13);

    result = rs1_op + rs2_op;
    temp_V = ((unsigned int)((rs1_op & rs2_op & (~result)) | ((~rs1_op) & (~rs2_op) & \
        result))) >> 31;
    if(!temp_V && (((rs1_op | rs2_op) & 0x00000003) != 0)){
        temp_V = 1;
    }
    this->ICC_writeTVAdd(this->result, this->temp_V, this->rs1_op, this->rs2_op);

    if(temp_V){
        RaiseException(TAG_OVERFLOW);
    }
    this->WB_icctv(this->rd, this->rd_bit, this->result, this->temp_V);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::TADDccTV_imm::replicate() const throw(){
    return new TADDccTV_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::TADDccTV_imm::getInstructionName(){
    return "TADDccTV_imm";
}

void leon3_funclt_trap::TADDccTV_imm::setParams( const unsigned int & bitString ) \
    throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::TADDccTV_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "taddcctv r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::TADDccTV_imm::TADDccTV_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), ICC_writeTVAdd_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_icctv_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, \
    FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, \
    Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::TADDccTV_imm::~TADDccTV_imm(){

}
unsigned int leon3_funclt_trap::WRITEtbr_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    result = rs1 ^ rs2;
    raiseException = (PSR[key_S] == 0);

    if(raiseException){
        RaiseException(PRIVILEDGE_INSTR);
    }

    if(!raiseException){
        TBR |= (result & 0xFFFFF000);
    }
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::WRITEtbr_reg::replicate() const throw(){
    return new WRITEtbr_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::WRITEtbr_reg::getInstructionName(){
    return "WRITEtbr_reg";
}

void leon3_funclt_trap::WRITEtbr_reg::setParams( const unsigned int & bitString ) \
    throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
    this->rd = (bitString & 0x3e000000) >> 25;
}

std::string leon3_funclt_trap::WRITEtbr_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "wr r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " tbr";
    return oss.str();
}

leon3_funclt_trap::WRITEtbr_reg::WRITEtbr_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::WRITEtbr_reg::~WRITEtbr_reg(){

}
unsigned int leon3_funclt_trap::SUBX_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    result = rs1_op - rs2_op - PSRbp[key_ICC_c];
    this->WB_plain(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::SUBX_reg::replicate() const throw(){
    return new SUBX_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::SUBX_reg::getInstructionName(){
    return "SUBX_reg";
}

void leon3_funclt_trap::SUBX_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::SUBX_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "subx r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::SUBX_reg::SUBX_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), WB_plain_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, \
    GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::SUBX_reg::~SUBX_reg(){

}
unsigned int leon3_funclt_trap::XNOR_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;

    result = rs1_op ^ ~(SignExtend(simm13, 13));
    this->WB_plain(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::XNOR_imm::replicate() const throw(){
    return new XNOR_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::XNOR_imm::getInstructionName(){
    return "XNOR_imm";
}

void leon3_funclt_trap::XNOR_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::XNOR_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "xnor r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::XNOR_imm::XNOR_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), WB_plain_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, \
    GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::XNOR_imm::~XNOR_imm(){

}
unsigned int leon3_funclt_trap::UDIV_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = SignExtend(simm13, 13);

    exception = rs2_op == 0;
    if(!exception){
        unsigned long long res64 = ((unsigned long long)((((unsigned long long)Ybp) << 32) \
            | (unsigned long long)rs1_op))/(unsigned long long)rs2_op;
        temp_V = (res64 & 0xFFFFFFFF00000000LL) != 0;
        if(temp_V){
            result = 0xFFFFFFFF;
        }
        else{
            result = (unsigned int)(res64 & 0x00000000FFFFFFFFLL);
        }
    }

    if(exception){
        RaiseException(DIV_ZERO);
    }
    this->WB_plain(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::UDIV_imm::replicate() const throw(){
    return new UDIV_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::UDIV_imm::getInstructionName(){
    return "UDIV_imm";
}

void leon3_funclt_trap::UDIV_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::UDIV_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "udiv";
    oss << " r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::UDIV_imm::UDIV_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_plain_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, \
    FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::UDIV_imm::~UDIV_imm(){

}
unsigned int leon3_funclt_trap::LDSH_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    address = rs1 + SignExtend(simm13, 13);

    notAligned = (address & 0x00000001) != 0;
    if(notAligned){
        flush();
    }

    if(!notAligned){
        readValue = SignExtend(dataMem.read_half(address), 16);
    }
    else{
        flush();
    }

    if(notAligned){
        RaiseException(MEM_ADDR_NOT_ALIGNED);
    }

    rd = readValue;
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::LDSH_imm::replicate() const throw(){
    return new LDSH_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::LDSH_imm::getInstructionName(){
    return "LDSH_imm";
}

void leon3_funclt_trap::LDSH_imm::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::LDSH_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "ldsh r";
    oss << this->rs1_bit;
    oss << "+";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::LDSH_imm::LDSH_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::LDSH_imm::~LDSH_imm(){

}
unsigned int leon3_funclt_trap::UNIMP::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    RaiseException(ILLEGAL_INSTR);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::UNIMP::replicate() const throw(){
    return new UNIMP(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::UNIMP::getInstructionName(){
    return "UNIMP";
}

void leon3_funclt_trap::UNIMP::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->imm22 = (bitString & 0x3fffff);
}

std::string leon3_funclt_trap::UNIMP::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "unimp ";
    oss << this->imm22;
    return oss.str();
}

leon3_funclt_trap::UNIMP::UNIMP( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, Reg32_2 \
    & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 & \
    Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::UNIMP::~UNIMP(){

}
unsigned int leon3_funclt_trap::LDSTUBA_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    address = rs1 + rs2;
    supervisor = PSR[key_S];

    if(!supervisor){
        flush();
    }

    if(supervisor){
        readValue = dataMem.read_byte(address);
        dataMem.write_byte(address, 0xff);
    }
    else{
        flush();
    }

    if(!supervisor){
        RaiseException(PRIVILEDGE_INSTR);
    }

    rd = readValue;
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::LDSTUBA_reg::replicate() const throw(){
    return new LDSTUBA_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::LDSTUBA_reg::getInstructionName(){
    return "LDSTUBA_reg";
}

void leon3_funclt_trap::LDSTUBA_reg::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->asi = (bitString & 0x1fe0) >> 5;
}

std::string leon3_funclt_trap::LDSTUBA_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "ldastub r";
    oss << this->rs1_bit;
    oss << "+r";
    oss << this->rs2_bit;
    oss << " ";
    oss << this->asi;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::LDSTUBA_reg::LDSTUBA_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::LDSTUBA_reg::~LDSTUBA_reg(){

}
unsigned int leon3_funclt_trap::UMULcc_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = SignExtend(simm13, 13);

    unsigned long long resultTemp = (unsigned long long)(((unsigned long long)((unsigned \
        int)rs1_op))*((unsigned long long)((unsigned int)rs2_op)));
    Ybp = resultTemp >> 32;
    result = resultTemp & 0x00000000FFFFFFFF;
    this->ICC_writeLogic(this->result);
    this->WB_yicc(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::UMULcc_imm::replicate() const throw(){
    return new UMULcc_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::UMULcc_imm::getInstructionName(){
    return "UMULcc_imm";
}

void leon3_funclt_trap::UMULcc_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::UMULcc_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "umulcc r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::UMULcc_imm::UMULcc_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), ICC_writeLogic_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_yicc_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, \
    FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, \
    Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::UMULcc_imm::~UMULcc_imm(){

}
unsigned int leon3_funclt_trap::ORcc_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    result = rs1_op | rs2_op;
    this->ICC_writeLogic(this->result);
    this->WB_icc(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::ORcc_reg::replicate() const throw(){
    return new ORcc_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::ORcc_reg::getInstructionName(){
    return "ORcc_reg";
}

void leon3_funclt_trap::ORcc_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::ORcc_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "orcc r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::ORcc_reg::ORcc_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), ICC_writeLogic_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_icc_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, \
    LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, \
    NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::ORcc_reg::~ORcc_reg(){

}
unsigned int leon3_funclt_trap::MULScc_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = SignExtend(simm13, 13);

    unsigned int yNew = (((unsigned int)Ybp) >> 1) | (rs1_op << 31);
    rs1_op = ((PSRbp[key_ICC_n] ^ PSRbp[key_ICC_v]) << 31) | (((unsigned int)rs1_op) >> 1);
    result = rs1_op;
    if((Ybp & 0x00000001) != 0){
        result += rs2_op;
    }
    else{
        rs2_op = 0;
    }
    Ybp = yNew;
    this->ICC_writeAdd(this->result, this->rs1_op, this->rs2_op);
    this->WB_yicc(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::MULScc_imm::replicate() const throw(){
    return new MULScc_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::MULScc_imm::getInstructionName(){
    return "MULScc_imm";
}

void leon3_funclt_trap::MULScc_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::MULScc_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "mulscc r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::MULScc_imm::MULScc_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), ICC_writeAdd_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_yicc_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, \
    FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, \
    Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::MULScc_imm::~MULScc_imm(){

}
unsigned int leon3_funclt_trap::XORcc_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    result = rs1_op ^ rs2_op;
    this->ICC_writeLogic(this->result);
    this->WB_icc(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::XORcc_reg::replicate() const throw(){
    return new XORcc_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::XORcc_reg::getInstructionName(){
    return "XORcc_reg";
}

void leon3_funclt_trap::XORcc_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::XORcc_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "xorcc r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::XORcc_reg::XORcc_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & \
    WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), ICC_writeLogic_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_icc_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, \
    LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, \
    NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::XORcc_reg::~XORcc_reg(){

}
unsigned int leon3_funclt_trap::SUB_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    result = rs1_op - rs2_op;
    this->WB_plain(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::SUB_reg::replicate() const throw(){
    return new SUB_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::SUB_reg::getInstructionName(){
    return "SUB_reg";
}

void leon3_funclt_trap::SUB_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::SUB_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "sub r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::SUB_reg::SUB_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), WB_plain_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, \
    GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::SUB_reg::~SUB_reg(){

}
unsigned int leon3_funclt_trap::WRITEwim_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    result = rs1 ^ rs2;
    raiseException = (PSR[key_S] == 0);

    if(raiseException){
        RaiseException(PRIVILEDGE_INSTR);
    }

    if(!raiseException){
        WIM = result & ((unsigned int)0xFFFFFFFF >> (32 - NUM_REG_WIN));
    }
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::WRITEwim_reg::replicate() const throw(){
    return new WRITEwim_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::WRITEwim_reg::getInstructionName(){
    return "WRITEwim_reg";
}

void leon3_funclt_trap::WRITEwim_reg::setParams( const unsigned int & bitString ) \
    throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
    this->rd = (bitString & 0x3e000000) >> 25;
}

std::string leon3_funclt_trap::WRITEwim_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "wr r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " wim";
    return oss.str();
}

leon3_funclt_trap::WRITEwim_reg::WRITEwim_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::WRITEwim_reg::~WRITEwim_reg(){

}
unsigned int leon3_funclt_trap::UMAC_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = SignExtend(simm13, 13);

    unsigned int resultTemp = ((unsigned int)rs1_op & 0x0000ffff)*((unsigned int)rs2_op \
        & 0x0000ffff);
    unsigned long long resultAcc = ((((unsigned long long)(Ybp & 0x000000ff)) << 32) \
        | (unsigned int)ASR18bp) + resultTemp;
    Ybp = (resultAcc & 0x000000ff00000000LL) >> 32;
    ASR18bp = resultAcc & 0x00000000FFFFFFFFLL;
    result = resultAcc & 0x00000000FFFFFFFFLL;
    this->WB_yasr(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::UMAC_imm::replicate() const throw(){
    return new UMAC_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::UMAC_imm::getInstructionName(){
    return "UMAC_imm";
}

void leon3_funclt_trap::UMAC_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::UMAC_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "umac r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::UMAC_imm::UMAC_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), WB_yasr_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, \
    GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::UMAC_imm::~UMAC_imm(){

}
unsigned int leon3_funclt_trap::TSUBcc_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    result = rs1_op - rs2_op;
    temp_V = ((unsigned int)((rs1_op & (~rs2_op) & (~result)) | ((~rs1_op) & rs2_op & \
        result))) >> 31;
    if(!temp_V && (((rs1_op | rs2_op) & 0x00000003) != 0)){
        temp_V = 1;
    }
    this->ICC_writeTSub(this->result, this->temp_V, this->rs1_op, this->rs2_op);
    this->WB_icc(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::TSUBcc_reg::replicate() const throw(){
    return new TSUBcc_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::TSUBcc_reg::getInstructionName(){
    return "TSUBcc_reg";
}

void leon3_funclt_trap::TSUBcc_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::TSUBcc_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "tsubcc r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::TSUBcc_reg::TSUBcc_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), ICC_writeTSub_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_icc_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, \
    LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, \
    NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::TSUBcc_reg::~TSUBcc_reg(){

}
unsigned int leon3_funclt_trap::BRANCH::behavior(){
    this->totalInstrCycles = 0;

    switch(cond){
        case 0x8:{
            // Branch Always
            unsigned int targetPc = PC + 4*(SignExtend(disp22, 22));
            #ifdef ACC_MODEL
            PC = targetPc;
            NPC = targetPc + 4;
            if(a == 1){
                flush();
            }
            #else
            if(a == 1){
                PC = targetPc - 4;
                NPC = targetPc;
            }
            else{
                PC = NPC;
                NPC = targetPc - 4;
            }
            #endif
        break;}
        case 0:{
            // Branch Never
            #ifdef ACC_MODEL
            if(a == 1){
                flush();
            }
            #else
            if(a == 1){
                PC = NPC + 4;
                NPC += 8;
            }
            else{
                PC = NPC;
                NPC += 4;
            }
            #endif
        break;}
        default:{
            bool icc_z = PSRbp[key_ICC_z];
            bool icc_n = PSRbp[key_ICC_n];
            bool icc_v = PSRbp[key_ICC_v];
            bool icc_c = PSRbp[key_ICC_c];
            // All the other non-special situations
            bool exec = ((cond == 0x9) && icc_z == 0) ||
            ((cond == 0x1) && icc_z != 0) ||
            ((cond == 0xa) && (icc_z == 0) && (icc_n == icc_v)) ||
            ((cond == 0x2) && ((icc_z != 0) || (icc_n != icc_v))) ||
            ((cond == 0xb) && icc_n == icc_v) ||
            ((cond == 0x3) && icc_n != icc_v) ||
            ((cond == 0xc) && (icc_c + icc_z) == 0) ||
            ((cond == 0x4) && (icc_c + icc_z) > 0) ||
            ((cond == 0xd) && icc_c == 0) ||
            ((cond == 0x5) && icc_c != 0) ||
            ((cond == 0xe) && icc_n == 0) ||
            ((cond == 0x6) && icc_n != 0) ||
            ((cond == 0xf) && icc_v == 0) ||
            ((cond == 0x7) && icc_v != 0);
            if(exec){
                unsigned int targetPc = PC + 4*(SignExtend(disp22, 22));
                #ifdef ACC_MODEL
                PC = targetPc;
                NPC = targetPc + 4;
                #else
                PC = NPC;
                NPC = targetPc - 4;
                #endif
            }
            else{
                if(a == 1){
                    #ifdef ACC_MODEL
                    flush();
                    #else
                    PC = NPC + 4;
                    NPC += 8;
                    #endif
                }
                #ifndef ACC_MODEL
                else{
                    PC = NPC;
                    NPC += 4;
                }
                #endif
            }
        break;}
    }
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::BRANCH::replicate() const throw(){
    return new BRANCH(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::BRANCH::getInstructionName(){
    return "BRANCH";
}

void leon3_funclt_trap::BRANCH::setParams( const unsigned int & bitString ) throw(){
    this->a = (bitString & 0x20000000) >> 29;
    this->cond = (bitString & 0x1e000000) >> 25;
    this->disp22 = (bitString & 0x3fffff);
}

std::string leon3_funclt_trap::BRANCH::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "b";
    switch(this->cond){
        case 0:{
            oss << "n";
        break;}
        case 1:{
            oss << "e";
        break;}
        case 2:{
            oss << "le";
        break;}
        case 3:{
            oss << "l";
        break;}
        case 4:{
            oss << "leu";
        break;}
        case 6:{
            oss << "neg";
        break;}
        case 7:{
            oss << "vs";
        break;}
        case 8:{
            oss << "a";
        break;}
        case 9:{
            oss << "ne";
        break;}
        case 10:{
            oss << "cs";
        break;}
        case 11:{
            oss << "ge";
        break;}
        case 12:{
            oss << "gu";
        break;}
        case 13:{
            oss << "cc";
        break;}
        case 14:{
            oss << "pos";
        break;}
        case 15:{
            oss << "vc";
        break;}
        default:
        break;
    }
    switch(this->a){
        case 1:{
            oss << ",a";
        break;}
        default:
        break;
    }
    oss << " ";
    oss << this->disp22;
    return oss.str();
}

leon3_funclt_trap::BRANCH::BRANCH( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck){

}

leon3_funclt_trap::BRANCH::~BRANCH(){

}
unsigned int leon3_funclt_trap::SMULcc_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    long long resultTemp = (long long)(((long long)((int)rs1_op))*((long long)((int)rs2_op)));
    Ybp = ((unsigned long long)resultTemp) >> 32;
    result = resultTemp & 0x00000000FFFFFFFF;
    this->ICC_writeLogic(this->result);
    this->WB_yicc(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::SMULcc_reg::replicate() const throw(){
    return new SMULcc_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::SMULcc_reg::getInstructionName(){
    return "SMULcc_reg";
}

void leon3_funclt_trap::SMULcc_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::SMULcc_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "smulcc r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::SMULcc_reg::SMULcc_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), ICC_writeLogic_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_yicc_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, \
    FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, \
    Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::SMULcc_reg::~SMULcc_reg(){

}
unsigned int leon3_funclt_trap::SUB_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = SignExtend(simm13, 13);

    result = rs1_op - rs2_op;
    this->WB_plain(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::SUB_imm::replicate() const throw(){
    return new SUB_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::SUB_imm::getInstructionName(){
    return "SUB_imm";
}

void leon3_funclt_trap::SUB_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::SUB_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "sub r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::SUB_imm::SUB_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), WB_plain_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, \
    GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::SUB_imm::~SUB_imm(){

}
unsigned int leon3_funclt_trap::ADDcc_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    result = rs1_op + rs2_op;
    this->ICC_writeAdd(this->result, this->rs1_op, this->rs2_op);
    this->WB_icc(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::ADDcc_reg::replicate() const throw(){
    return new ADDcc_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::ADDcc_reg::getInstructionName(){
    return "ADDcc_reg";
}

void leon3_funclt_trap::ADDcc_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::ADDcc_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "addcc r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::ADDcc_reg::ADDcc_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & \
    WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), ICC_writeAdd_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_icc_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, \
    LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, \
    NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::ADDcc_reg::~ADDcc_reg(){

}
unsigned int leon3_funclt_trap::XOR_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    result = rs1_op ^ rs2_op;
    this->WB_plain(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::XOR_reg::replicate() const throw(){
    return new XOR_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::XOR_reg::getInstructionName(){
    return "XOR_reg";
}

void leon3_funclt_trap::XOR_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::XOR_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "xor r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::XOR_reg::XOR_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), WB_plain_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, \
    GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::XOR_reg::~XOR_reg(){

}
unsigned int leon3_funclt_trap::SUBcc_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = SignExtend(simm13, 13);

    result = rs1_op - rs2_op;
    this->ICC_writeSub(this->result, this->rs1_op, this->rs2_op);
    this->WB_icc(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::SUBcc_imm::replicate() const throw(){
    return new SUBcc_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::SUBcc_imm::getInstructionName(){
    return "SUBcc_imm";
}

void leon3_funclt_trap::SUBcc_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::SUBcc_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "subcc r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::SUBcc_imm::SUBcc_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & \
    WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), ICC_writeSub_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_icc_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, \
    LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, \
    NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::SUBcc_imm::~SUBcc_imm(){

}
unsigned int leon3_funclt_trap::TADDccTV_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    result = rs1_op + rs2_op;
    temp_V = ((unsigned int)((rs1_op & rs2_op & (~result)) | ((~rs1_op) & (~rs2_op) & \
        result))) >> 31;
    if(!temp_V && (((rs1_op | rs2_op) & 0x00000003) != 0)){
        temp_V = 1;
    }
    this->ICC_writeTVAdd(this->result, this->temp_V, this->rs1_op, this->rs2_op);

    if(temp_V){
        RaiseException(TAG_OVERFLOW);
    }
    this->WB_icctv(this->rd, this->rd_bit, this->result, this->temp_V);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::TADDccTV_reg::replicate() const throw(){
    return new TADDccTV_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::TADDccTV_reg::getInstructionName(){
    return "TADDccTV_reg";
}

void leon3_funclt_trap::TADDccTV_reg::setParams( const unsigned int & bitString ) \
    throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::TADDccTV_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "taddcctv r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::TADDccTV_reg::TADDccTV_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), ICC_writeTVAdd_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_icctv_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, \
    FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, \
    Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::TADDccTV_reg::~TADDccTV_reg(){

}
unsigned int leon3_funclt_trap::SDIV_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    exception = rs2_op == 0;
    if(!exception){
        long long res64 = ((long long)((((unsigned long long)Ybp) << 32) | (unsigned long \
            long)rs1_op))/((long long)((int)rs2_op));
        temp_V = (res64 & 0xFFFFFFFF80000000LL) != 0 && (res64 & 0xFFFFFFFF80000000LL) != \
            0xFFFFFFFF80000000LL;
        if(temp_V){
            if(res64 > 0){
                result = 0x7FFFFFFF;
            }
            else{
                result = 0x80000000;
            }
        }
        else{
            result = (unsigned int)(res64 & 0x00000000FFFFFFFFLL);
        }
    }

    if(exception){
        RaiseException(DIV_ZERO);
    }
    this->WB_plain(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::SDIV_reg::replicate() const throw(){
    return new SDIV_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::SDIV_reg::getInstructionName(){
    return "SDIV_reg";
}

void leon3_funclt_trap::SDIV_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::SDIV_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "sdiv";
    oss << " r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::SDIV_reg::SDIV_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_plain_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, \
    FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::SDIV_reg::~SDIV_reg(){

}
unsigned int leon3_funclt_trap::SMULcc_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = SignExtend(simm13, 13);

    long long resultTemp = (long long)(((long long)((int)rs1_op))*((long long)((int)rs2_op)));
    Ybp = ((unsigned long long)resultTemp) >> 32;
    result = resultTemp & 0x00000000FFFFFFFF;
    this->ICC_writeLogic(this->result);
    this->WB_yicc(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::SMULcc_imm::replicate() const throw(){
    return new SMULcc_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::SMULcc_imm::getInstructionName(){
    return "SMULcc_imm";
}

void leon3_funclt_trap::SMULcc_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::SMULcc_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "smulcc r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::SMULcc_imm::SMULcc_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), ICC_writeLogic_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_yicc_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, \
    FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, \
    Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::SMULcc_imm::~SMULcc_imm(){

}
unsigned int leon3_funclt_trap::SWAP_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    address = rs1 + rs2;
    toWrite = rd;

    if(notAligned){
        RaiseException(MEM_ADDR_NOT_ALIGNED);
    }

    if(notAligned){
        flush();
    }
    else{
        readValue = dataMem.read_word(address);
        dataMem.write_word(address, toWrite);
    }

    notAligned = (address & 0x00000003) != 0;
    if(notAligned){
        flush();
    }

    rd = readValue;
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::SWAP_reg::replicate() const throw(){
    return new SWAP_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::SWAP_reg::getInstructionName(){
    return "SWAP_reg";
}

void leon3_funclt_trap::SWAP_reg::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->asi = (bitString & 0x1fe0) >> 5;
}

std::string leon3_funclt_trap::SWAP_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "swap r";
    oss << this->rs1_bit;
    oss << "+r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::SWAP_reg::SWAP_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::SWAP_reg::~SWAP_reg(){

}
unsigned int leon3_funclt_trap::SUBX_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = SignExtend(simm13, 13);

    result = rs1_op - rs2_op - PSRbp[key_ICC_c];
    this->WB_plain(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::SUBX_imm::replicate() const throw(){
    return new SUBX_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::SUBX_imm::getInstructionName(){
    return "SUBX_imm";
}

void leon3_funclt_trap::SUBX_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::SUBX_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "subx r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::SUBX_imm::SUBX_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), WB_plain_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, \
    GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::SUBX_imm::~SUBX_imm(){

}
unsigned int leon3_funclt_trap::STDA_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    address = rs1 + rs2;
    if(rd_bit % 2 == 0){
        toWrite = rd || (((unsigned long long)REGS[rd_bit + 1]) << 32);
    }
    else{
        toWrite = REGS[rd_bit + 1] || (((unsigned long long)rd) << 32);
    }
    supervisor = PSR[key_S];

    notAligned = (address & 0x00000003) != 0;
    if(!supervisor || notAligned){
        flush();
    }

    if(supervisor || !notAligned){
        dataMem.write_dword(address, toWrite);
    }
    else{
        flush();
    }

    if(!supervisor){
        RaiseException(PRIVILEDGE_INSTR);
    }
    if(notAligned){
        RaiseException(MEM_ADDR_NOT_ALIGNED);
    }
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::STDA_reg::replicate() const throw(){
    return new STDA_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::STDA_reg::getInstructionName(){
    return "STDA_reg";
}

void leon3_funclt_trap::STDA_reg::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->asi = (bitString & 0x1fe0) >> 5;
}

std::string leon3_funclt_trap::STDA_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "stda r";
    oss << this->rd_bit;
    oss << " r";
    oss << this->rs1_bit;
    oss << "+r";
    oss << this->rs2_bit;
    oss << " ";
    oss << this->asi;
    return oss.str();
}

leon3_funclt_trap::STDA_reg::STDA_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::STDA_reg::~STDA_reg(){

}
unsigned int leon3_funclt_trap::UMAC_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    unsigned int resultTemp = ((unsigned int)rs1_op & 0x0000ffff)*((unsigned int)rs2_op \
        & 0x0000ffff);
    unsigned long long resultAcc = ((((unsigned long long)(Ybp & 0x000000ff)) << 32) \
        | (unsigned int)ASR18bp) + resultTemp;
    Ybp = (resultAcc & 0x000000ff00000000LL) >> 32;
    ASR18bp = resultAcc & 0x00000000FFFFFFFFLL;
    result = resultAcc & 0x00000000FFFFFFFFLL;
    this->WB_yasr(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::UMAC_reg::replicate() const throw(){
    return new UMAC_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::UMAC_reg::getInstructionName(){
    return "UMAC_reg";
}

void leon3_funclt_trap::UMAC_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::UMAC_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "umac r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::UMAC_reg::UMAC_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), WB_yasr_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, \
    GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::UMAC_reg::~UMAC_reg(){

}
unsigned int leon3_funclt_trap::JUMP_imm::behavior(){
    this->totalInstrCycles = 0;

    unsigned int jumpAddr = rs1 + SignExtend(simm13, 13);
    if((jumpAddr & 0x00000003) != 0){
        trapNotAligned = true;
    }
    else{
        trapNotAligned = false;
        oldPC = PC - 4;
        #ifdef ACC_MODEL
        PC = jumpAddr;
        NPC = jumpAddr + 4;
        #else
        PC = NPC;
        NPC = jumpAddr;
        #endif
    }

    if(trapNotAligned){
        RaiseException(MEM_ADDR_NOT_ALIGNED);
    }

    if(!trapNotAligned){
        rd = oldPC;
    }
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::JUMP_imm::replicate() const throw(){
    return new JUMP_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::JUMP_imm::getInstructionName(){
    return "JUMP_imm";
}

void leon3_funclt_trap::JUMP_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::JUMP_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "jmpl";
    oss << " r";
    oss << this->rs1_bit;
    oss << "+";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::JUMP_imm::JUMP_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck){

}

leon3_funclt_trap::JUMP_imm::~JUMP_imm(){

}
unsigned int leon3_funclt_trap::SMUL_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    long long resultTemp = (long long)(((long long)((int)rs1_op))*((long long)((int)rs2_op)));
    Ybp = ((unsigned long long)resultTemp) >> 32;
    result = resultTemp & 0x00000000FFFFFFFF;
    this->WB_y(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::SMUL_reg::replicate() const throw(){
    return new SMUL_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::SMUL_reg::getInstructionName(){
    return "SMUL_reg";
}

void leon3_funclt_trap::SMUL_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::SMUL_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "smul r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::SMUL_reg::SMUL_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), WB_y_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, \
    GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::SMUL_reg::~SMUL_reg(){

}
unsigned int leon3_funclt_trap::XORcc_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;

    result = rs1_op ^ SignExtend(simm13, 13);
    this->ICC_writeLogic(this->result);
    this->WB_icc(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::XORcc_imm::replicate() const throw(){
    return new XORcc_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::XORcc_imm::getInstructionName(){
    return "XORcc_imm";
}

void leon3_funclt_trap::XORcc_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::XORcc_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "xorcc r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::XORcc_imm::XORcc_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & \
    WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), ICC_writeLogic_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_icc_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, \
    LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, \
    NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::XORcc_imm::~XORcc_imm(){

}
unsigned int leon3_funclt_trap::ORNcc_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;

    result = rs1_op | ~(SignExtend(simm13, 13));
    this->ICC_writeLogic(this->result);
    this->WB_icc(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::ORNcc_imm::replicate() const throw(){
    return new ORNcc_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::ORNcc_imm::getInstructionName(){
    return "ORNcc_imm";
}

void leon3_funclt_trap::ORNcc_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::ORNcc_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "orncc r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::ORNcc_imm::ORNcc_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & \
    WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), ICC_writeLogic_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_icc_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, \
    LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, \
    NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::ORNcc_imm::~ORNcc_imm(){

}
unsigned int leon3_funclt_trap::LDUBA_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    address = rs1 + rs2;
    supervisor = PSR[key_S];

    if(!supervisor){
        flush();
    }

    if(!supervisor){
        flush();
    }
    else{
        readValue = dataMem.read_byte(address);
    }

    if(!supervisor){
        RaiseException(PRIVILEDGE_INSTR);
    }

    rd = readValue;
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::LDUBA_reg::replicate() const throw(){
    return new LDUBA_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::LDUBA_reg::getInstructionName(){
    return "LDUBA_reg";
}

void leon3_funclt_trap::LDUBA_reg::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->asi = (bitString & 0x1fe0) >> 5;
}

std::string leon3_funclt_trap::LDUBA_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "lduba r";
    oss << this->rs1_bit;
    oss << "+r";
    oss << this->rs2_bit;
    oss << " ";
    oss << this->asi;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::LDUBA_reg::LDUBA_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & \
    WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::LDUBA_reg::~LDUBA_reg(){

}
unsigned int leon3_funclt_trap::JUMP_reg::behavior(){
    this->totalInstrCycles = 0;

    unsigned int jumpAddr = rs1 + rs2;
    if((jumpAddr & 0x00000003) != 0){
        trapNotAligned = true;
    }
    else{
        trapNotAligned = false;
        oldPC = PC - 4;
        #ifdef ACC_MODEL
        PC = jumpAddr;
        NPC = jumpAddr + 4;
        #else
        PC = NPC;
        NPC = jumpAddr;
        #endif
    }

    if(trapNotAligned){
        RaiseException(MEM_ADDR_NOT_ALIGNED);
    }

    if(!trapNotAligned){
        rd = oldPC;
    }
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::JUMP_reg::replicate() const throw(){
    return new JUMP_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::JUMP_reg::getInstructionName(){
    return "JUMP_reg";
}

void leon3_funclt_trap::JUMP_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::JUMP_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "jmpl";
    oss << " r";
    oss << this->rs1_bit;
    oss << "+r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::JUMP_reg::JUMP_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck){

}

leon3_funclt_trap::JUMP_reg::~JUMP_reg(){

}
unsigned int leon3_funclt_trap::ADDX_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    result = rs1_op + rs2_op + PSRbp[key_ICC_c];
    this->WB_plain(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::ADDX_reg::replicate() const throw(){
    return new ADDX_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::ADDX_reg::getInstructionName(){
    return "ADDX_reg";
}

void leon3_funclt_trap::ADDX_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::ADDX_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "addx r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::ADDX_reg::ADDX_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), WB_plain_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, \
    GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::ADDX_reg::~ADDX_reg(){

}
unsigned int leon3_funclt_trap::UDIV_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    exception = rs2_op == 0;
    if(!exception){
        unsigned long long res64 = ((unsigned long long)((((unsigned long long)Ybp) << 32) \
            | (unsigned long long)rs1_op))/(unsigned long long)rs2_op;
        temp_V = (res64 & 0xFFFFFFFF00000000LL) != 0;
        if(temp_V){
            result = 0xFFFFFFFF;
        }
        else{
            result = (unsigned int)(res64 & 0x00000000FFFFFFFFLL);
        }
    }

    if(exception){
        RaiseException(DIV_ZERO);
    }
    this->WB_plain(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::UDIV_reg::replicate() const throw(){
    return new UDIV_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::UDIV_reg::getInstructionName(){
    return "UDIV_reg";
}

void leon3_funclt_trap::UDIV_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::UDIV_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "udiv";
    oss << " r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::UDIV_reg::UDIV_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_plain_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, \
    FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::UDIV_reg::~UDIV_reg(){

}
unsigned int leon3_funclt_trap::XNORcc_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;

    result = rs1_op ^ ~(SignExtend(simm13, 13));
    this->ICC_writeLogic(this->result);
    this->WB_icc(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::XNORcc_imm::replicate() const throw(){
    return new XNORcc_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::XNORcc_imm::getInstructionName(){
    return "XNORcc_imm";
}

void leon3_funclt_trap::XNORcc_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::XNORcc_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "xnorcc r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::XNORcc_imm::XNORcc_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), ICC_writeLogic_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_icc_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, \
    LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, \
    NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::XNORcc_imm::~XNORcc_imm(){

}
unsigned int leon3_funclt_trap::STBAR::behavior(){
    this->totalInstrCycles = 0;

    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::STBAR::replicate() const throw(){
    return new STBAR(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::STBAR::getInstructionName(){
    return "STBAR";
}

void leon3_funclt_trap::STBAR::setParams( const unsigned int & bitString ) throw(){

}

std::string leon3_funclt_trap::STBAR::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "s";
    oss << "t";
    oss << "b";
    oss << "a";
    oss << "r";
    return oss.str();
}

leon3_funclt_trap::STBAR::STBAR( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, Reg32_2 \
    & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 & \
    Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck){

}

leon3_funclt_trap::STBAR::~STBAR(){

}
unsigned int leon3_funclt_trap::LDA_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    address = rs1 + rs2;
    supervisor = PSR[key_S];

    notAligned = (address & 0x00000003) != 0;
    if(notAligned || !supervisor){
        flush();
    }

    if(notAligned || !supervisor){
        flush();
    }
    else{
        readValue = dataMem.read_word(address);
    }

    if(!supervisor){
        RaiseException(PRIVILEDGE_INSTR);
    }
    if(notAligned){
        RaiseException(MEM_ADDR_NOT_ALIGNED);
    }

    rd = readValue;
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::LDA_reg::replicate() const throw(){
    return new LDA_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::LDA_reg::getInstructionName(){
    return "LDA_reg";
}

void leon3_funclt_trap::LDA_reg::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->asi = (bitString & 0x1fe0) >> 5;
}

std::string leon3_funclt_trap::LDA_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "lda r";
    oss << this->rs1_bit;
    oss << "+r";
    oss << this->rs2_bit;
    oss << " ";
    oss << this->asi;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::LDA_reg::LDA_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::LDA_reg::~LDA_reg(){

}
unsigned int leon3_funclt_trap::STHA_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    address = rs1 + rs2;
    toWrite = (unsigned short int)(rd & 0x0000FFFF);
    supervisor = PSR[key_S];

    notAligned = (address & 0x00000001) != 0;
    if(!supervisor || notAligned){
        flush();
    }

    if(supervisor || !notAligned){
        dataMem.write_half(address, toWrite);
    }
    else{
        flush();
    }

    if(!supervisor){
        RaiseException(PRIVILEDGE_INSTR);
    }
    if(notAligned){
        RaiseException(MEM_ADDR_NOT_ALIGNED);
    }
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::STHA_reg::replicate() const throw(){
    return new STHA_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::STHA_reg::getInstructionName(){
    return "STHA_reg";
}

void leon3_funclt_trap::STHA_reg::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->asi = (bitString & 0x1fe0) >> 5;
}

std::string leon3_funclt_trap::STHA_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "stha r";
    oss << this->rd_bit;
    oss << " r";
    oss << this->rs1_bit;
    oss << "+r";
    oss << this->rs2_bit;
    oss << " ";
    oss << this->asi;
    return oss.str();
}

leon3_funclt_trap::STHA_reg::STHA_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::STHA_reg::~STHA_reg(){

}
unsigned int leon3_funclt_trap::LDDA_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    #ifdef ACC_MODEL
    REGS[rd_bit | 0x1].lock();
    #endif

    address = rs1 + rs2;
    supervisor = PSR[key_S];

    notAligned = (address & 0x00000007) != 0;
    if(notAligned || !supervisor){
        #ifdef ACC_MODEL
        REGS[rd_bit | 0x1].unlock();
        #endif
        flush();
    }

    if(notAligned || !supervisor){
        flush();
    }
    else{
        readValue = dataMem.read_dword(address);
    }

    if(!supervisor){
        RaiseException(PRIVILEDGE_INSTR);
    }
    if(notAligned){
        RaiseException(MEM_ADDR_NOT_ALIGNED);
    }

    if(rd_bit % 2 == 0){
        rd = (unsigned int)(readValue & 0x00000000FFFFFFFFLL);
        REGS[rd_bit + 1] = (unsigned int)((readValue >> 32) & 0x00000000FFFFFFFFLL);
        #ifdef ACC_MODEL
        REGS[rd_bit + 1].unlock();
        #endif
    }
    else{
        REGS[rd_bit - 1] = (unsigned int)(readValue & 0x00000000FFFFFFFFLL);
        rd = (unsigned int)((readValue >> 32) & 0x00000000FFFFFFFFLL);
        #ifdef ACC_MODEL
        REGS[rd_bit - 1].unlock();
        #endif
    }
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::LDDA_reg::replicate() const throw(){
    return new LDDA_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::LDDA_reg::getInstructionName(){
    return "LDDA_reg";
}

void leon3_funclt_trap::LDDA_reg::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->asi = (bitString & 0x1fe0) >> 5;
}

std::string leon3_funclt_trap::LDDA_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "ldda r";
    oss << this->rs1_bit;
    oss << "+r";
    oss << this->rs2_bit;
    oss << " ";
    oss << this->asi;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::LDDA_reg::LDDA_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::LDDA_reg::~LDDA_reg(){

}
unsigned int leon3_funclt_trap::SLL_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    result = rs1_op << (rs2_op & 0x0000001f);
    this->WB_plain(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::SLL_reg::replicate() const throw(){
    return new SLL_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::SLL_reg::getInstructionName(){
    return "SLL_reg";
}

void leon3_funclt_trap::SLL_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::SLL_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "sll r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::SLL_reg::SLL_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), WB_plain_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, \
    GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::SLL_reg::~SLL_reg(){

}
unsigned int leon3_funclt_trap::RESTORE_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = SignExtend(simm13, 13);

    result = rs1_op + rs2_op;
    okNewWin = IncrementRegWindow();

    if(!okNewWin){
        RaiseException(WINDOW_UNDERFLOW);
    }

    if(okNewWin){
        rd = result;
    }
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::RESTORE_imm::replicate() const throw(){
    return new RESTORE_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::RESTORE_imm::getInstructionName(){
    return "RESTORE_imm";
}

void leon3_funclt_trap::RESTORE_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::RESTORE_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "restore";
    oss << " r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::RESTORE_imm::RESTORE_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::RESTORE_imm::~RESTORE_imm(){

}
unsigned int leon3_funclt_trap::LD_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    address = rs1 + SignExtend(simm13, 13);

    notAligned = (address & 0x00000003) != 0;
    if(notAligned){
        flush();
    }

    readValue = dataMem.read_word(address);

    if(notAligned){
        RaiseException(MEM_ADDR_NOT_ALIGNED);
    }

    rd = readValue;
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::LD_imm::replicate() const throw(){
    return new LD_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::LD_imm::getInstructionName(){
    return "LD_imm";
}

void leon3_funclt_trap::LD_imm::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::LD_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "ld r";
    oss << this->rs1_bit;
    oss << "+";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::LD_imm::LD_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::LD_imm::~LD_imm(){

}
unsigned int leon3_funclt_trap::TRAP_reg::behavior(){
    this->totalInstrCycles = 0;

    // All the other non-special situations
    raiseException = (cond == 0x8) ||
    ((cond == 0x9) && PSRbp[key_ICC_z] == 0) ||
    ((cond == 0x1) && PSRbp[key_ICC_z] != 0) ||
    ((cond == 0xa) && (PSRbp[key_ICC_z] == 0) && (PSRbp[key_ICC_n] == PSRbp[key_ICC_v])) ||
    ((cond == 0x2) && ((PSRbp[key_ICC_z] != 0) || (PSRbp[key_ICC_n] != PSRbp[key_ICC_v]))) ||
    ((cond == 0xb) && PSRbp[key_ICC_n] == PSRbp[key_ICC_v]) ||
    ((cond == 0x3) && PSRbp[key_ICC_n] != PSRbp[key_ICC_v]) ||
    ((cond == 0xc) && (PSRbp[key_ICC_c] + PSRbp[key_ICC_z]) == 0) ||
    ((cond == 0x4) && (PSRbp[key_ICC_c] + PSRbp[key_ICC_z]) > 0) ||
    ((cond == 0xd) && PSRbp[key_ICC_c] == 0) ||
    ((cond == 0x5) && PSRbp[key_ICC_c] != 0) ||
    ((cond == 0xe) && PSRbp[key_ICC_n] == 0) ||
    ((cond == 0x6) && PSRbp[key_ICC_n] != 0) ||
    ((cond == 0xf) && PSRbp[key_ICC_v] == 0) ||
    ((cond == 0x7) && PSRbp[key_ICC_v] != 0);

    if(raiseException){
        RaiseException(TRAP_INSTRUCTION, (rs1 + rs2) & 0x0000007F);
    }
    #ifndef ACC_MODEL
    else{
        PC = NPC;
        NPC += 4;
    }
    #endif
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::TRAP_reg::replicate() const throw(){
    return new TRAP_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::TRAP_reg::getInstructionName(){
    return "TRAP_reg";
}

void leon3_funclt_trap::TRAP_reg::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
    this->reserved1 = (bitString & 0x20000000) >> 29;
    this->cond = (bitString & 0x1e000000) >> 25;
    this->asi = (bitString & 0x1fe0) >> 5;
}

std::string leon3_funclt_trap::TRAP_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "t";
    switch(this->cond){
        case 0:{
            oss << "n";
        break;}
        case 1:{
            oss << "e";
        break;}
        case 2:{
            oss << "le";
        break;}
        case 3:{
            oss << "l";
        break;}
        case 4:{
            oss << "leu";
        break;}
        case 6:{
            oss << "neg";
        break;}
        case 7:{
            oss << "vs";
        break;}
        case 8:{
            oss << "a";
        break;}
        case 9:{
            oss << "ne";
        break;}
        case 10:{
            oss << "cs";
        break;}
        case 11:{
            oss << "ge";
        break;}
        case 12:{
            oss << "gu";
        break;}
        case 13:{
            oss << "cc";
        break;}
        case 14:{
            oss << "pos";
        break;}
        case 15:{
            oss << "vc";
        break;}
        default:
        break;
    }
    oss << " r";
    oss << this->rs1_bit;
    oss << "+r";
    oss << this->rs2_bit;
    return oss.str();
}

leon3_funclt_trap::TRAP_reg::TRAP_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck){

}

leon3_funclt_trap::TRAP_reg::~TRAP_reg(){

}
unsigned int leon3_funclt_trap::LDUB_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    address = rs1 + SignExtend(simm13, 13);

    readValue = dataMem.read_byte(address);

    rd = readValue;
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::LDUB_imm::replicate() const throw(){
    return new LDUB_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::LDUB_imm::getInstructionName(){
    return "LDUB_imm";
}

void leon3_funclt_trap::LDUB_imm::setParams( const unsigned int & bitString ) throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::LDUB_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "ldub r";
    oss << this->rs1_bit;
    oss << "+";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::LDUB_imm::LDUB_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::LDUB_imm::~LDUB_imm(){

}
unsigned int leon3_funclt_trap::RETT_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;
    exceptionEnabled = PSR[key_ET];
    supervisor = PSR[key_S];

    targetAddr = rs1_op + rs2_op;
    newCwp = ((unsigned int)(PSR[key_CWP] + 1)) % NUM_REG_WIN;

    if(exceptionEnabled){
        if(supervisor){
            RaiseException(ILLEGAL_INSTR);
        }
        else{
            RaiseException(PRIVILEDGE_INSTR);
        }
    }
    else if(!supervisor || ((0x01 << (newCwp)) & WIM) != 0 || (targetAddr & 0x00000003) \
        != 0){
        THROW_EXCEPTION("Invalid processor mode during execution of the RETT instruction \
            - supervisor: " << supervisor << " newCwp: " << std::hex << std::showbase << newCwp \
            << " targetAddr: " << std::hex << std::showbase << targetAddr);
    }
    for(int i = 8; i < 32; i++){
        REGS[i].updateAlias(WINREGS[(newCwp*16 + i - 8) % (16*NUM_REG_WIN)]);
    }

    PSRbp = (PSR & 0xFFFFFF40) | (newCwp | 0x20 | (PSR[key_PS] << 7));
    PSR.immediateWrite(PSRbp);
    PC = NPC;
    NPC = targetAddr;
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::RETT_reg::replicate() const throw(){
    return new RETT_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::RETT_reg::getInstructionName(){
    return "RETT_reg";
}

void leon3_funclt_trap::RETT_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::RETT_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "rett r";
    oss << this->rs1_bit;
    oss << "+r";
    oss << this->rs2_bit;
    return oss.str();
}

leon3_funclt_trap::RETT_reg::RETT_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::RETT_reg::~RETT_reg(){

}
unsigned int leon3_funclt_trap::SDIVcc_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = SignExtend(simm13, 13);

    exception = rs2_op == 0;
    if(!exception){
        long long res64 = ((long long)((((unsigned long long)Ybp) << 32) | (unsigned long \
            long)rs1_op))/((long long)((int)rs2_op));
        temp_V = (res64 & 0xFFFFFFFF80000000LL) != 0 && (res64 & 0xFFFFFFFF80000000LL) != \
            0xFFFFFFFF80000000LL;
        if(temp_V){
            if(res64 > 0){
                result = 0x7FFFFFFF;
            }
            else{
                result = 0x80000000;
            }
        }
        else{
            result = (unsigned int)(res64 & 0x00000000FFFFFFFFLL);
        }
    }
    this->ICC_writeDiv(this->result, this->temp_V, this->rs1_op, this->rs2_op);

    if(exception){
        RaiseException(DIV_ZERO);
    }
    this->WB_icc(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::SDIVcc_imm::replicate() const throw(){
    return new SDIVcc_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::SDIVcc_imm::getInstructionName(){
    return "SDIVcc_imm";
}

void leon3_funclt_trap::SDIVcc_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::SDIVcc_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "sdivcc";
    oss << " r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::SDIVcc_imm::SDIVcc_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), ICC_writeDiv_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, \
    FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), WB_icc_op(PSR, WIM, TBR, Y, PC, \
    NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::SDIVcc_imm::~SDIVcc_imm(){

}
unsigned int leon3_funclt_trap::SAVE_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    result = rs1_op + rs2_op;
    okNewWin = DecrementRegWindow();

    if(!okNewWin){
        RaiseException(WINDOW_OVERFLOW);
    }

    if(okNewWin){
        rd = result;
    }
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::SAVE_reg::replicate() const throw(){
    return new SAVE_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::SAVE_reg::getInstructionName(){
    return "SAVE_reg";
}

void leon3_funclt_trap::SAVE_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::SAVE_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "save";
    oss << " r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::SAVE_reg::SAVE_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::SAVE_reg::~SAVE_reg(){

}
unsigned int leon3_funclt_trap::OR_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;
    rs2_op = rs2;

    result = rs1_op | rs2_op;
    this->WB_plain(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::OR_reg::replicate() const throw(){
    return new OR_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::OR_reg::getInstructionName(){
    return "OR_reg";
}

void leon3_funclt_trap::OR_reg::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
}

std::string leon3_funclt_trap::OR_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "or r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::OR_reg::OR_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), WB_plain_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, \
    GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::OR_reg::~OR_reg(){

}
unsigned int leon3_funclt_trap::ORcc_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;

    result = rs1_op | SignExtend(simm13, 13);
    this->ICC_writeLogic(this->result);
    this->WB_icc(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::ORcc_imm::replicate() const throw(){
    return new ORcc_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::ORcc_imm::getInstructionName(){
    return "ORcc_imm";
}

void leon3_funclt_trap::ORcc_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::ORcc_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "orcc r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::ORcc_imm::ORcc_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, \
    Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 \
    & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck), ICC_writeLogic_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, \
    ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_icc_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, \
    LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, \
    NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::ORcc_imm::~ORcc_imm(){

}
unsigned int leon3_funclt_trap::CALL::behavior(){
    this->totalInstrCycles = 0;

    unsigned int curPC = PC;
    unsigned int target = curPC + (disp30 << 2);
    oldPC = curPC - 4;
    #ifdef ACC_MODEL
    PC = target;
    NPC = target + 4;
    #else
    PC = NPC;
    NPC = target - 4;
    #endif

    REGS[15] = oldPC;
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::CALL::replicate() const throw(){
    return new CALL(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::CALL::getInstructionName(){
    return "CALL";
}

void leon3_funclt_trap::CALL::setParams( const unsigned int & bitString ) throw(){
    this->disp30 = (bitString & 0x3fffffff);
}

std::string leon3_funclt_trap::CALL::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "call ";
    oss << this->disp30;
    return oss.str();
}

leon3_funclt_trap::CALL::CALL( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & WIM, Reg32_2 \
    & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, Reg32_3 & \
    Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, Reg32_3 \
    * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, WIM, \
    TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, \
    instrMem, dataMem, irqAck){

}

leon3_funclt_trap::CALL::~CALL(){

}
unsigned int leon3_funclt_trap::WRITEpsr_reg::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    // Note how we filter writes to EF and EC fields since we do not
    // have neither a co-processor nor the FPU
    result = ((rs1 ^ rs2) & 0x00FFCFFF) | 0xF3000000;
    supervisorException = (PSR[key_S] == 0);
    illegalCWP = (result & 0x0000001f) >= NUM_REG_WIN;

    if(!(supervisorException || illegalCWP)){
        PSRbp = result;
    }

    if(supervisorException){
        RaiseException(PRIVILEDGE_INSTR);
    }
    if(illegalCWP){
        RaiseException(ILLEGAL_INSTR);
    }

    PSR = result;
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::WRITEpsr_reg::replicate() const throw(){
    return new WRITEpsr_reg(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::WRITEpsr_reg::getInstructionName(){
    return "WRITEpsr_reg";
}

void leon3_funclt_trap::WRITEpsr_reg::setParams( const unsigned int & bitString ) \
    throw(){
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->rs2_bit = (bitString & 0x1f);
    this->rs2.directSetAlias(this->REGS[this->rs2_bit]);
    this->rd = (bitString & 0x3e000000) >> 25;
}

std::string leon3_funclt_trap::WRITEpsr_reg::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "wr r";
    oss << this->rs1_bit;
    oss << " r";
    oss << this->rs2_bit;
    oss << " psr";
    return oss.str();
}

leon3_funclt_trap::WRITEpsr_reg::WRITEpsr_reg( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 \
    & WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck){

}

leon3_funclt_trap::WRITEpsr_reg::~WRITEpsr_reg(){

}
unsigned int leon3_funclt_trap::ANDcc_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    rs1_op = rs1;

    result = rs1_op & SignExtend(simm13, 13);
    this->ICC_writeLogic(this->result);
    this->WB_icc(this->rd, this->rd_bit, this->result);
    return this->totalInstrCycles;
}

Instruction * leon3_funclt_trap::ANDcc_imm::replicate() const throw(){
    return new ANDcc_imm(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, \
        ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck);
}

std::string leon3_funclt_trap::ANDcc_imm::getInstructionName(){
    return "ANDcc_imm";
}

void leon3_funclt_trap::ANDcc_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0x3e000000) >> 25;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rs1_bit = (bitString & 0x7c000) >> 14;
    this->rs1.directSetAlias(this->REGS[this->rs1_bit]);
    this->simm13 = (bitString & 0x1fff);
}

std::string leon3_funclt_trap::ANDcc_imm::getMnemonic(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "andcc r";
    oss << this->rs1_bit;
    oss << " ";
    oss << this->simm13;
    oss << " r";
    oss << this->rd_bit;
    return oss.str();
}

leon3_funclt_trap::ANDcc_imm::ANDcc_imm( Reg32_0_delay_3 & PSR, Reg32_1_delay_3 & \
    WIM, Reg32_2 & TBR, Reg32_3 & Y, Reg32_3_off_4 & PC, Reg32_3 & NPC, Reg32_0 & PSRbp, \
    Reg32_3 & Ybp, Reg32_3 & ASR18bp, RegisterBankClass & GLOBAL, Reg32_3 * & WINREGS, \
    Reg32_3 * & ASR, Alias & FP, Alias & LR, Alias & SP, Alias & PCR, Alias * & REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 & irqAck ) : Instruction(PSR, \
    WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, \
    REGS, instrMem, dataMem, irqAck), ICC_writeLogic_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, \
    Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, dataMem, irqAck), \
    WB_icc_op(PSR, WIM, TBR, Y, PC, NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, \
    LR, SP, PCR, REGS, instrMem, dataMem, irqAck), IncrementPC_op(PSR, WIM, TBR, Y, PC, \
    NPC, PSRbp, Ybp, ASR18bp, GLOBAL, WINREGS, ASR, FP, LR, SP, PCR, REGS, instrMem, \
    dataMem, irqAck){

}

leon3_funclt_trap::ANDcc_imm::~ANDcc_imm(){

}

