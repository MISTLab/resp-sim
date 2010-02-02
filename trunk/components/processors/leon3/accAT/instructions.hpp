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
#include <map>
#include <vector>
#include <string>
#include <customExceptions.hpp>
#include <trap_utils.hpp>
#include <registers.hpp>
#include <alias.hpp>
#include <externalPorts.hpp>
#include <externalPins.hpp>
#include <sstream>
#include <systemc.h>

#define ACC_MODEL
#define AT_IF

#define RESET 0
#define DATA_STORE_ERROR 1
#define INSTR_ACCESS_MMU_MISS 2
#define INSTR_ACCESS_ERROR 3
#define R_REGISTER_ACCESS_ERROR 4
#define INSTR_ACCESS_EXC 5
#define PRIVILEDGE_INSTR 6
#define ILLEGAL_INSTR 7
#define FP_DISABLED 8
#define CP_DISABLED 9
#define UNIMPL_FLUSH 10
#define WATCHPOINT_DETECTED 11
#define WINDOW_OVERFLOW 12
#define WINDOW_UNDERFLOW 13
#define MEM_ADDR_NOT_ALIGNED 14
#define FP_EXCEPTION 15
#define CP_EXCEPTION 16
#define DATA_ACCESS_ERROR 17
#define DATA_ACCESS_MMU_MISS 18
#define DATA_ACCESS_EXC 19
#define TAG_OVERFLOW 20
#define DIV_ZERO 21
#define TRAP_INSTRUCTION 22
#define IRQ_LEV_15 23
#define IRQ_LEV_14 24
#define IRQ_LEV_13 25
#define IRQ_LEV_12 26
#define IRQ_LEV_11 27
#define IRQ_LEV_10 28
#define IRQ_LEV_9 29
#define IRQ_LEV_8 30
#define IRQ_LEV_7 31
#define IRQ_LEV_6 32
#define IRQ_LEV_5 33
#define IRQ_LEV_4 34
#define IRQ_LEV_3 35
#define IRQ_LEV_2 36
#define IRQ_LEV_1 37
#define IMPL_DEP_EXC 38

namespace leon3_accat_trap{

    class Instruction : public InstructionBase{

        protected:
        TLMMemory & instrMem;
        TLMMemory & dataMem;
        PinTLM_out_32 & irqAck;
        unsigned int stageCycles;
        const unsigned int NUM_REG_WIN;
        const bool PIPELINED_MULT;

        public:
        Instruction( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        virtual unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register \
            * > > & unlockQueue ) = 0;
        virtual unsigned int behavior_decode( std::map< unsigned int, std::vector< Register \
            * > > & unlockQueue ) = 0;
        virtual unsigned int behavior_regs( std::map< unsigned int, std::vector< Register \
            * > > & unlockQueue ) = 0;
        virtual unsigned int behavior_execute( std::map< unsigned int, std::vector< Register \
            * > > & unlockQueue ) = 0;
        virtual unsigned int behavior_memory( std::map< unsigned int, std::vector< Register \
            * > > & unlockQueue ) = 0;
        virtual unsigned int behavior_exception( std::map< unsigned int, std::vector< Register \
            * > > & unlockQueue ) = 0;
        virtual unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue ) = 0;
        virtual Instruction * replicate() const throw() = 0;
        virtual void setParams( const unsigned int & bitString ) throw() = 0;
        virtual bool checkHazard_fetch() = 0;
        virtual void lockRegs_fetch() = 0;
        virtual bool checkHazard_decode() = 0;
        virtual void lockRegs_decode() = 0;
        virtual bool checkHazard_regs() = 0;
        virtual void lockRegs_regs() = 0;
        virtual bool checkHazard_execute() = 0;
        virtual void lockRegs_execute() = 0;
        virtual bool checkHazard_memory() = 0;
        virtual void lockRegs_memory() = 0;
        virtual bool checkHazard_exception() = 0;
        virtual void lockRegs_exception() = 0;
        virtual bool checkHazard_wb() = 0;
        virtual void lockRegs_wb() = 0;
        virtual void getUnlock_decode( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue ) = 0;
        virtual void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue ) = 0;
        virtual void getUnlock_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue ) = 0;
        virtual void getUnlock_memory( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue ) = 0;
        virtual void getUnlock_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue ) = 0;
        virtual void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue ) = 0;
        unsigned int fetchPC;
        bool inPipeline;
        bool toDestroy;
        virtual std::string getInstructionName() const throw() = 0;
        virtual std::string getMnemonic() const throw() = 0;
        virtual unsigned int getId() const throw() = 0;
        inline void annull(){
            throw annull_exception();
        }
        inline void flush(){
            this->flushPipeline = true;
        }
        inline void stall( const unsigned int & numCycles ){
            this->stageCycles += numCycles;
        }
        bool IncrementRegWindow() throw();
        bool DecrementRegWindow() throw();
        int SignExtend( unsigned int bitSeq, unsigned int bitSeq_length ) const throw();
        void RaiseException( unsigned int pcounter, unsigned int npcounter, unsigned int \
            exceptionId, unsigned int customTrapOffset = 0 );
        bool checkIncrementWin() const throw();
        bool checkDecrementWin() const throw();
        PipelineRegister & PSR_pipe;
        PipelineRegister & WIM_pipe;
        PipelineRegister & TBR_pipe;
        PipelineRegister & Y_pipe;
        PipelineRegister & PC_pipe;
        PipelineRegister & NPC_pipe;
        PipelineRegister * GLOBAL_pipe;
        PipelineRegister * WINREGS_pipe;
        PipelineRegister * ASR_pipe;
        Reg32_0 & PSR_fetch;
        Reg32_1 & WIM_fetch;
        Reg32_2 & TBR_fetch;
        Reg32_3 & Y_fetch;
        Reg32_3 & PC_fetch;
        Reg32_3 & NPC_fetch;
        RegisterBankClass & GLOBAL_fetch;
        Reg32_3 * WINREGS_fetch;
        Reg32_3 * ASR_fetch;
        Alias & FP_fetch;
        Alias & LR_fetch;
        Alias & SP_fetch;
        Alias & PCR_fetch;
        Alias * REGS_fetch;
        Reg32_0 & PSR_decode;
        Reg32_1 & WIM_decode;
        Reg32_2 & TBR_decode;
        Reg32_3 & Y_decode;
        Reg32_3 & PC_decode;
        Reg32_3 & NPC_decode;
        RegisterBankClass & GLOBAL_decode;
        Reg32_3 * WINREGS_decode;
        Reg32_3 * ASR_decode;
        Alias & FP_decode;
        Alias & LR_decode;
        Alias & SP_decode;
        Alias & PCR_decode;
        Alias * REGS_decode;
        Reg32_0 & PSR_regs;
        Reg32_1 & WIM_regs;
        Reg32_2 & TBR_regs;
        Reg32_3 & Y_regs;
        Reg32_3 & PC_regs;
        Reg32_3 & NPC_regs;
        RegisterBankClass & GLOBAL_regs;
        Reg32_3 * WINREGS_regs;
        Reg32_3 * ASR_regs;
        Alias & FP_regs;
        Alias & LR_regs;
        Alias & SP_regs;
        Alias & PCR_regs;
        Alias * REGS_regs;
        Reg32_0 & PSR_execute;
        Reg32_1 & WIM_execute;
        Reg32_2 & TBR_execute;
        Reg32_3 & Y_execute;
        Reg32_3 & PC_execute;
        Reg32_3 & NPC_execute;
        RegisterBankClass & GLOBAL_execute;
        Reg32_3 * WINREGS_execute;
        Reg32_3 * ASR_execute;
        Alias & FP_execute;
        Alias & LR_execute;
        Alias & SP_execute;
        Alias & PCR_execute;
        Alias * REGS_execute;
        Reg32_0 & PSR_memory;
        Reg32_1 & WIM_memory;
        Reg32_2 & TBR_memory;
        Reg32_3 & Y_memory;
        Reg32_3 & PC_memory;
        Reg32_3 & NPC_memory;
        RegisterBankClass & GLOBAL_memory;
        Reg32_3 * WINREGS_memory;
        Reg32_3 * ASR_memory;
        Alias & FP_memory;
        Alias & LR_memory;
        Alias & SP_memory;
        Alias & PCR_memory;
        Alias * REGS_memory;
        Reg32_0 & PSR_exception;
        Reg32_1 & WIM_exception;
        Reg32_2 & TBR_exception;
        Reg32_3 & Y_exception;
        Reg32_3 & PC_exception;
        Reg32_3 & NPC_exception;
        RegisterBankClass & GLOBAL_exception;
        Reg32_3 * WINREGS_exception;
        Reg32_3 * ASR_exception;
        Alias & FP_exception;
        Alias & LR_exception;
        Alias & SP_exception;
        Alias & PCR_exception;
        Alias * REGS_exception;
        Reg32_0 & PSR_wb;
        Reg32_1 & WIM_wb;
        Reg32_2 & TBR_wb;
        Reg32_3 & Y_wb;
        Reg32_3 & PC_wb;
        Reg32_3 & NPC_wb;
        RegisterBankClass & GLOBAL_wb;
        Reg32_3 * WINREGS_wb;
        Reg32_3 * ASR_wb;
        Alias & FP_wb;
        Alias & LR_wb;
        Alias & SP_wb;
        Alias & PCR_wb;
        Alias * REGS_wb;
        bool flushPipeline;
        virtual ~Instruction();
    };

};

namespace leon3_accat_trap{

    class InvalidInstr : public Instruction{

        public:
        InvalidInstr( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        Instruction * replicate() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getInstructionName() const throw();
        std::string getMnemonic() const throw();
        unsigned int getId() const throw();
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        virtual ~InvalidInstr();
    };

};

namespace leon3_accat_trap{

    class NOPInstruction : public Instruction{

        public:
        NOPInstruction( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        Instruction * replicate() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getInstructionName() const throw();
        std::string getMnemonic() const throw();
        unsigned int getId() const throw();
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        virtual ~NOPInstruction();
    };

};

namespace leon3_accat_trap{

    class READasr : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int asr;

        protected:
        unsigned int asr_temp;

        public:
        READasr( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~READasr();
    };

};

namespace leon3_accat_trap{

    class WRITEY_reg : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;

        public:
        WRITEY_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~WRITEY_reg();
    };

};

namespace leon3_accat_trap{

    class XNOR_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        XNOR_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~XNOR_reg();
    };

};

namespace leon3_accat_trap{

    class ANDNcc_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        ANDNcc_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~ANDNcc_reg();
    };

};

namespace leon3_accat_trap{

    class LDSB_imm : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int simm13;

        protected:
        unsigned int address;
        unsigned int readValue;

        public:
        LDSB_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~LDSB_imm();
    };

};

namespace leon3_accat_trap{

    class WRITEpsr_imm : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int rd;
        unsigned int simm13;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        bool supervisorException;
        bool illegalCWP;
        unsigned int result;

        public:
        WRITEpsr_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~WRITEpsr_imm();
    };

};

namespace leon3_accat_trap{

    class READy : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;

        protected:
        unsigned int y_temp;

        public:
        READy( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~READy();
    };

};

namespace leon3_accat_trap{

    class XNORcc_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        XNORcc_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~XNORcc_reg();
    };

};

namespace leon3_accat_trap{

    class READpsr : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int asr;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        bool supervisor;
        unsigned int psr_temp;

        public:
        READpsr( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~READpsr();
    };

};

namespace leon3_accat_trap{

    class ANDN_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;
        unsigned int rs1_op;

        public:
        ANDN_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~ANDN_imm();
    };

};

namespace leon3_accat_trap{

    class ANDcc_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        ANDcc_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~ANDcc_reg();
    };

};

namespace leon3_accat_trap{

    class TSUBcc_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;
        bool temp_V;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        TSUBcc_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~TSUBcc_imm();
    };

};

namespace leon3_accat_trap{

    class LDSBA_reg : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int asi;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        unsigned int address;
        unsigned int readValue;
        bool supervisor;

        public:
        LDSBA_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~LDSBA_reg();
    };

};

namespace leon3_accat_trap{

    class LDUH_imm : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int simm13;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        unsigned int address;
        unsigned int readValue;
        bool notAligned;

        public:
        LDUH_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~LDUH_imm();
    };

};

namespace leon3_accat_trap{

    class STA_reg : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int asi;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        bool supervisor;
        bool notAligned;
        unsigned int address;
        unsigned int toWrite;

        public:
        STA_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~STA_reg();
    };

};

namespace leon3_accat_trap{

    class ORN_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        ORN_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~ORN_reg();
    };

};

namespace leon3_accat_trap{

    class LDSHA_reg : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int asi;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        unsigned int address;
        unsigned int readValue;
        bool supervisor;
        bool notAligned;

        public:
        LDSHA_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~LDSHA_reg();
    };

};

namespace leon3_accat_trap{

    class STBA_reg : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int asi;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        bool supervisor;
        unsigned int address;
        unsigned char toWrite;

        public:
        STBA_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~STBA_reg();
    };

};

namespace leon3_accat_trap{

    class ST_imm : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int simm13;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        bool notAligned;
        unsigned int address;
        unsigned int toWrite;

        public:
        ST_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~ST_imm();
    };

};

namespace leon3_accat_trap{

    class READtbr : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int asr;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        bool supervisor;
        unsigned int tbr_temp;

        public:
        READtbr( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~READtbr();
    };

};

namespace leon3_accat_trap{

    class UDIVcc_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        bool exception;
        unsigned int result;
        bool temp_V;
        unsigned int pcounter;
        unsigned int npcounter;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        UDIVcc_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~UDIVcc_imm();
    };

};

namespace leon3_accat_trap{

    class SWAPA_reg : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int asi;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        bool supervisor;
        bool notAligned;
        unsigned int address;
        unsigned int readValue;
        unsigned int toWrite;

        public:
        SWAPA_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SWAPA_reg();
    };

};

namespace leon3_accat_trap{

    class ADDXcc_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        ADDXcc_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~ADDXcc_imm();
    };

};

namespace leon3_accat_trap{

    class STB_imm : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int simm13;

        protected:
        unsigned int address;
        unsigned char toWrite;

        public:
        STB_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~STB_imm();
    };

};

namespace leon3_accat_trap{

    class SUBXcc_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        SUBXcc_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SUBXcc_imm();
    };

};

namespace leon3_accat_trap{

    class STH_reg : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int asi;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        bool notAligned;
        unsigned int address;
        unsigned short int toWrite;

        public:
        STH_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~STH_reg();
    };

};

namespace leon3_accat_trap{

    class SRL_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;
        unsigned int rs1_op;

        public:
        SRL_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SRL_imm();
    };

};

namespace leon3_accat_trap{

    class WRITEasr_imm : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int rd;
        unsigned int simm13;

        protected:
        unsigned int result;

        public:
        WRITEasr_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~WRITEasr_imm();
    };

};

namespace leon3_accat_trap{

    class UMULcc_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        UMULcc_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~UMULcc_reg();
    };

};

namespace leon3_accat_trap{

    class LDSTUB_reg : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int asi;

        protected:
        unsigned int address;
        unsigned int readValue;

        public:
        LDSTUB_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~LDSTUB_reg();
    };

};

namespace leon3_accat_trap{

    class XOR_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;
        unsigned int rs1_op;

        public:
        XOR_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~XOR_imm();
    };

};

namespace leon3_accat_trap{

    class SMAC_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        SMAC_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SMAC_reg();
    };

};

namespace leon3_accat_trap{

    class WRITEasr_reg : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;
        unsigned int rd;

        protected:
        unsigned int result;

        public:
        WRITEasr_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~WRITEasr_reg();
    };

};

namespace leon3_accat_trap{

    class LD_reg : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int asi;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        unsigned int address;
        unsigned int readValue;
        bool notAligned;

        public:
        LD_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~LD_reg();
    };

};

namespace leon3_accat_trap{

    class ST_reg : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int asi;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        bool notAligned;
        unsigned int address;
        unsigned int toWrite;

        public:
        ST_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~ST_reg();
    };

};

namespace leon3_accat_trap{

    class SUBcc_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        SUBcc_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SUBcc_reg();
    };

};

namespace leon3_accat_trap{

    class LDD_reg : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int asi;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        unsigned int address;
        sc_dt::uint64 readValue;
        bool notAligned;

        public:
        LDD_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~LDD_reg();
    };

};

namespace leon3_accat_trap{

    class ADDcc_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        ADDcc_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~ADDcc_imm();
    };

};

namespace leon3_accat_trap{

    class LDUH_reg : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int asi;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        unsigned int address;
        unsigned int readValue;
        bool notAligned;

        public:
        LDUH_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~LDUH_reg();
    };

};

namespace leon3_accat_trap{

    class SRL_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        SRL_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SRL_reg();
    };

};

namespace leon3_accat_trap{

    class SAVE_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        bool okNewWin;
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;
        unsigned int newCwp;

        public:
        SAVE_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SAVE_imm();
    };

};

namespace leon3_accat_trap{

    class MULScc_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        MULScc_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~MULScc_reg();
    };

};

namespace leon3_accat_trap{

    class OR_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;
        unsigned int rs1_op;

        public:
        OR_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~OR_imm();
    };

};

namespace leon3_accat_trap{

    class STD_imm : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int simm13;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        bool notAligned;
        unsigned int address;
        sc_dt::uint64 toWrite;

        public:
        STD_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~STD_imm();
    };

};

namespace leon3_accat_trap{

    class SUBXcc_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        SUBXcc_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SUBXcc_reg();
    };

};

namespace leon3_accat_trap{

    class ADDX_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        ADDX_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~ADDX_imm();
    };

};

namespace leon3_accat_trap{

    class SWAP_imm : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int simm13;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        bool notAligned;
        unsigned int address;
        unsigned int readValue;
        unsigned int toWrite;

        public:
        SWAP_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SWAP_imm();
    };

};

namespace leon3_accat_trap{

    class UMUL_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        UMUL_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~UMUL_reg();
    };

};

namespace leon3_accat_trap{

    class WRITEY_imm : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;

        public:
        WRITEY_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~WRITEY_imm();
    };

};

namespace leon3_accat_trap{

    class AND_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        AND_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~AND_reg();
    };

};

namespace leon3_accat_trap{

    class FLUSH_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        public:
        FLUSH_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~FLUSH_imm();
    };

};

namespace leon3_accat_trap{

    class SRA_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        SRA_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SRA_reg();
    };

};

namespace leon3_accat_trap{

    class STH_imm : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int simm13;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        bool notAligned;
        unsigned int address;
        unsigned short int toWrite;

        public:
        STH_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~STH_imm();
    };

};

namespace leon3_accat_trap{

    class WRITEwim_imm : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int rd;
        unsigned int simm13;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        bool raiseException;
        unsigned int result;

        public:
        WRITEwim_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~WRITEwim_imm();
    };

};

namespace leon3_accat_trap{

    class LDD_imm : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int simm13;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        unsigned int address;
        sc_dt::uint64 readValue;
        bool notAligned;

        public:
        LDD_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~LDD_imm();
    };

};

namespace leon3_accat_trap{

    class SLL_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;
        unsigned int rs1_op;

        public:
        SLL_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SLL_imm();
    };

};

namespace leon3_accat_trap{

    class LDUHA_reg : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int asi;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        unsigned int address;
        unsigned int readValue;
        bool supervisor;
        bool notAligned;

        public:
        LDUHA_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~LDUHA_reg();
    };

};

namespace leon3_accat_trap{

    class TADDcc_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;
        bool temp_V;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        TADDcc_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~TADDcc_reg();
    };

};

namespace leon3_accat_trap{

    class TADDcc_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;
        bool temp_V;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        TADDcc_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~TADDcc_imm();
    };

};

namespace leon3_accat_trap{

    class SDIV_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;
        unsigned int pcounter;
        unsigned int npcounter;
        bool exception;
        bool temp_V;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        SDIV_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SDIV_imm();
    };

};

namespace leon3_accat_trap{

    class TSUBccTV_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;
        bool temp_V;
        unsigned int rs1_op;
        unsigned int rs2_op;
        unsigned int pcounter;
        unsigned int npcounter;

        public:
        TSUBccTV_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~TSUBccTV_imm();
    };

};

namespace leon3_accat_trap{

    class FLUSH_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        public:
        FLUSH_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~FLUSH_reg();
    };

};

namespace leon3_accat_trap{

    class ORNcc_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        ORNcc_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~ORNcc_reg();
    };

};

namespace leon3_accat_trap{

    class RETT_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        unsigned int rs1_op;
        unsigned int rs2_op;
        unsigned int targetAddr;
        unsigned int newCwp;
        bool exceptionEnabled;
        bool invalidWin;
        bool notAligned;
        bool supervisor;

        public:
        RETT_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~RETT_imm();
    };

};

namespace leon3_accat_trap{

    class SDIVcc_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        bool exception;
        unsigned int result;
        bool temp_V;
        unsigned int pcounter;
        unsigned int npcounter;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        SDIVcc_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SDIVcc_reg();
    };

};

namespace leon3_accat_trap{

    class ADD_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        ADD_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~ADD_reg();
    };

};

namespace leon3_accat_trap{

    class TRAP_imm : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int reserved1;
        unsigned int cond;
        unsigned int reserved2;
        unsigned int imm7;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        bool raiseException;

        public:
        TRAP_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~TRAP_imm();
    };

};

namespace leon3_accat_trap{

    class WRITEtbr_imm : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int rd;
        unsigned int simm13;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        bool raiseException;
        unsigned int result;

        public:
        WRITEtbr_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~WRITEtbr_imm();
    };

};

namespace leon3_accat_trap{

    class LDUB_reg : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int asi;

        protected:
        unsigned int address;
        unsigned int readValue;

        public:
        LDUB_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~LDUB_reg();
    };

};

namespace leon3_accat_trap{

    class RESTORE_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        bool okNewWin;
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;
        unsigned int newCwp;

        public:
        RESTORE_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~RESTORE_reg();
    };

};

namespace leon3_accat_trap{

    class ADDXcc_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        ADDXcc_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~ADDXcc_reg();
    };

};

namespace leon3_accat_trap{

    class STB_reg : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int asi;

        protected:
        unsigned int address;
        unsigned char toWrite;

        public:
        STB_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~STB_reg();
    };

};

namespace leon3_accat_trap{

    class AND_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;
        unsigned int rs1_op;

        public:
        AND_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~AND_imm();
    };

};

namespace leon3_accat_trap{

    class SMUL_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        SMUL_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SMUL_imm();
    };

};

namespace leon3_accat_trap{

    class ADD_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        ADD_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~ADD_imm();
    };

};

namespace leon3_accat_trap{

    class UMUL_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        UMUL_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~UMUL_imm();
    };

};

namespace leon3_accat_trap{

    class READwim : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int asr;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        bool supervisor;
        unsigned int wim_temp;

        public:
        READwim( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~READwim();
    };

};

namespace leon3_accat_trap{

    class LDSTUB_imm : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int simm13;

        protected:
        unsigned int address;
        unsigned int readValue;

        public:
        LDSTUB_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~LDSTUB_imm();
    };

};

namespace leon3_accat_trap{

    class SMAC_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        SMAC_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SMAC_imm();
    };

};

namespace leon3_accat_trap{

    class LDSB_reg : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int asi;

        protected:
        unsigned int address;
        unsigned int readValue;

        public:
        LDSB_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~LDSB_reg();
    };

};

namespace leon3_accat_trap{

    class ANDN_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        ANDN_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~ANDN_reg();
    };

};

namespace leon3_accat_trap{

    class TSUBccTV_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;
        bool temp_V;
        unsigned int rs1_op;
        unsigned int rs2_op;
        unsigned int pcounter;
        unsigned int npcounter;

        public:
        TSUBccTV_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~TSUBccTV_reg();
    };

};

namespace leon3_accat_trap{

    class SETHI : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int imm22;

        protected:
        unsigned int result;

        public:
        SETHI( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SETHI();
    };

};

namespace leon3_accat_trap{

    class SRA_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;
        unsigned int rs1_op;

        public:
        SRA_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SRA_imm();
    };

};

namespace leon3_accat_trap{

    class LDSH_reg : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int asi;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        unsigned int address;
        unsigned int readValue;
        bool notAligned;

        public:
        LDSH_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~LDSH_reg();
    };

};

namespace leon3_accat_trap{

    class UDIVcc_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        bool exception;
        unsigned int result;
        bool temp_V;
        unsigned int pcounter;
        unsigned int npcounter;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        UDIVcc_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~UDIVcc_reg();
    };

};

namespace leon3_accat_trap{

    class ORN_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;
        unsigned int rs1_op;

        public:
        ORN_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~ORN_imm();
    };

};

namespace leon3_accat_trap{

    class STD_reg : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int asi;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        bool notAligned;
        unsigned int address;
        sc_dt::uint64 toWrite;

        public:
        STD_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~STD_reg();
    };

};

namespace leon3_accat_trap{

    class ANDNcc_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;
        unsigned int rs1_op;

        public:
        ANDNcc_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~ANDNcc_imm();
    };

};

namespace leon3_accat_trap{

    class TADDccTV_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;
        bool temp_V;
        unsigned int rs1_op;
        unsigned int rs2_op;
        unsigned int pcounter;
        unsigned int npcounter;

        public:
        TADDccTV_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~TADDccTV_imm();
    };

};

namespace leon3_accat_trap{

    class WRITEtbr_reg : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;
        unsigned int rd;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        bool raiseException;
        unsigned int result;

        public:
        WRITEtbr_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~WRITEtbr_reg();
    };

};

namespace leon3_accat_trap{

    class SUBX_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        SUBX_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SUBX_reg();
    };

};

namespace leon3_accat_trap{

    class XNOR_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;
        unsigned int rs1_op;

        public:
        XNOR_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~XNOR_imm();
    };

};

namespace leon3_accat_trap{

    class UDIV_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;
        unsigned int pcounter;
        unsigned int npcounter;
        bool exception;
        bool temp_V;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        UDIV_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~UDIV_imm();
    };

};

namespace leon3_accat_trap{

    class LDSH_imm : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int simm13;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        unsigned int address;
        unsigned int readValue;
        bool notAligned;

        public:
        LDSH_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~LDSH_imm();
    };

};

namespace leon3_accat_trap{

    class UNIMP : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int imm22;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;

        public:
        UNIMP( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~UNIMP();
    };

};

namespace leon3_accat_trap{

    class LDSTUBA_reg : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int asi;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        bool supervisor;
        unsigned int address;
        unsigned int readValue;

        public:
        LDSTUBA_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~LDSTUBA_reg();
    };

};

namespace leon3_accat_trap{

    class UMULcc_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        UMULcc_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~UMULcc_imm();
    };

};

namespace leon3_accat_trap{

    class ORcc_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        ORcc_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~ORcc_reg();
    };

};

namespace leon3_accat_trap{

    class MULScc_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        MULScc_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~MULScc_imm();
    };

};

namespace leon3_accat_trap{

    class XORcc_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        XORcc_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~XORcc_reg();
    };

};

namespace leon3_accat_trap{

    class SUB_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        SUB_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SUB_reg();
    };

};

namespace leon3_accat_trap{

    class WRITEwim_reg : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;
        unsigned int rd;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        bool raiseException;
        unsigned int result;

        public:
        WRITEwim_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~WRITEwim_reg();
    };

};

namespace leon3_accat_trap{

    class UMAC_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        UMAC_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~UMAC_imm();
    };

};

namespace leon3_accat_trap{

    class TSUBcc_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;
        bool temp_V;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        TSUBcc_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~TSUBcc_reg();
    };

};

namespace leon3_accat_trap{

    class BRANCH : public Instruction{
        private:
        unsigned int a;
        unsigned int cond;
        unsigned int disp22;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;

        public:
        BRANCH( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~BRANCH();
    };

};

namespace leon3_accat_trap{

    class SMULcc_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        SMULcc_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SMULcc_reg();
    };

};

namespace leon3_accat_trap{

    class SUB_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        SUB_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SUB_imm();
    };

};

namespace leon3_accat_trap{

    class ADDcc_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        ADDcc_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~ADDcc_reg();
    };

};

namespace leon3_accat_trap{

    class XOR_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        XOR_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~XOR_reg();
    };

};

namespace leon3_accat_trap{

    class SUBcc_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        SUBcc_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SUBcc_imm();
    };

};

namespace leon3_accat_trap{

    class TADDccTV_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;
        bool temp_V;
        unsigned int rs1_op;
        unsigned int rs2_op;
        unsigned int pcounter;
        unsigned int npcounter;

        public:
        TADDccTV_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~TADDccTV_reg();
    };

};

namespace leon3_accat_trap{

    class SDIV_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;
        unsigned int pcounter;
        unsigned int npcounter;
        bool exception;
        bool temp_V;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        SDIV_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SDIV_reg();
    };

};

namespace leon3_accat_trap{

    class SMULcc_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        SMULcc_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SMULcc_imm();
    };

};

namespace leon3_accat_trap{

    class SWAP_reg : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int asi;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        bool notAligned;
        unsigned int address;
        unsigned int readValue;
        unsigned int toWrite;

        public:
        SWAP_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SWAP_reg();
    };

};

namespace leon3_accat_trap{

    class SUBX_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        SUBX_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SUBX_imm();
    };

};

namespace leon3_accat_trap{

    class STDA_reg : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int asi;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        bool supervisor;
        bool notAligned;
        unsigned int address;
        sc_dt::uint64 toWrite;

        public:
        STDA_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~STDA_reg();
    };

};

namespace leon3_accat_trap{

    class UMAC_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        UMAC_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~UMAC_reg();
    };

};

namespace leon3_accat_trap{

    class JUMP_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        bool trapNotAligned;

        public:
        JUMP_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~JUMP_imm();
    };

};

namespace leon3_accat_trap{

    class SMUL_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        SMUL_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SMUL_reg();
    };

};

namespace leon3_accat_trap{

    class XORcc_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;
        unsigned int rs1_op;

        public:
        XORcc_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~XORcc_imm();
    };

};

namespace leon3_accat_trap{

    class ORNcc_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;
        unsigned int rs1_op;

        public:
        ORNcc_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~ORNcc_imm();
    };

};

namespace leon3_accat_trap{

    class LDUBA_reg : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int asi;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        unsigned int address;
        unsigned int readValue;
        bool supervisor;

        public:
        LDUBA_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~LDUBA_reg();
    };

};

namespace leon3_accat_trap{

    class JUMP_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        bool trapNotAligned;

        public:
        JUMP_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~JUMP_reg();
    };

};

namespace leon3_accat_trap{

    class ADDX_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        ADDX_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~ADDX_reg();
    };

};

namespace leon3_accat_trap{

    class UDIV_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;
        unsigned int pcounter;
        unsigned int npcounter;
        bool exception;
        bool temp_V;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        UDIV_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~UDIV_reg();
    };

};

namespace leon3_accat_trap{

    class XNORcc_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;
        unsigned int rs1_op;

        public:
        XNORcc_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~XNORcc_imm();
    };

};

namespace leon3_accat_trap{

    class STBAR : public Instruction{

        public:
        STBAR( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~STBAR();
    };

};

namespace leon3_accat_trap{

    class LDA_reg : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int asi;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        unsigned int address;
        unsigned int readValue;
        bool supervisor;
        bool notAligned;

        public:
        LDA_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~LDA_reg();
    };

};

namespace leon3_accat_trap{

    class STHA_reg : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int asi;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        bool supervisor;
        bool notAligned;
        unsigned int address;
        unsigned short int toWrite;

        public:
        STHA_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~STHA_reg();
    };

};

namespace leon3_accat_trap{

    class LDDA_reg : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int asi;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        unsigned int address;
        sc_dt::uint64 readValue;
        bool supervisor;
        bool notAligned;

        public:
        LDDA_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~LDDA_reg();
    };

};

namespace leon3_accat_trap{

    class SLL_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        SLL_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SLL_reg();
    };

};

namespace leon3_accat_trap{

    class RESTORE_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        bool okNewWin;
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;
        unsigned int newCwp;

        public:
        RESTORE_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~RESTORE_imm();
    };

};

namespace leon3_accat_trap{

    class LD_imm : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int simm13;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        unsigned int address;
        unsigned int readValue;
        bool notAligned;

        public:
        LD_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~LD_imm();
    };

};

namespace leon3_accat_trap{

    class TRAP_reg : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;
        unsigned int reserved1;
        unsigned int cond;
        unsigned int asi;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        bool raiseException;

        public:
        TRAP_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~TRAP_reg();
    };

};

namespace leon3_accat_trap{

    class LDUB_imm : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        unsigned int simm13;

        protected:
        unsigned int address;
        unsigned int readValue;

        public:
        LDUB_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~LDUB_imm();
    };

};

namespace leon3_accat_trap{

    class RETT_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        unsigned int rs1_op;
        unsigned int rs2_op;
        unsigned int targetAddr;
        unsigned int newCwp;
        bool exceptionEnabled;
        bool invalidWin;
        bool notAligned;
        bool supervisor;

        public:
        RETT_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~RETT_reg();
    };

};

namespace leon3_accat_trap{

    class SDIVcc_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        bool exception;
        unsigned int result;
        bool temp_V;
        unsigned int pcounter;
        unsigned int npcounter;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        SDIVcc_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SDIVcc_imm();
    };

};

namespace leon3_accat_trap{

    class SAVE_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        bool okNewWin;
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;
        unsigned int newCwp;

        public:
        SAVE_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~SAVE_reg();
    };

};

namespace leon3_accat_trap{

    class OR_reg : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;

        protected:
        unsigned int result;
        unsigned int rs1_op;
        unsigned int rs2_op;

        public:
        OR_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~OR_reg();
    };

};

namespace leon3_accat_trap{

    class ORcc_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;
        unsigned int rs1_op;

        public:
        ORcc_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~ORcc_imm();
    };

};

namespace leon3_accat_trap{

    class CALL : public Instruction{
        private:
        unsigned int disp30;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        unsigned int oldPC;

        public:
        CALL( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~CALL();
    };

};

namespace leon3_accat_trap{

    class WRITEpsr_reg : public Instruction{
        private:
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        Alias rs2_fetch;
        Alias rs2_decode;
        Alias rs2_regs;
        Alias rs2_execute;
        Alias rs2_memory;
        Alias rs2_exception;
        Alias rs2_wb;
        unsigned int rs2_bit;
        unsigned int rd;

        protected:
        unsigned int pcounter;
        unsigned int npcounter;
        bool supervisorException;
        bool illegalCWP;
        unsigned int result;

        public:
        WRITEpsr_reg( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~WRITEpsr_reg();
    };

};

namespace leon3_accat_trap{

    class ANDcc_imm : public Instruction{
        private:
        Alias rd_fetch;
        Alias rd_decode;
        Alias rd_regs;
        Alias rd_execute;
        Alias rd_memory;
        Alias rd_exception;
        Alias rd_wb;
        unsigned int rd_bit;
        Alias rs1_fetch;
        Alias rs1_decode;
        Alias rs1_regs;
        Alias rs1_execute;
        Alias rs1_memory;
        Alias rs1_exception;
        Alias rs1_wb;
        unsigned int rs1_bit;
        unsigned int simm13;

        protected:
        unsigned int result;
        unsigned int rs1_op;

        public:
        ANDcc_imm( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        std::string printBusyRegs();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        Instruction * replicate() const throw();
        std::string getInstructionName() const throw();
        unsigned int getId() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getMnemonic() const throw();
        virtual ~ANDcc_imm();
    };

};

namespace leon3_accat_trap{

    class IRQ_IRQ_Instruction : public Instruction{

        protected:
        unsigned int pcounter;
        unsigned int npcounter;

        public:
        IRQ_IRQ_Instruction( PipelineRegister & PSR_pipe, PipelineRegister & WIM_pipe, PipelineRegister \
            & TBR_pipe, PipelineRegister & Y_pipe, PipelineRegister & PC_pipe, PipelineRegister \
            & NPC_pipe, PipelineRegister * GLOBAL_pipe, PipelineRegister * WINREGS_pipe, PipelineRegister \
            * ASR_pipe, Reg32_0 & PSR_fetch, Reg32_1 & WIM_fetch, Reg32_2 & TBR_fetch, Reg32_3 \
            & Y_fetch, Reg32_3 & PC_fetch, Reg32_3 & NPC_fetch, RegisterBankClass & GLOBAL_fetch, \
            Reg32_3 * WINREGS_fetch, Reg32_3 * ASR_fetch, Alias & FP_fetch, Alias & LR_fetch, \
            Alias & SP_fetch, Alias & PCR_fetch, Alias * REGS_fetch, Reg32_0 & PSR_decode, Reg32_1 \
            & WIM_decode, Reg32_2 & TBR_decode, Reg32_3 & Y_decode, Reg32_3 & PC_decode, Reg32_3 \
            & NPC_decode, RegisterBankClass & GLOBAL_decode, Reg32_3 * WINREGS_decode, Reg32_3 \
            * ASR_decode, Alias & FP_decode, Alias & LR_decode, Alias & SP_decode, Alias & PCR_decode, \
            Alias * REGS_decode, Reg32_0 & PSR_regs, Reg32_1 & WIM_regs, Reg32_2 & TBR_regs, \
            Reg32_3 & Y_regs, Reg32_3 & PC_regs, Reg32_3 & NPC_regs, RegisterBankClass & GLOBAL_regs, \
            Reg32_3 * WINREGS_regs, Reg32_3 * ASR_regs, Alias & FP_regs, Alias & LR_regs, Alias \
            & SP_regs, Alias & PCR_regs, Alias * REGS_regs, Reg32_0 & PSR_execute, Reg32_1 & \
            WIM_execute, Reg32_2 & TBR_execute, Reg32_3 & Y_execute, Reg32_3 & PC_execute, Reg32_3 \
            & NPC_execute, RegisterBankClass & GLOBAL_execute, Reg32_3 * WINREGS_execute, Reg32_3 \
            * ASR_execute, Alias & FP_execute, Alias & LR_execute, Alias & SP_execute, Alias \
            & PCR_execute, Alias * REGS_execute, Reg32_0 & PSR_memory, Reg32_1 & WIM_memory, \
            Reg32_2 & TBR_memory, Reg32_3 & Y_memory, Reg32_3 & PC_memory, Reg32_3 & NPC_memory, \
            RegisterBankClass & GLOBAL_memory, Reg32_3 * WINREGS_memory, Reg32_3 * ASR_memory, \
            Alias & FP_memory, Alias & LR_memory, Alias & SP_memory, Alias & PCR_memory, Alias \
            * REGS_memory, Reg32_0 & PSR_exception, Reg32_1 & WIM_exception, Reg32_2 & TBR_exception, \
            Reg32_3 & Y_exception, Reg32_3 & PC_exception, Reg32_3 & NPC_exception, RegisterBankClass \
            & GLOBAL_exception, Reg32_3 * WINREGS_exception, Reg32_3 * ASR_exception, Alias & \
            FP_exception, Alias & LR_exception, Alias & SP_exception, Alias & PCR_exception, \
            Alias * REGS_exception, Reg32_0 & PSR_wb, Reg32_1 & WIM_wb, Reg32_2 & TBR_wb, Reg32_3 \
            & Y_wb, Reg32_3 & PC_wb, Reg32_3 & NPC_wb, RegisterBankClass & GLOBAL_wb, Reg32_3 \
            * WINREGS_wb, Reg32_3 * ASR_wb, Alias & FP_wb, Alias & LR_wb, Alias & SP_wb, Alias \
            & PCR_wb, Alias * REGS_wb, TLMMemory & instrMem, TLMMemory & dataMem, PinTLM_out_32 \
            & irqAck, unsigned int & IRQ );
        unsigned int behavior_fetch( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_decode( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_regs( std::map< unsigned int, std::vector< Register * > > & \
            unlockQueue );
        unsigned int behavior_execute( std::map< unsigned int, std::vector< Register * > \
            > & unlockQueue );
        unsigned int behavior_memory( std::map< unsigned int, std::vector< Register * > > \
            & unlockQueue );
        unsigned int behavior_exception( std::map< unsigned int, std::vector< Register * \
            > > & unlockQueue );
        unsigned int behavior_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        Instruction * replicate() const throw();
        void setParams( const unsigned int & bitString ) throw();
        std::string getInstructionName() const throw();
        std::string getMnemonic() const throw();
        unsigned int getId() const throw();
        bool checkHazard_fetch();
        void lockRegs_fetch();
        bool checkHazard_decode();
        void lockRegs_decode();
        bool checkHazard_regs();
        void lockRegs_regs();
        bool checkHazard_execute();
        void lockRegs_execute();
        bool checkHazard_memory();
        void lockRegs_memory();
        bool checkHazard_exception();
        void lockRegs_exception();
        bool checkHazard_wb();
        void lockRegs_wb();
        void getUnlock_decode( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_regs( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        void getUnlock_execute( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_memory( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_exception( std::map< unsigned int, std::vector< Register * > > & unlockQueue \
            );
        void getUnlock_wb( std::map< unsigned int, std::vector< Register * > > & unlockQueue );
        std::string printBusyRegs();
        unsigned int & IRQ;
        inline void setInterruptValue( const unsigned int & interruptValue ) throw(){
            this->IRQ = interruptValue;
        }
        virtual ~IRQ_IRQ_Instruction();
    };

};



#endif
