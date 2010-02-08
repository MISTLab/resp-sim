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


#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include <systemc.h>
#include <instructions.hpp>
#include <map>
#include <vector>
#include <registers.hpp>
#include <alias.hpp>
#include <externalPorts.hpp>
#include <decoder.hpp>
#ifdef __GNUC__
#ifdef __GNUC_MINOR__
#if (__GNUC__ >= 4 && __GNUC_MINOR__ >= 3)
#include <tr1/unordered_map>
#define template_map std::tr1::unordered_map
#else
#include <ext/hash_map>
#define  template_map __gnu_cxx::hash_map
#endif
#else
#include <ext/hash_map>
#define  template_map __gnu_cxx::hash_map
#endif
#else
#ifdef _WIN32
#include <hash_map>
#define  template_map stdext::hash_map
#else
#include <map>
#define  template_map std::map
#endif
#endif

#include <ToolsIf.hpp>
#include <iostream>
#include <fstream>
#include <boost/circular_buffer.hpp>
#include <instructionBase.hpp>

#define ACC_MODEL
#define LT_IF
using namespace trap;
namespace leon3_acclt_trap{

    class BasePipeStage{

        protected:
        sc_time latency;
        BasePipeStage * stage_fetch;
        BasePipeStage * stage_decode;
        BasePipeStage * stage_regs;
        BasePipeStage * stage_execute;
        BasePipeStage * stage_memory;
        BasePipeStage * stage_exception;
        BasePipeStage * stage_wb;
        static std::map< unsigned int, std::vector< Register * > > unlockQueue;

        public:
        BasePipeStage( sc_time & latency, BasePipeStage * stage_fetch, BasePipeStage * stage_decode, \
            BasePipeStage * stage_regs, BasePipeStage * stage_execute, BasePipeStage * stage_memory, \
            BasePipeStage * stage_exception, BasePipeStage * stage_wb, BasePipeStage * prevStage \
            = NULL, BasePipeStage * succStage = NULL );
        bool stageEnded;
        bool stageBeginning;
        bool hasToFlush;
        sc_event stageEndedEv;
        sc_event stageBeginningEv;
        NOPInstruction * NOPInstrInstance;
        IRQ_IRQ_Instruction * IRQ_irqInstr;
        Instruction * curInstruction;
        Instruction * nextInstruction;
        void flush() throw();
        bool chStalled;
        bool stalled;
        BasePipeStage * prevStage;
        BasePipeStage * succStage;
    };

};

namespace leon3_acclt_trap{

    class FETCH_PipeStage : public BasePipeStage, public sc_module{
        private:
        void waitPipeBegin() throw();
        void waitPipeEnd() throw();
        void refreshRegisters() throw();
        PipelineRegister & PSR;
        PipelineRegister & WIM;
        PipelineRegister & TBR;
        PipelineRegister & Y;
        PipelineRegister & NPC;
        PipelineRegister * GLOBAL;
        PipelineRegister * WINREGS;
        PipelineRegister * ASR;
        Alias & FP_fetch;
        Alias & LR_fetch;
        Alias & SP_fetch;
        Alias & PCR_fetch;
        Alias * REGS_fetch;
        Alias & FP_decode;
        Alias & LR_decode;
        Alias & SP_decode;
        Alias & PCR_decode;
        Alias * REGS_decode;
        Alias & FP_regs;
        Alias & LR_regs;
        Alias & SP_regs;
        Alias & PCR_regs;
        Alias * REGS_regs;
        Alias & FP_execute;
        Alias & LR_execute;
        Alias & SP_execute;
        Alias & PCR_execute;
        Alias * REGS_execute;
        Alias & FP_memory;
        Alias & LR_memory;
        Alias & SP_memory;
        Alias & PCR_memory;
        Alias * REGS_memory;
        Alias & FP_exception;
        Alias & LR_exception;
        Alias & SP_exception;
        Alias & PCR_exception;
        Alias * REGS_exception;
        Alias & FP_wb;
        Alias & LR_wb;
        Alias & SP_wb;
        Alias & PCR_wb;
        Alias * REGS_wb;
        TLMMemory & instrMem;
        Instruction * * & INSTRUCTIONS;
        PipelineRegister & PC;
        unsigned int & numInstructions;
        bool & instrExecuting;
        sc_event & instrEndEvent;
        unsigned int & IRQ;
        template_map< unsigned int, CacheElem > instrCache;
        ToolsManager< unsigned int > & toolManager;

        public:
        SC_HAS_PROCESS( FETCH_PipeStage );
        FETCH_PipeStage( sc_time & profTimeEnd, sc_time & profTimeStart, ToolsManager< unsigned \
            int > & toolManager, unsigned int & IRQ, sc_event & instrEndEvent, bool & instrExecuting, \
            unsigned int & numInstructions, PipelineRegister & PC, Instruction * * & INSTRUCTIONS, \
            TLMMemory & instrMem, Alias * REGS_wb, Alias & PCR_wb, Alias & SP_wb, Alias & LR_wb, \
            Alias & FP_wb, Alias * REGS_exception, Alias & PCR_exception, Alias & SP_exception, \
            Alias & LR_exception, Alias & FP_exception, Alias * REGS_memory, Alias & PCR_memory, \
            Alias & SP_memory, Alias & LR_memory, Alias & FP_memory, Alias * REGS_execute, Alias \
            & PCR_execute, Alias & SP_execute, Alias & LR_execute, Alias & FP_execute, Alias \
            * REGS_regs, Alias & PCR_regs, Alias & SP_regs, Alias & LR_regs, Alias & FP_regs, \
            Alias * REGS_decode, Alias & PCR_decode, Alias & SP_decode, Alias & LR_decode, Alias \
            & FP_decode, Alias * REGS_fetch, Alias & PCR_fetch, Alias & SP_fetch, Alias & LR_fetch, \
            Alias & FP_fetch, PipelineRegister * ASR, PipelineRegister * WINREGS, PipelineRegister \
            * GLOBAL, PipelineRegister & NPC, PipelineRegister & Y, PipelineRegister & TBR, PipelineRegister \
            & WIM, PipelineRegister & PSR, sc_module_name pipeName, sc_time & latency, BasePipeStage \
            * stage_fetch, BasePipeStage * stage_decode, BasePipeStage * stage_regs, BasePipeStage \
            * stage_execute, BasePipeStage * stage_memory, BasePipeStage * stage_exception, BasePipeStage \
            * stage_wb, BasePipeStage * prevStage = NULL, BasePipeStage * succStage = NULL );
        void behavior();
        Decoder decoder;
        sc_time & profTimeStart;
        sc_time & profTimeEnd;
        unsigned int profStartAddr;
        unsigned int profEndAddr;
        std::ofstream histFile;
        bool historyEnabled;
        boost::circular_buffer< HistoryInstrType > instHistoryQueue;
        unsigned int undumpedHistElems;
        ~FETCH_PipeStage();
    };

};

namespace leon3_acclt_trap{

    class DECODE_PipeStage : public BasePipeStage, public sc_module{
        private:
        void waitPipeBegin() throw();
        void waitPipeEnd() throw();

        public:
        SC_HAS_PROCESS( DECODE_PipeStage );
        DECODE_PipeStage( sc_module_name pipeName, sc_time & latency, BasePipeStage * stage_fetch, \
            BasePipeStage * stage_decode, BasePipeStage * stage_regs, BasePipeStage * stage_execute, \
            BasePipeStage * stage_memory, BasePipeStage * stage_exception, BasePipeStage * stage_wb, \
            BasePipeStage * prevStage = NULL, BasePipeStage * succStage = NULL );
        void behavior();
    };

};

namespace leon3_acclt_trap{

    class REGS_PipeStage : public BasePipeStage, public sc_module{
        private:
        void waitPipeBegin() throw();
        void waitPipeEnd() throw();

        public:
        SC_HAS_PROCESS( REGS_PipeStage );
        REGS_PipeStage( sc_module_name pipeName, sc_time & latency, BasePipeStage * stage_fetch, \
            BasePipeStage * stage_decode, BasePipeStage * stage_regs, BasePipeStage * stage_execute, \
            BasePipeStage * stage_memory, BasePipeStage * stage_exception, BasePipeStage * stage_wb, \
            BasePipeStage * prevStage = NULL, BasePipeStage * succStage = NULL );
        void behavior();
    };

};

namespace leon3_acclt_trap{

    class EXECUTE_PipeStage : public BasePipeStage, public sc_module{
        private:
        void waitPipeBegin() throw();
        void waitPipeEnd() throw();

        public:
        SC_HAS_PROCESS( EXECUTE_PipeStage );
        EXECUTE_PipeStage( sc_module_name pipeName, sc_time & latency, BasePipeStage * stage_fetch, \
            BasePipeStage * stage_decode, BasePipeStage * stage_regs, BasePipeStage * stage_execute, \
            BasePipeStage * stage_memory, BasePipeStage * stage_exception, BasePipeStage * stage_wb, \
            BasePipeStage * prevStage = NULL, BasePipeStage * succStage = NULL );
        void behavior();
    };

};

namespace leon3_acclt_trap{

    class MEMORY_PipeStage : public BasePipeStage, public sc_module{
        private:
        void waitPipeBegin() throw();
        void waitPipeEnd() throw();

        public:
        SC_HAS_PROCESS( MEMORY_PipeStage );
        MEMORY_PipeStage( sc_module_name pipeName, sc_time & latency, BasePipeStage * stage_fetch, \
            BasePipeStage * stage_decode, BasePipeStage * stage_regs, BasePipeStage * stage_execute, \
            BasePipeStage * stage_memory, BasePipeStage * stage_exception, BasePipeStage * stage_wb, \
            BasePipeStage * prevStage = NULL, BasePipeStage * succStage = NULL );
        void behavior();
    };

};

namespace leon3_acclt_trap{

    class EXCEPTION_PipeStage : public BasePipeStage, public sc_module{
        private:
        void waitPipeBegin() throw();
        void waitPipeEnd() throw();

        public:
        SC_HAS_PROCESS( EXCEPTION_PipeStage );
        EXCEPTION_PipeStage( sc_module_name pipeName, sc_time & latency, BasePipeStage * \
            stage_fetch, BasePipeStage * stage_decode, BasePipeStage * stage_regs, BasePipeStage \
            * stage_execute, BasePipeStage * stage_memory, BasePipeStage * stage_exception, BasePipeStage \
            * stage_wb, BasePipeStage * prevStage = NULL, BasePipeStage * succStage = NULL );
        void behavior();
    };

};

namespace leon3_acclt_trap{

    class WB_PipeStage : public BasePipeStage, public sc_module{
        private:
        void waitPipeBegin() throw();
        void waitPipeEnd() throw();

        public:
        SC_HAS_PROCESS( WB_PipeStage );
        WB_PipeStage( sc_module_name pipeName, sc_time & latency, BasePipeStage * stage_fetch, \
            BasePipeStage * stage_decode, BasePipeStage * stage_regs, BasePipeStage * stage_execute, \
            BasePipeStage * stage_memory, BasePipeStage * stage_exception, BasePipeStage * stage_wb, \
            BasePipeStage * prevStage = NULL, BasePipeStage * succStage = NULL );
        void behavior();
    };

};



#endif
