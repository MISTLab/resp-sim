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


#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include <systemc.h>
#include <instructions.hpp>
#include <interface.hpp>
#include <ToolsIf.hpp>
#include <registers.hpp>
#include <alias.hpp>
#include <externalPorts.hpp>
#ifdef __GNUC__
#include <ext/hash_map>
#define  template_map __gnu_cxx::hash_map
#else
#ifdef _WIN32
#include <hash_map>
#define  template_map stdext::hash_map
#else
#include <map>
#define  template_map std::map
#endif
#endif

#include <irqPorts.hpp>
#include <externalPins.hpp>
#include <string>
#include <pipeline.hpp>

#define ACC_MODEL
#define LT_IF
using namespace trap;
namespace leon3_acclt_trap{

    class Processor_leon3_acclt : public sc_module{
        private:
        bool instrExecuting;
        sc_event instrEndEvent;
        static Instruction * * INSTRUCTIONS;
        template_map< unsigned int, CacheElem > instrCache;
        static int numInstances;
        unsigned int IRQ;

        public:
        SC_HAS_PROCESS( Processor_leon3_acclt );
        Processor_leon3_acclt( sc_module_name name, sc_time latency );
        void resetOp();
        void end_of_elaboration();
        Instruction * decode( unsigned int bitString );
        LEON3_ABIIf * abiIf;
        LEON3_ABIIf & getInterface();
        ToolsManager< unsigned int > toolManager;
        Reg32_0 PSR;
        Reg32_0 PSR_fetch;
        Reg32_0 PSR_decode;
        Reg32_0 PSR_regs;
        Reg32_0 PSR_execute;
        Reg32_0 PSR_memory;
        Reg32_0 PSR_exception;
        Reg32_0 PSR_wb;
        PipelineRegister PSR_pipe;
        Reg32_1 WIM;
        Reg32_1 WIM_fetch;
        Reg32_1 WIM_decode;
        Reg32_1 WIM_regs;
        Reg32_1 WIM_execute;
        Reg32_1 WIM_memory;
        Reg32_1 WIM_exception;
        Reg32_1 WIM_wb;
        PipelineRegister WIM_pipe;
        Reg32_2 TBR;
        Reg32_2 TBR_fetch;
        Reg32_2 TBR_decode;
        Reg32_2 TBR_regs;
        Reg32_2 TBR_execute;
        Reg32_2 TBR_memory;
        Reg32_2 TBR_exception;
        Reg32_2 TBR_wb;
        PipelineRegister TBR_pipe;
        Reg32_3 Y;
        Reg32_3 Y_fetch;
        Reg32_3 Y_decode;
        Reg32_3 Y_regs;
        Reg32_3 Y_execute;
        Reg32_3 Y_memory;
        Reg32_3 Y_exception;
        Reg32_3 Y_wb;
        PipelineRegister Y_pipe;
        Reg32_3 PC;
        Reg32_3 PC_fetch;
        Reg32_3 PC_decode;
        Reg32_3 PC_regs;
        Reg32_3 PC_execute;
        Reg32_3 PC_memory;
        Reg32_3 PC_exception;
        Reg32_3 PC_wb;
        PipelineRegister_exception_decode_fetch PC_pipe;
        Reg32_3 NPC;
        Reg32_3 NPC_fetch;
        Reg32_3 NPC_decode;
        Reg32_3 NPC_regs;
        Reg32_3 NPC_execute;
        Reg32_3 NPC_memory;
        Reg32_3 NPC_exception;
        Reg32_3 NPC_wb;
        PipelineRegister_exception_decode_fetch NPC_pipe;
        RegisterBankClass GLOBAL;
        RegisterBankClass GLOBAL_fetch;
        RegisterBankClass GLOBAL_decode;
        RegisterBankClass GLOBAL_regs;
        RegisterBankClass GLOBAL_execute;
        RegisterBankClass GLOBAL_memory;
        RegisterBankClass GLOBAL_exception;
        RegisterBankClass GLOBAL_wb;
        PipelineRegister GLOBAL_pipe[8];
        Reg32_3 WINREGS[128];
        Reg32_3 WINREGS_fetch[128];
        Reg32_3 WINREGS_decode[128];
        Reg32_3 WINREGS_regs[128];
        Reg32_3 WINREGS_execute[128];
        Reg32_3 WINREGS_memory[128];
        Reg32_3 WINREGS_exception[128];
        Reg32_3 WINREGS_wb[128];
        PipelineRegister WINREGS_pipe[128];
        Reg32_3 ASR[32];
        Reg32_3 ASR_fetch[32];
        Reg32_3 ASR_decode[32];
        Reg32_3 ASR_regs[32];
        Reg32_3 ASR_execute[32];
        Reg32_3 ASR_memory[32];
        Reg32_3 ASR_exception[32];
        Reg32_3 ASR_wb[32];
        PipelineRegister ASR_pipe[32];
        Alias FP_fetch;
        Alias FP_decode;
        Alias FP_regs;
        Alias FP_execute;
        Alias FP_memory;
        Alias FP_exception;
        Alias FP_wb;
        Alias LR_fetch;
        Alias LR_decode;
        Alias LR_regs;
        Alias LR_execute;
        Alias LR_memory;
        Alias LR_exception;
        Alias LR_wb;
        Alias SP_fetch;
        Alias SP_decode;
        Alias SP_regs;
        Alias SP_execute;
        Alias SP_memory;
        Alias SP_exception;
        Alias SP_wb;
        Alias PCR_fetch;
        Alias PCR_decode;
        Alias PCR_regs;
        Alias PCR_execute;
        Alias PCR_memory;
        Alias PCR_exception;
        Alias PCR_wb;
        Alias REGS_fetch[32];
        Alias REGS_decode[32];
        Alias REGS_regs[32];
        Alias REGS_execute[32];
        Alias REGS_memory[32];
        Alias REGS_exception[32];
        Alias REGS_wb[32];
        TLMMemory instrMem;
        TLMMemory dataMem;
        sc_time latency;
        sc_time profTimeStart;
        sc_time profTimeEnd;
        unsigned int numInstructions;
        unsigned int ENTRY_POINT;
        unsigned int PROGRAM_LIMIT;
        unsigned int PROGRAM_START;
        IntrTLMPort_32 IRQ_port;
        PinTLM_out_32 irqAck;
        void setProfilingRange( unsigned int startAddr, unsigned int endAddr );
        void enableHistory( std::string fileName = "" );
        WB_PipeStage wb_stage;
        EXCEPTION_PipeStage exception_stage;
        MEMORY_PipeStage memory_stage;
        EXECUTE_PipeStage execute_stage;
        REGS_PipeStage regs_stage;
        DECODE_PipeStage decode_stage;
        FETCH_PipeStage fetch_stage;
        static NOPInstruction * NOPInstrInstance;
        IRQ_IRQ_Instruction * IRQ_irqInstr;
        ~Processor_leon3_acclt();
    };

};



#endif
