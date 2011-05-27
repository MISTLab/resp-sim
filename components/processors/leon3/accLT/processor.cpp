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



#include <processor.hpp>
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

using namespace leon3_acclt_trap;
using namespace trap;
void leon3_acclt_trap::Processor_leon3_acclt::resetOp(){
    for(int i = 0; i < 8; i++){
        GLOBAL[i] = 0;
    }
    for(int i = 0; i < 128; i++){
        WINREGS[i] = 0;
    }
    ASR[0] = 0x0;
    ASR[1] = 0x0;
    ASR[2] = 0x0;
    ASR[3] = 0x0;
    ASR[4] = 0x0;
    ASR[5] = 0x0;
    ASR[6] = 0x0;
    ASR[7] = 0x0;
    ASR[8] = 0x0;
    ASR[9] = 0x0;
    ASR[10] = 0x0;
    ASR[11] = 0x0;
    ASR[12] = 0x0;
    ASR[13] = 0x0;
    ASR[14] = 0x0;
    ASR[15] = 0x0;
    ASR[16] = 0x0;
    ASR[17] = 0x308;
    ASR[18] = 0x0;
    ASR[19] = 0x0;
    ASR[20] = 0x0;
    ASR[21] = 0x0;
    ASR[22] = 0x0;
    ASR[23] = 0x0;
    ASR[24] = 0x0;
    ASR[25] = 0x0;
    ASR[26] = 0x0;
    ASR[27] = 0x0;
    ASR[28] = 0x0;
    ASR[29] = 0x0;
    ASR[30] = 0x0;
    ASR[31] = 0x0;
    PSR = 0xf3000080L;
    WIM = 0x0;
    TBR = 0x0;
    Y = 0x0;
    PC = ENTRY_POINT;
    NPC = ENTRY_POINT + 4;
    PSR_fetch = PSR;
    PSR_decode = PSR;
    PSR_regs = PSR;
    PSR_execute = PSR;
    PSR_memory = PSR;
    PSR_exception = PSR;
    PSR_wb = PSR;
    PSR_pipe.hasToPropagate = false;
    WIM_fetch = WIM;
    WIM_decode = WIM;
    WIM_regs = WIM;
    WIM_execute = WIM;
    WIM_memory = WIM;
    WIM_exception = WIM;
    WIM_wb = WIM;
    WIM_pipe.hasToPropagate = false;
    TBR_fetch = TBR;
    TBR_decode = TBR;
    TBR_regs = TBR;
    TBR_execute = TBR;
    TBR_memory = TBR;
    TBR_exception = TBR;
    TBR_wb = TBR;
    TBR_pipe.hasToPropagate = false;
    Y_fetch = Y;
    Y_decode = Y;
    Y_regs = Y;
    Y_execute = Y;
    Y_memory = Y;
    Y_exception = Y;
    Y_wb = Y;
    Y_pipe.hasToPropagate = false;
    PC_fetch = PC;
    PC_decode = PC;
    PC_regs = PC;
    PC_execute = PC;
    PC_memory = PC;
    PC_exception = PC;
    PC_wb = PC;
    PC_pipe.hasToPropagate = false;
    NPC_fetch = NPC;
    NPC_decode = NPC;
    NPC_regs = NPC;
    NPC_execute = NPC;
    NPC_memory = NPC;
    NPC_exception = NPC;
    NPC_wb = NPC;
    NPC_pipe.hasToPropagate = false;
    for(int i = 0; i < 8; i++){
        GLOBAL_fetch[i] = GLOBAL[i];
        GLOBAL_decode[i] = GLOBAL[i];
        GLOBAL_regs[i] = GLOBAL[i];
        GLOBAL_execute[i] = GLOBAL[i];
        GLOBAL_memory[i] = GLOBAL[i];
        GLOBAL_exception[i] = GLOBAL[i];
        GLOBAL_wb[i] = GLOBAL[i];
        GLOBAL_pipe[i].hasToPropagate = false;
    }
    for(int i = 0; i < 128; i++){
        WINREGS_fetch[i] = WINREGS[i];
        WINREGS_decode[i] = WINREGS[i];
        WINREGS_regs[i] = WINREGS[i];
        WINREGS_execute[i] = WINREGS[i];
        WINREGS_memory[i] = WINREGS[i];
        WINREGS_exception[i] = WINREGS[i];
        WINREGS_wb[i] = WINREGS[i];
        WINREGS_pipe[i].hasToPropagate = false;
    }
    for(int i = 0; i < 32; i++){
        ASR_fetch[i] = ASR[i];
        ASR_decode[i] = ASR[i];
        ASR_regs[i] = ASR[i];
        ASR_execute[i] = ASR[i];
        ASR_memory[i] = ASR[i];
        ASR_exception[i] = ASR[i];
        ASR_wb[i] = ASR[i];
        ASR_pipe[i].hasToPropagate = false;
    }
    this->IRQ = -1;
}

void leon3_acclt_trap::Processor_leon3_acclt::end_of_elaboration(){
    this->resetOp();
}

Instruction * leon3_acclt_trap::Processor_leon3_acclt::decode( unsigned int bitString ){
    int instrId = this->fetch_stage.decoder.decode(bitString);
    if(instrId >= 0){
        Instruction * instr = Processor_leon3_acclt::INSTRUCTIONS[instrId];
        instr->setParams(bitString);
        return instr;
    }
    return NULL;
}

LEON3_ABIIf & leon3_acclt_trap::Processor_leon3_acclt::getInterface(){
    return *this->abiIf;
}

Instruction * * leon3_acclt_trap::Processor_leon3_acclt::INSTRUCTIONS = NULL;
int leon3_acclt_trap::Processor_leon3_acclt::numInstances = 0;
void leon3_acclt_trap::Processor_leon3_acclt::setProfilingRange( unsigned int startAddr, \
    unsigned int endAddr ){
    this->fetch_stage.profStartAddr = startAddr;
    this->fetch_stage.profEndAddr = endAddr;
}

void leon3_acclt_trap::Processor_leon3_acclt::enableHistory( std::string fileName ){
    this->fetch_stage.historyEnabled = true;
    this->fetch_stage.histFile.open(fileName.c_str(), ios::out | ios::ate);
}

NOPInstruction * leon3_acclt_trap::Processor_leon3_acclt::NOPInstrInstance;
leon3_acclt_trap::Processor_leon3_acclt::Processor_leon3_acclt( sc_module_name name, \
    sc_time latency ) : sc_module(name), latency(latency), instrMem("instrMem"), dataMem("dataMem"), \
    IRQ_port("IRQ_IRQ", IRQ), irqAck("irqAck_PIN"),
wb_stage("wb", latency, &fetch_stage, &decode_stage, &regs_stage, &execute_stage, \
    &memory_stage, &exception_stage, NULL, &exception_stage, NULL),
exception_stage("exception", latency, &fetch_stage, &decode_stage, &regs_stage, &execute_stage, \
    &memory_stage, NULL, &wb_stage, &memory_stage, &wb_stage),
memory_stage("memory", latency, &fetch_stage, &decode_stage, &regs_stage, &execute_stage, \
    NULL, &exception_stage, &wb_stage, &execute_stage, &exception_stage),
execute_stage("execute", latency, &fetch_stage, &decode_stage, &regs_stage, NULL, \
    &memory_stage, &exception_stage, &wb_stage, &regs_stage, &memory_stage),
regs_stage("regs", latency, &fetch_stage, &decode_stage, NULL, &execute_stage, &memory_stage, \
    &exception_stage, &wb_stage, &decode_stage, &execute_stage),
decode_stage("decode", latency, &fetch_stage, NULL, &regs_stage, &execute_stage, \
    &memory_stage, &exception_stage, &wb_stage, &fetch_stage, &regs_stage),
fetch_stage(profTimeEnd, profTimeStart, toolManager, this->IRQ, instrEndEvent, instrExecuting, \
    numInstructions, PC_pipe, Processor_leon3_acclt::INSTRUCTIONS, instrMem, REGS_wb, \
    PCR_wb, SP_wb, LR_wb, FP_wb, REGS_exception, PCR_exception, SP_exception, LR_exception, \
    FP_exception, REGS_memory, PCR_memory, SP_memory, LR_memory, FP_memory, REGS_execute, \
    PCR_execute, SP_execute, LR_execute, FP_execute, REGS_regs, PCR_regs, SP_regs, LR_regs, \
    FP_regs, REGS_decode, PCR_decode, SP_decode, LR_decode, FP_decode, REGS_fetch, PCR_fetch, \
    SP_fetch, LR_fetch, FP_fetch, ASR_pipe, WINREGS_pipe, GLOBAL_pipe, NPC_pipe, Y_pipe, \
    TBR_pipe, WIM_pipe, PSR_pipe, "fetch", latency, NULL, &decode_stage, &regs_stage, \
    &execute_stage, &memory_stage, &exception_stage, &wb_stage, NULL, &decode_stage){
    Processor_leon3_acclt::numInstances++;
    if(Processor_leon3_acclt::INSTRUCTIONS == NULL){
        // Initialization of the array holding the initial instance of the instructions
        Processor_leon3_acclt::INSTRUCTIONS = new Instruction *[145];
        Processor_leon3_acclt::INSTRUCTIONS[126] = new READasr(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[130] = new WRITEY_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[58] = new XNOR_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[44] = new ANDNcc_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[0] = new LDSB_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[135] = new WRITEpsr_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[125] = new READy(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[60] = new XNORcc_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[127] = new READpsr(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[41] = new ANDN_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[40] = new ANDcc_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[87] = new TSUBcc_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[12] = new LDSBA_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[6] = new LDUH_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[28] = new STA_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[50] = new ORN_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[13] = new LDSHA_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[26] = new STBA_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[22] = new ST_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[129] = new READtbr(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[109] = new UDIVcc_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[35] = new SWAPA_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[73] = new ADDXcc_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[18] = new STB_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[85] = new SUBXcc_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[21] = new STH_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[63] = new SRL_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[133] = new WRITEasr_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[98] = new UMULcc_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[31] = new LDSTUB_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[53] = new XOR_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[104] = new SMAC_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[132] = new WRITEasr_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[9] = new LD_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[23] = new ST_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[82] = new SUBcc_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[11] = new LDD_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[69] = new ADDcc_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[7] = new LDUH_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[64] = new SRL_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[113] = new SAVE_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[92] = new MULScc_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[45] = new OR_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[24] = new STD_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[86] = new SUBXcc_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[71] = new ADDX_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[33] = new SWAP_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[94] = new UMUL_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[131] = new WRITEY_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[38] = new AND_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[143] = new FLUSH_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[66] = new SRA_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[20] = new STH_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[137] = new WRITEwim_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[10] = new LDD_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[61] = new SLL_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[15] = new LDUHA_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[76] = new TADDcc_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[75] = new TADDcc_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[107] = new SDIV_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[89] = new TSUBccTV_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[142] = new FLUSH_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[52] = new ORNcc_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[121] = new RETT_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[112] = new SDIVcc_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[68] = new ADD_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[123] = new TRAP_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[139] = new WRITEtbr_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[5] = new LDUB_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[116] = new RESTORE_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[74] = new ADDXcc_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[19] = new STB_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[37] = new AND_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[95] = new SMUL_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[67] = new ADD_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[93] = new UMUL_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[128] = new READwim(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[30] = new LDSTUB_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[103] = new SMAC_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[1] = new LDSB_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[42] = new ANDN_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[90] = new TSUBccTV_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[36] = new SETHI(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[65] = new SRA_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[3] = new LDSH_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[110] = new UDIVcc_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[49] = new ORN_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[25] = new STD_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[43] = new ANDNcc_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[77] = new TADDccTV_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[138] = new WRITEtbr_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[84] = new SUBX_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[57] = new XNOR_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[105] = new UDIV_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[2] = new LDSH_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[141] = new UNIMP(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[32] = new LDSTUBA_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[97] = new UMULcc_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[48] = new ORcc_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[91] = new MULScc_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[56] = new XORcc_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[80] = new SUB_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[136] = new WRITEwim_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[101] = new UMAC_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[88] = new TSUBcc_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[117] = new BRANCH(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[100] = new SMULcc_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[79] = new SUB_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[70] = new ADDcc_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[54] = new XOR_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[81] = new SUBcc_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[78] = new TADDccTV_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[108] = new SDIV_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[99] = new SMULcc_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[34] = new SWAP_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[83] = new SUBX_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[29] = new STDA_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[102] = new UMAC_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[119] = new JUMP_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[96] = new SMUL_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[55] = new XORcc_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[51] = new ORNcc_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[14] = new LDUBA_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[120] = new JUMP_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[72] = new ADDX_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[106] = new UDIV_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[59] = new XNORcc_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[140] = new STBAR(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[16] = new LDA_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[27] = new STHA_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[17] = new LDDA_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[62] = new SLL_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[115] = new RESTORE_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[8] = new LD_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[124] = new TRAP_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[4] = new LDUB_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[122] = new RETT_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[111] = new SDIVcc_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[114] = new SAVE_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[46] = new OR_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[47] = new ORcc_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[118] = new CALL(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[134] = new WRITEpsr_reg(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[39] = new ANDcc_imm(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::INSTRUCTIONS[144] = new InvalidInstr(PSR_pipe, WIM_pipe, TBR_pipe, \
            Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, \
            TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, \
            FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, \
            Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, \
            LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, \
            PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, \
            PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        Processor_leon3_acclt::NOPInstrInstance = new NOPInstruction(PSR_pipe, WIM_pipe, \
            TBR_pipe, Y_pipe, PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, \
            WIM_fetch, TBR_fetch, Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, \
            ASR_fetch, FP_fetch, LR_fetch, SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, \
            TBR_decode, Y_decode, PC_decode, NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, \
            FP_decode, LR_decode, SP_decode, PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, \
            Y_regs, PC_regs, NPC_regs, GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, \
            SP_regs, PCR_regs, REGS_regs, PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, \
            NPC_execute, GLOBAL_execute, WINREGS_execute, ASR_execute, FP_execute, LR_execute, \
            SP_execute, PCR_execute, REGS_execute, PSR_memory, WIM_memory, TBR_memory, Y_memory, \
            PC_memory, NPC_memory, GLOBAL_memory, WINREGS_memory, ASR_memory, FP_memory, LR_memory, \
            SP_memory, PCR_memory, REGS_memory, PSR_exception, WIM_exception, TBR_exception, \
            Y_exception, PC_exception, NPC_exception, GLOBAL_exception, WINREGS_exception, ASR_exception, \
            FP_exception, LR_exception, SP_exception, PCR_exception, REGS_exception, PSR_wb, \
            WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, WINREGS_wb, ASR_wb, FP_wb, LR_wb, \
            SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck);
        fetch_stage.NOPInstrInstance = Processor_leon3_acclt::NOPInstrInstance;
        decode_stage.NOPInstrInstance = Processor_leon3_acclt::NOPInstrInstance;
        regs_stage.NOPInstrInstance = Processor_leon3_acclt::NOPInstrInstance;
        execute_stage.NOPInstrInstance = Processor_leon3_acclt::NOPInstrInstance;
        memory_stage.NOPInstrInstance = Processor_leon3_acclt::NOPInstrInstance;
        exception_stage.NOPInstrInstance = Processor_leon3_acclt::NOPInstrInstance;
        wb_stage.NOPInstrInstance = Processor_leon3_acclt::NOPInstrInstance;
    }
    this->IRQ_irqInstr = new IRQ_IRQ_Instruction(PSR_pipe, WIM_pipe, TBR_pipe, Y_pipe, \
        PC_pipe, NPC_pipe, GLOBAL_pipe, WINREGS_pipe, ASR_pipe, PSR_fetch, WIM_fetch, TBR_fetch, \
        Y_fetch, PC_fetch, NPC_fetch, GLOBAL_fetch, WINREGS_fetch, ASR_fetch, FP_fetch, LR_fetch, \
        SP_fetch, PCR_fetch, REGS_fetch, PSR_decode, WIM_decode, TBR_decode, Y_decode, PC_decode, \
        NPC_decode, GLOBAL_decode, WINREGS_decode, ASR_decode, FP_decode, LR_decode, SP_decode, \
        PCR_decode, REGS_decode, PSR_regs, WIM_regs, TBR_regs, Y_regs, PC_regs, NPC_regs, \
        GLOBAL_regs, WINREGS_regs, ASR_regs, FP_regs, LR_regs, SP_regs, PCR_regs, REGS_regs, \
        PSR_execute, WIM_execute, TBR_execute, Y_execute, PC_execute, NPC_execute, GLOBAL_execute, \
        WINREGS_execute, ASR_execute, FP_execute, LR_execute, SP_execute, PCR_execute, REGS_execute, \
        PSR_memory, WIM_memory, TBR_memory, Y_memory, PC_memory, NPC_memory, GLOBAL_memory, \
        WINREGS_memory, ASR_memory, FP_memory, LR_memory, SP_memory, PCR_memory, REGS_memory, \
        PSR_exception, WIM_exception, TBR_exception, Y_exception, PC_exception, NPC_exception, \
        GLOBAL_exception, WINREGS_exception, ASR_exception, FP_exception, LR_exception, SP_exception, \
        PCR_exception, REGS_exception, PSR_wb, WIM_wb, TBR_wb, Y_wb, PC_wb, NPC_wb, GLOBAL_wb, \
        WINREGS_wb, ASR_wb, FP_wb, LR_wb, SP_wb, PCR_wb, REGS_wb, instrMem, dataMem, irqAck, \
        this->IRQ);
    this->fetch_stage.IRQ_irqInstr = this->IRQ_irqInstr;
    this->decode_stage.IRQ_irqInstr = this->IRQ_irqInstr;
    this->regs_stage.IRQ_irqInstr = this->IRQ_irqInstr;
    this->execute_stage.IRQ_irqInstr = this->IRQ_irqInstr;
    this->memory_stage.IRQ_irqInstr = this->IRQ_irqInstr;
    this->exception_stage.IRQ_irqInstr = this->IRQ_irqInstr;
    this->wb_stage.IRQ_irqInstr = this->IRQ_irqInstr;
    // Initialization of the standard registers
    this->PSR_pipe.setRegister(&PSR);
    this->PSR_pipe.setRegister(&PSR_fetch, 0);
    this->PSR_pipe.setRegister(&PSR_decode, 1);
    this->PSR_pipe.setRegister(&PSR_regs, 2);
    this->PSR_pipe.setRegister(&PSR_execute, 3);
    this->PSR_pipe.setRegister(&PSR_memory, 4);
    this->PSR_pipe.setRegister(&PSR_exception, 5);
    this->PSR_pipe.setRegister(&PSR_wb, 6);
    this->WIM_pipe.setRegister(&WIM);
    this->WIM_pipe.setRegister(&WIM_fetch, 0);
    this->WIM_pipe.setRegister(&WIM_decode, 1);
    this->WIM_pipe.setRegister(&WIM_regs, 2);
    this->WIM_pipe.setRegister(&WIM_execute, 3);
    this->WIM_pipe.setRegister(&WIM_memory, 4);
    this->WIM_pipe.setRegister(&WIM_exception, 5);
    this->WIM_pipe.setRegister(&WIM_wb, 6);
    this->TBR_pipe.setRegister(&TBR);
    this->TBR_pipe.setRegister(&TBR_fetch, 0);
    this->TBR_pipe.setRegister(&TBR_decode, 1);
    this->TBR_pipe.setRegister(&TBR_regs, 2);
    this->TBR_pipe.setRegister(&TBR_execute, 3);
    this->TBR_pipe.setRegister(&TBR_memory, 4);
    this->TBR_pipe.setRegister(&TBR_exception, 5);
    this->TBR_pipe.setRegister(&TBR_wb, 6);
    this->Y_pipe.setRegister(&Y);
    this->Y_pipe.setRegister(&Y_fetch, 0);
    this->Y_pipe.setRegister(&Y_decode, 1);
    this->Y_pipe.setRegister(&Y_regs, 2);
    this->Y_pipe.setRegister(&Y_execute, 3);
    this->Y_pipe.setRegister(&Y_memory, 4);
    this->Y_pipe.setRegister(&Y_exception, 5);
    this->Y_pipe.setRegister(&Y_wb, 6);
    this->PC_pipe.setRegister(&PC);
    this->PC_pipe.setRegister(&PC_fetch, 0);
    this->PC_pipe.setRegister(&PC_decode, 1);
    this->PC_pipe.setRegister(&PC_regs, 2);
    this->PC_pipe.setRegister(&PC_execute, 3);
    this->PC_pipe.setRegister(&PC_memory, 4);
    this->PC_pipe.setRegister(&PC_exception, 5);
    this->PC_pipe.setRegister(&PC_wb, 6);
    this->NPC_pipe.setRegister(&NPC);
    this->NPC_pipe.setRegister(&NPC_fetch, 0);
    this->NPC_pipe.setRegister(&NPC_decode, 1);
    this->NPC_pipe.setRegister(&NPC_regs, 2);
    this->NPC_pipe.setRegister(&NPC_execute, 3);
    this->NPC_pipe.setRegister(&NPC_memory, 4);
    this->NPC_pipe.setRegister(&NPC_exception, 5);
    this->NPC_pipe.setRegister(&NPC_wb, 6);
    // Initialization of the register banks
    this->GLOBAL.setSize(8);
    this->GLOBAL.setNewRegister(0, new Reg32_3_const_0());
    this->GLOBAL.setNewRegister(1, new Reg32_3());
    this->GLOBAL.setNewRegister(2, new Reg32_3());
    this->GLOBAL.setNewRegister(3, new Reg32_3());
    this->GLOBAL.setNewRegister(4, new Reg32_3());
    this->GLOBAL.setNewRegister(5, new Reg32_3());
    this->GLOBAL.setNewRegister(6, new Reg32_3());
    this->GLOBAL.setNewRegister(7, new Reg32_3());
    this->GLOBAL_fetch.setSize(8);
    this->GLOBAL_fetch.setNewRegister(0, new Reg32_3_const_0());
    this->GLOBAL_fetch.setNewRegister(1, new Reg32_3());
    this->GLOBAL_fetch.setNewRegister(2, new Reg32_3());
    this->GLOBAL_fetch.setNewRegister(3, new Reg32_3());
    this->GLOBAL_fetch.setNewRegister(4, new Reg32_3());
    this->GLOBAL_fetch.setNewRegister(5, new Reg32_3());
    this->GLOBAL_fetch.setNewRegister(6, new Reg32_3());
    this->GLOBAL_fetch.setNewRegister(7, new Reg32_3());
    this->GLOBAL_decode.setSize(8);
    this->GLOBAL_decode.setNewRegister(0, new Reg32_3_const_0());
    this->GLOBAL_decode.setNewRegister(1, new Reg32_3());
    this->GLOBAL_decode.setNewRegister(2, new Reg32_3());
    this->GLOBAL_decode.setNewRegister(3, new Reg32_3());
    this->GLOBAL_decode.setNewRegister(4, new Reg32_3());
    this->GLOBAL_decode.setNewRegister(5, new Reg32_3());
    this->GLOBAL_decode.setNewRegister(6, new Reg32_3());
    this->GLOBAL_decode.setNewRegister(7, new Reg32_3());
    this->GLOBAL_regs.setSize(8);
    this->GLOBAL_regs.setNewRegister(0, new Reg32_3_const_0());
    this->GLOBAL_regs.setNewRegister(1, new Reg32_3());
    this->GLOBAL_regs.setNewRegister(2, new Reg32_3());
    this->GLOBAL_regs.setNewRegister(3, new Reg32_3());
    this->GLOBAL_regs.setNewRegister(4, new Reg32_3());
    this->GLOBAL_regs.setNewRegister(5, new Reg32_3());
    this->GLOBAL_regs.setNewRegister(6, new Reg32_3());
    this->GLOBAL_regs.setNewRegister(7, new Reg32_3());
    this->GLOBAL_execute.setSize(8);
    this->GLOBAL_execute.setNewRegister(0, new Reg32_3_const_0());
    this->GLOBAL_execute.setNewRegister(1, new Reg32_3());
    this->GLOBAL_execute.setNewRegister(2, new Reg32_3());
    this->GLOBAL_execute.setNewRegister(3, new Reg32_3());
    this->GLOBAL_execute.setNewRegister(4, new Reg32_3());
    this->GLOBAL_execute.setNewRegister(5, new Reg32_3());
    this->GLOBAL_execute.setNewRegister(6, new Reg32_3());
    this->GLOBAL_execute.setNewRegister(7, new Reg32_3());
    this->GLOBAL_memory.setSize(8);
    this->GLOBAL_memory.setNewRegister(0, new Reg32_3_const_0());
    this->GLOBAL_memory.setNewRegister(1, new Reg32_3());
    this->GLOBAL_memory.setNewRegister(2, new Reg32_3());
    this->GLOBAL_memory.setNewRegister(3, new Reg32_3());
    this->GLOBAL_memory.setNewRegister(4, new Reg32_3());
    this->GLOBAL_memory.setNewRegister(5, new Reg32_3());
    this->GLOBAL_memory.setNewRegister(6, new Reg32_3());
    this->GLOBAL_memory.setNewRegister(7, new Reg32_3());
    this->GLOBAL_exception.setSize(8);
    this->GLOBAL_exception.setNewRegister(0, new Reg32_3_const_0());
    this->GLOBAL_exception.setNewRegister(1, new Reg32_3());
    this->GLOBAL_exception.setNewRegister(2, new Reg32_3());
    this->GLOBAL_exception.setNewRegister(3, new Reg32_3());
    this->GLOBAL_exception.setNewRegister(4, new Reg32_3());
    this->GLOBAL_exception.setNewRegister(5, new Reg32_3());
    this->GLOBAL_exception.setNewRegister(6, new Reg32_3());
    this->GLOBAL_exception.setNewRegister(7, new Reg32_3());
    this->GLOBAL_wb.setSize(8);
    this->GLOBAL_wb.setNewRegister(0, new Reg32_3_const_0());
    this->GLOBAL_wb.setNewRegister(1, new Reg32_3());
    this->GLOBAL_wb.setNewRegister(2, new Reg32_3());
    this->GLOBAL_wb.setNewRegister(3, new Reg32_3());
    this->GLOBAL_wb.setNewRegister(4, new Reg32_3());
    this->GLOBAL_wb.setNewRegister(5, new Reg32_3());
    this->GLOBAL_wb.setNewRegister(6, new Reg32_3());
    this->GLOBAL_wb.setNewRegister(7, new Reg32_3());
    for(int i = 0; i < 8; i++){
        this->GLOBAL_pipe[i].setRegister(&GLOBAL_fetch[i], 0);
        this->GLOBAL_pipe[i].setRegister(&GLOBAL_decode[i], 1);
        this->GLOBAL_pipe[i].setRegister(&GLOBAL_regs[i], 2);
        this->GLOBAL_pipe[i].setRegister(&GLOBAL_execute[i], 3);
        this->GLOBAL_pipe[i].setRegister(&GLOBAL_memory[i], 4);
        this->GLOBAL_pipe[i].setRegister(&GLOBAL_exception[i], 5);
        this->GLOBAL_pipe[i].setRegister(&GLOBAL_wb[i], 6);
        this->GLOBAL_pipe[i].setRegister(&GLOBAL[i]);
    }
    for(int i = 0; i < 128; i++){
        this->WINREGS_pipe[i].setRegister(&WINREGS_fetch[i], 0);
        this->WINREGS_pipe[i].setRegister(&WINREGS_decode[i], 1);
        this->WINREGS_pipe[i].setRegister(&WINREGS_regs[i], 2);
        this->WINREGS_pipe[i].setRegister(&WINREGS_execute[i], 3);
        this->WINREGS_pipe[i].setRegister(&WINREGS_memory[i], 4);
        this->WINREGS_pipe[i].setRegister(&WINREGS_exception[i], 5);
        this->WINREGS_pipe[i].setRegister(&WINREGS_wb[i], 6);
        this->WINREGS_pipe[i].setRegister(&WINREGS[i]);
    }
    for(int i = 0; i < 32; i++){
        this->ASR_pipe[i].setRegister(&ASR_fetch[i], 0);
        this->ASR_pipe[i].setRegister(&ASR_decode[i], 1);
        this->ASR_pipe[i].setRegister(&ASR_regs[i], 2);
        this->ASR_pipe[i].setRegister(&ASR_execute[i], 3);
        this->ASR_pipe[i].setRegister(&ASR_memory[i], 4);
        this->ASR_pipe[i].setRegister(&ASR_exception[i], 5);
        this->ASR_pipe[i].setRegister(&ASR_wb[i], 6);
        this->ASR_pipe[i].setRegister(&ASR[i]);
    }
    // Initialization of the aliases (plain and banks)
    FP_fetch.setPipeId(0);
    FP_decode.setPipeId(1);
    FP_regs.setPipeId(2);
    FP_execute.setPipeId(3);
    FP_memory.setPipeId(4);
    FP_exception.setPipeId(5);
    FP_wb.setPipeId(6);
    LR_fetch.setPipeId(0);
    LR_decode.setPipeId(1);
    LR_regs.setPipeId(2);
    LR_execute.setPipeId(3);
    LR_memory.setPipeId(4);
    LR_exception.setPipeId(5);
    LR_wb.setPipeId(6);
    SP_fetch.setPipeId(0);
    SP_decode.setPipeId(1);
    SP_regs.setPipeId(2);
    SP_execute.setPipeId(3);
    SP_memory.setPipeId(4);
    SP_exception.setPipeId(5);
    SP_wb.setPipeId(6);
    PCR_fetch.setPipeId(0);
    PCR_decode.setPipeId(1);
    PCR_regs.setPipeId(2);
    PCR_execute.setPipeId(3);
    PCR_memory.setPipeId(4);
    PCR_exception.setPipeId(5);
    PCR_wb.setPipeId(6);
    this->PCR_fetch.updateAlias(this->ASR_pipe[17]);
    this->PCR_decode.updateAlias(this->ASR_pipe[17]);
    this->PCR_regs.updateAlias(this->ASR_pipe[17]);
    this->PCR_execute.updateAlias(this->ASR_pipe[17]);
    this->PCR_memory.updateAlias(this->ASR_pipe[17]);
    this->PCR_exception.updateAlias(this->ASR_pipe[17]);
    this->PCR_wb.updateAlias(this->ASR_pipe[17]);
    for(int i = 0; i < 32; i++){
        this->REGS_fetch[i].setPipeId(0);
        this->REGS_decode[i].setPipeId(1);
        this->REGS_regs[i].setPipeId(2);
        this->REGS_execute[i].setPipeId(3);
        this->REGS_memory[i].setPipeId(4);
        this->REGS_exception[i].setPipeId(5);
        this->REGS_wb[i].setPipeId(6);
    }
    for(int i = 0; i < 8; i++){
        this->REGS_fetch[i].updateAlias(this->GLOBAL_pipe[i]);
        this->REGS_decode[i].updateAlias(this->GLOBAL_pipe[i]);
        this->REGS_regs[i].updateAlias(this->GLOBAL_pipe[i]);
        this->REGS_execute[i].updateAlias(this->GLOBAL_pipe[i]);
        this->REGS_memory[i].updateAlias(this->GLOBAL_pipe[i]);
        this->REGS_exception[i].updateAlias(this->GLOBAL_pipe[i]);
        this->REGS_wb[i].updateAlias(this->GLOBAL_pipe[i]);
    }
    for(int i = 0; i < 24; i++){
        this->REGS_fetch[i + 8].updateAlias(this->WINREGS_pipe[i]);
        this->REGS_decode[i + 8].updateAlias(this->WINREGS_pipe[i]);
        this->REGS_regs[i + 8].updateAlias(this->WINREGS_pipe[i]);
        this->REGS_execute[i + 8].updateAlias(this->WINREGS_pipe[i]);
        this->REGS_memory[i + 8].updateAlias(this->WINREGS_pipe[i]);
        this->REGS_exception[i + 8].updateAlias(this->WINREGS_pipe[i]);
        this->REGS_wb[i + 8].updateAlias(this->WINREGS_pipe[i]);
    }
    this->FP_fetch.updateAlias(this->REGS_fetch[30]);
    this->FP_decode.updateAlias(this->REGS_decode[30]);
    this->FP_regs.updateAlias(this->REGS_regs[30]);
    this->FP_execute.updateAlias(this->REGS_execute[30]);
    this->FP_memory.updateAlias(this->REGS_memory[30]);
    this->FP_exception.updateAlias(this->REGS_exception[30]);
    this->FP_wb.updateAlias(this->REGS_wb[30]);
    this->SP_fetch.updateAlias(this->REGS_fetch[14]);
    this->SP_decode.updateAlias(this->REGS_decode[14]);
    this->SP_regs.updateAlias(this->REGS_regs[14]);
    this->SP_execute.updateAlias(this->REGS_execute[14]);
    this->SP_memory.updateAlias(this->REGS_memory[14]);
    this->SP_exception.updateAlias(this->REGS_exception[14]);
    this->SP_wb.updateAlias(this->REGS_wb[14]);
    this->LR_fetch.updateAlias(this->REGS_fetch[31]);
    this->LR_decode.updateAlias(this->REGS_decode[31]);
    this->LR_regs.updateAlias(this->REGS_regs[31]);
    this->LR_execute.updateAlias(this->REGS_execute[31]);
    this->LR_memory.updateAlias(this->REGS_memory[31]);
    this->LR_exception.updateAlias(this->REGS_exception[31]);
    this->LR_wb.updateAlias(this->REGS_wb[31]);
    this->profTimeStart = SC_ZERO_TIME;
    this->profTimeEnd = SC_ZERO_TIME;
    this->numInstructions = 0;
    this->ENTRY_POINT = 0;
    this->PROGRAM_LIMIT = 0;
    this->PROGRAM_START = 0;
    this->abiIf = new LEON3_ABIIf(this->PROGRAM_LIMIT, this->dataMem, this->PSR_pipe, \
        this->WIM_pipe, this->TBR_pipe, this->Y_pipe, this->PC_pipe, this->NPC_pipe, this->GLOBAL_pipe, \
        this->WINREGS_pipe, this->ASR_pipe, this->FP_wb, this->LR_wb, this->SP_wb, this->PCR_wb, \
        this->REGS_wb, this->instrExecuting, this->instrEndEvent, this->fetch_stage.instHistoryQueue);
    end_module();
}

leon3_acclt_trap::Processor_leon3_acclt::~Processor_leon3_acclt(){
    Processor_leon3_acclt::numInstances--;
    if(Processor_leon3_acclt::numInstances == 0){
        for(int i = 0; i < 145; i++){
            delete Processor_leon3_acclt::INSTRUCTIONS[i];
        }
        delete [] Processor_leon3_acclt::INSTRUCTIONS;
        Processor_leon3_acclt::INSTRUCTIONS = NULL;
        delete Processor_leon3_acclt::NOPInstrInstance;
    }
    delete this->abiIf;
    delete this->IRQ_irqInstr;
}

