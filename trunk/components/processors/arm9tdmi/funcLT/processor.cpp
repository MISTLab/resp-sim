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
#include <customExceptions.hpp>
#include <instructions.hpp>
#include <decoder.hpp>
#include <interface.hpp>
#include <ToolsIf.hpp>
#include <tlm_utils/tlm_quantumkeeper.h>
#include <registers.hpp>
#include <alias.hpp>
#include <externalPorts.hpp>
#include <iostream>
#include <fstream>
#include <boost/circular_buffer.hpp>
#include <instructionBase.hpp>
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

#include <string>

using namespace arm9tdmi_funclt_trap;
using namespace trap;
void arm9tdmi_funclt_trap::Processor_arm9tdmi_funclt::mainLoop(){
    bool startMet = false;
    template_map< unsigned int, CacheElem >::iterator instrCacheEnd = this->instrCache.end();

    while(true){
        unsigned int numCycles = 0;
        this->instrExecuting = true;
        unsigned int curPC = this->PC-4;
        if(!startMet && curPC == this->profStartAddr){
            this->profTimeStart = sc_time_stamp();
        }
        if(startMet && curPC == this->profEndAddr){
            this->profTimeEnd = sc_time_stamp();
        }
        #ifdef ENABLE_HISTORY
        HistoryInstrType instrQueueElem;
        if(this->historyEnabled){
            instrQueueElem.cycle = (unsigned int)(this->quantKeeper.get_current_time()/this->latency);
            instrQueueElem.address = curPC;
        }
        #endif
        template_map< unsigned int, CacheElem >::iterator cachedInstr = this->instrCache.find(curPC);
        if(cachedInstr != instrCacheEnd){
            Instruction * curInstrPtr = cachedInstr->second.instr;
            // I can call the instruction, I have found it
            if(curInstrPtr != NULL){
                #ifdef ENABLE_HISTORY
                if(this->historyEnabled){
                    instrQueueElem.name = curInstrPtr->getInstructionName();
                    instrQueueElem.mnemonic = curInstrPtr->getMnemonic();
                }
                #endif
                try{
                    #ifndef DISABLE_TOOLS
                    if(!(this->toolManager.newIssue(curPC, curInstrPtr))){
                        #endif
                        numCycles = curInstrPtr->behavior();
                        #ifndef DISABLE_TOOLS
                    }
                    #endif
                }
                catch(annull_exception &etc){
                    numCycles = 0;
                }
            }
            else{
                unsigned int bitString = this->instrMem.read_word(curPC);
                unsigned int & curCount = cachedInstr->second.count;
                int instrId = this->decoder.decode(bitString);
                Instruction * instr = this->INSTRUCTIONS[instrId];
                instr->setParams(bitString);
                #ifdef ENABLE_HISTORY
                if(this->historyEnabled){
                    instrQueueElem.name = instr->getInstructionName();
                    instrQueueElem.mnemonic = instr->getMnemonic();
                }
                #endif
                try{
                    #ifndef DISABLE_TOOLS
                    if(!(this->toolManager.newIssue(curPC, instr))){
                        #endif
                        numCycles = instr->behavior();
                        #ifndef DISABLE_TOOLS
                    }
                    #endif
                }
                catch(annull_exception &etc){
                    numCycles = 0;
                }
                if(curCount < 256){
                    curCount++;
                }
                else{
                    // ... and then add the instruction to the cache
                    cachedInstr->second.instr = instr;
                    this->INSTRUCTIONS[instrId] = instr->replicate();
                }
            }
        }
        else{
            // The current instruction is not present in the cache:
            // I have to perform the normal decoding phase ...
            unsigned int bitString = this->instrMem.read_word(curPC);
            int instrId = this->decoder.decode(bitString);
            Instruction * instr = this->INSTRUCTIONS[instrId];
            instr->setParams(bitString);
            #ifdef ENABLE_HISTORY
            if(this->historyEnabled){
                instrQueueElem.name = instr->getInstructionName();
                instrQueueElem.mnemonic = instr->getMnemonic();
            }
            #endif
            try{
                #ifndef DISABLE_TOOLS
                if(!(this->toolManager.newIssue(curPC, instr))){
                    #endif
                    numCycles = instr->behavior();
                    #ifndef DISABLE_TOOLS
                }
                #endif
            }
            catch(annull_exception &etc){
                numCycles = 0;
            }
            this->instrCache.insert(std::pair< unsigned int, CacheElem >(bitString, CacheElem()));
            instrCacheEnd = this->instrCache.end();
        }
        #ifdef ENABLE_HISTORY
        if(this->historyEnabled){
            // First I add the new element to the queue
            this->instHistoryQueue.push_back(instrQueueElem);
            //Now, in case the queue dump file has been specified, I have to check if I need \
                to save it
            if(this->histFile){
                this->undumpedHistElems++;
                if(undumpedHistElems == this->instHistoryQueue.capacity()){
                    boost::circular_buffer<HistoryInstrType>::const_iterator beg, end;
                    for(beg = this->instHistoryQueue.begin(), end = this->instHistoryQueue.end(); beg \
                        != end; beg++){
                        this->histFile << beg->toStr() << std::endl;
                    }
                    this->undumpedHistElems = 0;
                }
            }
        }
        #endif
        this->quantKeeper.inc((numCycles + 1)*this->latency);
        if(this->quantKeeper.need_sync()){
            this->quantKeeper.sync();
        }
        this->instrExecuting = false;
        this->instrEndEvent.notify();
        this->numInstructions++;

        this->quantKeeper.sync();
    }
}

void arm9tdmi_funclt_trap::Processor_arm9tdmi_funclt::resetOp(){
    for(int i = 0; i < 30; i++){
        RB[i] = 0;
    }
    for(int i = 0; i < 5; i++){
        SPSR[i] = 0;
    }
    CPSR.immediateWrite(0xd3);
    MP_ID.immediateWrite(0x0);
    PC.immediateWrite(ENTRY_POINT);
    this->resetCalled = true;
}

void arm9tdmi_funclt_trap::Processor_arm9tdmi_funclt::end_of_elaboration(){
    if(!this->resetCalled){
        this->resetOp();
    }
    this->resetCalled = false;
}

Instruction * arm9tdmi_funclt_trap::Processor_arm9tdmi_funclt::decode( unsigned int \
    bitString ){
    int instrId = this->decoder.decode(bitString);
    if(instrId >= 0){
        Instruction * instr = this->INSTRUCTIONS[instrId];
        instr->setParams(bitString);
        return instr;
    }
    return NULL;
}

ARM9TDMI_ABIIf & arm9tdmi_funclt_trap::Processor_arm9tdmi_funclt::getInterface(){
    return *this->abiIf;
}

int arm9tdmi_funclt_trap::Processor_arm9tdmi_funclt::numInstances = 0;
void arm9tdmi_funclt_trap::Processor_arm9tdmi_funclt::setProfilingRange( unsigned \
    int startAddr, unsigned int endAddr ){
    this->profStartAddr = startAddr;
    this->profEndAddr = endAddr;
}

void arm9tdmi_funclt_trap::Processor_arm9tdmi_funclt::enableHistory( std::string \
    fileName ){
    this->historyEnabled = true;
    this->histFile.open(fileName.c_str(), ios::out | ios::ate);
}

arm9tdmi_funclt_trap::Processor_arm9tdmi_funclt::Processor_arm9tdmi_funclt( sc_module_name \
    name, sc_time latency ) : sc_module(name), latency(latency), LR_IRQ(&RB[22], 0), \
    instrMem("instrMem", this->quantKeeper, MP_ID), dataMem("dataMem", this->quantKeeper, \
    MP_ID){
    this->resetCalled = false;
    Processor_arm9tdmi_funclt::numInstances++;
    // Initialization of the array holding the initial instance of the instructions
    this->INSTRUCTIONS = new Instruction *[91];
    this->INSTRUCTIONS[64] = new TST_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[59] = new SUB_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[3] = new ADD_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[11] = new BIC_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[19] = new CMP_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[10] = new BRANCHX(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[12] = new BIC_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[4] = new ADD_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[58] = new SUB_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[65] = new TST_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[24] = new LDR_imm(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[46] = new ORR_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[5] = new ADD_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[68] = new STR_imm(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[40] = new mrs_Instr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[30] = new LDRSB_off(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[47] = new ORR_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[77] = new LDRT_imm(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[82] = new STRT_off(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[7] = new AND_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[37] = new MOV_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[38] = new MOV_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[36] = new umull_Instr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
        SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
        dataMem);
    this->INSTRUCTIONS[6] = new AND_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[49] = new RSB_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[86] = new MRC(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[51] = new RSB_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[79] = new STRBT_imm(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[0] = new ADC_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[56] = new SBC_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[14] = new CMN_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[22] = new EOR_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[1] = new ADC_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[83] = new LDC(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[42] = new msr_reg_Instr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
        SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
        dataMem);
    this->INSTRUCTIONS[23] = new LDM(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[87] = new CDP(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[2] = new ADC_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[72] = new STRH_off(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[18] = new CMP_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[75] = new LDRBT_imm(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[76] = new LDRBT_off(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[17] = new CMP_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[61] = new TEQ_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[54] = new RSC_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[70] = new STRB_imm(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[39] = new MOV_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[48] = new ORR_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[29] = new LDRSH_off(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[26] = new LDRB_imm(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[16] = new CMN_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[78] = new LDRT_off(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[73] = new SWAP(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[81] = new STRT_imm(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[28] = new LDRH_off(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[89] = new BKPT(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[52] = new RSC_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[35] = new umlal_Instr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
        SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
        dataMem);
    this->INSTRUCTIONS[8] = new AND_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[53] = new RSC_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[33] = new smlal_Instr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
        SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
        dataMem);
    this->INSTRUCTIONS[32] = new mul_Instr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[31] = new mla_Instr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[43] = new MVN_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[45] = new MVN_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[44] = new MVN_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[9] = new BRANCH(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[88] = new SWI(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[21] = new EOR_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[55] = new SBC_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[66] = new TST_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[74] = new SWAPB(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[50] = new RSB_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[57] = new SBC_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[69] = new STR_off(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[84] = new STC(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[67] = new STM(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[60] = new SUB_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[25] = new LDR_off(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[13] = new BIC_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[62] = new TEQ_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[34] = new smull_Instr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
        SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
        dataMem);
    this->INSTRUCTIONS[71] = new STRB_off(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[15] = new CMN_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[80] = new STRBT_off(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[41] = new msr_imm_Instr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
        SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
        dataMem);
    this->INSTRUCTIONS[27] = new LDRB_off(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[20] = new EOR_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[85] = new MCR(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[63] = new TEQ_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
        LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
    this->INSTRUCTIONS[90] = new InvalidInstr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
        SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
        dataMem);
    this->quantKeeper.set_global_quantum( this->latency*100 );
    this->quantKeeper.reset();
    // Initialization of the standard registers
    // Initialization of the register banks
    // Initialization of the aliases (plain and banks)
    this->REGS[0].updateAlias(this->RB[0]);
    this->REGS[1].updateAlias(this->RB[1]);
    this->REGS[2].updateAlias(this->RB[2]);
    this->REGS[3].updateAlias(this->RB[3]);
    this->REGS[4].updateAlias(this->RB[4]);
    this->REGS[5].updateAlias(this->RB[5]);
    this->REGS[6].updateAlias(this->RB[6]);
    this->REGS[7].updateAlias(this->RB[7]);
    this->REGS[8].updateAlias(this->RB[8]);
    this->REGS[9].updateAlias(this->RB[9]);
    this->REGS[10].updateAlias(this->RB[10]);
    this->REGS[11].updateAlias(this->RB[11]);
    this->REGS[12].updateAlias(this->RB[12]);
    this->REGS[13].updateAlias(this->RB[13]);
    this->REGS[14].updateAlias(this->RB[14]);
    this->REGS[15].updateAlias(this->RB[15], 4);
    this->FP.updateAlias(this->REGS[11], 0);
    this->SP_SVC.updateAlias(this->RB[15], 0);
    this->LR_FIQ.updateAlias(this->RB[29], 0);
    this->SP_FIQ.updateAlias(this->RB[28], 0);
    this->SPTR.updateAlias(this->REGS[13], 0);
    this->SP_IRQ.updateAlias(this->RB[21], 0);
    this->LR_ABT.updateAlias(this->RB[18], 0);
    this->SP_ABT.updateAlias(this->RB[17], 0);
    this->LR_SVC.updateAlias(this->RB[16], 0);
    this->PC.updateAlias(this->REGS[15], 0);
    this->LINKR.updateAlias(this->REGS[14], 0);
    this->profTimeStart = SC_ZERO_TIME;
    this->profTimeEnd = SC_ZERO_TIME;
    this->profStartAddr = (unsigned int)-1;
    this->profEndAddr = (unsigned int)-1;
    this->historyEnabled = false;
    this->instHistoryQueue.set_capacity(1000);
    this->undumpedHistElems = 0;
    this->numInstructions = 0;
    this->ENTRY_POINT = 0;
    this->MPROC_ID = 0;
    this->PROGRAM_LIMIT = 0;
    this->PROGRAM_START = 0;
    this->abiIf = new ARM9TDMI_ABIIf(this->PROGRAM_LIMIT, this->dataMem, this->CPSR, \
        this->MP_ID, this->RB, this->SPSR, this->FP, this->SPTR, this->LINKR, this->SP_SVC, \
        this->LR_SVC, this->SP_ABT, this->LR_ABT, this->SP_IRQ, this->LR_IRQ, this->SP_FIQ, \
        this->LR_FIQ, this->PC, this->REGS, this->instrExecuting, this->instrEndEvent, this->instHistoryQueue);
    SC_THREAD(mainLoop);
    end_module();
}

arm9tdmi_funclt_trap::Processor_arm9tdmi_funclt::~Processor_arm9tdmi_funclt(){
    Processor_arm9tdmi_funclt::numInstances--;
    for(int i = 0; i < 91; i++){
        delete this->INSTRUCTIONS[i];
    }
    delete [] this->INSTRUCTIONS;
    template_map< unsigned int, CacheElem >::const_iterator cacheIter, cacheEnd;
    for(cacheIter = this->instrCache.begin(), cacheEnd = this->instrCache.end(); cacheIter \
        != cacheEnd; cacheIter++){
        delete cacheIter->second.instr;
    }
    delete this->abiIf;
    #ifdef ENABLE_HISTORY
    if(this->historyEnabled){
        //Now, in case the queue dump file has been specified, I have to check if I need \
            to save the yet undumped elements
        if(this->histFile){
            if(this->undumpedHistElems > 0){
                std::vector<std::string> histVec;
                boost::circular_buffer<HistoryInstrType>::const_reverse_iterator beg, end;
                unsigned int histRead = 0;
                for(histRead = 0, beg = this->instHistoryQueue.rbegin(), end = this->instHistoryQueue.rend(); \
                    beg != end && histRead < this->undumpedHistElems; beg++, histRead++){
                    histVec.push_back(beg->toStr());
                }
                std::vector<std::string>::const_reverse_iterator histVecBeg, histVecEnd;
                for(histVecBeg = histVec.rbegin(), histVecEnd = histVec.rend(); histVecBeg != histVecEnd; \
                    histVecBeg++){
                    this->histFile <<  *histVecBeg << std::endl;
                }
            }
            this->histFile.flush();
            this->histFile.close();
        }
    }
    #endif
}

