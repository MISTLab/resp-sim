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



#include <instructionBase.hpp>
#include <string>
#include <customExceptions.hpp>
#include <trap_utils.hpp>
#include <registers.hpp>
#include <alias.hpp>
#include <externalPorts.hpp>
#include <instructions.hpp>
#include <systemc.h>
#include <sstream>

using namespace arm7tdmi_funclt_trap;
void arm7tdmi_funclt_trap::Instruction::restoreSPSR(){

    unsigned int curMode = CPSR[key_mode];
    switch(curMode){
        case 0x1:{
            //I'm in FIQ mode
            CPSR = SPSR[0];
        break;}
        case 0x2:{
            //I'm in IRQ mode
            CPSR = SPSR[1];
        break;}
        case 0x3:{
            //I'm in SVC mode
            CPSR = SPSR[2];
        break;}
        case 0x7:{
            //I'm in ABT mode
            CPSR = SPSR[3];
        break;}
        case 0xB:{
            //I'm in UND mode
            CPSR = SPSR[4];
        break;}
        default:{
            THROW_EXCEPTION("Unable to restore the PSR when starting from user or supervisor mode");
        break;}
    }
    updateAliases(curMode, CPSR[key_mode]);
}

void arm7tdmi_funclt_trap::Instruction::updateAliases( unsigned int fromMode, unsigned \
    int toMode ){

    switch(toMode){
        case 0x0:
        case 0xF:{
            //User or System mode
            REGS[13].updateAlias(RB[13]);
            REGS[14].updateAlias(RB[14]);
        break;}
        case 0x2:{
            //IRQ mode
            REGS[13].updateAlias(RB[21]);
            REGS[14].updateAlias(RB[22]);
        break;}
        case 0x1:{
            //FIQ mode
            REGS[8].updateAlias(RB[23]);
            REGS[9].updateAlias(RB[24]);
            REGS[10].updateAlias(RB[25]);
            REGS[11].updateAlias(RB[26]);
            REGS[12].updateAlias(RB[27]);
            REGS[13].updateAlias(RB[28]);
            REGS[14].updateAlias(RB[29]);
        break;}
        case 0x3:{
            //SVC mode
            REGS[13].updateAlias(RB[15]);
            REGS[14].updateAlias(RB[16]);
        break;}
        case 0x7:{
            //ABT mode
            REGS[13].updateAlias(RB[17]);
            REGS[14].updateAlias(RB[18]);
        break;}
        case 0xB:{
            //UND mode
            REGS[13].updateAlias(RB[19]);
            REGS[14].updateAlias(RB[20]);
        break;}
        default:{
            THROW_EXCEPTION("Not valid toMode " << toMode << " when changing the registers");
        break;}
    }

    if(fromMode == 0x1 && toMode != 0x1){
        REGS[8].updateAlias(RB[8]);
        REGS[9].updateAlias(RB[9]);
        REGS[10].updateAlias(RB[10]);
        REGS[11].updateAlias(RB[11]);
        REGS[12].updateAlias(RB[12]);
    }
}

unsigned int arm7tdmi_funclt_trap::Instruction::ArithmeticShiftRight( unsigned int \
    shift_amm, unsigned int toShift ){
    {
        unsigned int shifted;

        shifted = toShift >> shift_amm;
        //Controlling the sign extensions
        if((toShift & 0x80000000) != 0){
            shifted |= (((unsigned int)0xFFFFFFFF) << (32 - shift_amm));
        }
        else{
            shifted &= (((unsigned int)0xFFFFFFFF) >> shift_amm);
        }
        return shifted;
    }
}

unsigned int arm7tdmi_funclt_trap::Instruction::RotateRight( unsigned int rotate_amm, \
    unsigned int toRotate ){
    {
        unsigned int rotated;
        unsigned int toGlue;

        //value which must be glued to the left of the shifted quantity
        toGlue = toRotate & (((unsigned int)0xFFFFFFFF) >> (32 - rotate_amm));
        rotated = ((toRotate >> rotate_amm) & (((unsigned int)0xFFFFFFFF) >> rotate_amm));
        toGlue <<= (32 - rotate_amm);
        return (toGlue | rotated);
    }
}

unsigned int arm7tdmi_funclt_trap::Instruction::LSRegShift( unsigned int shift_type, \
    unsigned int shift_amm, unsigned int toShift ){

    //Case on the type of shift
    switch(shift_type){
        case 0:{
            // Shift left
            if(shift_amm == 0){
                return toShift;
            }
            else
            return (toShift << shift_amm);
        break;}
        case 1:{
            // logical shift right
            if(shift_amm == 0){
                // represents a 32 bit shift
                return 0;
            }
            else{
                return ((unsigned int) toShift) >> shift_amm;
            }
        break;}
        case 2:{
            // arithmetic shift right
            if(shift_amm == 0){
                if ((toShift & 0x80000000) != 0){
                    return 0xFFFFFFFF;
                }
                else{
                    return 0;
                }
            }
            else{
                return ArithmeticShiftRight(shift_amm, toShift);
            }
        break;}
        case 3:{
            if(shift_amm == 0){
                return (((unsigned int)toShift) >> 1) | ((CPSR[key_C]) << 31);
            }
            else{
                // rotate right
                return RotateRight(shift_amm, toShift);
            }
        break;}
        default:{
            THROW_EXCEPTION("Unknown Shift Type in Load/Store register shift");
        break;}
    }
    return toShift;
}

unsigned int arm7tdmi_funclt_trap::Instruction::SignExtend( unsigned int bitSeq, \
    unsigned int bitSeq_length ){

    if((bitSeq & (1 << (bitSeq_length - 1))) != 0)
    bitSeq |= (((unsigned int)0xFFFFFFFF) << bitSeq_length);
    return bitSeq;
}

void arm7tdmi_funclt_trap::Instruction::UpdatePSRBitM( unsigned int result, bool carry ){

    // N flag if the results is negative
    CPSR[key_N] = ((result & 0x80000000) != 0);
    //Update flag Z if the result is 0
    CPSR[key_Z] = (result == 0);
    //Update the C flag if a carry occurred in the operation
    CPSR[key_C] = (carry != 0);
    //No updates performed to the V flag.
}

void arm7tdmi_funclt_trap::Instruction::UpdatePSRAddInner( unsigned int operand1, \
    unsigned int operand2, bool carry ){

    long long resultSign = (long long)((long long)((int)operand1) + (long long)((int)operand2)) \
        + (long long)((int)carry);

    // N flag if the results is negative
    CPSR[key_N] = ((resultSign & 0x0000000080000000LL) != 0);

    //Update flag Z if the result is 0
    CPSR[key_Z] = (resultSign == 0);

    //Update the C resultUnSign if a carry occurred in the operation
    CPSR[key_C] = (((operand1 ^ operand2 ^ ((unsigned int)(resultSign >> 1))) & 0x80000000) \
        != 0);

    //Update the V flag if an overflow occurred in the operation
    CPSR[key_V] = ((((unsigned int)(resultSign >> 1)) ^ ((unsigned int)resultSign)) & \
        0x80000000) != 0;
}

void arm7tdmi_funclt_trap::Instruction::UpdatePSRSubInner( unsigned int operand1, \
    unsigned int operand2, bool carry ){

    long long resultSign = (long long)((long long)((int)operand1) - (long long)((int)operand2)) \
        - (long long)((int)carry);
    //unsigned long long resultUnSign = (unsigned long long)((unsigned long long)operand1 \
        - (unsigned long long)operand2);
    // N flag if the results is negative
    CPSR[key_N] = ((resultSign & 0x0000000080000000LL) != 0);
    //Update flag Z if the result is 0
    CPSR[key_Z] = (resultSign == 0);
    //Update the C flag if a borrow didn't occurr in the operation
    //operand2 = (int)-operand2;
    CPSR[key_C] = (((operand1 ^ operand2 ^ ((unsigned int)(resultSign >> 1))) & 0x80000000) \
        == 0);
    //Update the V flag if an overflow occurred in the operation
    CPSR[key_V] = ((((unsigned int)(resultSign >> 1)) ^ ((unsigned int)resultSign)) & \
        0x80000000) != 0;
}

arm7tdmi_funclt_trap::Instruction::Instruction( Reg32_0 & CPSR, Reg32_1 & MP_ID, \
    Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, \
    Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem ) : CPSR(CPSR), MP_ID(MP_ID), RB(RB), SPSR(SPSR), \
    FP(FP), SPTR(SPTR), LINKR(LINKR), SP_IRQ(SP_IRQ), LR_IRQ(LR_IRQ), SP_FIQ(SP_FIQ), \
    LR_FIQ(LR_FIQ), PC(PC), REGS(REGS), instrMem(instrMem), dataMem(dataMem){
    this->totalInstrCycles = 0;
}

arm7tdmi_funclt_trap::Instruction::~Instruction(){

}
arm7tdmi_funclt_trap::DPI_shift_imm_op::~DPI_shift_imm_op(){

}
arm7tdmi_funclt_trap::DPI_shift_imm_op::DPI_shift_imm_op( Reg32_0 & CPSR, Reg32_1 \
    & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias \
    & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, \
    FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::UpdatePSRSumWithCarry_op::~UpdatePSRSumWithCarry_op(){

}
arm7tdmi_funclt_trap::UpdatePSRSumWithCarry_op::UpdatePSRSumWithCarry_op( Reg32_0 \
    & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, \
    Alias & LINKR, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
    & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem ) : Instruction(CPSR, \
    MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
    dataMem){

}

arm7tdmi_funclt_trap::UpdatePSRSub_op::~UpdatePSRSub_op(){

}
arm7tdmi_funclt_trap::UpdatePSRSub_op::UpdatePSRSub_op( Reg32_0 & CPSR, Reg32_1 & \
    MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias \
    & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, \
    FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::DPI_reg_shift_op::~DPI_reg_shift_op(){

}
arm7tdmi_funclt_trap::DPI_reg_shift_op::DPI_reg_shift_op( Reg32_0 & CPSR, Reg32_1 \
    & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias \
    & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, \
    FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::UpdatePSRSum_op::~UpdatePSRSum_op(){

}
arm7tdmi_funclt_trap::UpdatePSRSum_op::UpdatePSRSum_op( Reg32_0 & CPSR, Reg32_1 & \
    MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias \
    & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, \
    FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::UpdatePSRBit_op::~UpdatePSRBit_op(){

}
arm7tdmi_funclt_trap::UpdatePSRBit_op::UpdatePSRBit_op( Reg32_0 & CPSR, Reg32_1 & \
    MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias \
    & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, \
    FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::UpdatePSRSubWithCarry_op::~UpdatePSRSubWithCarry_op(){

}
arm7tdmi_funclt_trap::UpdatePSRSubWithCarry_op::UpdatePSRSubWithCarry_op( Reg32_0 \
    & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, \
    Alias & LINKR, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
    & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem ) : Instruction(CPSR, \
    MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
    dataMem){

}

arm7tdmi_funclt_trap::DPI_imm_op::~DPI_imm_op(){

}
arm7tdmi_funclt_trap::DPI_imm_op::DPI_imm_op( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 \
    * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::LS_imm_op::~LS_imm_op(){

}
arm7tdmi_funclt_trap::LS_imm_op::LS_imm_op( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 \
    * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::LSM_reglist_op::~LSM_reglist_op(){

}
arm7tdmi_funclt_trap::LSM_reglist_op::LSM_reglist_op( Reg32_0 & CPSR, Reg32_1 & MP_ID, \
    Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, \
    Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, \
    LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::LS_sh_op::~LS_sh_op(){

}
arm7tdmi_funclt_trap::LS_sh_op::LS_sh_op( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 \
    * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::LS_reg_op::~LS_reg_op(){

}
arm7tdmi_funclt_trap::LS_reg_op::LS_reg_op( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 \
    * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::UpdatePSRmul_64_op::~UpdatePSRmul_64_op(){

}
arm7tdmi_funclt_trap::UpdatePSRmul_64_op::UpdatePSRmul_64_op( Reg32_0 & CPSR, Reg32_1 \
    & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias \
    & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, \
    FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::UpdatePSRmul_op::~UpdatePSRmul_op(){

}
arm7tdmi_funclt_trap::UpdatePSRmul_op::UpdatePSRmul_op( Reg32_0 & CPSR, Reg32_1 & \
    MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias \
    & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, \
    TLMMemory & instrMem, TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, \
    FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

unsigned int arm7tdmi_funclt_trap::InvalidInstr::behavior(){
    THROW_EXCEPTION("Unknown Instruction at PC: " << std::hex << std::showbase << this->PC-4);
    return 0;
}

Instruction * arm7tdmi_funclt_trap::InvalidInstr::replicate() const throw(){
    return new InvalidInstr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

void arm7tdmi_funclt_trap::InvalidInstr::setParams( const unsigned int & bitString \
    ) throw(){

}

std::string arm7tdmi_funclt_trap::InvalidInstr::getInstructionName() const throw(){
    return "InvalidInstruction";
}

std::string arm7tdmi_funclt_trap::InvalidInstr::getMnemonic() const throw(){
    return "invalid";
}

unsigned int arm7tdmi_funclt_trap::InvalidInstr::getId() const throw(){
    return 75;
}

arm7tdmi_funclt_trap::InvalidInstr::InvalidInstr( Reg32_0 & CPSR, Reg32_1 & MP_ID, \
    Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, \
    Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, \
    LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::InvalidInstr::~InvalidInstr(){

}
unsigned int arm7tdmi_funclt_trap::TST_si::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_shift_imm(this->rm, this->rm_bit, this->shift_amm, this->shift_op, this->operand, \
        this->carry);

    result = rn & operand;
    UpdatePSRBitM(result, carry);
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::TST_si::replicate() const throw(){
    return new TST_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::TST_si::getInstructionName() const throw(){
    return "TST_si";
}

unsigned int arm7tdmi_funclt_trap::TST_si::getId() const throw(){
    return 64;
}

void arm7tdmi_funclt_trap::TST_si::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->shift_amm = (bitString & 0xf80) >> 7;
    this->shift_op = (bitString & 0x60) >> 5;
}

std::string arm7tdmi_funclt_trap::TST_si::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::TST_si::TST_si( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * \
    RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), DPI_shift_imm_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::TST_si::~TST_si(){

}
unsigned int arm7tdmi_funclt_trap::ADC_si::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_shift_imm(this->rm, this->rm_bit, this->shift_amm, this->shift_op, this->operand, \
        this->carry);

    operand1 = (int)rn;
    operand2 = (int)operand;

    rd = operand1 + operand2;

    if (CPSR[key_C]){
        rd = ((int)rd) + 1;
    }
    this->UpdatePSRSumWithCarry(this->rn, this->rn_bit, this->rd, this->rd_bit, this->s, \
        this->operand1, this->operand2, this->carry);


    if(rd_bit == 15){
        //In case the destination register is the program counter I have to
        //specify that I have a latency of two clock cycles
        stall(2);
        flush();
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::ADC_si::replicate() const throw(){
    return new ADC_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::ADC_si::getInstructionName() const throw(){
    return "ADC_si";
}

unsigned int arm7tdmi_funclt_trap::ADC_si::getId() const throw(){
    return 0;
}

void arm7tdmi_funclt_trap::ADC_si::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->shift_amm = (bitString & 0xf80) >> 7;
    this->shift_op = (bitString & 0x60) >> 5;
}

std::string arm7tdmi_funclt_trap::ADC_si::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::ADC_si::ADC_si( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * \
    RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRSumWithCarry_op(CPSR, \
    MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
    dataMem), DPI_shift_imm_op(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, \
    SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::ADC_si::~ADC_si(){

}
unsigned int arm7tdmi_funclt_trap::SUB_sr::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_reg_shift(this->rm, this->rm_bit, this->rs, this->rs_bit, this->shift_op, \
        this->operand, this->carry);

    operand1 = (int)rn;
    operand2 = (int)operand;
    rd =  operand1 - operand2;
    this->UpdatePSRSub(this->rn, this->rn_bit, this->rd, this->rd_bit, this->s, this->operand1, \
        this->operand2, this->carry);


    if(rd_bit == 15){
        //In case the destination register is the program counter I have to
        //specify that I have a latency of two clock cycles
        stall(2);
        flush();
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::SUB_sr::replicate() const throw(){
    return new SUB_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::SUB_sr::getInstructionName() const throw(){
    return "SUB_sr";
}

unsigned int arm7tdmi_funclt_trap::SUB_sr::getId() const throw(){
    return 59;
}

void arm7tdmi_funclt_trap::SUB_sr::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->rs_bit = (bitString & 0xf00) >> 8;
    this->rs.directSetAlias(this->REGS[this->rs_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->shift_op = (bitString & 0x60) >> 5;
}

std::string arm7tdmi_funclt_trap::SUB_sr::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::SUB_sr::SUB_sr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * \
    RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRSub_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), \
    DPI_reg_shift_op(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
    LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::SUB_sr::~SUB_sr(){

}
unsigned int arm7tdmi_funclt_trap::ADD_si::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_shift_imm(this->rm, this->rm_bit, this->shift_amm, this->shift_op, this->operand, \
        this->carry);

    operand1 = (int)rn;
    operand2 = (int)operand;
    rd = operand1 + operand2;
    this->UpdatePSRSum(this->rn, this->rn_bit, this->rd, this->rd_bit, this->s, this->operand1, \
        this->operand2, this->carry);


    if(rd_bit == 15){
        //In case the destination register is the program counter I have to
        //specify that I have a latency of two clock cycles
        stall(2);
        flush();
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::ADD_si::replicate() const throw(){
    return new ADD_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::ADD_si::getInstructionName() const throw(){
    return "ADD_si";
}

unsigned int arm7tdmi_funclt_trap::ADD_si::getId() const throw(){
    return 3;
}

void arm7tdmi_funclt_trap::ADD_si::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->shift_amm = (bitString & 0xf80) >> 7;
    this->shift_op = (bitString & 0x60) >> 5;
}

std::string arm7tdmi_funclt_trap::ADD_si::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::ADD_si::ADD_si( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * \
    RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRSum_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), \
    DPI_shift_imm_op(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
    LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::ADD_si::~ADD_si(){

}
unsigned int arm7tdmi_funclt_trap::BIC_si::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_shift_imm(this->rm, this->rm_bit, this->shift_amm, this->shift_op, this->operand, \
        this->carry);

    result = rn & ~operand;
    rd = result;
    this->UpdatePSRBit(this->rn, this->rn_bit, this->rd, this->rd_bit, this->s, this->carry, \
        this->result);


    if(rd_bit == 15){
        //In case the destination register is the program counter I have to
        //specify that I have a latency of two clock cycles
        stall(2);
        flush();
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::BIC_si::replicate() const throw(){
    return new BIC_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::BIC_si::getInstructionName() const throw(){
    return "BIC_si";
}

unsigned int arm7tdmi_funclt_trap::BIC_si::getId() const throw(){
    return 11;
}

void arm7tdmi_funclt_trap::BIC_si::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->shift_amm = (bitString & 0xf80) >> 7;
    this->shift_op = (bitString & 0x60) >> 5;
}

std::string arm7tdmi_funclt_trap::BIC_si::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::BIC_si::BIC_si( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * \
    RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRBit_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), \
    DPI_shift_imm_op(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
    LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::BIC_si::~BIC_si(){

}
unsigned int arm7tdmi_funclt_trap::SBC_sr::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_reg_shift(this->rm, this->rm_bit, this->rs, this->rs_bit, this->shift_op, \
        this->operand, this->carry);

    operand1 = (int)rn;
    operand2 = (int)operand;
    rd =  operand1 - operand2;
    if (CPSR[key_C] == 0){
        rd = ((int)rd) -1;
    }
    this->UpdatePSRSubWithCarry(this->rn, this->rn_bit, this->rd, this->rd_bit, this->s, \
        this->operand1, this->operand2, this->carry);


    if(rd_bit == 15){
        //In case the destination register is the program counter I have to
        //specify that I have a latency of two clock cycles
        stall(2);
        flush();
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::SBC_sr::replicate() const throw(){
    return new SBC_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::SBC_sr::getInstructionName() const throw(){
    return "SBC_sr";
}

unsigned int arm7tdmi_funclt_trap::SBC_sr::getId() const throw(){
    return 56;
}

void arm7tdmi_funclt_trap::SBC_sr::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->rs_bit = (bitString & 0xf00) >> 8;
    this->rs.directSetAlias(this->REGS[this->rs_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->shift_op = (bitString & 0x60) >> 5;
}

std::string arm7tdmi_funclt_trap::SBC_sr::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::SBC_sr::SBC_sr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * \
    RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRSubWithCarry_op(CPSR, \
    MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
    dataMem), DPI_reg_shift_op(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, \
    SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::SBC_sr::~SBC_sr(){

}
unsigned int arm7tdmi_funclt_trap::CMP_i::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_imm(this->rotate, this->immediate, this->operand, this->carry);

    carry = 0;
    UpdatePSRSubInner(rn, operand, 0);
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::CMP_i::replicate() const throw(){
    return new CMP_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::CMP_i::getInstructionName() const throw(){
    return "CMP_i";
}

unsigned int arm7tdmi_funclt_trap::CMP_i::getId() const throw(){
    return 19;
}

void arm7tdmi_funclt_trap::CMP_i::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->rotate = (bitString & 0xf00) >> 8;
    this->immediate = (bitString & 0xff);
}

std::string arm7tdmi_funclt_trap::CMP_i::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::CMP_i::CMP_i( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, \
    Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias & \
    LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), DPI_imm_op(CPSR, MP_ID, RB, \
    SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::CMP_i::~CMP_i(){

}
unsigned int arm7tdmi_funclt_trap::BRANCHX::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);

    // Note how the T bit is not considered since we do not bother with
    // thumb mode
    PC = (rm & 0xFFFFFFFC);
    stall(2);
    flush();
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::BRANCHX::replicate() const throw(){
    return new BRANCHX(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::BRANCHX::getInstructionName() const throw(){
    return "BRANCHX";
}

unsigned int arm7tdmi_funclt_trap::BRANCHX::getId() const throw(){
    return 10;
}

void arm7tdmi_funclt_trap::BRANCHX::setParams( const unsigned int & bitString ) throw(){
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
}

std::string arm7tdmi_funclt_trap::BRANCHX::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::BRANCHX::BRANCHX( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 \
    * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::BRANCHX::~BRANCHX(){

}
unsigned int arm7tdmi_funclt_trap::BIC_sr::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_reg_shift(this->rm, this->rm_bit, this->rs, this->rs_bit, this->shift_op, \
        this->operand, this->carry);

    result = rn & ~operand;
    rd = result;
    this->UpdatePSRBit(this->rn, this->rn_bit, this->rd, this->rd_bit, this->s, this->carry, \
        this->result);


    if(rd_bit == 15){
        //In case the destination register is the program counter I have to
        //specify that I have a latency of two clock cycles
        stall(2);
        flush();
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::BIC_sr::replicate() const throw(){
    return new BIC_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::BIC_sr::getInstructionName() const throw(){
    return "BIC_sr";
}

unsigned int arm7tdmi_funclt_trap::BIC_sr::getId() const throw(){
    return 12;
}

void arm7tdmi_funclt_trap::BIC_sr::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->rs_bit = (bitString & 0xf00) >> 8;
    this->rs.directSetAlias(this->REGS[this->rs_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->shift_op = (bitString & 0x60) >> 5;
}

std::string arm7tdmi_funclt_trap::BIC_sr::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::BIC_sr::BIC_sr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * \
    RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRBit_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), \
    DPI_reg_shift_op(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
    LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::BIC_sr::~BIC_sr(){

}
unsigned int arm7tdmi_funclt_trap::ADD_sr::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_reg_shift(this->rm, this->rm_bit, this->rs, this->rs_bit, this->shift_op, \
        this->operand, this->carry);

    operand1 = (int)rn;
    operand2 = (int)operand;
    rd = operand1 + operand2;
    this->UpdatePSRSum(this->rn, this->rn_bit, this->rd, this->rd_bit, this->s, this->operand1, \
        this->operand2, this->carry);


    if(rd_bit == 15){
        //In case the destination register is the program counter I have to
        //specify that I have a latency of two clock cycles
        stall(2);
        flush();
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::ADD_sr::replicate() const throw(){
    return new ADD_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::ADD_sr::getInstructionName() const throw(){
    return "ADD_sr";
}

unsigned int arm7tdmi_funclt_trap::ADD_sr::getId() const throw(){
    return 4;
}

void arm7tdmi_funclt_trap::ADD_sr::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->rs_bit = (bitString & 0xf00) >> 8;
    this->rs.directSetAlias(this->REGS[this->rs_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->shift_op = (bitString & 0x60) >> 5;
}

std::string arm7tdmi_funclt_trap::ADD_sr::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::ADD_sr::ADD_sr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * \
    RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRSum_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), \
    DPI_reg_shift_op(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
    LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::ADD_sr::~ADD_sr(){

}
unsigned int arm7tdmi_funclt_trap::MOV_i::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_imm(this->rotate, this->immediate, this->operand, this->carry);

    rd = operand;
    result = operand;
    this->UpdatePSRBit(this->rn, this->rn_bit, this->rd, this->rd_bit, this->s, this->carry, \
        this->result);


    if(rd_bit == 15){
        //In case the destination register is the program counter I have to
        //specify that I have a latency of two clock cycles
        stall(2);
        flush();
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::MOV_i::replicate() const throw(){
    return new MOV_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::MOV_i::getInstructionName() const throw(){
    return "MOV_i";
}

unsigned int arm7tdmi_funclt_trap::MOV_i::getId() const throw(){
    return 39;
}

void arm7tdmi_funclt_trap::MOV_i::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->rotate = (bitString & 0xf00) >> 8;
    this->immediate = (bitString & 0xff);
}

std::string arm7tdmi_funclt_trap::MOV_i::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::MOV_i::MOV_i( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, \
    Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias & \
    LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRBit_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), \
    DPI_imm_op(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, \
    PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::MOV_i::~MOV_i(){

}
unsigned int arm7tdmi_funclt_trap::SWAPB::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);

    temp = dataMem.read_byte(rn);
    dataMem.write_byte(rn, (unsigned char)(rm & 0x000000FF));
    rd = temp;
    stall(3);
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::SWAPB::replicate() const throw(){
    return new SWAPB(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::SWAPB::getInstructionName() const throw(){
    return "SWAPB";
}

unsigned int arm7tdmi_funclt_trap::SWAPB::getId() const throw(){
    return 74;
}

void arm7tdmi_funclt_trap::SWAPB::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
}

std::string arm7tdmi_funclt_trap::SWAPB::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::SWAPB::SWAPB( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, \
    Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias & \
    LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::SWAPB::~SWAPB(){

}
unsigned int arm7tdmi_funclt_trap::MVN_si::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_shift_imm(this->rm, this->rm_bit, this->shift_amm, this->shift_op, this->operand, \
        this->carry);

    result = ~operand;
    rd = result;
    this->UpdatePSRBit(this->rn, this->rn_bit, this->rd, this->rd_bit, this->s, this->carry, \
        this->result);


    if(rd_bit == 15){
        //In case the destination register is the program counter I have to
        //specify that I have a latency of two clock cycles
        stall(2);
        flush();
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::MVN_si::replicate() const throw(){
    return new MVN_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::MVN_si::getInstructionName() const throw(){
    return "MVN_si";
}

unsigned int arm7tdmi_funclt_trap::MVN_si::getId() const throw(){
    return 43;
}

void arm7tdmi_funclt_trap::MVN_si::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->shift_amm = (bitString & 0xf80) >> 7;
    this->shift_op = (bitString & 0x60) >> 5;
}

std::string arm7tdmi_funclt_trap::MVN_si::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::MVN_si::MVN_si( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * \
    RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRBit_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), \
    DPI_shift_imm_op(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
    LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::MVN_si::~MVN_si(){

}
unsigned int arm7tdmi_funclt_trap::EOR_i::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_imm(this->rotate, this->immediate, this->operand, this->carry);

    result = rn ^ operand;
    rd = result;
    this->UpdatePSRBit(this->rn, this->rn_bit, this->rd, this->rd_bit, this->s, this->carry, \
        this->result);


    if(rd_bit == 15){
        //In case the destination register is the program counter I have to
        //specify that I have a latency of two clock cycles
        stall(2);
        flush();
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::EOR_i::replicate() const throw(){
    return new EOR_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::EOR_i::getInstructionName() const throw(){
    return "EOR_i";
}

unsigned int arm7tdmi_funclt_trap::EOR_i::getId() const throw(){
    return 22;
}

void arm7tdmi_funclt_trap::EOR_i::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->rotate = (bitString & 0xf00) >> 8;
    this->immediate = (bitString & 0xff);
}

std::string arm7tdmi_funclt_trap::EOR_i::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::EOR_i::EOR_i( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, \
    Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias & \
    LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRBit_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), \
    DPI_imm_op(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, \
    PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::EOR_i::~EOR_i(){

}
unsigned int arm7tdmi_funclt_trap::ADC_sr::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_reg_shift(this->rm, this->rm_bit, this->rs, this->rs_bit, this->shift_op, \
        this->operand, this->carry);

    operand1 = (int)rn;
    operand2 = (int)operand;

    rd = operand1 + operand2;

    if (CPSR[key_C]){
        rd = ((int)rd) + 1;
    }
    this->UpdatePSRSumWithCarry(this->rn, this->rn_bit, this->rd, this->rd_bit, this->s, \
        this->operand1, this->operand2, this->carry);


    if(rd_bit == 15){
        //In case the destination register is the program counter I have to
        //specify that I have a latency of two clock cycles
        stall(2);
        flush();
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::ADC_sr::replicate() const throw(){
    return new ADC_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::ADC_sr::getInstructionName() const throw(){
    return "ADC_sr";
}

unsigned int arm7tdmi_funclt_trap::ADC_sr::getId() const throw(){
    return 1;
}

void arm7tdmi_funclt_trap::ADC_sr::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->rs_bit = (bitString & 0xf00) >> 8;
    this->rs.directSetAlias(this->REGS[this->rs_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->shift_op = (bitString & 0x60) >> 5;
}

std::string arm7tdmi_funclt_trap::ADC_sr::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::ADC_sr::ADC_sr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * \
    RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRSumWithCarry_op(CPSR, \
    MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
    dataMem), DPI_reg_shift_op(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, \
    SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::ADC_sr::~ADC_sr(){

}
unsigned int arm7tdmi_funclt_trap::TST_sr::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_reg_shift(this->rm, this->rm_bit, this->rs, this->rs_bit, this->shift_op, \
        this->operand, this->carry);

    result = rn & operand;
    UpdatePSRBitM(result, carry);
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::TST_sr::replicate() const throw(){
    return new TST_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::TST_sr::getInstructionName() const throw(){
    return "TST_sr";
}

unsigned int arm7tdmi_funclt_trap::TST_sr::getId() const throw(){
    return 65;
}

void arm7tdmi_funclt_trap::TST_sr::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->rs_bit = (bitString & 0xf00) >> 8;
    this->rs.directSetAlias(this->REGS[this->rs_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->shift_op = (bitString & 0x60) >> 5;
}

std::string arm7tdmi_funclt_trap::TST_sr::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::TST_sr::TST_sr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * \
    RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), DPI_reg_shift_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::TST_sr::~TST_sr(){

}
unsigned int arm7tdmi_funclt_trap::ORR_i::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_imm(this->rotate, this->immediate, this->operand, this->carry);

    result = rn | operand;
    rd = result;
    this->UpdatePSRBit(this->rn, this->rn_bit, this->rd, this->rd_bit, this->s, this->carry, \
        this->result);


    if(rd_bit == 15){
        //In case the destination register is the program counter I have to
        //specify that I have a latency of two clock cycles
        stall(2);
        flush();
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::ORR_i::replicate() const throw(){
    return new ORR_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::ORR_i::getInstructionName() const throw(){
    return "ORR_i";
}

unsigned int arm7tdmi_funclt_trap::ORR_i::getId() const throw(){
    return 48;
}

void arm7tdmi_funclt_trap::ORR_i::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->rotate = (bitString & 0xf00) >> 8;
    this->immediate = (bitString & 0xff);
}

std::string arm7tdmi_funclt_trap::ORR_i::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::ORR_i::ORR_i( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, \
    Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias & \
    LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRBit_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), \
    DPI_imm_op(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, \
    PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::ORR_i::~ORR_i(){

}
unsigned int arm7tdmi_funclt_trap::LDR_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->LS_imm(this->rn, this->rn_bit, this->p, this->w, this->u, this->immediate, \
        this->address);

    memLastBits = address & 0x00000003;
    // if the memory address is not word aligned I have to rotate the loaded value
    if(memLastBits == 0){
        value = dataMem.read_word(address);
    }
    else{
        value = RotateRight(8*memLastBits, dataMem.read_word(address));
    }

    //Perform the writeback; as usual I have to behave differently
    //if a load a value to the PC
    if(rd_bit == 15){
        //I don't consider the 2 less significant bits since I don't bother with
        //thumb mode.
        PC = value & 0xFFFFFFFC;
        stall(4);
        flush();
    }
    else{
        rd = value;
        stall(2);
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::LDR_imm::replicate() const throw(){
    return new LDR_imm(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::LDR_imm::getInstructionName() const throw(){
    return "LDR_imm";
}

unsigned int arm7tdmi_funclt_trap::LDR_imm::getId() const throw(){
    return 24;
}

void arm7tdmi_funclt_trap::LDR_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->p = (bitString & 0x1000000) >> 24;
    this->u = (bitString & 0x800000) >> 23;
    this->w = (bitString & 0x200000) >> 21;
    this->immediate = (bitString & 0xfff);
}

std::string arm7tdmi_funclt_trap::LDR_imm::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::LDR_imm::LDR_imm( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 \
    * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), LS_imm_op(CPSR, MP_ID, RB, \
    SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::LDR_imm::~LDR_imm(){

}
unsigned int arm7tdmi_funclt_trap::ORR_si::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_shift_imm(this->rm, this->rm_bit, this->shift_amm, this->shift_op, this->operand, \
        this->carry);

    result = rn | operand;
    rd = result;
    this->UpdatePSRBit(this->rn, this->rn_bit, this->rd, this->rd_bit, this->s, this->carry, \
        this->result);


    if(rd_bit == 15){
        //In case the destination register is the program counter I have to
        //specify that I have a latency of two clock cycles
        stall(2);
        flush();
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::ORR_si::replicate() const throw(){
    return new ORR_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::ORR_si::getInstructionName() const throw(){
    return "ORR_si";
}

unsigned int arm7tdmi_funclt_trap::ORR_si::getId() const throw(){
    return 46;
}

void arm7tdmi_funclt_trap::ORR_si::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->shift_amm = (bitString & 0xf80) >> 7;
    this->shift_op = (bitString & 0x60) >> 5;
}

std::string arm7tdmi_funclt_trap::ORR_si::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::ORR_si::ORR_si( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * \
    RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRBit_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), \
    DPI_shift_imm_op(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
    LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::ORR_si::~ORR_si(){

}
unsigned int arm7tdmi_funclt_trap::LDRB_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->LS_imm(this->rn, this->rn_bit, this->p, this->w, this->u, this->immediate, \
        this->address);

    rd = dataMem.read_byte(address);
    stall(2);
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::LDRB_imm::replicate() const throw(){
    return new LDRB_imm(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::LDRB_imm::getInstructionName() const throw(){
    return "LDRB_imm";
}

unsigned int arm7tdmi_funclt_trap::LDRB_imm::getId() const throw(){
    return 26;
}

void arm7tdmi_funclt_trap::LDRB_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->p = (bitString & 0x1000000) >> 24;
    this->u = (bitString & 0x800000) >> 23;
    this->w = (bitString & 0x200000) >> 21;
    this->immediate = (bitString & 0xfff);
}

std::string arm7tdmi_funclt_trap::LDRB_imm::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::LDRB_imm::LDRB_imm( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 \
    * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), LS_imm_op(CPSR, MP_ID, RB, \
    SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::LDRB_imm::~LDRB_imm(){

}
unsigned int arm7tdmi_funclt_trap::msr_reg_Instr::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);

    //Checking for unvalid bits
    if((rm & 0x00000010) == 0){
        THROW_EXCEPTION("MSR called with unvalid mode " << std::hex << std::showbase << rm \
            << ": we are trying to switch to 26 bit PC");
    }
    unsigned int currentMode = CPSR[key_mode];
    //Firs of all I check whether I have to modify the CPSR or the SPSR
    if(r == 0){
        //CPSR
        //Now I modify the fields; note that in user mode I can just update the flags.
        if((mask & 0x1) != 0 && currentMode != 0){
            CPSR &= 0xFFFFFF00;
            CPSR |= rm & 0x000000FF;
            //Now if I change mode I also have to update the registry bank
            if(currentMode != (CPSR & 0x0000000F)){
                restoreSPSR();
            }
        }
        if((mask & 0x2) != 0 && currentMode != 0){
            CPSR &= 0xFFFF00FF;
            CPSR |= rm & 0x0000FF00;
        }
        if((mask & 0x4) != 0 && currentMode != 0){
            CPSR &= 0xFF00FFFF;
            CPSR |= rm & 0x00FF0000;
        }
        if((mask & 0x8) != 0){
            CPSR &= 0x00FFFFFF;
            CPSR |= rm & 0xFF000000;
        }
    }
    else{
        //SPSR
        switch(currentMode){
            case 0x1:{
                //I'm in FIQ mode
                if((mask & 0x1) != 0){
                    SPSR[0] &= 0xFFFFFF00;
                    SPSR[0] |= rm & 0x000000FF;
                }
                if((mask & 0x2) != 0){
                    SPSR[0] &= 0xFFFF00FF;
                    SPSR[0] |= rm & 0x0000FF00;
                }
                if((mask & 0x4) != 0){
                    SPSR[0] &= 0xFF00FFFF;
                    SPSR[0] |= rm & 0x00FF0000;
                }
                if((mask & 0x8) != 0){
                    SPSR[0] &= 0x00FFFFFF;
                    SPSR[0] |= rm & 0xFF000000;
                }
            break;}
            case 0x2:{
                //I'm in IRQ mode
                if((mask & 0x1) != 0){
                    SPSR[1] &= 0xFFFFFF00;
                    SPSR[1] |= rm & 0x000000FF;
                }
                if((mask & 0x2) != 0){
                    SPSR[1] &= 0xFFFF00FF;
                    SPSR[1] |= rm & 0x0000FF00;
                }
                if((mask & 0x4) != 0){
                    SPSR[1] &= 0xFF00FFFF;
                    SPSR[1] |= rm & 0x00FF0000;
                }
                if((mask & 0x8) != 0){
                    SPSR[1] &= 0x00FFFFFF;
                    SPSR[1] |= rm & 0xFF000000;
                }
            break;}
            case 0x3:{
                //I'm in SVC mode
                if((mask & 0x1) != 0){
                    SPSR[2] &= 0xFFFFFF00;
                    SPSR[2] |= rm & 0x000000FF;
                }
                if((mask & 0x2) != 0){
                    SPSR[2] &= 0xFFFF00FF;
                    SPSR[2] |= rm & 0x0000FF00;
                }
                if((mask & 0x4) != 0){
                    SPSR[2] &= 0xFF00FFFF;
                    SPSR[2] |= rm & 0x00FF0000;
                }
                if((mask & 0x8) != 0){
                    SPSR[2] &= 0x00FFFFFF;
                    SPSR[2] |= rm & 0xFF000000;
                }
            break;}
            case 0x7:{
                //I'm in ABT mode
                if((mask & 0x1) != 0){
                    SPSR[3] &= 0xFFFFFF00;
                    SPSR[3] |= rm & 0x000000FF;
                }
                if((mask & 0x2) != 0){
                    SPSR[3] &= 0xFFFF00FF;
                    SPSR[3] |= rm & 0x0000FF00;
                }
                if((mask & 0x4) != 0){
                    SPSR[3] &= 0xFF00FFFF;
                    SPSR[3] |= rm & 0x00FF0000;
                }
                if((mask & 0x8) != 0){
                    SPSR[3] &= 0x00FFFFFF;
                    SPSR[3] |= rm & 0xFF000000;
                }
            break;}
            case 0xB:{
                //I'm in UND mode
                if((mask & 0x1) != 0){
                    SPSR[4] &= 0xFFFFFF00;
                    SPSR[4] |= rm & 0x000000FF;
                }
                if((mask & 0x2) != 0){
                    SPSR[4] &= 0xFFFF00FF;
                    SPSR[4] |= rm & 0x0000FF00;
                }
                if((mask & 0x4) != 0){
                    SPSR[4] &= 0xFF00FFFF;
                    SPSR[4] |= rm & 0x00FF0000;
                }
                if((mask & 0x8) != 0){
                    SPSR[4] &= 0x00FFFFFF;
                    SPSR[4] |= rm & 0xFF000000;
                }
            break;}
            default:
            break;
        }
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::msr_reg_Instr::replicate() const throw(){
    return new msr_reg_Instr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, \
        SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::msr_reg_Instr::getInstructionName() const throw(){
    return "msr_reg_Instr";
}

unsigned int arm7tdmi_funclt_trap::msr_reg_Instr::getId() const throw(){
    return 42;
}

void arm7tdmi_funclt_trap::msr_reg_Instr::setParams( const unsigned int & bitString \
    ) throw(){
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->r = (bitString & 0x400000) >> 22;
    this->mask = (bitString & 0xf0000) >> 16;
}

std::string arm7tdmi_funclt_trap::msr_reg_Instr::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::msr_reg_Instr::msr_reg_Instr( Reg32_0 & CPSR, Reg32_1 & MP_ID, \
    Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, \
    Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, \
    LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::msr_reg_Instr::~msr_reg_Instr(){

}
unsigned int arm7tdmi_funclt_trap::CMN_i::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_imm(this->rotate, this->immediate, this->operand, this->carry);

    //carry  = 0
    UpdatePSRAddInner(rn, operand, 0);
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::CMN_i::replicate() const throw(){
    return new CMN_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::CMN_i::getInstructionName() const throw(){
    return "CMN_i";
}

unsigned int arm7tdmi_funclt_trap::CMN_i::getId() const throw(){
    return 16;
}

void arm7tdmi_funclt_trap::CMN_i::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->rotate = (bitString & 0xf00) >> 8;
    this->immediate = (bitString & 0xff);
}

std::string arm7tdmi_funclt_trap::CMN_i::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::CMN_i::CMN_i( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, \
    Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias & \
    LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), DPI_imm_op(CPSR, MP_ID, RB, \
    SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::CMN_i::~CMN_i(){

}
unsigned int arm7tdmi_funclt_trap::ADD_i::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_imm(this->rotate, this->immediate, this->operand, this->carry);

    operand1 = (int)rn;
    operand2 = (int)operand;
    rd = operand1 + operand2;
    this->UpdatePSRSum(this->rn, this->rn_bit, this->rd, this->rd_bit, this->s, this->operand1, \
        this->operand2, this->carry);


    if(rd_bit == 15){
        //In case the destination register is the program counter I have to
        //specify that I have a latency of two clock cycles
        stall(2);
        flush();
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::ADD_i::replicate() const throw(){
    return new ADD_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::ADD_i::getInstructionName() const throw(){
    return "ADD_i";
}

unsigned int arm7tdmi_funclt_trap::ADD_i::getId() const throw(){
    return 5;
}

void arm7tdmi_funclt_trap::ADD_i::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->rotate = (bitString & 0xf00) >> 8;
    this->immediate = (bitString & 0xff);
}

std::string arm7tdmi_funclt_trap::ADD_i::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::ADD_i::ADD_i( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, \
    Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias & \
    LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRSum_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), \
    DPI_imm_op(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, \
    PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::ADD_i::~ADD_i(){

}
unsigned int arm7tdmi_funclt_trap::AND_si::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_shift_imm(this->rm, this->rm_bit, this->shift_amm, this->shift_op, this->operand, \
        this->carry);

    result = rn & operand;
    rd = result;
    this->UpdatePSRBit(this->rn, this->rn_bit, this->rd, this->rd_bit, this->s, this->carry, \
        this->result);


    if(rd_bit == 15){
        //In case the destination register is the program counter I have to
        //specify that I have a latency of two clock cycles
        stall(2);
        flush();
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::AND_si::replicate() const throw(){
    return new AND_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::AND_si::getInstructionName() const throw(){
    return "AND_si";
}

unsigned int arm7tdmi_funclt_trap::AND_si::getId() const throw(){
    return 6;
}

void arm7tdmi_funclt_trap::AND_si::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->shift_amm = (bitString & 0xf80) >> 7;
    this->shift_op = (bitString & 0x60) >> 5;
}

std::string arm7tdmi_funclt_trap::AND_si::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::AND_si::AND_si( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * \
    RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRBit_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), \
    DPI_shift_imm_op(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
    LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::AND_si::~AND_si(){

}
unsigned int arm7tdmi_funclt_trap::CMN_si::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_shift_imm(this->rm, this->rm_bit, this->shift_amm, this->shift_op, this->operand, \
        this->carry);

    //carry  = 0
    UpdatePSRAddInner(rn, operand, 0);
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::CMN_si::replicate() const throw(){
    return new CMN_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::CMN_si::getInstructionName() const throw(){
    return "CMN_si";
}

unsigned int arm7tdmi_funclt_trap::CMN_si::getId() const throw(){
    return 14;
}

void arm7tdmi_funclt_trap::CMN_si::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->shift_amm = (bitString & 0xf80) >> 7;
    this->shift_op = (bitString & 0x60) >> 5;
}

std::string arm7tdmi_funclt_trap::CMN_si::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::CMN_si::CMN_si( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * \
    RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), DPI_shift_imm_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::CMN_si::~CMN_si(){

}
unsigned int arm7tdmi_funclt_trap::LDM::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    #ifdef ACC_MODEL
    for(int i = 0; i < 16; i++){
        if((reg_list & (0x00000001 << i)) != 0){
            RB[i].lock();
        }
    }
    #endif
    this->condition_check(this->cond);
    this->LSM_reglist(this->rn, this->rn_bit, this->p, this->u, this->reg_list, this->start_address, \
        this->wb_address);

    unsigned int numRegsToLoad = 0;
    unsigned int loadLatency = 0;
    if(s == 0){
        //I'm dealing just with the current registers: LDM type one or three
        //First af all I read the memory in the register I in the register list.
        for(int i = 0; i < 15; i++){
            if((reg_list & (0x00000001 << i)) != 0){
                REGS[i] = dataMem.read_word(start_address);
                start_address += 4;
                numRegsToLoad++;
            }
        }
        loadLatency = numRegsToLoad + 1;

        //I tread in a special way the PC, since loading a value
        //in the PC is like performing a branch.
        if((reg_list & 0x00008000) != 0){
            //I have to load also the PC: it is like a branch; since I don't bother with
            //Thumb mode, bits 0 and 1 of the PC are ignored
            PC = dataMem.read_word(start_address) & 0xFFFFFFFC;
            numRegsToLoad++;
            loadLatency += 2;
            flush();
        }
        // First of all if it is necessary I perform the writeback
        if(w != 0){
            rn = wb_address;
        }
    }
    else if((reg_list & 0x00008000) != 0){
        //I'm dealing just with the current registers: LDM type one or three
        //First af all I read the memory in the register I in the register list.
        for(int i = 0; i < 15; i++){
            if((reg_list & (0x00000001 << i)) != 0){
                REGS[i] = dataMem.read_word(start_address);
                start_address += 4;
                numRegsToLoad++;
            }
        }
        loadLatency = numRegsToLoad + 1;

        //I tread in a special way the PC, since loading a value
        //in the PC is like performing a branch.
        //I have to load also the PC: it is like a branch; since I don't bother with
        //Thumb mode, bits 0 and 1 of the PC are ignored
        PC = dataMem.read_word(start_address) & 0xFFFFFFFC;
        //LDM type three: in this type of operation I also have to restore the PSR.
        restoreSPSR();
        numRegsToLoad++;
        loadLatency += 2;
        flush();
    }
    else{
        //I'm dealing with user-mode registers: LDM type two
        //Load the registers common to all modes
        for(int i = 0; i < 16; i++){
            if((reg_list & (0x00000001 << i)) != 0){
                RB[i] = dataMem.read_word(start_address);
                start_address += 4;
                numRegsToLoad++;
            }
        }
        loadLatency = numRegsToLoad + 1;
    }
    stall(loadLatency);
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::LDM::replicate() const throw(){
    return new LDM(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, \
        PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::LDM::getInstructionName() const throw(){
    return "LDM";
}

unsigned int arm7tdmi_funclt_trap::LDM::getId() const throw(){
    return 23;
}

void arm7tdmi_funclt_trap::LDM::setParams( const unsigned int & bitString ) throw(){
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->p = (bitString & 0x1000000) >> 24;
    this->u = (bitString & 0x800000) >> 23;
    this->s = (bitString & 0x400000) >> 22;
    this->w = (bitString & 0x200000) >> 21;
    this->reg_list = (bitString & 0xffff);
}

std::string arm7tdmi_funclt_trap::LDM::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::LDM::LDM( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 \
    * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias & LR_IRQ, \
    Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory \
    & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, \
    SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), LSM_reglist_op(CPSR, MP_ID, RB, SPSR, \
    FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::LDM::~LDM(){

}
unsigned int arm7tdmi_funclt_trap::LDRSH_off::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->LS_sh(this->rn, this->rn_bit, this->p, this->w, this->u, this->i, this->addr_mode0, \
        this->addr_mode1, this->address);

    rd = (int)SignExtend(dataMem.read_half(address), 16) ;
    stall(2);
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::LDRSH_off::replicate() const throw(){
    return new LDRSH_off(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::LDRSH_off::getInstructionName() const throw(){
    return "LDRSH_off";
}

unsigned int arm7tdmi_funclt_trap::LDRSH_off::getId() const throw(){
    return 29;
}

void arm7tdmi_funclt_trap::LDRSH_off::setParams( const unsigned int & bitString ) \
    throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->p = (bitString & 0x1000000) >> 24;
    this->u = (bitString & 0x800000) >> 23;
    this->i = (bitString & 0x400000) >> 22;
    this->w = (bitString & 0x200000) >> 21;
    this->l = (bitString & 0x100000) >> 20;
    this->addr_mode0 = (bitString & 0xf00) >> 8;
    this->addr_mode1 = (bitString & 0xf);
}

std::string arm7tdmi_funclt_trap::LDRSH_off::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::LDRSH_off::LDRSH_off( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 \
    * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), LS_sh_op(CPSR, MP_ID, RB, SPSR, \
    FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::LDRSH_off::~LDRSH_off(){

}
unsigned int arm7tdmi_funclt_trap::STR_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->LS_imm(this->rn, this->rn_bit, this->p, this->w, this->u, this->immediate, \
        this->address);

    dataMem.write_word(address, rd);
    stall(1);
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::STR_imm::replicate() const throw(){
    return new STR_imm(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::STR_imm::getInstructionName() const throw(){
    return "STR_imm";
}

unsigned int arm7tdmi_funclt_trap::STR_imm::getId() const throw(){
    return 68;
}

void arm7tdmi_funclt_trap::STR_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->p = (bitString & 0x1000000) >> 24;
    this->u = (bitString & 0x800000) >> 23;
    this->w = (bitString & 0x200000) >> 21;
    this->immediate = (bitString & 0xfff);
}

std::string arm7tdmi_funclt_trap::STR_imm::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::STR_imm::STR_imm( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 \
    * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), LS_imm_op(CPSR, MP_ID, RB, \
    SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::STR_imm::~STR_imm(){

}
unsigned int arm7tdmi_funclt_trap::RSB_si::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_shift_imm(this->rm, this->rm_bit, this->shift_amm, this->shift_op, this->operand, \
        this->carry);

    operand1 = (int)operand;
    operand2 = (int)rn;
    rd = operand1 - operand2;
    this->UpdatePSRSub(this->rn, this->rn_bit, this->rd, this->rd_bit, this->s, this->operand1, \
        this->operand2, this->carry);


    if(rd_bit == 15){
        //In case the destination register is the program counter I have to
        //specify that I have a latency of two clock cycles
        stall(2);
        flush();
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::RSB_si::replicate() const throw(){
    return new RSB_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::RSB_si::getInstructionName() const throw(){
    return "RSB_si";
}

unsigned int arm7tdmi_funclt_trap::RSB_si::getId() const throw(){
    return 49;
}

void arm7tdmi_funclt_trap::RSB_si::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->shift_amm = (bitString & 0xf80) >> 7;
    this->shift_op = (bitString & 0x60) >> 5;
}

std::string arm7tdmi_funclt_trap::RSB_si::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::RSB_si::RSB_si( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * \
    RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRSub_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), \
    DPI_shift_imm_op(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
    LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::RSB_si::~RSB_si(){

}
unsigned int arm7tdmi_funclt_trap::LDRSB_off::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->LS_sh(this->rn, this->rn_bit, this->p, this->w, this->u, this->i, this->addr_mode0, \
        this->addr_mode1, this->address);

    rd = (int)SignExtend(dataMem.read_byte(address), 8);
    stall(2);
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::LDRSB_off::replicate() const throw(){
    return new LDRSB_off(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::LDRSB_off::getInstructionName() const throw(){
    return "LDRSB_off";
}

unsigned int arm7tdmi_funclt_trap::LDRSB_off::getId() const throw(){
    return 30;
}

void arm7tdmi_funclt_trap::LDRSB_off::setParams( const unsigned int & bitString ) \
    throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->p = (bitString & 0x1000000) >> 24;
    this->u = (bitString & 0x800000) >> 23;
    this->i = (bitString & 0x400000) >> 22;
    this->w = (bitString & 0x200000) >> 21;
    this->l = (bitString & 0x100000) >> 20;
    this->addr_mode0 = (bitString & 0xf00) >> 8;
    this->addr_mode1 = (bitString & 0xf);
}

std::string arm7tdmi_funclt_trap::LDRSB_off::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::LDRSB_off::LDRSB_off( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 \
    * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), LS_sh_op(CPSR, MP_ID, RB, SPSR, \
    FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::LDRSB_off::~LDRSB_off(){

}
unsigned int arm7tdmi_funclt_trap::SBC_i::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_imm(this->rotate, this->immediate, this->operand, this->carry);

    operand1 = (int)rn;
    operand2 = (int)operand;
    rd =  operand1 - operand2;
    if (CPSR[key_C] == 0){
        rd = ((int)rd) -1;
    }
    this->UpdatePSRSubWithCarry(this->rn, this->rn_bit, this->rd, this->rd_bit, this->s, \
        this->operand1, this->operand2, this->carry);


    if(rd_bit == 15){
        //In case the destination register is the program counter I have to
        //specify that I have a latency of two clock cycles
        stall(2);
        flush();
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::SBC_i::replicate() const throw(){
    return new SBC_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::SBC_i::getInstructionName() const throw(){
    return "SBC_i";
}

unsigned int arm7tdmi_funclt_trap::SBC_i::getId() const throw(){
    return 57;
}

void arm7tdmi_funclt_trap::SBC_i::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->rotate = (bitString & 0xf00) >> 8;
    this->immediate = (bitString & 0xff);
}

std::string arm7tdmi_funclt_trap::SBC_i::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::SBC_i::SBC_i( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, \
    Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias & \
    LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRSubWithCarry_op(CPSR, \
    MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
    dataMem), DPI_imm_op(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
    LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::SBC_i::~SBC_i(){

}
unsigned int arm7tdmi_funclt_trap::TST_i::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_imm(this->rotate, this->immediate, this->operand, this->carry);

    result = rn & operand;
    UpdatePSRBitM(result, carry);
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::TST_i::replicate() const throw(){
    return new TST_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::TST_i::getInstructionName() const throw(){
    return "TST_i";
}

unsigned int arm7tdmi_funclt_trap::TST_i::getId() const throw(){
    return 66;
}

void arm7tdmi_funclt_trap::TST_i::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->rotate = (bitString & 0xf00) >> 8;
    this->immediate = (bitString & 0xff);
}

std::string arm7tdmi_funclt_trap::TST_i::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::TST_i::TST_i( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, \
    Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias & \
    LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), DPI_imm_op(CPSR, MP_ID, RB, \
    SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::TST_i::~TST_i(){

}
unsigned int arm7tdmi_funclt_trap::ADC_i::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_imm(this->rotate, this->immediate, this->operand, this->carry);

    operand1 = (int)rn;
    operand2 = (int)operand;

    rd = operand1 + operand2;

    if (CPSR[key_C]){
        rd = ((int)rd) + 1;
    }
    this->UpdatePSRSumWithCarry(this->rn, this->rn_bit, this->rd, this->rd_bit, this->s, \
        this->operand1, this->operand2, this->carry);


    if(rd_bit == 15){
        //In case the destination register is the program counter I have to
        //specify that I have a latency of two clock cycles
        stall(2);
        flush();
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::ADC_i::replicate() const throw(){
    return new ADC_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::ADC_i::getInstructionName() const throw(){
    return "ADC_i";
}

unsigned int arm7tdmi_funclt_trap::ADC_i::getId() const throw(){
    return 2;
}

void arm7tdmi_funclt_trap::ADC_i::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->rotate = (bitString & 0xf00) >> 8;
    this->immediate = (bitString & 0xff);
}

std::string arm7tdmi_funclt_trap::ADC_i::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::ADC_i::ADC_i( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, \
    Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias & \
    LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRSumWithCarry_op(CPSR, \
    MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
    dataMem), DPI_imm_op(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
    LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::ADC_i::~ADC_i(){

}
unsigned int arm7tdmi_funclt_trap::LDRH_off::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->LS_sh(this->rn, this->rn_bit, this->p, this->w, this->u, this->i, this->addr_mode0, \
        this->addr_mode1, this->address);

    rd = dataMem.read_half(address);
    stall(2);
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::LDRH_off::replicate() const throw(){
    return new LDRH_off(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::LDRH_off::getInstructionName() const throw(){
    return "LDRH_off";
}

unsigned int arm7tdmi_funclt_trap::LDRH_off::getId() const throw(){
    return 28;
}

void arm7tdmi_funclt_trap::LDRH_off::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->p = (bitString & 0x1000000) >> 24;
    this->u = (bitString & 0x800000) >> 23;
    this->i = (bitString & 0x400000) >> 22;
    this->w = (bitString & 0x200000) >> 21;
    this->addr_mode0 = (bitString & 0xf00) >> 8;
    this->addr_mode1 = (bitString & 0xf);
}

std::string arm7tdmi_funclt_trap::LDRH_off::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::LDRH_off::LDRH_off( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 \
    * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), LS_sh_op(CPSR, MP_ID, RB, SPSR, \
    FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::LDRH_off::~LDRH_off(){

}
unsigned int arm7tdmi_funclt_trap::SWAP::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);

    memLastBits = rn & 0x00000003;
    //Depending whether the address is word aligned or not I have to rotate the
    //read word.
    temp = dataMem.read_word(rn);
    if(memLastBits != 0){
        temp = RotateRight(8*memLastBits, temp);
    }
    dataMem.write_word(rn, rm);
    rd = temp;
    stall(3);
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::SWAP::replicate() const throw(){
    return new SWAP(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, \
        PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::SWAP::getInstructionName() const throw(){
    return "SWAP";
}

unsigned int arm7tdmi_funclt_trap::SWAP::getId() const throw(){
    return 73;
}

void arm7tdmi_funclt_trap::SWAP::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
}

std::string arm7tdmi_funclt_trap::SWAP::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::SWAP::SWAP( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, \
    Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias & \
    LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::SWAP::~SWAP(){

}
unsigned int arm7tdmi_funclt_trap::STR_off::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->LS_reg(this->rn, this->rn_bit, this->rm, this->rm_bit, this->p, this->w, this->u, \
        this->shift_amm, this->shift_op, this->address);

    dataMem.write_word(address, rd);
    stall(1);
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::STR_off::replicate() const throw(){
    return new STR_off(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::STR_off::getInstructionName() const throw(){
    return "STR_off";
}

unsigned int arm7tdmi_funclt_trap::STR_off::getId() const throw(){
    return 69;
}

void arm7tdmi_funclt_trap::STR_off::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->p = (bitString & 0x1000000) >> 24;
    this->u = (bitString & 0x800000) >> 23;
    this->w = (bitString & 0x200000) >> 21;
    this->shift_amm = (bitString & 0xf80) >> 7;
    this->shift_op = (bitString & 0x60) >> 5;
}

std::string arm7tdmi_funclt_trap::STR_off::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::STR_off::STR_off( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 \
    * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), LS_reg_op(CPSR, MP_ID, RB, \
    SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::STR_off::~STR_off(){

}
unsigned int arm7tdmi_funclt_trap::STRH_off::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->LS_sh(this->rn, this->rn_bit, this->p, this->w, this->u, this->i, this->addr_mode0, \
        this->addr_mode1, this->address);

    dataMem.write_half(address, (unsigned short)(rd & 0x0000FFFF));
    stall(1);
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::STRH_off::replicate() const throw(){
    return new STRH_off(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::STRH_off::getInstructionName() const throw(){
    return "STRH_off";
}

unsigned int arm7tdmi_funclt_trap::STRH_off::getId() const throw(){
    return 72;
}

void arm7tdmi_funclt_trap::STRH_off::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->p = (bitString & 0x1000000) >> 24;
    this->u = (bitString & 0x800000) >> 23;
    this->i = (bitString & 0x400000) >> 22;
    this->w = (bitString & 0x200000) >> 21;
    this->addr_mode0 = (bitString & 0xf00) >> 8;
    this->addr_mode1 = (bitString & 0xf);
}

std::string arm7tdmi_funclt_trap::STRH_off::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::STRH_off::STRH_off( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 \
    * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), LS_sh_op(CPSR, MP_ID, RB, SPSR, \
    FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::STRH_off::~STRH_off(){

}
unsigned int arm7tdmi_funclt_trap::CMP_sr::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_reg_shift(this->rm, this->rm_bit, this->rs, this->rs_bit, this->shift_op, \
        this->operand, this->carry);

    carry = 0;
    UpdatePSRSubInner(rn, operand, 0);
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::CMP_sr::replicate() const throw(){
    return new CMP_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::CMP_sr::getInstructionName() const throw(){
    return "CMP_sr";
}

unsigned int arm7tdmi_funclt_trap::CMP_sr::getId() const throw(){
    return 18;
}

void arm7tdmi_funclt_trap::CMP_sr::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->rs_bit = (bitString & 0xf00) >> 8;
    this->rs.directSetAlias(this->REGS[this->rs_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->shift_op = (bitString & 0x60) >> 5;
}

std::string arm7tdmi_funclt_trap::CMP_sr::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::CMP_sr::CMP_sr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * \
    RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), DPI_reg_shift_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::CMP_sr::~CMP_sr(){

}
unsigned int arm7tdmi_funclt_trap::SUB_i::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_imm(this->rotate, this->immediate, this->operand, this->carry);

    operand1 = (int)rn;
    operand2 = (int)operand;
    rd =  operand1 - operand2;
    this->UpdatePSRSub(this->rn, this->rn_bit, this->rd, this->rd_bit, this->s, this->operand1, \
        this->operand2, this->carry);


    if(rd_bit == 15){
        //In case the destination register is the program counter I have to
        //specify that I have a latency of two clock cycles
        stall(2);
        flush();
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::SUB_i::replicate() const throw(){
    return new SUB_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::SUB_i::getInstructionName() const throw(){
    return "SUB_i";
}

unsigned int arm7tdmi_funclt_trap::SUB_i::getId() const throw(){
    return 60;
}

void arm7tdmi_funclt_trap::SUB_i::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->rotate = (bitString & 0xf00) >> 8;
    this->immediate = (bitString & 0xff);
}

std::string arm7tdmi_funclt_trap::SUB_i::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::SUB_i::SUB_i( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, \
    Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias & \
    LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRSub_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), \
    DPI_imm_op(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, \
    PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::SUB_i::~SUB_i(){

}
unsigned int arm7tdmi_funclt_trap::RSB_sr::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_reg_shift(this->rm, this->rm_bit, this->rs, this->rs_bit, this->shift_op, \
        this->operand, this->carry);

    operand1 = (int)operand;
    operand2 = (int)rn;
    rd = operand1 - operand2;
    this->UpdatePSRSub(this->rn, this->rn_bit, this->rd, this->rd_bit, this->s, this->operand1, \
        this->operand2, this->carry);


    if(rd_bit == 15){
        //In case the destination register is the program counter I have to
        //specify that I have a latency of two clock cycles
        stall(2);
        flush();
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::RSB_sr::replicate() const throw(){
    return new RSB_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::RSB_sr::getInstructionName() const throw(){
    return "RSB_sr";
}

unsigned int arm7tdmi_funclt_trap::RSB_sr::getId() const throw(){
    return 50;
}

void arm7tdmi_funclt_trap::RSB_sr::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->rs_bit = (bitString & 0xf00) >> 8;
    this->rs.directSetAlias(this->REGS[this->rs_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->shift_op = (bitString & 0x60) >> 5;
}

std::string arm7tdmi_funclt_trap::RSB_sr::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::RSB_sr::RSB_sr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * \
    RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRSub_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), \
    DPI_reg_shift_op(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
    LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::RSB_sr::~RSB_sr(){

}
unsigned int arm7tdmi_funclt_trap::AND_sr::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_reg_shift(this->rm, this->rm_bit, this->rs, this->rs_bit, this->shift_op, \
        this->operand, this->carry);

    result = rn & operand;
    rd = result;
    this->UpdatePSRBit(this->rn, this->rn_bit, this->rd, this->rd_bit, this->s, this->carry, \
        this->result);


    if(rd_bit == 15){
        //In case the destination register is the program counter I have to
        //specify that I have a latency of two clock cycles
        stall(2);
        flush();
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::AND_sr::replicate() const throw(){
    return new AND_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::AND_sr::getInstructionName() const throw(){
    return "AND_sr";
}

unsigned int arm7tdmi_funclt_trap::AND_sr::getId() const throw(){
    return 7;
}

void arm7tdmi_funclt_trap::AND_sr::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->rs_bit = (bitString & 0xf00) >> 8;
    this->rs.directSetAlias(this->REGS[this->rs_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->shift_op = (bitString & 0x60) >> 5;
}

std::string arm7tdmi_funclt_trap::AND_sr::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::AND_sr::AND_sr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * \
    RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRBit_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), \
    DPI_reg_shift_op(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
    LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::AND_sr::~AND_sr(){

}
unsigned int arm7tdmi_funclt_trap::RSC_si::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_shift_imm(this->rm, this->rm_bit, this->shift_amm, this->shift_op, this->operand, \
        this->carry);

    operand1 = (int)operand;
    operand2 = (int)rn;
    rd =  operand1 - operand2;
    if (CPSR[key_C] == 0){
        rd = ((int)rd) -1;
    }
    this->UpdatePSRSubWithCarry(this->rn, this->rn_bit, this->rd, this->rd_bit, this->s, \
        this->operand1, this->operand2, this->carry);


    if(rd_bit == 15){
        //In case the destination register is the program counter I have to
        //specify that I have a latency of two clock cycles
        stall(2);
        flush();
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::RSC_si::replicate() const throw(){
    return new RSC_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::RSC_si::getInstructionName() const throw(){
    return "RSC_si";
}

unsigned int arm7tdmi_funclt_trap::RSC_si::getId() const throw(){
    return 52;
}

void arm7tdmi_funclt_trap::RSC_si::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->shift_amm = (bitString & 0xf80) >> 7;
    this->shift_op = (bitString & 0x60) >> 5;
}

std::string arm7tdmi_funclt_trap::RSC_si::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::RSC_si::RSC_si( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * \
    RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRSubWithCarry_op(CPSR, \
    MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
    dataMem), DPI_shift_imm_op(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, \
    SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::RSC_si::~RSC_si(){

}
unsigned int arm7tdmi_funclt_trap::SBC_si::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_shift_imm(this->rm, this->rm_bit, this->shift_amm, this->shift_op, this->operand, \
        this->carry);

    operand1 = (int)rn;
    operand2 = (int)operand;
    rd =  operand1 - operand2;
    if (CPSR[key_C] == 0){
        rd = ((int)rd) -1;
    }
    this->UpdatePSRSubWithCarry(this->rn, this->rn_bit, this->rd, this->rd_bit, this->s, \
        this->operand1, this->operand2, this->carry);


    if(rd_bit == 15){
        //In case the destination register is the program counter I have to
        //specify that I have a latency of two clock cycles
        stall(2);
        flush();
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::SBC_si::replicate() const throw(){
    return new SBC_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::SBC_si::getInstructionName() const throw(){
    return "SBC_si";
}

unsigned int arm7tdmi_funclt_trap::SBC_si::getId() const throw(){
    return 55;
}

void arm7tdmi_funclt_trap::SBC_si::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->shift_amm = (bitString & 0xf80) >> 7;
    this->shift_op = (bitString & 0x60) >> 5;
}

std::string arm7tdmi_funclt_trap::SBC_si::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::SBC_si::SBC_si( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * \
    RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRSubWithCarry_op(CPSR, \
    MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
    dataMem), DPI_shift_imm_op(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, \
    SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::SBC_si::~SBC_si(){

}
unsigned int arm7tdmi_funclt_trap::MOV_si::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_shift_imm(this->rm, this->rm_bit, this->shift_amm, this->shift_op, this->operand, \
        this->carry);

    rd = operand;
    result = operand;
    this->UpdatePSRBit(this->rn, this->rn_bit, this->rd, this->rd_bit, this->s, this->carry, \
        this->result);


    if(rd_bit == 15){
        //In case the destination register is the program counter I have to
        //specify that I have a latency of two clock cycles
        stall(2);
        flush();
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::MOV_si::replicate() const throw(){
    return new MOV_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::MOV_si::getInstructionName() const throw(){
    return "MOV_si";
}

unsigned int arm7tdmi_funclt_trap::MOV_si::getId() const throw(){
    return 37;
}

void arm7tdmi_funclt_trap::MOV_si::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->shift_amm = (bitString & 0xf80) >> 7;
    this->shift_op = (bitString & 0x60) >> 5;
}

std::string arm7tdmi_funclt_trap::MOV_si::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::MOV_si::MOV_si( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * \
    RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRBit_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), \
    DPI_shift_imm_op(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
    LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::MOV_si::~MOV_si(){

}
unsigned int arm7tdmi_funclt_trap::umlal_Instr::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);

    //Perform the operation
    result = (unsigned long long)(((unsigned long long)(((unsigned long long)((unsigned \
        int)rm)) * ((unsigned long long)((unsigned int)rs)))) + (((unsigned long long)rd) \
        << 32) + (unsigned int)REGS[rn]);
    //Check if I have to update the processor flags
    rd = (unsigned int)((result >> 32) & 0x00000000FFFFFFFF);
    REGS[rn] = (unsigned int)(result & 0x00000000FFFFFFFFLL);

    if((rs & 0xFFFFFF00) == 0x0 || (rs & 0xFFFFFF00) == 0xFFFFFF00){
        stall(3);
    }
    else if((rs & 0xFFFF0000) == 0x0 || (rs & 0xFFFF0000) == 0xFFFF0000){
        stall(4);
    }
    else if((rs & 0xFF000000) == 0x0 || (rs & 0xFF000000) == 0xFF000000){
        stall(5);
    }
    else{
        stall(6);
    }
    this->UpdatePSRmul_64(this->rd, this->rd_bit, this->s, this->carry, this->result);
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::umlal_Instr::replicate() const throw(){
    return new umlal_Instr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::umlal_Instr::getInstructionName() const throw(){
    return "umlal_Instr";
}

unsigned int arm7tdmi_funclt_trap::umlal_Instr::getId() const throw(){
    return 35;
}

void arm7tdmi_funclt_trap::umlal_Instr::setParams( const unsigned int & bitString \
    ) throw(){
    this->rd_bit = (bitString & 0xf0000) >> 16;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rs_bit = (bitString & 0xf00) >> 8;
    this->rs.directSetAlias(this->REGS[this->rs_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->rn = (bitString & 0xf000) >> 12;
}

std::string arm7tdmi_funclt_trap::umlal_Instr::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::umlal_Instr::umlal_Instr( Reg32_0 & CPSR, Reg32_1 & MP_ID, \
    Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, \
    Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, \
    LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRmul_64_op(CPSR, \
    MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
    dataMem){

}

arm7tdmi_funclt_trap::umlal_Instr::~umlal_Instr(){

}
unsigned int arm7tdmi_funclt_trap::AND_i::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_imm(this->rotate, this->immediate, this->operand, this->carry);

    result = rn & operand;
    rd = result;
    this->UpdatePSRBit(this->rn, this->rn_bit, this->rd, this->rd_bit, this->s, this->carry, \
        this->result);


    if(rd_bit == 15){
        //In case the destination register is the program counter I have to
        //specify that I have a latency of two clock cycles
        stall(2);
        flush();
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::AND_i::replicate() const throw(){
    return new AND_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::AND_i::getInstructionName() const throw(){
    return "AND_i";
}

unsigned int arm7tdmi_funclt_trap::AND_i::getId() const throw(){
    return 8;
}

void arm7tdmi_funclt_trap::AND_i::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->rotate = (bitString & 0xf00) >> 8;
    this->immediate = (bitString & 0xff);
}

std::string arm7tdmi_funclt_trap::AND_i::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::AND_i::AND_i( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, \
    Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias & \
    LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRBit_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), \
    DPI_imm_op(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, \
    PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::AND_i::~AND_i(){

}
unsigned int arm7tdmi_funclt_trap::LDR_off::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->LS_reg(this->rn, this->rn_bit, this->rm, this->rm_bit, this->p, this->w, this->u, \
        this->shift_amm, this->shift_op, this->address);

    memLastBits = address & 0x00000003;
    // if the memory address is not word aligned I have to rotate the loaded value
    if(memLastBits == 0){
        value = dataMem.read_word(address);
    }
    else{
        value = RotateRight(8*memLastBits, dataMem.read_word(address));
    }

    //Perform the writeback; as usual I have to behave differently
    //if a load a value to the PC
    if(rd_bit == 15){
        //I don't consider the 2 less significant bits since I don't bother with
        //thumb mode.
        PC = value & 0xFFFFFFFC;
        stall(4);
        flush();
    }
    else{
        rd = value;
        stall(2);
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::LDR_off::replicate() const throw(){
    return new LDR_off(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::LDR_off::getInstructionName() const throw(){
    return "LDR_off";
}

unsigned int arm7tdmi_funclt_trap::LDR_off::getId() const throw(){
    return 25;
}

void arm7tdmi_funclt_trap::LDR_off::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->p = (bitString & 0x1000000) >> 24;
    this->u = (bitString & 0x800000) >> 23;
    this->w = (bitString & 0x200000) >> 21;
    this->shift_amm = (bitString & 0xf80) >> 7;
    this->shift_op = (bitString & 0x60) >> 5;
}

std::string arm7tdmi_funclt_trap::LDR_off::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::LDR_off::LDR_off( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 \
    * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), LS_reg_op(CPSR, MP_ID, RB, \
    SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::LDR_off::~LDR_off(){

}
unsigned int arm7tdmi_funclt_trap::MOV_sr::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_reg_shift(this->rm, this->rm_bit, this->rs, this->rs_bit, this->shift_op, \
        this->operand, this->carry);

    rd = operand;
    result = operand;
    this->UpdatePSRBit(this->rn, this->rn_bit, this->rd, this->rd_bit, this->s, this->carry, \
        this->result);


    if(rd_bit == 15){
        //In case the destination register is the program counter I have to
        //specify that I have a latency of two clock cycles
        stall(2);
        flush();
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::MOV_sr::replicate() const throw(){
    return new MOV_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::MOV_sr::getInstructionName() const throw(){
    return "MOV_sr";
}

unsigned int arm7tdmi_funclt_trap::MOV_sr::getId() const throw(){
    return 38;
}

void arm7tdmi_funclt_trap::MOV_sr::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->rs_bit = (bitString & 0xf00) >> 8;
    this->rs.directSetAlias(this->REGS[this->rs_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->shift_op = (bitString & 0x60) >> 5;
}

std::string arm7tdmi_funclt_trap::MOV_sr::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::MOV_sr::MOV_sr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * \
    RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRBit_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), \
    DPI_reg_shift_op(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
    LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::MOV_sr::~MOV_sr(){

}
unsigned int arm7tdmi_funclt_trap::RSC_sr::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_reg_shift(this->rm, this->rm_bit, this->rs, this->rs_bit, this->shift_op, \
        this->operand, this->carry);

    operand1 = (int)operand;
    operand2 = (int)rn;
    rd =  operand1 - operand2;
    if (CPSR[key_C] == 0){
        rd = ((int)rd) -1;
    }
    this->UpdatePSRSubWithCarry(this->rn, this->rn_bit, this->rd, this->rd_bit, this->s, \
        this->operand1, this->operand2, this->carry);


    if(rd_bit == 15){
        //In case the destination register is the program counter I have to
        //specify that I have a latency of two clock cycles
        stall(2);
        flush();
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::RSC_sr::replicate() const throw(){
    return new RSC_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::RSC_sr::getInstructionName() const throw(){
    return "RSC_sr";
}

unsigned int arm7tdmi_funclt_trap::RSC_sr::getId() const throw(){
    return 53;
}

void arm7tdmi_funclt_trap::RSC_sr::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->rs_bit = (bitString & 0xf00) >> 8;
    this->rs.directSetAlias(this->REGS[this->rs_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->shift_op = (bitString & 0x60) >> 5;
}

std::string arm7tdmi_funclt_trap::RSC_sr::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::RSC_sr::RSC_sr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * \
    RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRSubWithCarry_op(CPSR, \
    MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
    dataMem), DPI_reg_shift_op(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, \
    SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::RSC_sr::~RSC_sr(){

}
unsigned int arm7tdmi_funclt_trap::BIC_i::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_imm(this->rotate, this->immediate, this->operand, this->carry);

    result = rn & ~operand;
    rd = result;
    this->UpdatePSRBit(this->rn, this->rn_bit, this->rd, this->rd_bit, this->s, this->carry, \
        this->result);


    if(rd_bit == 15){
        //In case the destination register is the program counter I have to
        //specify that I have a latency of two clock cycles
        stall(2);
        flush();
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::BIC_i::replicate() const throw(){
    return new BIC_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::BIC_i::getInstructionName() const throw(){
    return "BIC_i";
}

unsigned int arm7tdmi_funclt_trap::BIC_i::getId() const throw(){
    return 13;
}

void arm7tdmi_funclt_trap::BIC_i::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->rotate = (bitString & 0xf00) >> 8;
    this->immediate = (bitString & 0xff);
}

std::string arm7tdmi_funclt_trap::BIC_i::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::BIC_i::BIC_i( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, \
    Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias & \
    LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRBit_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), \
    DPI_imm_op(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, \
    PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::BIC_i::~BIC_i(){

}
unsigned int arm7tdmi_funclt_trap::TEQ_sr::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_reg_shift(this->rm, this->rm_bit, this->rs, this->rs_bit, this->shift_op, \
        this->operand, this->carry);

    result = rn ^ operand;
    UpdatePSRBitM(result, carry);
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::TEQ_sr::replicate() const throw(){
    return new TEQ_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::TEQ_sr::getInstructionName() const throw(){
    return "TEQ_sr";
}

unsigned int arm7tdmi_funclt_trap::TEQ_sr::getId() const throw(){
    return 62;
}

void arm7tdmi_funclt_trap::TEQ_sr::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->rs_bit = (bitString & 0xf00) >> 8;
    this->rs.directSetAlias(this->REGS[this->rs_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->shift_op = (bitString & 0x60) >> 5;
}

std::string arm7tdmi_funclt_trap::TEQ_sr::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::TEQ_sr::TEQ_sr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * \
    RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), DPI_reg_shift_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::TEQ_sr::~TEQ_sr(){

}
unsigned int arm7tdmi_funclt_trap::smull_Instr::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);

    //Perform the operation
    result = (long long)(((long long)((int)rm)) * ((long long)((int)rs)));
    //Check if I have to update the processor flags
    rd = (int)((result >> 32) & 0x00000000FFFFFFFF);
    REGS[rn] = (int)(result & 0x00000000FFFFFFFF);

    if((rs & 0xFFFFFF00) == 0x0 || (rs & 0xFFFFFF00) == 0xFFFFFF00){
        stall(3);
    }
    else if((rs & 0xFFFF0000) == 0x0 || (rs & 0xFFFF0000) == 0xFFFF0000){
        stall(4);
    }
    else if((rs & 0xFF000000) == 0x0 || (rs & 0xFF000000) == 0xFF000000){
        stall(5);
    }
    else{
        stall(6);
    }
    this->UpdatePSRmul_64(this->rd, this->rd_bit, this->s, this->carry, this->result);
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::smull_Instr::replicate() const throw(){
    return new smull_Instr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::smull_Instr::getInstructionName() const throw(){
    return "smull_Instr";
}

unsigned int arm7tdmi_funclt_trap::smull_Instr::getId() const throw(){
    return 34;
}

void arm7tdmi_funclt_trap::smull_Instr::setParams( const unsigned int & bitString \
    ) throw(){
    this->rd_bit = (bitString & 0xf0000) >> 16;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rs_bit = (bitString & 0xf00) >> 8;
    this->rs.directSetAlias(this->REGS[this->rs_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->rn = (bitString & 0xf000) >> 12;
}

std::string arm7tdmi_funclt_trap::smull_Instr::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::smull_Instr::smull_Instr( Reg32_0 & CPSR, Reg32_1 & MP_ID, \
    Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, \
    Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, \
    LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRmul_64_op(CPSR, \
    MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
    dataMem){

}

arm7tdmi_funclt_trap::smull_Instr::~smull_Instr(){

}
unsigned int arm7tdmi_funclt_trap::STRB_off::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->LS_reg(this->rn, this->rn_bit, this->rm, this->rm_bit, this->p, this->w, this->u, \
        this->shift_amm, this->shift_op, this->address);

    dataMem.write_byte(address, (unsigned char)(rd & 0x000000FF));
    stall(1);
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::STRB_off::replicate() const throw(){
    return new STRB_off(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::STRB_off::getInstructionName() const throw(){
    return "STRB_off";
}

unsigned int arm7tdmi_funclt_trap::STRB_off::getId() const throw(){
    return 71;
}

void arm7tdmi_funclt_trap::STRB_off::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->p = (bitString & 0x1000000) >> 24;
    this->u = (bitString & 0x800000) >> 23;
    this->w = (bitString & 0x200000) >> 21;
    this->shift_amm = (bitString & 0xf80) >> 7;
    this->shift_op = (bitString & 0x60) >> 5;
}

std::string arm7tdmi_funclt_trap::STRB_off::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::STRB_off::STRB_off( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 \
    * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), LS_reg_op(CPSR, MP_ID, RB, \
    SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::STRB_off::~STRB_off(){

}
unsigned int arm7tdmi_funclt_trap::smlal_Instr::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);

    //Perform the operation
    result = (long long)((((long long)(((long long)((int)rm)) * ((long long)((int)rs)))) \
        + (((long long)rd) << 32)) + (int)REGS[rn]);
    //Check if I have to update the processor flags
    rd = (int)((result >> 32) & 0x00000000FFFFFFFF);
    REGS[rn] = (int)(result & 0x00000000FFFFFFFF);

    if((rs & 0xFFFFFF00) == 0x0 || (rs & 0xFFFFFF00) == 0xFFFFFF00){
        stall(3);
    }
    else if((rs & 0xFFFF0000) == 0x0 || (rs & 0xFFFF0000) == 0xFFFF0000){
        stall(4);
    }
    else if((rs & 0xFF000000) == 0x0 || (rs & 0xFF000000) == 0xFF000000){
        stall(5);
    }
    else{
        stall(6);
    }
    this->UpdatePSRmul_64(this->rd, this->rd_bit, this->s, this->carry, this->result);
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::smlal_Instr::replicate() const throw(){
    return new smlal_Instr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::smlal_Instr::getInstructionName() const throw(){
    return "smlal_Instr";
}

unsigned int arm7tdmi_funclt_trap::smlal_Instr::getId() const throw(){
    return 33;
}

void arm7tdmi_funclt_trap::smlal_Instr::setParams( const unsigned int & bitString \
    ) throw(){
    this->rd_bit = (bitString & 0xf0000) >> 16;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rs_bit = (bitString & 0xf00) >> 8;
    this->rs.directSetAlias(this->REGS[this->rs_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->rn = (bitString & 0xf000) >> 12;
}

std::string arm7tdmi_funclt_trap::smlal_Instr::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::smlal_Instr::smlal_Instr( Reg32_0 & CPSR, Reg32_1 & MP_ID, \
    Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, \
    Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, \
    LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRmul_64_op(CPSR, \
    MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
    dataMem){

}

arm7tdmi_funclt_trap::smlal_Instr::~smlal_Instr(){

}
unsigned int arm7tdmi_funclt_trap::CMP_si::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_shift_imm(this->rm, this->rm_bit, this->shift_amm, this->shift_op, this->operand, \
        this->carry);

    carry = 0;
    UpdatePSRSubInner(rn, operand, 0);
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::CMP_si::replicate() const throw(){
    return new CMP_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::CMP_si::getInstructionName() const throw(){
    return "CMP_si";
}

unsigned int arm7tdmi_funclt_trap::CMP_si::getId() const throw(){
    return 17;
}

void arm7tdmi_funclt_trap::CMP_si::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->shift_amm = (bitString & 0xf80) >> 7;
    this->shift_op = (bitString & 0x60) >> 5;
}

std::string arm7tdmi_funclt_trap::CMP_si::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::CMP_si::CMP_si( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * \
    RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), DPI_shift_imm_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::CMP_si::~CMP_si(){

}
unsigned int arm7tdmi_funclt_trap::TEQ_si::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_shift_imm(this->rm, this->rm_bit, this->shift_amm, this->shift_op, this->operand, \
        this->carry);

    result = rn ^ operand;
    UpdatePSRBitM(result, carry);
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::TEQ_si::replicate() const throw(){
    return new TEQ_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::TEQ_si::getInstructionName() const throw(){
    return "TEQ_si";
}

unsigned int arm7tdmi_funclt_trap::TEQ_si::getId() const throw(){
    return 61;
}

void arm7tdmi_funclt_trap::TEQ_si::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->shift_amm = (bitString & 0xf80) >> 7;
    this->shift_op = (bitString & 0x60) >> 5;
}

std::string arm7tdmi_funclt_trap::TEQ_si::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::TEQ_si::TEQ_si( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * \
    RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), DPI_shift_imm_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::TEQ_si::~TEQ_si(){

}
unsigned int arm7tdmi_funclt_trap::CMN_sr::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_reg_shift(this->rm, this->rm_bit, this->rs, this->rs_bit, this->shift_op, \
        this->operand, this->carry);

    //carry  = 0
    UpdatePSRAddInner(rn, operand, 0);
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::CMN_sr::replicate() const throw(){
    return new CMN_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::CMN_sr::getInstructionName() const throw(){
    return "CMN_sr";
}

unsigned int arm7tdmi_funclt_trap::CMN_sr::getId() const throw(){
    return 15;
}

void arm7tdmi_funclt_trap::CMN_sr::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->rs_bit = (bitString & 0xf00) >> 8;
    this->rs.directSetAlias(this->REGS[this->rs_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->shift_op = (bitString & 0x60) >> 5;
}

std::string arm7tdmi_funclt_trap::CMN_sr::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::CMN_sr::CMN_sr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * \
    RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), DPI_reg_shift_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::CMN_sr::~CMN_sr(){

}
unsigned int arm7tdmi_funclt_trap::mrs_Instr::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);

    if(r == 1){ // I have to save the SPSR
    switch(CPSR[key_mode]){
        case 0x1:{
            //I'm in FIQ mode
            rd = SPSR[0];
        break;}
        case 0x2:{
            //I'm in IRQ mode
            rd = SPSR[1];
        break;}
        case 0x3:{
            //I'm in SVC mode
            rd = SPSR[2];
        break;}
        case 0x7:{
            //I'm in ABT mode
            rd = SPSR[3];
        break;}
        case 0xB:{
            //I'm in UND mode
            rd = SPSR[4];
        break;}
        default:
        break;
    }
}
else{
    // I have to save the CPSR
    rd = CPSR;
}
return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::mrs_Instr::replicate() const throw(){
    return new mrs_Instr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::mrs_Instr::getInstructionName() const throw(){
    return "mrs_Instr";
}

unsigned int arm7tdmi_funclt_trap::mrs_Instr::getId() const throw(){
    return 40;
}

void arm7tdmi_funclt_trap::mrs_Instr::setParams( const unsigned int & bitString ) \
    throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->r = (bitString & 0x400000) >> 22;
}

std::string arm7tdmi_funclt_trap::mrs_Instr::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::mrs_Instr::mrs_Instr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 \
    * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::mrs_Instr::~mrs_Instr(){

}
unsigned int arm7tdmi_funclt_trap::mul_Instr::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);

    rd = (int)rm * (int)rs;

    if((rs & 0xFFFFFF00) == 0x0 || (rs & 0xFFFFFF00) == 0xFFFFFF00){
        stall(1);
    }
    else if((rs & 0xFFFF0000) == 0x0 || (rs & 0xFFFF0000) == 0xFFFF0000){
        stall(2);
    }
    else if((rs & 0xFF000000) == 0x0 || (rs & 0xFF000000) == 0xFF000000){
        stall(3);
    }
    else{
        stall(4);
    }
    this->UpdatePSRmul(this->rd, this->rd_bit, this->s);
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::mul_Instr::replicate() const throw(){
    return new mul_Instr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::mul_Instr::getInstructionName() const throw(){
    return "mul_Instr";
}

unsigned int arm7tdmi_funclt_trap::mul_Instr::getId() const throw(){
    return 32;
}

void arm7tdmi_funclt_trap::mul_Instr::setParams( const unsigned int & bitString ) \
    throw(){
    this->rd_bit = (bitString & 0xf0000) >> 16;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rs_bit = (bitString & 0xf00) >> 8;
    this->rs.directSetAlias(this->REGS[this->rs_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->rn = (bitString & 0xf000) >> 12;
}

std::string arm7tdmi_funclt_trap::mul_Instr::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::mul_Instr::mul_Instr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 \
    * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRmul_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::mul_Instr::~mul_Instr(){

}
unsigned int arm7tdmi_funclt_trap::SUB_si::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_shift_imm(this->rm, this->rm_bit, this->shift_amm, this->shift_op, this->operand, \
        this->carry);

    operand1 = (int)rn;
    operand2 = (int)operand;
    rd =  operand1 - operand2;
    this->UpdatePSRSub(this->rn, this->rn_bit, this->rd, this->rd_bit, this->s, this->operand1, \
        this->operand2, this->carry);


    if(rd_bit == 15){
        //In case the destination register is the program counter I have to
        //specify that I have a latency of two clock cycles
        stall(2);
        flush();
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::SUB_si::replicate() const throw(){
    return new SUB_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::SUB_si::getInstructionName() const throw(){
    return "SUB_si";
}

unsigned int arm7tdmi_funclt_trap::SUB_si::getId() const throw(){
    return 58;
}

void arm7tdmi_funclt_trap::SUB_si::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->shift_amm = (bitString & 0xf80) >> 7;
    this->shift_op = (bitString & 0x60) >> 5;
}

std::string arm7tdmi_funclt_trap::SUB_si::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::SUB_si::SUB_si( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * \
    RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRSub_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), \
    DPI_shift_imm_op(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
    LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::SUB_si::~SUB_si(){

}
unsigned int arm7tdmi_funclt_trap::STM::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();

    #ifdef ACC_MODEL
    for(int i = 0; i < 16; i++){
        if((reg_list & (0x00000001 << i)) != 0){
            RB[i].isLocked();
        }
    }
    #endif
    this->condition_check(this->cond);
    this->LSM_reglist(this->rn, this->rn_bit, this->p, this->u, this->reg_list, this->start_address, \
        this->wb_address);

    int numRegsToStore = 0;
    //I use word aligned addresses
    start_address &= 0xFFFFFFFC;
    //I have to distinguish whether I have to store user-mode registers
    //or the currently used ones.
    if(s != 0){
        //Now I can save the registers
        //Save the registers common to all modes
        for(int i = 0; i < 8; i++){
            if((reg_list & (0x00000001 << i)) != 0) {
                dataMem.write_word(start_address, REGS[i]);
                start_address += 4;
                numRegsToStore++;
            }
        }
        //Save the User Mode registers.
        for(int i = 8; i < 16; i++){
            if((reg_list & (0x00000001 << i)) != 0) {
                dataMem.write_word(start_address, RB[i]);
                start_address += 4;
                numRegsToStore++;
            }
        }
    }
    else{
        //Normal registers
        //Save the registers common to all modes; note that using the writeBack strategy
        //and putting the base register in the list of register to be saved is defined by the
        //ARM as an undefined operation; actually most ARM implementations save the original
        //base register if it is first in the register list, otherwise they save the updated
        //version.
        for(int i = 0; i < 16; i++){
            if((reg_list & (0x00000001 << i)) != 0) {
                dataMem.write_word(start_address, REGS[i]);
                start_address += 4;
                numRegsToStore++;
            }
            //Now If necessary I update the base register; it gets updated
            //after the first register has been written.
            if(w != 0 && i == 0){
                rn = wb_address;
            }
        }
    }
    stall(numRegsToStore);
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::STM::replicate() const throw(){
    return new STM(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, \
        PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::STM::getInstructionName() const throw(){
    return "STM";
}

unsigned int arm7tdmi_funclt_trap::STM::getId() const throw(){
    return 67;
}

void arm7tdmi_funclt_trap::STM::setParams( const unsigned int & bitString ) throw(){
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->p = (bitString & 0x1000000) >> 24;
    this->u = (bitString & 0x800000) >> 23;
    this->s = (bitString & 0x400000) >> 22;
    this->w = (bitString & 0x200000) >> 21;
    this->reg_list = (bitString & 0xffff);
}

std::string arm7tdmi_funclt_trap::STM::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::STM::STM( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 \
    * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias & LR_IRQ, \
    Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory \
    & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, \
    SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), LSM_reglist_op(CPSR, MP_ID, RB, SPSR, \
    FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::STM::~STM(){

}
unsigned int arm7tdmi_funclt_trap::ORR_sr::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_reg_shift(this->rm, this->rm_bit, this->rs, this->rs_bit, this->shift_op, \
        this->operand, this->carry);

    result = rn | operand;
    rd = result;
    this->UpdatePSRBit(this->rn, this->rn_bit, this->rd, this->rd_bit, this->s, this->carry, \
        this->result);


    if(rd_bit == 15){
        //In case the destination register is the program counter I have to
        //specify that I have a latency of two clock cycles
        stall(2);
        flush();
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::ORR_sr::replicate() const throw(){
    return new ORR_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::ORR_sr::getInstructionName() const throw(){
    return "ORR_sr";
}

unsigned int arm7tdmi_funclt_trap::ORR_sr::getId() const throw(){
    return 47;
}

void arm7tdmi_funclt_trap::ORR_sr::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->rs_bit = (bitString & 0xf00) >> 8;
    this->rs.directSetAlias(this->REGS[this->rs_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->shift_op = (bitString & 0x60) >> 5;
}

std::string arm7tdmi_funclt_trap::ORR_sr::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::ORR_sr::ORR_sr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * \
    RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRBit_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), \
    DPI_reg_shift_op(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
    LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::ORR_sr::~ORR_sr(){

}
unsigned int arm7tdmi_funclt_trap::RSB_i::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_imm(this->rotate, this->immediate, this->operand, this->carry);

    operand1 = (int)operand;
    operand2 = (int)rn;
    rd = operand1 - operand2;
    this->UpdatePSRSub(this->rn, this->rn_bit, this->rd, this->rd_bit, this->s, this->operand1, \
        this->operand2, this->carry);


    if(rd_bit == 15){
        //In case the destination register is the program counter I have to
        //specify that I have a latency of two clock cycles
        stall(2);
        flush();
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::RSB_i::replicate() const throw(){
    return new RSB_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::RSB_i::getInstructionName() const throw(){
    return "RSB_i";
}

unsigned int arm7tdmi_funclt_trap::RSB_i::getId() const throw(){
    return 51;
}

void arm7tdmi_funclt_trap::RSB_i::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->rotate = (bitString & 0xf00) >> 8;
    this->immediate = (bitString & 0xff);
}

std::string arm7tdmi_funclt_trap::RSB_i::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::RSB_i::RSB_i( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, \
    Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias & \
    LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRSub_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), \
    DPI_imm_op(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, \
    PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::RSB_i::~RSB_i(){

}
unsigned int arm7tdmi_funclt_trap::mla_Instr::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);

    rd = ((int)rm * (int)rs) + (int)REGS[rn];

    if((rs & 0xFFFFFF00) == 0x0 || (rs & 0xFFFFFF00) == 0xFFFFFF00){
        stall(2);
    }
    else if((rs & 0xFFFF0000) == 0x0 || (rs & 0xFFFF0000) == 0xFFFF0000){
        stall(3);
    }
    else if((rs & 0xFF000000) == 0x0 || (rs & 0xFF000000) == 0xFF000000){
        stall(4);
    }
    else{
        stall(5);
    }
    this->UpdatePSRmul(this->rd, this->rd_bit, this->s);
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::mla_Instr::replicate() const throw(){
    return new mla_Instr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::mla_Instr::getInstructionName() const throw(){
    return "mla_Instr";
}

unsigned int arm7tdmi_funclt_trap::mla_Instr::getId() const throw(){
    return 31;
}

void arm7tdmi_funclt_trap::mla_Instr::setParams( const unsigned int & bitString ) \
    throw(){
    this->rd_bit = (bitString & 0xf0000) >> 16;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rs_bit = (bitString & 0xf00) >> 8;
    this->rs.directSetAlias(this->REGS[this->rs_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->rn = (bitString & 0xf000) >> 12;
}

std::string arm7tdmi_funclt_trap::mla_Instr::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::mla_Instr::mla_Instr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 \
    * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRmul_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::mla_Instr::~mla_Instr(){

}
unsigned int arm7tdmi_funclt_trap::EOR_sr::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_reg_shift(this->rm, this->rm_bit, this->rs, this->rs_bit, this->shift_op, \
        this->operand, this->carry);

    result = rn ^ operand;
    rd = result;
    this->UpdatePSRBit(this->rn, this->rn_bit, this->rd, this->rd_bit, this->s, this->carry, \
        this->result);


    if(rd_bit == 15){
        //In case the destination register is the program counter I have to
        //specify that I have a latency of two clock cycles
        stall(2);
        flush();
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::EOR_sr::replicate() const throw(){
    return new EOR_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::EOR_sr::getInstructionName() const throw(){
    return "EOR_sr";
}

unsigned int arm7tdmi_funclt_trap::EOR_sr::getId() const throw(){
    return 21;
}

void arm7tdmi_funclt_trap::EOR_sr::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->rs_bit = (bitString & 0xf00) >> 8;
    this->rs.directSetAlias(this->REGS[this->rs_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->shift_op = (bitString & 0x60) >> 5;
}

std::string arm7tdmi_funclt_trap::EOR_sr::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::EOR_sr::EOR_sr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * \
    RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRBit_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), \
    DPI_reg_shift_op(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
    LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::EOR_sr::~EOR_sr(){

}
unsigned int arm7tdmi_funclt_trap::msr_imm_Instr::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);

    value = RotateRight(rotate*2, immediate);
    //Checking for unvalid bits
    if((value & 0x00000010) == 0){
        THROW_EXCEPTION("MSR called with unvalid mode " << std::hex << std::showbase << value \
            << ": we are trying to switch to 26 bit PC");
    }
    unsigned int currentMode = CPSR[key_mode];
    //Firs of all I check whether I have to modify the CPSR or the SPSR
    if(r == 0){
        //CPSR
        //Now I modify the fields; note that in user mode I can just update the flags.
        if((mask & 0x1) != 0 && currentMode != 0){
            CPSR &= 0xFFFFFF00;
            CPSR |= value & 0x000000FF;
            //Now if I change mode I also have to update the registry bank
            if(currentMode != (CPSR & 0x0000000F)){
                restoreSPSR();
            }
        }
        if((mask & 0x2) != 0 && currentMode != 0){
            CPSR &= 0xFFFF00FF;
            CPSR |= value & 0x0000FF00;
        }
        if((mask & 0x4) != 0 && currentMode != 0){
            CPSR &= 0xFF00FFFF;
            CPSR |= value & 0x00FF0000;
        }
        if((mask & 0x8) != 0){
            CPSR &= 0x00FFFFFF;
            CPSR |= value & 0xFF000000;
        }
    }
    else{
        //SPSR
        switch(currentMode){
            case 0x1:{
                //I'm in FIQ mode
                if((mask & 0x1) != 0){
                    SPSR[0] &= 0xFFFFFF00;
                    SPSR[0] |= value & 0x000000FF;
                }
                if((mask & 0x2) != 0){
                    SPSR[0] &= 0xFFFF00FF;
                    SPSR[0] |= value & 0x0000FF00;
                }
                if((mask & 0x4) != 0){
                    SPSR[0] &= 0xFF00FFFF;
                    SPSR[0] |= value & 0x00FF0000;
                }
                if((mask & 0x8) != 0){
                    SPSR[0] &= 0x00FFFFFF;
                    SPSR[0] |= value & 0xFF000000;
                }
            break;}
            case 0x2:{
                //I'm in IRQ mode
                if((mask & 0x1) != 0){
                    SPSR[1] &= 0xFFFFFF00;
                    SPSR[1] |= value & 0x000000FF;
                }
                if((mask & 0x2) != 0){
                    SPSR[1] &= 0xFFFF00FF;
                    SPSR[1] |= value & 0x0000FF00;
                }
                if((mask & 0x4) != 0){
                    SPSR[1] &= 0xFF00FFFF;
                    SPSR[1] |= value & 0x00FF0000;
                }
                if((mask & 0x8) != 0){
                    SPSR[1] &= 0x00FFFFFF;
                    SPSR[1] |= value & 0xFF000000;
                }
            break;}
            case 0x3:{
                //I'm in SVC mode
                if((mask & 0x1) != 0){
                    SPSR[2] &= 0xFFFFFF00;
                    SPSR[2] |= value & 0x000000FF;
                }
                if((mask & 0x2) != 0){
                    SPSR[2] &= 0xFFFF00FF;
                    SPSR[2] |= value & 0x0000FF00;
                }
                if((mask & 0x4) != 0){
                    SPSR[2] &= 0xFF00FFFF;
                    SPSR[2] |= value & 0x00FF0000;
                }
                if((mask & 0x8) != 0){
                    SPSR[2] &= 0x00FFFFFF;
                    SPSR[2] |= value & 0xFF000000;
                }
            break;}
            case 0x7:{
                //I'm in ABT mode
                if((mask & 0x1) != 0){
                    SPSR[3] &= 0xFFFFFF00;
                    SPSR[3] |= value & 0x000000FF;
                }
                if((mask & 0x2) != 0){
                    SPSR[3] &= 0xFFFF00FF;
                    SPSR[3] |= value & 0x0000FF00;
                }
                if((mask & 0x4) != 0){
                    SPSR[3] &= 0xFF00FFFF;
                    SPSR[3] |= value & 0x00FF0000;
                }
                if((mask & 0x8) != 0){
                    SPSR[3] &= 0x00FFFFFF;
                    SPSR[3] |= value & 0xFF000000;
                }
            break;}
            case 0xB:{
                //I'm in UND mode
                if((mask & 0x1) != 0){
                    SPSR[4] &= 0xFFFFFF00;
                    SPSR[4] |= value & 0x000000FF;
                }
                if((mask & 0x2) != 0){
                    SPSR[4] &= 0xFFFF00FF;
                    SPSR[4] |= value & 0x0000FF00;
                }
                if((mask & 0x4) != 0){
                    SPSR[4] &= 0xFF00FFFF;
                    SPSR[4] |= value & 0x00FF0000;
                }
                if((mask & 0x8) != 0){
                    SPSR[4] &= 0x00FFFFFF;
                    SPSR[4] |= value & 0xFF000000;
                }
            break;}
            default:
            break;
        }
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::msr_imm_Instr::replicate() const throw(){
    return new msr_imm_Instr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, \
        SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::msr_imm_Instr::getInstructionName() const throw(){
    return "msr_imm_Instr";
}

unsigned int arm7tdmi_funclt_trap::msr_imm_Instr::getId() const throw(){
    return 41;
}

void arm7tdmi_funclt_trap::msr_imm_Instr::setParams( const unsigned int & bitString \
    ) throw(){
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->r = (bitString & 0x400000) >> 22;
    this->mask = (bitString & 0xf0000) >> 16;
    this->rotate = (bitString & 0xf00) >> 8;
    this->immediate = (bitString & 0xff);
}

std::string arm7tdmi_funclt_trap::msr_imm_Instr::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::msr_imm_Instr::msr_imm_Instr( Reg32_0 & CPSR, Reg32_1 & MP_ID, \
    Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, \
    Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, \
    LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::msr_imm_Instr::~msr_imm_Instr(){

}
unsigned int arm7tdmi_funclt_trap::LDRB_off::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->LS_reg(this->rn, this->rn_bit, this->rm, this->rm_bit, this->p, this->w, this->u, \
        this->shift_amm, this->shift_op, this->address);

    rd = dataMem.read_byte(address);
    stall(2);
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::LDRB_off::replicate() const throw(){
    return new LDRB_off(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::LDRB_off::getInstructionName() const throw(){
    return "LDRB_off";
}

unsigned int arm7tdmi_funclt_trap::LDRB_off::getId() const throw(){
    return 27;
}

void arm7tdmi_funclt_trap::LDRB_off::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->p = (bitString & 0x1000000) >> 24;
    this->u = (bitString & 0x800000) >> 23;
    this->w = (bitString & 0x200000) >> 21;
    this->shift_amm = (bitString & 0xf80) >> 7;
    this->shift_op = (bitString & 0x60) >> 5;
}

std::string arm7tdmi_funclt_trap::LDRB_off::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::LDRB_off::LDRB_off( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 \
    * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), LS_reg_op(CPSR, MP_ID, RB, \
    SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::LDRB_off::~LDRB_off(){

}
unsigned int arm7tdmi_funclt_trap::umull_Instr::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);

    //Perform the operation
    result = (unsigned long long)(((unsigned long long)((unsigned int)rm)) * ((unsigned \
        long long)((unsigned int)rs)));
    //Check if I have to update the processor flags
    rd = (unsigned int)((result >> 32) & 0x00000000FFFFFFFF);
    REGS[rn] = (unsigned int)(result & 0x00000000FFFFFFFFLL);

    if((rs & 0xFFFFFF00) == 0x0 || (rs & 0xFFFFFF00) == 0xFFFFFF00){
        stall(3);
    }
    else if((rs & 0xFFFF0000) == 0x0 || (rs & 0xFFFF0000) == 0xFFFF0000){
        stall(4);
    }
    else if((rs & 0xFF000000) == 0x0 || (rs & 0xFF000000) == 0xFF000000){
        stall(5);
    }
    else{
        stall(6);
    }
    this->UpdatePSRmul_64(this->rd, this->rd_bit, this->s, this->carry, this->result);
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::umull_Instr::replicate() const throw(){
    return new umull_Instr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::umull_Instr::getInstructionName() const throw(){
    return "umull_Instr";
}

unsigned int arm7tdmi_funclt_trap::umull_Instr::getId() const throw(){
    return 36;
}

void arm7tdmi_funclt_trap::umull_Instr::setParams( const unsigned int & bitString \
    ) throw(){
    this->rd_bit = (bitString & 0xf0000) >> 16;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rs_bit = (bitString & 0xf00) >> 8;
    this->rs.directSetAlias(this->REGS[this->rs_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->rn = (bitString & 0xf000) >> 12;
}

std::string arm7tdmi_funclt_trap::umull_Instr::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::umull_Instr::umull_Instr( Reg32_0 & CPSR, Reg32_1 & MP_ID, \
    Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, \
    Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory \
    & instrMem, TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, \
    LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRmul_64_op(CPSR, \
    MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
    dataMem){

}

arm7tdmi_funclt_trap::umull_Instr::~umull_Instr(){

}
unsigned int arm7tdmi_funclt_trap::MVN_i::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_imm(this->rotate, this->immediate, this->operand, this->carry);

    result = ~operand;
    rd = result;
    this->UpdatePSRBit(this->rn, this->rn_bit, this->rd, this->rd_bit, this->s, this->carry, \
        this->result);


    if(rd_bit == 15){
        //In case the destination register is the program counter I have to
        //specify that I have a latency of two clock cycles
        stall(2);
        flush();
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::MVN_i::replicate() const throw(){
    return new MVN_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::MVN_i::getInstructionName() const throw(){
    return "MVN_i";
}

unsigned int arm7tdmi_funclt_trap::MVN_i::getId() const throw(){
    return 45;
}

void arm7tdmi_funclt_trap::MVN_i::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->rotate = (bitString & 0xf00) >> 8;
    this->immediate = (bitString & 0xff);
}

std::string arm7tdmi_funclt_trap::MVN_i::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::MVN_i::MVN_i( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, \
    Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias & \
    LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRBit_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), \
    DPI_imm_op(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, \
    PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::MVN_i::~MVN_i(){

}
unsigned int arm7tdmi_funclt_trap::MVN_sr::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_reg_shift(this->rm, this->rm_bit, this->rs, this->rs_bit, this->shift_op, \
        this->operand, this->carry);

    result = ~operand;
    rd = result;
    this->UpdatePSRBit(this->rn, this->rn_bit, this->rd, this->rd_bit, this->s, this->carry, \
        this->result);


    if(rd_bit == 15){
        //In case the destination register is the program counter I have to
        //specify that I have a latency of two clock cycles
        stall(2);
        flush();
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::MVN_sr::replicate() const throw(){
    return new MVN_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::MVN_sr::getInstructionName() const throw(){
    return "MVN_sr";
}

unsigned int arm7tdmi_funclt_trap::MVN_sr::getId() const throw(){
    return 44;
}

void arm7tdmi_funclt_trap::MVN_sr::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->rs_bit = (bitString & 0xf00) >> 8;
    this->rs.directSetAlias(this->REGS[this->rs_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->shift_op = (bitString & 0x60) >> 5;
}

std::string arm7tdmi_funclt_trap::MVN_sr::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::MVN_sr::MVN_sr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * \
    RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRBit_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), \
    DPI_reg_shift_op(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
    LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::MVN_sr::~MVN_sr(){

}
unsigned int arm7tdmi_funclt_trap::BRANCH::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);

    if(l == 1) {
        LINKR = PC - 4;
    }
    PC = PC + (((int)SignExtend(offset, 24)) << 2);
    stall(2);
    flush();
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::BRANCH::replicate() const throw(){
    return new BRANCH(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::BRANCH::getInstructionName() const throw(){
    return "BRANCH";
}

unsigned int arm7tdmi_funclt_trap::BRANCH::getId() const throw(){
    return 9;
}

void arm7tdmi_funclt_trap::BRANCH::setParams( const unsigned int & bitString ) throw(){
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->l = (bitString & 0x1000000) >> 24;
    this->offset = (bitString & 0xffffff);
}

std::string arm7tdmi_funclt_trap::BRANCH::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::BRANCH::BRANCH( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * \
    RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::BRANCH::~BRANCH(){

}
unsigned int arm7tdmi_funclt_trap::EOR_si::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_shift_imm(this->rm, this->rm_bit, this->shift_amm, this->shift_op, this->operand, \
        this->carry);

    result = rn ^ operand;
    rd = result;
    this->UpdatePSRBit(this->rn, this->rn_bit, this->rd, this->rd_bit, this->s, this->carry, \
        this->result);


    if(rd_bit == 15){
        //In case the destination register is the program counter I have to
        //specify that I have a latency of two clock cycles
        stall(2);
        flush();
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::EOR_si::replicate() const throw(){
    return new EOR_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::EOR_si::getInstructionName() const throw(){
    return "EOR_si";
}

unsigned int arm7tdmi_funclt_trap::EOR_si::getId() const throw(){
    return 20;
}

void arm7tdmi_funclt_trap::EOR_si::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rm_bit = (bitString & 0xf);
    this->rm.directSetAlias(this->REGS[this->rm_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->shift_amm = (bitString & 0xf80) >> 7;
    this->shift_op = (bitString & 0x60) >> 5;
}

std::string arm7tdmi_funclt_trap::EOR_si::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::EOR_si::EOR_si( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * \
    RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRBit_op(CPSR, MP_ID, \
    RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), \
    DPI_shift_imm_op(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
    LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::EOR_si::~EOR_si(){

}
unsigned int arm7tdmi_funclt_trap::STRB_imm::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->LS_imm(this->rn, this->rn_bit, this->p, this->w, this->u, this->immediate, \
        this->address);

    dataMem.write_byte(address, (unsigned char)(rd & 0x000000FF));
    stall(1);
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::STRB_imm::replicate() const throw(){
    return new STRB_imm(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::STRB_imm::getInstructionName() const throw(){
    return "STRB_imm";
}

unsigned int arm7tdmi_funclt_trap::STRB_imm::getId() const throw(){
    return 70;
}

void arm7tdmi_funclt_trap::STRB_imm::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->p = (bitString & 0x1000000) >> 24;
    this->u = (bitString & 0x800000) >> 23;
    this->w = (bitString & 0x200000) >> 21;
    this->immediate = (bitString & 0xfff);
}

std::string arm7tdmi_funclt_trap::STRB_imm::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::STRB_imm::STRB_imm( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 \
    * RB, Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias \
    & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), LS_imm_op(CPSR, MP_ID, RB, \
    SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::STRB_imm::~STRB_imm(){

}
unsigned int arm7tdmi_funclt_trap::RSC_i::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_imm(this->rotate, this->immediate, this->operand, this->carry);

    operand1 = (int)operand;
    operand2 = (int)rn;
    rd =  operand1 - operand2;
    if (CPSR[key_C] == 0){
        rd = ((int)rd) -1;
    }
    this->UpdatePSRSubWithCarry(this->rn, this->rn_bit, this->rd, this->rd_bit, this->s, \
        this->operand1, this->operand2, this->carry);


    if(rd_bit == 15){
        //In case the destination register is the program counter I have to
        //specify that I have a latency of two clock cycles
        stall(2);
        flush();
    }
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::RSC_i::replicate() const throw(){
    return new RSC_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::RSC_i::getInstructionName() const throw(){
    return "RSC_i";
}

unsigned int arm7tdmi_funclt_trap::RSC_i::getId() const throw(){
    return 54;
}

void arm7tdmi_funclt_trap::RSC_i::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->s = (bitString & 0x100000) >> 20;
    this->rotate = (bitString & 0xf00) >> 8;
    this->immediate = (bitString & 0xff);
}

std::string arm7tdmi_funclt_trap::RSC_i::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::RSC_i::RSC_i( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, \
    Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias & \
    LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), UpdatePSRSubWithCarry_op(CPSR, \
    MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
    dataMem), DPI_imm_op(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
    LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::RSC_i::~RSC_i(){

}
unsigned int arm7tdmi_funclt_trap::TEQ_i::behavior(){
    this->totalInstrCycles = 0;
    this->IncrementPC();
    this->condition_check(this->cond);
    this->DPI_imm(this->rotate, this->immediate, this->operand, this->carry);

    result = rn ^ operand;
    UpdatePSRBitM(result, carry);
    return this->totalInstrCycles;
}

Instruction * arm7tdmi_funclt_trap::TEQ_i::replicate() const throw(){
    return new TEQ_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, \
        LR_FIQ, PC, REGS, instrMem, dataMem);
}

std::string arm7tdmi_funclt_trap::TEQ_i::getInstructionName() const throw(){
    return "TEQ_i";
}

unsigned int arm7tdmi_funclt_trap::TEQ_i::getId() const throw(){
    return 63;
}

void arm7tdmi_funclt_trap::TEQ_i::setParams( const unsigned int & bitString ) throw(){
    this->rd_bit = (bitString & 0xf000) >> 12;
    this->rd.directSetAlias(this->REGS[this->rd_bit]);
    this->rn_bit = (bitString & 0xf0000) >> 16;
    this->rn.directSetAlias(this->REGS[this->rn_bit]);
    this->cond = (bitString & 0xf0000000L) >> 28;
    this->rotate = (bitString & 0xf00) >> 8;
    this->immediate = (bitString & 0xff);
}

std::string arm7tdmi_funclt_trap::TEQ_i::getMnemonic() const throw(){
    std::ostringstream oss (std::ostringstream::out);
    oss << "T";
    oss << "O";
    oss << "D";
    oss << "O";
    return oss.str();
}

arm7tdmi_funclt_trap::TEQ_i::TEQ_i( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, \
    Reg32_0 * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_IRQ, Alias & \
    LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, \
    TLMMemory & dataMem ) : Instruction(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_IRQ, \
    LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem), DPI_imm_op(CPSR, MP_ID, RB, \
    SPSR, FP, SPTR, LINKR, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem){

}

arm7tdmi_funclt_trap::TEQ_i::~TEQ_i(){

}

