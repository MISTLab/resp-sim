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


using namespace arm9tdmi_funclt_trap;
using namespace trap;
void arm9tdmi_funclt_trap::ARM9Processor::mainLoop(){
    template_map< unsigned int, CacheElem >::iterator instrCacheEnd = this->instrCache.end();while(true){
        unsigned int numCycles = 0;
        this->instrExecuting = true;
        unsigned int curPC = this->PC-4;
        template_map< unsigned int, CacheElem >::iterator cachedInstr = this->instrCache.find(curPC);
        if(cachedInstr != instrCacheEnd){
            Instruction * curInstrPtr = cachedInstr->second.instr;
            // I can call the instruction, I have found it
            if(curInstrPtr != NULL){
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
                Instruction * instr = ARM9Processor::INSTRUCTIONS[instrId];
                instr->setParams(bitString);
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
                    ARM9Processor::INSTRUCTIONS[instrId] = instr->replicate();
                }
            }
        }
        else{
            // The current instruction is not present in the cache:
            // I have to perform the normal decoding phase ...
            unsigned int bitString = this->instrMem.read_word(curPC);
            int instrId = this->decoder.decode(bitString);
            Instruction * instr = ARM9Processor::INSTRUCTIONS[instrId];
            instr->setParams(bitString);
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
        this->quantKeeper.inc((numCycles + 1)*this->latency);
        if(this->quantKeeper.need_sync()){
            this->quantKeeper.sync();
        }
        this->instrExecuting = false;
        this->instrEndEvent.notify();
        this->numInstructions++;

    }
}

void arm9tdmi_funclt_trap::ARM9Processor::resetOp(){
    for(int i = 0; i < 30; i++){
        RB[i] = 0;
    }
    for(int i = 0; i < 5; i++){
        SPSR[i] = 0;
    }
    CPSR.immediateWrite(0xd3);
    MP_ID.immediateWrite(0x0);
    PC.immediateWrite(ENTRY_POINT);
}

void arm9tdmi_funclt_trap::ARM9Processor::end_of_elaboration(){
    this->resetOp();
}

Instruction * arm9tdmi_funclt_trap::ARM9Processor::decode( unsigned int bitString ){
    int instrId = this->decoder.decode(bitString);
    if(instrId >= 0){
        Instruction * instr = ARM9Processor::INSTRUCTIONS[instrId];
        instr->setParams(bitString);
        return instr;
    }
    return NULL;
}

ARM9TDMI_ABIIf & arm9tdmi_funclt_trap::ARM9Processor::getInterface(){
    return *this->abiIf;
}

Instruction * * arm9tdmi_funclt_trap::ARM9Processor::INSTRUCTIONS = NULL;
int arm9tdmi_funclt_trap::ARM9Processor::numInstances = 0;
arm9tdmi_funclt_trap::ARM9Processor::ARM9Processor( sc_module_name name, sc_time latency \
    ) : sc_module(name), latency(latency), SP_SVC(&RB[15], 0), instrMem("instrMem", this->quantKeeper, \
    MP_ID), dataMem("dataMem", this->quantKeeper, MP_ID){
    ARM9Processor::numInstances++;
    if(ARM9Processor::INSTRUCTIONS == NULL){
        // Initialization of the array holding the initial instance of the instructions
        ARM9Processor::INSTRUCTIONS = new Instruction *[91];
        ARM9Processor::INSTRUCTIONS[64] = new TST_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[59] = new SUB_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[3] = new ADD_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
            LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
        ARM9Processor::INSTRUCTIONS[11] = new BIC_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[19] = new CMP_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
            LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
        ARM9Processor::INSTRUCTIONS[10] = new BRANCHX(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[12] = new BIC_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[4] = new ADD_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
            LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
        ARM9Processor::INSTRUCTIONS[58] = new SUB_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[65] = new TST_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[24] = new LDR_imm(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[46] = new ORR_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[5] = new ADD_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
            LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
        ARM9Processor::INSTRUCTIONS[68] = new STR_imm(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[40] = new mrs_Instr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[30] = new LDRSB_off(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[47] = new ORR_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[77] = new LDRT_imm(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[82] = new STRT_off(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[7] = new AND_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
            LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
        ARM9Processor::INSTRUCTIONS[37] = new MOV_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[38] = new MOV_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[36] = new umull_Instr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[6] = new AND_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
            LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
        ARM9Processor::INSTRUCTIONS[49] = new RSB_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[86] = new MRC(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
            LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
        ARM9Processor::INSTRUCTIONS[51] = new RSB_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
            LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
        ARM9Processor::INSTRUCTIONS[79] = new STRBT_imm(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[0] = new ADC_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
            LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
        ARM9Processor::INSTRUCTIONS[56] = new SBC_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[14] = new CMN_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[22] = new EOR_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
            LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
        ARM9Processor::INSTRUCTIONS[1] = new ADC_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
            LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
        ARM9Processor::INSTRUCTIONS[83] = new LDC(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
            LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
        ARM9Processor::INSTRUCTIONS[42] = new msr_reg_Instr(CPSR, MP_ID, RB, SPSR, FP, SPTR, \
            LINKR, SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, \
            instrMem, dataMem);
        ARM9Processor::INSTRUCTIONS[23] = new LDM(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
            LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
        ARM9Processor::INSTRUCTIONS[87] = new CDP(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
            LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
        ARM9Processor::INSTRUCTIONS[2] = new ADC_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
            LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
        ARM9Processor::INSTRUCTIONS[72] = new STRH_off(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[18] = new CMP_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[75] = new LDRBT_imm(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[76] = new LDRBT_off(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[17] = new CMP_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[61] = new TEQ_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[54] = new RSC_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
            LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
        ARM9Processor::INSTRUCTIONS[70] = new STRB_imm(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[39] = new MOV_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
            LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
        ARM9Processor::INSTRUCTIONS[48] = new ORR_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
            LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
        ARM9Processor::INSTRUCTIONS[29] = new LDRSH_off(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[26] = new LDRB_imm(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[16] = new CMN_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
            LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
        ARM9Processor::INSTRUCTIONS[78] = new LDRT_off(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[73] = new SWAP(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
            LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
        ARM9Processor::INSTRUCTIONS[81] = new STRT_imm(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[28] = new LDRH_off(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[89] = new BKPT(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
            LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
        ARM9Processor::INSTRUCTIONS[52] = new RSC_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[35] = new umlal_Instr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[8] = new AND_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
            LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
        ARM9Processor::INSTRUCTIONS[53] = new RSC_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[33] = new smlal_Instr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[32] = new mul_Instr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[31] = new mla_Instr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[43] = new MVN_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[45] = new MVN_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
            LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
        ARM9Processor::INSTRUCTIONS[44] = new MVN_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[9] = new BRANCH(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
            LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
        ARM9Processor::INSTRUCTIONS[88] = new SWI(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
            LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
        ARM9Processor::INSTRUCTIONS[21] = new EOR_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[55] = new SBC_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[66] = new TST_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
            LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
        ARM9Processor::INSTRUCTIONS[74] = new SWAPB(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
            LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
        ARM9Processor::INSTRUCTIONS[50] = new RSB_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[57] = new SBC_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
            LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
        ARM9Processor::INSTRUCTIONS[69] = new STR_off(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[84] = new STC(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
            LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
        ARM9Processor::INSTRUCTIONS[67] = new STM(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
            LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
        ARM9Processor::INSTRUCTIONS[60] = new SUB_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
            LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
        ARM9Processor::INSTRUCTIONS[25] = new LDR_off(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[13] = new BIC_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
            LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
        ARM9Processor::INSTRUCTIONS[62] = new TEQ_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[34] = new smull_Instr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[71] = new STRB_off(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[15] = new CMN_sr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[80] = new STRBT_off(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[41] = new msr_imm_Instr(CPSR, MP_ID, RB, SPSR, FP, SPTR, \
            LINKR, SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, \
            instrMem, dataMem);
        ARM9Processor::INSTRUCTIONS[27] = new LDRB_off(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[20] = new EOR_si(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
        ARM9Processor::INSTRUCTIONS[85] = new MCR(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
            LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
        ARM9Processor::INSTRUCTIONS[63] = new TEQ_i(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, SP_SVC, \
            LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, dataMem);
        ARM9Processor::INSTRUCTIONS[90] = new InvalidInstr(CPSR, MP_ID, RB, SPSR, FP, SPTR, LINKR, \
            SP_SVC, LR_SVC, SP_ABT, LR_ABT, SP_IRQ, LR_IRQ, SP_FIQ, LR_FIQ, PC, REGS, instrMem, \
            dataMem);
    }
    quantKeeper.set_global_quantum( this->latency*100 );
    quantKeeper.reset();
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
    this->SPTR.updateAlias(this->REGS[13], 0);
    this->FP.updateAlias(this->REGS[11], 0);
    this->LINKR.updateAlias(this->REGS[14], 0);
    this->LR_SVC.updateAlias(this->RB[16], 0);
    this->SP_ABT.updateAlias(this->RB[17], 0);
    this->LR_ABT.updateAlias(this->RB[18], 0);
    this->SP_IRQ.updateAlias(this->RB[21], 0);
    this->LR_IRQ.updateAlias(this->RB[22], 0);
    this->SP_FIQ.updateAlias(this->RB[28], 0);
    this->LR_FIQ.updateAlias(this->RB[29], 0);
    this->PC.updateAlias(this->REGS[15], 0);
    this->numInstructions = 0;
    this->ENTRY_POINT = 0;
    this->PROGRAM_LIMIT = 0;
    this->PROGRAM_START = 0;
    this->abiIf = new ARM9TDMI_ABIIf(this->PROGRAM_LIMIT, this->dataMem, this->CPSR, \
        this->MP_ID, this->RB, this->SPSR, this->FP, this->SPTR, this->LINKR, this->SP_SVC, \
        this->LR_SVC, this->SP_ABT, this->LR_ABT, this->SP_IRQ, this->LR_IRQ, this->SP_FIQ, \
        this->LR_FIQ, this->PC, this->REGS, this->instrExecuting, this->instrEndEvent);
    SC_THREAD(mainLoop);
    end_module();
}

arm9tdmi_funclt_trap::ARM9Processor::~ARM9Processor(){
    ARM9Processor::numInstances--;
    if(ARM9Processor::numInstances == 0){
        for(int i = 0; i < 91; i++){
            delete ARM9Processor::INSTRUCTIONS[i];
        }
        delete [] ARM9Processor::INSTRUCTIONS;
        ARM9Processor::INSTRUCTIONS = NULL;
    }
    template_map< unsigned int, CacheElem >::const_iterator cacheIter, cacheEnd;
    for(cacheIter = this->instrCache.begin(), cacheEnd = this->instrCache.end(); cacheIter \
        != cacheEnd; cacheIter++){
        delete cacheIter->second.instr;
    }
    delete this->abiIf;
}

