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


#ifndef INSTRUCTIONS_HPP
#define INSTRUCTIONS_HPP

#include <instructionBase.hpp>
#include <string>
#include <customExceptions.hpp>
#include <trap_utils.hpp>
#include <registers.hpp>
#include <alias.hpp>
#include <externalPorts.hpp>
#include <systemc.h>
#include <sstream>

#define FUNC_MODEL
#define LT_IF
namespace arm9tdmi_funclt_trap{

    class Instruction : public InstructionBase{

        protected:
        Reg32_0 & CPSR;
        Reg32_1 & MP_ID;
        Reg32_1 * RB;
        Reg32_0 * SPSR;
        Alias & FP;
        Alias & SPTR;
        Alias & LINKR;
        Alias & SP_SVC;
        Alias & LR_SVC;
        Alias & SP_ABT;
        Alias & LR_ABT;
        Alias & SP_IRQ;
        Alias & LR_IRQ;
        Alias & SP_FIQ;
        Alias & LR_FIQ;
        Alias & PC;
        Alias * REGS;
        TLMMemory & instrMem;
        TLMMemory & dataMem;

        public:
        Instruction( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias \
            & FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        virtual unsigned int behavior() = 0;
        virtual Instruction * replicate() const throw() = 0;
        virtual void setParams( const unsigned int & bitString ) throw() = 0;
        virtual std::string getInstructionName() const throw() = 0;
        virtual std::string getMnemonic() const throw() = 0;
        virtual unsigned int getId() const throw() = 0;
        inline void annull(){
            throw annull_exception();
        }
        inline void flush(){

        }
        inline void stall( const unsigned int & numCycles ){
            this->totalInstrCycles += numCycles;
        }
        void restoreSPSR();
        void updateAliases( unsigned int fromMode, unsigned int toMode );
        unsigned int ArithmeticShiftRight( unsigned int shift_amm, unsigned int toShift );
        unsigned int RotateRight( unsigned int rotate_amm, unsigned int toRotate );
        unsigned int LSRegShift( unsigned int shift_type, unsigned int shift_amm, unsigned \
            int toShift );
        unsigned int SignExtend( unsigned int bitSeq, unsigned int bitSeq_length );
        void UpdatePSRBitM( unsigned int result, bool carry );
        void UpdatePSRAddInner( unsigned int operand1, unsigned int operand2, bool carry );
        void UpdatePSRSubInner( unsigned int operand1, unsigned int operand2, bool carry );
        unsigned int totalInstrCycles;
        virtual ~Instruction();
    };

};

namespace arm9tdmi_funclt_trap{

    class condition_check_op : public virtual Instruction{

        protected:
        inline void condition_check( unsigned int & cond ){

            if(cond != 0xE){
                // Of course the previous if is redundand, the case would be enough, but
                // since cond == 0xE is the most common situation, treating it in a particular way
                // makes the code a bit faster
                switch(cond){
                    case 0x0:{
                        // EQ
                        if (CPSR[key_Z] == 0x0){
                            annull();
                        }
                        break;
                    }
                    case 0x1:{
                        // NE
                        if (CPSR[key_Z] != 0x0){
                            annull();
                        }
                        break;
                    }
                    case 0x2:{
                        // CS/HS
                        if (CPSR[key_C] == 0x0){
                            annull();
                        }
                        break;
                    }
                    case 0x3:{
                        // CC/LO
                        if (CPSR[key_C] != 0x0){
                            annull();
                        }
                        break;
                    }
                    case 0x4:{
                        // MI
                        if (CPSR[key_N] == 0x0){
                            annull();
                        }
                        break;
                    }
                    case 0x5:{
                        // PL
                        if (CPSR[key_N] != 0x0){
                            annull();
                        }
                        break;
                    }
                    case 0x6:{
                        // VS
                        if (CPSR[key_V] == 0x0){
                            annull();
                        }
                        break;
                    }
                    case 0x7:{
                        // VC
                        if (CPSR[key_V] != 0x0){
                            annull();
                        }
                        break;
                    }
                    case 0x8:{
                        // HI
                        if ((CPSR & 0x60000000) != 0x20000000){
                            annull();
                        }
                        break;
                    }
                    case 0x9:{
                        // LS
                        if ((CPSR & 0x60000000) == 0x20000000){
                            annull();
                        }
                        break;
                    }
                    case 0xA:{
                        // GE
                        if (CPSR[key_V] != CPSR[key_N]){
                            annull();
                        }
                        break;
                    }
                    case 0xB:{
                        // LT
                        if (CPSR[key_V] == CPSR[key_N]){
                            annull();
                        }
                        break;
                    }
                    case 0xC:{
                        // GT
                        if ((CPSR[key_Z] != 0x0) || (CPSR[key_V] != CPSR[key_N])){
                            annull();
                        }
                        break;
                    }
                    case 0xD:{
                        // LE
                        if ((CPSR[key_Z] == 0x0) && (CPSR[key_V] == CPSR[key_N])){
                            annull();
                        }
                        break;
                    }
                    case 0xE:{
                        // AL
                        break;
                    }
                    default:{
                        // Not recognized condition code
                        THROW_EXCEPTION("Unrecognized condition code: " << cond);
                        break;
                    }
                }
            }
        }

        public:
        condition_check_op( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, \
            Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & \
            SP_ABT, Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, \
            Alias & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        virtual ~condition_check_op();
    };

};

namespace arm9tdmi_funclt_trap{

    class DPI_shift_imm_op : public virtual Instruction{

        protected:
        inline void DPI_shift_imm( Alias & rm, unsigned int & rm_bit, unsigned int & shift_amm, \
            unsigned int & shift_op, unsigned int & operand, bool & carry ){

            if(shift_op == 0 && shift_amm == 0){
                operand = rm;
                carry = (CPSR[key_C] != 0);
            }
            else{
                switch(shift_op) {
                    case 0x0:{
                        // Logical shift left
                        operand = rm << shift_amm;
                        carry = ((rm & (0x01 << (32 - shift_amm))) != 0);
                    break;}
                    case 0x1:{
                        // Logical shift right
                        if (shift_amm == 0){
                            //Which means shift of 32 bits, the whole number.
                            operand = 0;
                            carry = ((rm & 0x80000000) != 0);
                        }
                        else {
                            operand = rm >> shift_amm;
                            carry = ((rm & (0x01 << (shift_amm - 1))) != 0);
                        }
                    break;}
                    case 0x2:{
                        // Arithmetic shift right
                        if (shift_amm == 0){
                            //Which means shift of 32 bits
                            if ((rm & 0x80000000) == 0x0){
                                operand = 0;
                                carry = false;
                            }
                            else{
                                operand = 0xFFFFFFFF;
                                carry = true;
                            }
                        }
                        else {
                            operand = ArithmeticShiftRight(shift_amm, rm);
                            carry = ((rm & (0x01 << (shift_amm - 1))) != 0);
                        }
                    break;}
                    case 0x3:{
                        // Rotate right
                        if (shift_amm == 0){
                            //Rotate rigth with extend
                            operand = (rm >> 1) | ((CPSR[key_C]) << 31);
                            carry = ((rm & 0x00000001) != 0);
                        }
                        else {
                            operand = RotateRight(shift_amm, rm);
                            carry = ((rm & (0x01 << (shift_amm - 1))) != 0);
                        }
                    break;}
                    default:{
                        THROW_EXCEPTION("Shift operation " << shift_op << " not valid");
                    break;}
                }
            }
        }

        public:
        DPI_shift_imm_op( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, \
            Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & \
            SP_ABT, Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, \
            Alias & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        virtual ~DPI_shift_imm_op();
    };

};

namespace arm9tdmi_funclt_trap{

    class UpdatePSRSub_op : public virtual Instruction{

        protected:
        inline void UpdatePSRSub( Alias & rn, unsigned int & rn_bit, Alias & rd, unsigned \
            int & rd_bit, unsigned int & s, unsigned int & operand1, unsigned int & operand2, \
            bool & carry ){

            if (s == 0x1){
                if(rd_bit == 15){
                    // In case the destination register is the program counter,
                    // I have to switch to the saved program status register
                    restoreSPSR();
                }
                else{
                    //Here I have to normally update the flags
                    //We don't care about carry bit so set it to 0
                    UpdatePSRSubInner(operand1, operand2, 0);
                }
            }
        }

        public:
        UpdatePSRSub_op( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias \
            & FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        virtual ~UpdatePSRSub_op();
    };

};

namespace arm9tdmi_funclt_trap{

    class DPI_reg_shift_op : public virtual Instruction{

        protected:
        inline void DPI_reg_shift( Alias & rm, unsigned int & rm_bit, Alias & rs, unsigned \
            int & rs_bit, unsigned int & shift_op, unsigned int & operand, bool & carry ){

            unsigned int shift_amm = rs & 0x000000FF;
            switch(shift_op) {
                case 0x0:{
                    // Logical shift left
                    if(shift_amm == 0){
                        operand = rm;
                        carry = (CPSR[key_C] != 0);
                    }
                    else if (shift_amm < 32){
                        operand = rm << shift_amm;
                        carry = ((rm & (0x01 << (32 - shift_amm))) != 0);
                    }
                    else if (shift_amm == 32){
                        operand = 0;
                        carry = ((rm & 0x00000001) != 0);
                    }
                    else if (shift_amm > 32){
                        operand = 0;
                        carry = false;
                    }
                break;}
                case 0x1:{
                    // Logical shift right
                    if(shift_amm == 0){
                        operand = rm;
                        carry = (CPSR[key_C] != 0);
                    }
                    else if (shift_amm < 32){
                        operand = rm >> shift_amm;
                        carry = ((rm & (0x01 << (shift_amm - 1))) != 0);
                    }
                    else if (shift_amm == 32){
                        operand = 0;
                        carry = ((rm & 0x80000000) != 0);
                    }
                    else if (shift_amm > 32){
                        operand = 0;
                        carry = 0;
                    }
                break;}
                case 0x2:{
                    // Arithmetic shift right
                    if(shift_amm == 0){
                        operand = rm;
                        carry = (CPSR[key_C] != 0);
                    }
                    else if (shift_amm < 32){
                        operand = ArithmeticShiftRight(shift_amm, rm);
                        carry = ((rm & (0x01 << (shift_amm - 1)))!= 0);
                    }
                    else{
                        // shiftamount >= 32
                        carry = ((rm & 0x80000000) != 0);
                        if (!carry){
                            operand = 0x0;
                        }
                        else{
                            operand = 0xFFFFFFFF;
                        }
                    }
                break;}
                case 0x3:{
                    // Rotate right
                    if(shift_amm == 0){
                        operand = rm;
                        carry = (CPSR[key_C] != 0);
                    }
                    else if((shift_amm & 0x0000001F) == 0){
                        operand = rm;
                        carry = ((rm & 0x80000000) != 0);
                    }
                    else{
                        operand = RotateRight(shift_amm & 0x0000001F, rm);
                        carry = ((rm & (0x01 << ((shift_amm & 0x0000001F) -1))) != 0);
                    }
                break;}
                default:{
                    THROW_EXCEPTION("Shift operation " << shift_op << " not valid");
                break;}
            }
            //Ok, this operation is such that it stall the pipeline for 1 stage;
            //note how the stall is not performed at this exact moment, but when
            //all the operations of the stage have ended
            stall(1);
        }

        public:
        DPI_reg_shift_op( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, \
            Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & \
            SP_ABT, Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, \
            Alias & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        virtual ~DPI_reg_shift_op();
    };

};

namespace arm9tdmi_funclt_trap{

    class UpdatePSRSum_op : public virtual Instruction{

        protected:
        inline void UpdatePSRSum( Alias & rn, unsigned int & rn_bit, Alias & rd, unsigned \
            int & rd_bit, unsigned int & s, unsigned int & operand1, unsigned int & operand2, \
            bool & carry ){

            if (s == 0x1){
                if(rd_bit == 15){
                    // In case the destination register is the program counter,
                    // I have to switch to the saved program status register
                    restoreSPSR();
                }
                else{
                    //Here I have to normally update the flags
                    //We don't care about carry bit
                    UpdatePSRAddInner(operand1, operand2, 0);
                }
            }
        }

        public:
        UpdatePSRSum_op( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias \
            & FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        virtual ~UpdatePSRSum_op();
    };

};

namespace arm9tdmi_funclt_trap{

    class UpdatePSRBit_op : public virtual Instruction{

        protected:
        inline void UpdatePSRBit( Alias & rn, unsigned int & rn_bit, Alias & rd, unsigned \
            int & rd_bit, unsigned int & s, bool & carry, unsigned int & result ){

            if (s == 0x1){
                if(rd_bit == 15){
                    // In case the destination register is the program counter,
                    // I have to switch to the saved program status register
                    restoreSPSR();
                }
                else{
                    //Here I have to normally update the flags
                    UpdatePSRBitM(result, carry);
                }
            }
        }

        public:
        UpdatePSRBit_op( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias \
            & FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        virtual ~UpdatePSRBit_op();
    };

};

namespace arm9tdmi_funclt_trap{

    class DPI_imm_op : public virtual Instruction{

        protected:
        inline void DPI_imm( unsigned int & rotate, unsigned int & immediate, unsigned int \
            & operand, bool & carry ){

            if (rotate == 0){
                operand = immediate;
                carry = (CPSR[key_C] != 0);
            }
            else{
                operand = RotateRight(rotate*2, immediate);
                carry = (operand & 0x80000000) != 0 ;
            }
        }

        public:
        DPI_imm_op( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias \
            & FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        virtual ~DPI_imm_op();
    };

};

namespace arm9tdmi_funclt_trap{

    class LS_imm_op : public virtual Instruction{

        protected:
        inline void LS_imm( Alias & rn, unsigned int & rn_bit, unsigned int & p, unsigned \
            int & w, unsigned int & u, unsigned int & immediate, unsigned int & address ){

            address = 0;
            if((p == 1) && (w == 0)) {
                // immediate offset
                if(u == 1){
                    address = rn + immediate;
                }
                else{
                    address = rn - immediate;
                }
            }
            else if((p == 1) && (w == 1)) {
                // immediate pre-indexed
                if(u == 1){
                    address = rn + immediate;
                }
                else{
                    address = rn - immediate;
                }
                rn = address;
            }
            else if(p == 0) {
                // immediate post-indexed
                //Post-index means that the address calculated doesn't include the
                //offset
                address = rn;

                if(u == 1){
                    rn += immediate;
                }
                else{
                    rn -= immediate;
                }
            }
            else{
                THROW_EXCEPTION("Load/Store immediate --> Unknown indexing mode");
            }
        }

        public:
        LS_imm_op( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & \
            FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        virtual ~LS_imm_op();
    };

};

namespace arm9tdmi_funclt_trap{

    class LS_sh_op : public virtual Instruction{

        protected:
        inline void LS_sh( Alias & rn, unsigned int & rn_bit, unsigned int & p, unsigned \
            int & w, unsigned int & u, unsigned int & i, unsigned int & addr_mode0, unsigned \
            int & addr_mode1, unsigned int & address ){
            {
                unsigned int off8;

                address = 0;
                //First of all I check whether this instruction uses an immediate or a register offset
                if(i == 1){
                    //Immediate offset
                    off8 = ((addr_mode0 << 4) | addr_mode1);
                    if((p == 1) && (w == 0)){
                        //immediate offset normal mode
                        if(u == 1){
                            address = rn + off8;
                        }
                        else{
                            address = rn - off8;
                        }
                    }
                    else if((p == 1) && (w == 1)){
                        //immediate pre-indexing
                        if(u == 1){
                            address = rn + off8;
                        }
                        else{
                            address = rn - off8;
                        }
                        rn = address;
                    }
                    else if((p == 0) && (w == 0)){
                        // Immediate post indexing
                        address = rn;

                        if(u == 1){
                            rn = address + off8;
                        }
                        else{
                            rn = address - off8;
                        }
                    }
                }
                else{
                    //register offset
                    unsigned int regVal = REGS[addr_mode1];

                    if((p == 1) && (w == 0)){
                        //register normal mode
                        if(u == 1){
                            address = rn + regVal;
                        }
                        else{
                            address = rn - regVal;
                        }
                    }
                    else if((p == 1) && (w == 1)){
                        //register pre-indexing
                        if(u == 1){
                            address = rn + regVal;
                        }
                        else{
                            address = rn - regVal;
                        }
                        rn = address;
                    }
                    else if((p == 0) && (w == 0)){
                        // register post indexing
                        address = rn;

                        if(u == 1){
                            rn += regVal;
                        }
                        else{
                            rn -= regVal;
                        }
                    }
                }
            }
        }

        public:
        LS_sh_op( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & \
            FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        virtual ~LS_sh_op();
    };

};

namespace arm9tdmi_funclt_trap{

    class ls_imm_PI_op : public virtual Instruction{

        protected:
        inline void ls_imm_PI( Alias & rn, unsigned int & rn_bit, unsigned int & u, unsigned \
            int & immediate, unsigned int & address ){

            // immediate post-indexed
            //Post-index means that the address calculated doesn't include the
            //offset
            address = rn;
            if(u == 1){
                rn += immediate;
            }
            else{
                rn -= immediate;
            }
        }

        public:
        ls_imm_PI_op( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias \
            & FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        virtual ~ls_imm_PI_op();
    };

};

namespace arm9tdmi_funclt_trap{

    class LS_reg_PI_op : public virtual Instruction{

        protected:
        inline void LS_reg_PI( Alias & rn, unsigned int & rn_bit, Alias & rm, unsigned int \
            & rm_bit, unsigned int & u, unsigned int & shift_amm, unsigned int & shift_op, unsigned \
            int & address ){

            //post-indexed
            address = rn;
            if(u == 1){
                rn += LSRegShift(shift_op, shift_amm, rm);
            }
            else{
                rn -= LSRegShift(shift_op, shift_amm, rm);
            }
        }

        public:
        LS_reg_PI_op( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias \
            & FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        virtual ~LS_reg_PI_op();
    };

};

namespace arm9tdmi_funclt_trap{

    class UpdatePSRmul_64_op : public virtual Instruction{

        protected:
        inline void UpdatePSRmul_64( Alias & rd, unsigned int & rd_bit, unsigned int & s, \
            bool & carry, sc_dt::uint64 & result ){

            if (s == 0x1){
                if(rd_bit == 15){
                    //In case the destination register is the program counter I have to
                    //specify that I have a latency of two clock cycles
                    stall(2);
                    flush();
                }else{
                    //Here I have to normally update the flags
                    // N flag if the results is negative
                    CPSR[key_N] = ((rd & 0x80000000) != 0);
                    //Update flag Z if the result is 0
                    CPSR[key_Z] = (result == 0);
                    //No updates performed to the C flag.
                    //No updates performed to the V flag.
                }
            }
        }

        public:
        UpdatePSRmul_64_op( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, \
            Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & \
            SP_ABT, Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, \
            Alias & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        virtual ~UpdatePSRmul_64_op();
    };

};

namespace arm9tdmi_funclt_trap{

    class UpdatePSRSumWithCarry_op : public virtual Instruction{

        protected:
        inline void UpdatePSRSumWithCarry( Alias & rn, unsigned int & rn_bit, Alias & rd, \
            unsigned int & rd_bit, unsigned int & s, unsigned int & operand1, unsigned int & \
            operand2, bool & carry ){

            if (s == 0x1){
                if(rd_bit == 15){
                    // In case the destination register is the program counter,
                    // I have to switch to the saved program status register
                    restoreSPSR();
                }
                else{
                    //Here I have to normally update the flags
                    //Carry bit is counted
                    carry = CPSR[key_C];
                    UpdatePSRAddInner(operand1, operand2, carry);
                }
            }
        }

        public:
        UpdatePSRSumWithCarry_op( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 \
            * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, \
            Alias & SP_ABT, Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias \
            & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        virtual ~UpdatePSRSumWithCarry_op();
    };

};

namespace arm9tdmi_funclt_trap{

    class UpdatePSRSubWithCarry_op : public virtual Instruction{

        protected:
        inline void UpdatePSRSubWithCarry( Alias & rn, unsigned int & rn_bit, Alias & rd, \
            unsigned int & rd_bit, unsigned int & s, unsigned int & operand1, unsigned int & \
            operand2, bool & carry ){

            if (s == 0x1){
                if(rd_bit == 15){
                    // In case the destination register is the program counter,
                    // I have to switch to the saved program status register
                    restoreSPSR();
                }
                else{
                    //Here I have to normally update the flags
                    //Carry bit is counted
                    carry = (CPSR[key_C] == 0);
                    UpdatePSRSubInner(operand1, operand2, carry);
                }
            }
        }

        public:
        UpdatePSRSubWithCarry_op( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 \
            * SPSR, Alias & FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, \
            Alias & SP_ABT, Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias \
            & LR_FIQ, Alias & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        virtual ~UpdatePSRSubWithCarry_op();
    };

};

namespace arm9tdmi_funclt_trap{

    class LSM_reglist_op : public virtual Instruction{

        protected:
        inline void LSM_reglist( Alias & rn, unsigned int & rn_bit, unsigned int & p, unsigned \
            int & u, unsigned int & reg_list, unsigned int & start_address, unsigned int & wb_address \
            ){

            //Now I calculate the start and end addresses of the
            //mem area where I will save the registers.
            unsigned int setbits = 0;
            for (unsigned int i = 0; i < 16; i++) {
                if ((reg_list & (1 << i)) != 0)
                setbits++;
            }
            if((p == 0) && (u == 1)) {
                // increment after
                start_address = rn;
                wb_address = start_address + (setbits * 4);
            }
            else if((p == 1) && (u == 1)) {
                // increment before
                start_address = rn + 4;
                wb_address = start_address - 4 + (setbits * 4);
            }
            else if((p == 0) && (u == 0)) {
                // decrement after
                start_address = rn - (setbits * 4) + 4;
                wb_address = start_address + 4;
            }
            else {
                // decrement before
                start_address = rn - (setbits * 4);
                wb_address = start_address;
            }

            //Note that the addresses considered are word aligned, so the last 2 bit
            //of the addresses are not considered.
            start_address &= 0xFFFFFFFC;
            wb_address &= 0xFFFFFFFC;
        }

        public:
        LSM_reglist_op( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias \
            & FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        virtual ~LSM_reglist_op();
    };

};

namespace arm9tdmi_funclt_trap{

    class UpdatePSRmul_op : public virtual Instruction{

        protected:
        inline void UpdatePSRmul( Alias & rd, unsigned int & rd_bit, unsigned int & s ){

            if (s == 0x1){
                if(rd_bit == 15){
                    // In case the destination register is the program counter,
                    // I have to switch to the saved program status register
                    restoreSPSR();
                }
                else{
                    //Here I have to normally update the flags
                    // N flag if the results is negative
                    CPSR[key_N] = ((rd & 0x80000000) != 0);
                    //Update flag Z if the result is 0
                    CPSR[key_Z] = (rd == 0);
                    //No updates performed to the C flag.
                    //No updates performed to the V flag.
                }
            }
        }

        public:
        UpdatePSRmul_op( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias \
            & FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        virtual ~UpdatePSRmul_op();
    };

};

namespace arm9tdmi_funclt_trap{

    class LS_reg_op : public virtual Instruction{

        protected:
        inline void LS_reg( Alias & rn, unsigned int & rn_bit, Alias & rm, unsigned int & \
            rm_bit, unsigned int & p, unsigned int & w, unsigned int & u, unsigned int & shift_amm, \
            unsigned int & shift_op, unsigned int & address ){

            if ((p == 1) && (w == 0)) {
                // offset
                if(u == 1){
                    address = rn + LSRegShift(shift_op, shift_amm, rm);
                }
                else{
                    address = rn - LSRegShift(shift_op, shift_amm, rm);
                }
            }
            else if((p == 1) && (w == 1)){
                // pre-indexed
                if(u == 1){
                    address = rn + LSRegShift(shift_op, shift_amm, rm);
                }
                else{
                    address = rn - LSRegShift(shift_op, shift_amm, rm);
                }
                rn = address;
            }
            else if(p == 0) {
                // post-indexed
                address = rn;
                if(u == 1){
                    rn += LSRegShift(shift_op, shift_amm, rm);
                }
                else{
                    rn -= LSRegShift(shift_op, shift_amm, rm);
                }
            }
            else{
                THROW_EXCEPTION("Load/Store register --> Unknown indexing mode");
            }
        }

        public:
        LS_reg_op( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & \
            FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        virtual ~LS_reg_op();
    };

};

namespace arm9tdmi_funclt_trap{

    class InvalidInstr : public Instruction{

        public:
        InvalidInstr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias \
            & FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getInstructionName() const throw();
        std::string getMnemonic() const throw();
        unsigned int getId() const throw();
        virtual ~InvalidInstr();
    };

};

namespace arm9tdmi_funclt_trap{

    class TST_si : public condition_check_op, public DPI_shift_imm_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int shift_amm;
        unsigned int shift_op;

        protected:
        unsigned int operand;
        bool carry;
        unsigned int result;

        public:
        TST_si( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~TST_si();
    };

};

namespace arm9tdmi_funclt_trap{

    class SUB_sr : public UpdatePSRSub_op, public condition_check_op, public DPI_reg_shift_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        Alias rs;
        unsigned int rs_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int shift_op;

        protected:
        unsigned int operand1;
        unsigned int operand2;
        bool carry;
        unsigned int operand;

        public:
        SUB_sr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SUB_sr();
    };

};

namespace arm9tdmi_funclt_trap{

    class ADD_si : public UpdatePSRSum_op, public condition_check_op, public DPI_shift_imm_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int shift_amm;
        unsigned int shift_op;

        protected:
        unsigned int operand1;
        unsigned int operand2;
        bool carry;
        unsigned int operand;

        public:
        ADD_si( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~ADD_si();
    };

};

namespace arm9tdmi_funclt_trap{

    class BIC_si : public UpdatePSRBit_op, public condition_check_op, public DPI_shift_imm_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int shift_amm;
        unsigned int shift_op;

        protected:
        bool carry;
        unsigned int result;
        unsigned int operand;

        public:
        BIC_si( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~BIC_si();
    };

};

namespace arm9tdmi_funclt_trap{

    class CMP_i : public condition_check_op, public DPI_imm_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int rotate;
        unsigned int immediate;

        protected:
        unsigned int operand;
        bool carry;

        public:
        CMP_i( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~CMP_i();
    };

};

namespace arm9tdmi_funclt_trap{

    class BRANCHX : public condition_check_op{
        private:
        Alias rm;
        unsigned int rm_bit;
        unsigned int cond;

        public:
        BRANCHX( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~BRANCHX();
    };

};

namespace arm9tdmi_funclt_trap{

    class BIC_sr : public UpdatePSRBit_op, public condition_check_op, public DPI_reg_shift_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        Alias rs;
        unsigned int rs_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int shift_op;

        protected:
        bool carry;
        unsigned int result;
        unsigned int operand;

        public:
        BIC_sr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~BIC_sr();
    };

};

namespace arm9tdmi_funclt_trap{

    class ADD_sr : public UpdatePSRSum_op, public condition_check_op, public DPI_reg_shift_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        Alias rs;
        unsigned int rs_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int shift_op;

        protected:
        unsigned int operand1;
        unsigned int operand2;
        bool carry;
        unsigned int operand;

        public:
        ADD_sr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~ADD_sr();
    };

};

namespace arm9tdmi_funclt_trap{

    class SUB_si : public UpdatePSRSub_op, public condition_check_op, public DPI_shift_imm_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int shift_amm;
        unsigned int shift_op;

        protected:
        unsigned int operand1;
        unsigned int operand2;
        bool carry;
        unsigned int operand;

        public:
        SUB_si( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SUB_si();
    };

};

namespace arm9tdmi_funclt_trap{

    class TST_sr : public condition_check_op, public DPI_reg_shift_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        Alias rs;
        unsigned int rs_bit;
        unsigned int cond;
        unsigned int shift_op;

        protected:
        unsigned int operand;
        bool carry;
        unsigned int result;

        public:
        TST_sr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~TST_sr();
    };

};

namespace arm9tdmi_funclt_trap{

    class LDR_imm : public condition_check_op, public LS_imm_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int p;
        unsigned int u;
        unsigned int w;
        unsigned int immediate;

        protected:
        unsigned int address;
        unsigned int memLastBits;
        unsigned int value;

        public:
        LDR_imm( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~LDR_imm();
    };

};

namespace arm9tdmi_funclt_trap{

    class ORR_si : public UpdatePSRBit_op, public condition_check_op, public DPI_shift_imm_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int shift_amm;
        unsigned int shift_op;

        protected:
        bool carry;
        unsigned int result;
        unsigned int operand;

        public:
        ORR_si( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~ORR_si();
    };

};

namespace arm9tdmi_funclt_trap{

    class ADD_i : public UpdatePSRSum_op, public condition_check_op, public DPI_imm_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int rotate;
        unsigned int immediate;

        protected:
        unsigned int operand1;
        unsigned int operand2;
        bool carry;
        unsigned int operand;

        public:
        ADD_i( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~ADD_i();
    };

};

namespace arm9tdmi_funclt_trap{

    class STR_imm : public condition_check_op, public LS_imm_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int p;
        unsigned int u;
        unsigned int w;
        unsigned int immediate;

        protected:
        unsigned int address;
        unsigned int memLastBits;
        unsigned int value;

        public:
        STR_imm( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~STR_imm();
    };

};

namespace arm9tdmi_funclt_trap{

    class mrs_Instr : public condition_check_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        unsigned int cond;
        unsigned int r;

        public:
        mrs_Instr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & \
            FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~mrs_Instr();
    };

};

namespace arm9tdmi_funclt_trap{

    class LDRSB_off : public condition_check_op, public LS_sh_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int p;
        unsigned int u;
        unsigned int i;
        unsigned int w;
        unsigned int l;
        unsigned int addr_mode0;
        unsigned int addr_mode1;

        protected:
        unsigned int address;

        public:
        LDRSB_off( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & \
            FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~LDRSB_off();
    };

};

namespace arm9tdmi_funclt_trap{

    class ORR_sr : public UpdatePSRBit_op, public condition_check_op, public DPI_reg_shift_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        Alias rs;
        unsigned int rs_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int shift_op;

        protected:
        bool carry;
        unsigned int result;
        unsigned int operand;

        public:
        ORR_sr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~ORR_sr();
    };

};

namespace arm9tdmi_funclt_trap{

    class LDRT_imm : public condition_check_op, public ls_imm_PI_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int u;
        unsigned int immediate;

        protected:
        unsigned int address;
        unsigned int memLastBits;

        public:
        LDRT_imm( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & \
            FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~LDRT_imm();
    };

};

namespace arm9tdmi_funclt_trap{

    class STRT_off : public condition_check_op, public LS_reg_PI_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int u;
        unsigned int shift_amm;
        unsigned int shift_op;

        protected:
        unsigned int address;

        public:
        STRT_off( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & \
            FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~STRT_off();
    };

};

namespace arm9tdmi_funclt_trap{

    class AND_sr : public UpdatePSRBit_op, public condition_check_op, public DPI_reg_shift_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        Alias rs;
        unsigned int rs_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int shift_op;

        protected:
        bool carry;
        unsigned int result;
        unsigned int operand;

        public:
        AND_sr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~AND_sr();
    };

};

namespace arm9tdmi_funclt_trap{

    class MOV_si : public UpdatePSRBit_op, public condition_check_op, public DPI_shift_imm_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int shift_amm;
        unsigned int shift_op;

        protected:
        bool carry;
        unsigned int result;
        unsigned int operand;

        public:
        MOV_si( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~MOV_si();
    };

};

namespace arm9tdmi_funclt_trap{

    class MOV_sr : public UpdatePSRBit_op, public condition_check_op, public DPI_reg_shift_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        Alias rs;
        unsigned int rs_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int shift_op;

        protected:
        bool carry;
        unsigned int result;
        unsigned int operand;

        public:
        MOV_sr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~MOV_sr();
    };

};

namespace arm9tdmi_funclt_trap{

    class umull_Instr : public UpdatePSRmul_64_op, public condition_check_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rs;
        unsigned int rs_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int rn;

        protected:
        bool carry;
        sc_dt::uint64 result;

        public:
        umull_Instr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias \
            & FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~umull_Instr();
    };

};

namespace arm9tdmi_funclt_trap{

    class AND_si : public UpdatePSRBit_op, public condition_check_op, public DPI_shift_imm_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int shift_amm;
        unsigned int shift_op;

        protected:
        bool carry;
        unsigned int result;
        unsigned int operand;

        public:
        AND_si( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~AND_si();
    };

};

namespace arm9tdmi_funclt_trap{

    class RSB_si : public UpdatePSRSub_op, public condition_check_op, public DPI_shift_imm_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int shift_amm;
        unsigned int shift_op;

        protected:
        unsigned int operand1;
        unsigned int operand2;
        bool carry;
        unsigned int operand;

        public:
        RSB_si( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~RSB_si();
    };

};

namespace arm9tdmi_funclt_trap{

    class MRC : public Instruction{
        private:
        unsigned int cond;
        unsigned int opcode1;
        unsigned int crn;
        unsigned int crd;
        unsigned int cpnum;
        unsigned int opcode2;
        unsigned int crm;

        public:
        MRC( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, Alias \
            & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias & LR_ABT, \
            Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias \
            * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~MRC();
    };

};

namespace arm9tdmi_funclt_trap{

    class RSB_i : public UpdatePSRSub_op, public condition_check_op, public DPI_imm_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int rotate;
        unsigned int immediate;

        protected:
        unsigned int operand1;
        unsigned int operand2;
        bool carry;
        unsigned int operand;

        public:
        RSB_i( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~RSB_i();
    };

};

namespace arm9tdmi_funclt_trap{

    class STRBT_imm : public condition_check_op, public ls_imm_PI_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int u;
        unsigned int immediate;

        protected:
        unsigned int address;

        public:
        STRBT_imm( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & \
            FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~STRBT_imm();
    };

};

namespace arm9tdmi_funclt_trap{

    class ADC_si : public UpdatePSRSumWithCarry_op, public condition_check_op, public \
        DPI_shift_imm_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int shift_amm;
        unsigned int shift_op;

        protected:
        unsigned int operand1;
        unsigned int operand2;
        bool carry;
        unsigned int operand;

        public:
        ADC_si( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~ADC_si();
    };

};

namespace arm9tdmi_funclt_trap{

    class SBC_sr : public UpdatePSRSubWithCarry_op, public condition_check_op, public \
        DPI_reg_shift_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        Alias rs;
        unsigned int rs_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int shift_op;

        protected:
        unsigned int operand1;
        unsigned int operand2;
        bool carry;
        unsigned int operand;

        public:
        SBC_sr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SBC_sr();
    };

};

namespace arm9tdmi_funclt_trap{

    class CMN_si : public condition_check_op, public DPI_shift_imm_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int shift_amm;
        unsigned int shift_op;

        protected:
        unsigned int operand;
        bool carry;

        public:
        CMN_si( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~CMN_si();
    };

};

namespace arm9tdmi_funclt_trap{

    class EOR_i : public UpdatePSRBit_op, public condition_check_op, public DPI_imm_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int rotate;
        unsigned int immediate;

        protected:
        bool carry;
        unsigned int result;
        unsigned int operand;

        public:
        EOR_i( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~EOR_i();
    };

};

namespace arm9tdmi_funclt_trap{

    class ADC_sr : public UpdatePSRSumWithCarry_op, public condition_check_op, public \
        DPI_reg_shift_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        Alias rs;
        unsigned int rs_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int shift_op;

        protected:
        unsigned int operand1;
        unsigned int operand2;
        bool carry;
        unsigned int operand;

        public:
        ADC_sr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~ADC_sr();
    };

};

namespace arm9tdmi_funclt_trap{

    class LDC : public Instruction{
        private:
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int p;
        unsigned int u;
        unsigned int n;
        unsigned int w;
        unsigned int crd;
        unsigned int cpnum;
        unsigned int offset;

        public:
        LDC( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, Alias \
            & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias & LR_ABT, \
            Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias \
            * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~LDC();
    };

};

namespace arm9tdmi_funclt_trap{

    class msr_reg_Instr : public condition_check_op{
        private:
        Alias rm;
        unsigned int rm_bit;
        unsigned int cond;
        unsigned int r;
        unsigned int mask;

        public:
        msr_reg_Instr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias \
            & FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~msr_reg_Instr();
    };

};

namespace arm9tdmi_funclt_trap{

    class LDM : public condition_check_op, public LSM_reglist_op{
        private:
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int p;
        unsigned int u;
        unsigned int s;
        unsigned int w;
        unsigned int reg_list;

        protected:
        unsigned int start_address;
        unsigned int wb_address;

        public:
        LDM( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, Alias \
            & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias & LR_ABT, \
            Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias \
            * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~LDM();
    };

};

namespace arm9tdmi_funclt_trap{

    class CDP : public Instruction{
        private:
        unsigned int cond;
        unsigned int opcode1;
        unsigned int crn;
        unsigned int crd;
        unsigned int cpnum;
        unsigned int opcode2;
        unsigned int crm;

        public:
        CDP( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, Alias \
            & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias & LR_ABT, \
            Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias \
            * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~CDP();
    };

};

namespace arm9tdmi_funclt_trap{

    class ADC_i : public UpdatePSRSumWithCarry_op, public condition_check_op, public \
        DPI_imm_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int rotate;
        unsigned int immediate;

        protected:
        unsigned int operand1;
        unsigned int operand2;
        bool carry;
        unsigned int operand;

        public:
        ADC_i( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~ADC_i();
    };

};

namespace arm9tdmi_funclt_trap{

    class STRH_off : public condition_check_op, public LS_sh_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int p;
        unsigned int u;
        unsigned int i;
        unsigned int w;
        unsigned int addr_mode0;
        unsigned int addr_mode1;

        protected:
        unsigned int address;

        public:
        STRH_off( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & \
            FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~STRH_off();
    };

};

namespace arm9tdmi_funclt_trap{

    class CMP_sr : public condition_check_op, public DPI_reg_shift_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        Alias rs;
        unsigned int rs_bit;
        unsigned int cond;
        unsigned int shift_op;

        protected:
        unsigned int operand;
        bool carry;

        public:
        CMP_sr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~CMP_sr();
    };

};

namespace arm9tdmi_funclt_trap{

    class LDRBT_imm : public condition_check_op, public ls_imm_PI_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int u;
        unsigned int immediate;

        protected:
        unsigned int address;

        public:
        LDRBT_imm( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & \
            FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~LDRBT_imm();
    };

};

namespace arm9tdmi_funclt_trap{

    class LDRBT_off : public condition_check_op, public LS_reg_PI_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int u;
        unsigned int shift_amm;
        unsigned int shift_op;

        protected:
        unsigned int address;

        public:
        LDRBT_off( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & \
            FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~LDRBT_off();
    };

};

namespace arm9tdmi_funclt_trap{

    class CMP_si : public condition_check_op, public DPI_shift_imm_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int shift_amm;
        unsigned int shift_op;

        protected:
        unsigned int operand;
        bool carry;

        public:
        CMP_si( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~CMP_si();
    };

};

namespace arm9tdmi_funclt_trap{

    class TEQ_si : public condition_check_op, public DPI_shift_imm_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int shift_amm;
        unsigned int shift_op;

        protected:
        unsigned int operand;
        bool carry;
        unsigned int result;

        public:
        TEQ_si( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~TEQ_si();
    };

};

namespace arm9tdmi_funclt_trap{

    class RSC_i : public UpdatePSRSubWithCarry_op, public condition_check_op, public \
        DPI_imm_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int rotate;
        unsigned int immediate;

        protected:
        unsigned int operand1;
        unsigned int operand2;
        bool carry;
        unsigned int operand;

        public:
        RSC_i( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~RSC_i();
    };

};

namespace arm9tdmi_funclt_trap{

    class STRB_imm : public condition_check_op, public LS_imm_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int p;
        unsigned int u;
        unsigned int w;
        unsigned int immediate;

        protected:
        unsigned int address;

        public:
        STRB_imm( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & \
            FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~STRB_imm();
    };

};

namespace arm9tdmi_funclt_trap{

    class MOV_i : public UpdatePSRBit_op, public condition_check_op, public DPI_imm_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int rotate;
        unsigned int immediate;

        protected:
        bool carry;
        unsigned int result;
        unsigned int operand;

        public:
        MOV_i( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~MOV_i();
    };

};

namespace arm9tdmi_funclt_trap{

    class ORR_i : public UpdatePSRBit_op, public condition_check_op, public DPI_imm_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int rotate;
        unsigned int immediate;

        protected:
        bool carry;
        unsigned int result;
        unsigned int operand;

        public:
        ORR_i( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~ORR_i();
    };

};

namespace arm9tdmi_funclt_trap{

    class LDRSH_off : public condition_check_op, public LS_sh_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int p;
        unsigned int u;
        unsigned int i;
        unsigned int w;
        unsigned int l;
        unsigned int addr_mode0;
        unsigned int addr_mode1;

        protected:
        unsigned int address;

        public:
        LDRSH_off( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & \
            FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~LDRSH_off();
    };

};

namespace arm9tdmi_funclt_trap{

    class LDRB_imm : public condition_check_op, public LS_imm_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int p;
        unsigned int u;
        unsigned int w;
        unsigned int immediate;

        protected:
        unsigned int address;

        public:
        LDRB_imm( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & \
            FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~LDRB_imm();
    };

};

namespace arm9tdmi_funclt_trap{

    class CMN_i : public condition_check_op, public DPI_imm_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int rotate;
        unsigned int immediate;

        protected:
        unsigned int operand;
        bool carry;

        public:
        CMN_i( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~CMN_i();
    };

};

namespace arm9tdmi_funclt_trap{

    class LDRT_off : public condition_check_op, public LS_reg_PI_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int u;
        unsigned int shift_amm;
        unsigned int shift_op;

        protected:
        unsigned int address;
        unsigned int memLastBits;

        public:
        LDRT_off( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & \
            FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~LDRT_off();
    };

};

namespace arm9tdmi_funclt_trap{

    class SWAP : public condition_check_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;

        protected:
        unsigned int temp;
        unsigned int memLastBits;

        public:
        SWAP( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SWAP();
    };

};

namespace arm9tdmi_funclt_trap{

    class STRT_imm : public condition_check_op, public ls_imm_PI_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int u;
        unsigned int immediate;

        protected:
        unsigned int address;

        public:
        STRT_imm( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & \
            FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~STRT_imm();
    };

};

namespace arm9tdmi_funclt_trap{

    class LDRH_off : public condition_check_op, public LS_sh_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int p;
        unsigned int u;
        unsigned int i;
        unsigned int w;
        unsigned int addr_mode0;
        unsigned int addr_mode1;

        protected:
        unsigned int address;

        public:
        LDRH_off( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & \
            FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~LDRH_off();
    };

};

namespace arm9tdmi_funclt_trap{

    class BKPT : public Instruction{
        private:
        unsigned int immediate0;
        unsigned int immediate1;

        public:
        BKPT( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~BKPT();
    };

};

namespace arm9tdmi_funclt_trap{

    class RSC_si : public UpdatePSRSubWithCarry_op, public condition_check_op, public \
        DPI_shift_imm_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int shift_amm;
        unsigned int shift_op;

        protected:
        unsigned int operand1;
        unsigned int operand2;
        bool carry;
        unsigned int operand;

        public:
        RSC_si( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~RSC_si();
    };

};

namespace arm9tdmi_funclt_trap{

    class umlal_Instr : public UpdatePSRmul_64_op, public condition_check_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rs;
        unsigned int rs_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int rn;

        protected:
        bool carry;
        sc_dt::uint64 result;

        public:
        umlal_Instr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias \
            & FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~umlal_Instr();
    };

};

namespace arm9tdmi_funclt_trap{

    class AND_i : public UpdatePSRBit_op, public condition_check_op, public DPI_imm_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int rotate;
        unsigned int immediate;

        protected:
        bool carry;
        unsigned int result;
        unsigned int operand;

        public:
        AND_i( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~AND_i();
    };

};

namespace arm9tdmi_funclt_trap{

    class RSC_sr : public UpdatePSRSubWithCarry_op, public condition_check_op, public \
        DPI_reg_shift_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        Alias rs;
        unsigned int rs_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int shift_op;

        protected:
        unsigned int operand1;
        unsigned int operand2;
        bool carry;
        unsigned int operand;

        public:
        RSC_sr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~RSC_sr();
    };

};

namespace arm9tdmi_funclt_trap{

    class smlal_Instr : public UpdatePSRmul_64_op, public condition_check_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rs;
        unsigned int rs_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int rn;

        protected:
        bool carry;
        sc_dt::uint64 result;

        public:
        smlal_Instr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias \
            & FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~smlal_Instr();
    };

};

namespace arm9tdmi_funclt_trap{

    class mul_Instr : public UpdatePSRmul_op, public condition_check_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rs;
        unsigned int rs_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int rn;

        public:
        mul_Instr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & \
            FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~mul_Instr();
    };

};

namespace arm9tdmi_funclt_trap{

    class mla_Instr : public UpdatePSRmul_op, public condition_check_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rs;
        unsigned int rs_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int rn;

        public:
        mla_Instr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & \
            FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~mla_Instr();
    };

};

namespace arm9tdmi_funclt_trap{

    class MVN_si : public UpdatePSRBit_op, public condition_check_op, public DPI_shift_imm_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int shift_amm;
        unsigned int shift_op;

        protected:
        bool carry;
        unsigned int result;
        unsigned int operand;

        public:
        MVN_si( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~MVN_si();
    };

};

namespace arm9tdmi_funclt_trap{

    class MVN_i : public UpdatePSRBit_op, public condition_check_op, public DPI_imm_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int rotate;
        unsigned int immediate;

        protected:
        bool carry;
        unsigned int result;
        unsigned int operand;

        public:
        MVN_i( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~MVN_i();
    };

};

namespace arm9tdmi_funclt_trap{

    class MVN_sr : public UpdatePSRBit_op, public condition_check_op, public DPI_reg_shift_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        Alias rs;
        unsigned int rs_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int shift_op;

        protected:
        bool carry;
        unsigned int result;
        unsigned int operand;

        public:
        MVN_sr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~MVN_sr();
    };

};

namespace arm9tdmi_funclt_trap{

    class BRANCH : public condition_check_op{
        private:
        unsigned int cond;
        unsigned int l;
        unsigned int offset;

        public:
        BRANCH( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~BRANCH();
    };

};

namespace arm9tdmi_funclt_trap{

    class SWI : public condition_check_op{
        private:
        unsigned int cond;
        unsigned int swi_number;

        public:
        SWI( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, Alias \
            & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias & LR_ABT, \
            Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias \
            * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SWI();
    };

};

namespace arm9tdmi_funclt_trap{

    class EOR_sr : public UpdatePSRBit_op, public condition_check_op, public DPI_reg_shift_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        Alias rs;
        unsigned int rs_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int shift_op;

        protected:
        bool carry;
        unsigned int result;
        unsigned int operand;

        public:
        EOR_sr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~EOR_sr();
    };

};

namespace arm9tdmi_funclt_trap{

    class SBC_si : public UpdatePSRSubWithCarry_op, public condition_check_op, public \
        DPI_shift_imm_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int shift_amm;
        unsigned int shift_op;

        protected:
        unsigned int operand1;
        unsigned int operand2;
        bool carry;
        unsigned int operand;

        public:
        SBC_si( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SBC_si();
    };

};

namespace arm9tdmi_funclt_trap{

    class TST_i : public condition_check_op, public DPI_imm_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int rotate;
        unsigned int immediate;

        protected:
        unsigned int operand;
        bool carry;
        unsigned int result;

        public:
        TST_i( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~TST_i();
    };

};

namespace arm9tdmi_funclt_trap{

    class SWAPB : public condition_check_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;

        protected:
        unsigned char temp;

        public:
        SWAPB( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SWAPB();
    };

};

namespace arm9tdmi_funclt_trap{

    class RSB_sr : public UpdatePSRSub_op, public condition_check_op, public DPI_reg_shift_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        Alias rs;
        unsigned int rs_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int shift_op;

        protected:
        unsigned int operand1;
        unsigned int operand2;
        bool carry;
        unsigned int operand;

        public:
        RSB_sr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~RSB_sr();
    };

};

namespace arm9tdmi_funclt_trap{

    class SBC_i : public UpdatePSRSubWithCarry_op, public condition_check_op, public \
        DPI_imm_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int rotate;
        unsigned int immediate;

        protected:
        unsigned int operand1;
        unsigned int operand2;
        bool carry;
        unsigned int operand;

        public:
        SBC_i( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SBC_i();
    };

};

namespace arm9tdmi_funclt_trap{

    class STR_off : public condition_check_op, public LS_reg_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int p;
        unsigned int u;
        unsigned int w;
        unsigned int shift_amm;
        unsigned int shift_op;

        protected:
        unsigned int address;
        unsigned int memLastBits;
        unsigned int value;

        public:
        STR_off( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~STR_off();
    };

};

namespace arm9tdmi_funclt_trap{

    class STC : public Instruction{
        private:
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int p;
        unsigned int u;
        unsigned int n;
        unsigned int w;
        unsigned int crd;
        unsigned int cpnum;
        unsigned int offset;

        public:
        STC( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, Alias \
            & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias & LR_ABT, \
            Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias \
            * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~STC();
    };

};

namespace arm9tdmi_funclt_trap{

    class STM : public condition_check_op, public LSM_reglist_op{
        private:
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int p;
        unsigned int u;
        unsigned int s;
        unsigned int w;
        unsigned int reg_list;

        protected:
        unsigned int start_address;
        unsigned int wb_address;

        public:
        STM( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, Alias \
            & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias & LR_ABT, \
            Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias \
            * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~STM();
    };

};

namespace arm9tdmi_funclt_trap{

    class SUB_i : public UpdatePSRSub_op, public condition_check_op, public DPI_imm_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int rotate;
        unsigned int immediate;

        protected:
        unsigned int operand1;
        unsigned int operand2;
        bool carry;
        unsigned int operand;

        public:
        SUB_i( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SUB_i();
    };

};

namespace arm9tdmi_funclt_trap{

    class LDR_off : public condition_check_op, public LS_reg_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int p;
        unsigned int u;
        unsigned int w;
        unsigned int shift_amm;
        unsigned int shift_op;

        protected:
        unsigned int address;
        unsigned int memLastBits;
        unsigned int value;

        public:
        LDR_off( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~LDR_off();
    };

};

namespace arm9tdmi_funclt_trap{

    class BIC_i : public UpdatePSRBit_op, public condition_check_op, public DPI_imm_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int rotate;
        unsigned int immediate;

        protected:
        bool carry;
        unsigned int result;
        unsigned int operand;

        public:
        BIC_i( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~BIC_i();
    };

};

namespace arm9tdmi_funclt_trap{

    class TEQ_sr : public condition_check_op, public DPI_reg_shift_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        Alias rs;
        unsigned int rs_bit;
        unsigned int cond;
        unsigned int shift_op;

        protected:
        unsigned int operand;
        bool carry;
        unsigned int result;

        public:
        TEQ_sr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~TEQ_sr();
    };

};

namespace arm9tdmi_funclt_trap{

    class smull_Instr : public UpdatePSRmul_64_op, public condition_check_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rs;
        unsigned int rs_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int rn;

        protected:
        bool carry;
        sc_dt::uint64 result;

        public:
        smull_Instr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias \
            & FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~smull_Instr();
    };

};

namespace arm9tdmi_funclt_trap{

    class STRB_off : public condition_check_op, public LS_reg_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int p;
        unsigned int u;
        unsigned int w;
        unsigned int shift_amm;
        unsigned int shift_op;

        protected:
        unsigned int address;

        public:
        STRB_off( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & \
            FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~STRB_off();
    };

};

namespace arm9tdmi_funclt_trap{

    class CMN_sr : public condition_check_op, public DPI_reg_shift_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        Alias rs;
        unsigned int rs_bit;
        unsigned int cond;
        unsigned int shift_op;

        protected:
        unsigned int operand;
        bool carry;

        public:
        CMN_sr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~CMN_sr();
    };

};

namespace arm9tdmi_funclt_trap{

    class STRBT_off : public condition_check_op, public LS_reg_PI_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int u;
        unsigned int shift_amm;
        unsigned int shift_op;

        protected:
        unsigned int address;

        public:
        STRBT_off( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & \
            FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~STRBT_off();
    };

};

namespace arm9tdmi_funclt_trap{

    class msr_imm_Instr : public condition_check_op{
        private:
        unsigned int cond;
        unsigned int r;
        unsigned int mask;
        unsigned int rotate;
        unsigned int immediate;

        protected:
        unsigned int value;

        public:
        msr_imm_Instr( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias \
            & FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~msr_imm_Instr();
    };

};

namespace arm9tdmi_funclt_trap{

    class LDRB_off : public condition_check_op, public LS_reg_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int p;
        unsigned int u;
        unsigned int w;
        unsigned int shift_amm;
        unsigned int shift_op;

        protected:
        unsigned int address;

        public:
        LDRB_off( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & \
            FP, Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, \
            Alias & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias \
            & PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~LDRB_off();
    };

};

namespace arm9tdmi_funclt_trap{

    class EOR_si : public UpdatePSRBit_op, public condition_check_op, public DPI_shift_imm_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rm;
        unsigned int rm_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int s;
        unsigned int shift_amm;
        unsigned int shift_op;

        protected:
        bool carry;
        unsigned int result;
        unsigned int operand;

        public:
        EOR_si( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~EOR_si();
    };

};

namespace arm9tdmi_funclt_trap{

    class MCR : public Instruction{
        private:
        unsigned int cond;
        unsigned int opcode1;
        unsigned int crn;
        unsigned int crd;
        unsigned int cpnum;
        unsigned int opcode2;
        unsigned int crm;

        public:
        MCR( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, Alias \
            & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias & LR_ABT, \
            Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & PC, Alias \
            * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~MCR();
    };

};

namespace arm9tdmi_funclt_trap{

    class TEQ_i : public condition_check_op, public DPI_imm_op{
        private:
        Alias rd;
        unsigned int rd_bit;
        Alias rn;
        unsigned int rn_bit;
        unsigned int cond;
        unsigned int rotate;
        unsigned int immediate;

        protected:
        unsigned int operand;
        bool carry;
        unsigned int result;

        public:
        TEQ_i( Reg32_0 & CPSR, Reg32_1 & MP_ID, Reg32_1 * RB, Reg32_0 * SPSR, Alias & FP, \
            Alias & SPTR, Alias & LINKR, Alias & SP_SVC, Alias & LR_SVC, Alias & SP_ABT, Alias \
            & LR_ABT, Alias & SP_IRQ, Alias & LR_IRQ, Alias & SP_FIQ, Alias & LR_FIQ, Alias & \
            PC, Alias * REGS, TLMMemory & instrMem, TLMMemory & dataMem );
        unsigned int behavior();
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~TEQ_i();
    };

};



#endif
