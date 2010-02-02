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



#include <pipeline.hpp>
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

using namespace leon3_accat_trap;
using namespace trap;
void leon3_accat_trap::BasePipeStage::flush() throw(){
    this->hasToFlush = true;
    if(this->prevStage != NULL){
        this->prevStage->flush();
    }
}

std::map< unsigned int, std::vector< Register * > > leon3_accat_trap::BasePipeStage::unlockQueue;
leon3_accat_trap::BasePipeStage::BasePipeStage( sc_time & latency, BasePipeStage \
    * stage_fetch, BasePipeStage * stage_decode, BasePipeStage * stage_regs, BasePipeStage \
    * stage_execute, BasePipeStage * stage_memory, BasePipeStage * stage_exception, BasePipeStage \
    * stage_wb, BasePipeStage * prevStage, BasePipeStage * succStage ) : latency(latency), \
    stage_fetch(stage_fetch), stage_decode(stage_decode), stage_regs(stage_regs), stage_execute(stage_execute), \
    stage_memory(stage_memory), stage_exception(stage_exception), stage_wb(stage_wb), \
    prevStage(prevStage), succStage(succStage){
    this->chStalled = false;
    this->stalled = false;
    this->stageEnded = false;
    this->stageBeginning = false;
    this->hasToFlush = false;
    this->NOPInstrInstance = NULL;
    this->IRQ_irqInstr = NULL;
    this->curInstruction = NULL;
    this->nextInstruction = NULL;
}

void leon3_accat_trap::FETCH_PipeStage::behavior(){
    this->curInstruction = this->NOPInstrInstance;
    this->nextInstruction = this->NOPInstrInstance;
    unsigned int numNOPS = 0;
    template_map< unsigned int, CacheElem >::iterator instrCacheEnd = this->instrCache.end();
    while(true){
        this->instrExecuting = true;
        unsigned int numCycles = 0;
        if(!this->chStalled){

            // HERE WAIT FOR BEGIN OF ALL STAGES
            this->waitPipeBegin();
            if((IRQ != -1) && (PSR[key_ET] && (IRQ == 15 || IRQ > PSR[key_PIL]))){
                this->IRQ_irqInstr->setInterruptValue(IRQ);
                try{
                    numCycles = this->IRQ_irqInstr->behavior_fetch(BasePipeStage::unlockQueue);
                    this->curInstruction = this->IRQ_irqInstr;
                }
                catch(annull_exception &etc){
                    numCycles = 0;
                }

            }
            else{
                unsigned int curPC = this->PC;

                #ifndef DISABLE_TOOLS
                // code necessary to check the tools, to see if they need
                // the pipeline to be empty before being able to procede with execution
                if(this->toolManager.emptyPipeline(curPC)){
                    numNOPS++;
                }
                else{
                    numNOPS = 0;
                }
                if(numNOPS > 0 && numNOPS < 7){
                    this->curInstruction = this->NOPInstrInstance;
                }
                else{
                    numNOPS = 0;
                    #endif
                    //Ok, either the pipeline is empty or there is not tool which needs the pipeline
                    //to be empty: I can procede with the execution
                    unsigned int bitString = this->instrMem.read_word(curPC);
                    template_map< unsigned int, CacheElem >::iterator cachedInstr = this->instrCache.find(bitString);
                    if(cachedInstr != instrCacheEnd){
                        Instruction * curInstrPtr = cachedInstr->second.instr;
                        // I can call the instruction, I have found it
                        if(curInstrPtr != NULL){
                            if(curInstrPtr->inPipeline){
                                curInstrPtr = curInstrPtr->replicate();
                                curInstrPtr->setParams(bitString);
                                curInstrPtr->toDestroy = true;
                            }
                            curInstrPtr->inPipeline = true;
                            curInstrPtr->fetchPC = curPC;
                            try{
                                #ifndef DISABLE_TOOLS
                                if(!(this->toolManager.newIssue(curInstrPtr->fetchPC, curInstrPtr))){
                                    #endif
                                    numCycles = curInstrPtr->behavior_fetch(BasePipeStage::unlockQueue);
                                    this->curInstruction = curInstrPtr;
                                    #ifndef DISABLE_TOOLS
                                }
                                else{
                                    if(curInstrPtr->toDestroy){
                                        delete curInstrPtr;
                                    }
                                    else{
                                        curInstrPtr->inPipeline = false;
                                    }
                                    this->curInstruction = this->NOPInstrInstance;
                                }
                                #endif
                            }
                            catch(annull_exception &etc){

                                if(curInstrPtr->toDestroy){
                                    delete curInstrPtr;
                                }
                                else{
                                    curInstrPtr->inPipeline = false;
                                }
                                this->curInstruction = this->NOPInstrInstance;
                                numCycles = 0;
                            }
                        }
                        else{
                            unsigned int & curCount = cachedInstr->second.count;
                            int instrId = this->decoder.decode(bitString);
                            Instruction * instr = FETCH_PipeStage::INSTRUCTIONS[instrId];
                            if(instr->inPipeline){
                                instr = instr->replicate();
                                instr->toDestroy = true;
                            }
                            instr->inPipeline = true;
                            instr->fetchPC = curPC;
                            instr->setParams(bitString);
                            try{
                                #ifndef DISABLE_TOOLS
                                if(!(this->toolManager.newIssue(instr->fetchPC, instr))){
                                    #endif
                                    numCycles = instr->behavior_fetch(BasePipeStage::unlockQueue);
                                    this->curInstruction = instr;
                                    #ifndef DISABLE_TOOLS
                                }
                                else{
                                    if(instr->toDestroy && curCount < 256){
                                        delete instr;
                                    }
                                    else{
                                        instr->inPipeline = false;
                                    }
                                    this->curInstruction = this->NOPInstrInstance;
                                }
                                #endif
                            }
                            catch(annull_exception &etc){

                                if(instr->toDestroy && curCount < 256){
                                    delete instr;
                                }
                                else{
                                    instr->inPipeline = false;
                                }
                                this->curInstruction = this->NOPInstrInstance;
                                numCycles = 0;
                            }
                            if(curCount < 256){
                                curCount++;
                            }
                            else{
                                // ... and then add the instruction to the cache
                                cachedInstr->second.instr = instr;
                                if(instr->toDestroy){
                                    instr->toDestroy = false;
                                }
                                else{
                                    FETCH_PipeStage::INSTRUCTIONS[instrId] = instr->replicate();
                                }
                            }
                        }
                    }
                    else{
                        // The current instruction is not present in the cache:
                        // I have to perform the normal decoding phase ...
                        int instrId = this->decoder.decode(bitString);
                        Instruction * instr = FETCH_PipeStage::INSTRUCTIONS[instrId];
                        if(instr->inPipeline){
                            instr = instr->replicate();
                            instr->toDestroy = true;
                        }
                        instr->inPipeline = true;
                        instr->fetchPC = curPC;
                        instr->setParams(bitString);
                        try{
                            #ifndef DISABLE_TOOLS
                            if(!(this->toolManager.newIssue(instr->fetchPC, instr))){
                                #endif
                                numCycles = instr->behavior_fetch(BasePipeStage::unlockQueue);
                                this->curInstruction = instr;
                                #ifndef DISABLE_TOOLS
                            }
                            else{
                                if(instr->toDestroy){
                                    delete instr;
                                }
                                else{
                                    instr->inPipeline = false;
                                }
                                this->curInstruction = this->NOPInstrInstance;
                            }
                            #endif
                        }
                        catch(annull_exception &etc){

                            if(instr->toDestroy){
                                delete instr;
                            }
                            else{
                                instr->inPipeline = false;
                            }
                            this->curInstruction = this->NOPInstrInstance;
                            numCycles = 0;
                        }
                        this->instrCache.insert(std::pair< unsigned int, CacheElem >(bitString, CacheElem()));
                        instrCacheEnd = this->instrCache.end();
                    }
                    this->numInstructions++;
                    #ifndef DISABLE_TOOLS
                }
                #endif
            }
            wait((numCycles + 1)*this->latency);
            // HERE WAIT FOR END OF ALL STAGES
            this->waitPipeEnd();


            // Now I have to propagate the instruction to the next cycle if
            // the next stage has completed elaboration
            if(this->hasToFlush){
                if(this->curInstruction->toDestroy){
                    delete this->curInstruction;
                }
                else{
                    this->curInstruction->inPipeline = false;
                }
                this->curInstruction = this->NOPInstrInstance;
                this->nextInstruction = this->NOPInstrInstance;
                this->hasToFlush = false;
            }
            this->succStage->nextInstruction = this->curInstruction;
        }
        else{
            //The current stage is not doing anything since one of the following stages
            //is blocked to a data hazard.
            this->waitPipeBegin();
            //Note that I need to return controll to the scheduler, otherwise
            //I will be impossible to procede, this thread will always execute
            wait(this->latency);
            this->waitPipeEnd();
            if(this->hasToFlush){
                if(this->curInstruction->toDestroy){
                    delete this->curInstruction;
                }
                else{
                    this->curInstruction->inPipeline = false;
                }
                this->curInstruction = this->NOPInstrInstance;
                this->nextInstruction = this->NOPInstrInstance;
                this->hasToFlush = false;
            }
        }
        this->refreshRegisters();
        this->instrExecuting = false;
        this->instrEndEvent.notify();
        Instruction * succToCheck = this->succStage->nextInstruction;
        if(!succToCheck->checkHazard_decode()){
            this->chStalled = true;
            this->succStage->chStalled = true;
        }
        else{
            this->chStalled = false;
            this->succStage->chStalled = false;
        }
    }
}

void leon3_accat_trap::FETCH_PipeStage::waitPipeBegin() throw(){
    this->stageBeginning = true;
    this->stageBeginningEv.notify();
    if(!this->stage_decode->stageBeginning){
        wait(this->stage_decode->stageBeginningEv);
    }
    if(!this->stage_regs->stageBeginning){
        wait(this->stage_regs->stageBeginningEv);
    }
    if(!this->stage_execute->stageBeginning){
        wait(this->stage_execute->stageBeginningEv);
    }
    if(!this->stage_memory->stageBeginning){
        wait(this->stage_memory->stageBeginningEv);
    }
    if(!this->stage_exception->stageBeginning){
        wait(this->stage_exception->stageBeginningEv);
    }
    if(!this->stage_wb->stageBeginning){
        wait(this->stage_wb->stageBeginningEv);
    }
    this->stageEnded = false;
}

void leon3_accat_trap::FETCH_PipeStage::waitPipeEnd() throw(){
    this->stageBeginning = false;
    this->stageEnded = true;
    this->stageEndedEv.notify();
    if(!this->stage_decode->stageEnded){
        wait(this->stage_decode->stageEndedEv);
    }
    if(!this->stage_regs->stageEnded){
        wait(this->stage_regs->stageEndedEv);
    }
    if(!this->stage_execute->stageEnded){
        wait(this->stage_execute->stageEndedEv);
    }
    if(!this->stage_memory->stageEnded){
        wait(this->stage_memory->stageEndedEv);
    }
    if(!this->stage_exception->stageEnded){
        wait(this->stage_exception->stageEndedEv);
    }
    if(!this->stage_wb->stageEnded){
        wait(this->stage_wb->stageEndedEv);
    }
}

void leon3_accat_trap::FETCH_PipeStage::refreshRegisters() throw(){
    // Now we update the registers to propagate the values in the pipeline
    if(this->PSR.hasToPropagate){
        this->PSR.propagate();
    }
    if(this->WIM.hasToPropagate){
        this->WIM.propagate();
    }
    if(this->TBR.hasToPropagate){
        this->TBR.propagate();
    }
    if(this->Y.hasToPropagate){
        this->Y.propagate();
    }
    if(this->PC.hasToPropagate){
        this->PC.propagate();
    }
    if(this->NPC.hasToPropagate){
        this->NPC.propagate();
    }
    for(int i = 0; i < 8; i++){
        if(this->GLOBAL[i].hasToPropagate){
            this->GLOBAL[i].propagate();
        }
    }
    for(int i = 0; i < 128; i++){
        if(this->WINREGS[i].hasToPropagate){
            this->WINREGS[i].propagate();
        }
    }
    for(int i = 0; i < 32; i++){
        if(this->ASR[i].hasToPropagate){
            this->ASR[i].propagate();
        }
    }

    //Here we update the aliases, so that what they point to is updated in the pipeline
    if(this->REGS_wb[8].getPipeReg() != this->REGS_exception[8].getPipeReg()){
        for(int i = 8; i < 32; i++){
            this->REGS_wb[i].propagateAlias(*(this->REGS_exception[i].getPipeReg()));
        }
    }
    if(this->REGS_exception[8].getPipeReg() != this->REGS_memory[8].getPipeReg()){
        for(int i = 8; i < 32; i++){
            this->REGS_exception[i].propagateAlias(*(this->REGS_memory[i].getPipeReg()));
        }
    }
    if(this->REGS_memory[8].getPipeReg() != this->REGS_execute[8].getPipeReg()){
        for(int i = 8; i < 32; i++){
            this->REGS_memory[i].propagateAlias(*(this->REGS_execute[i].getPipeReg()));
        }
    }
    if(this->REGS_execute[8].getPipeReg() != this->REGS_regs[8].getPipeReg()){
        for(int i = 8; i < 32; i++){
            this->REGS_execute[i].propagateAlias(*(this->REGS_regs[i].getPipeReg()));
        }
    }
    if(this->REGS_regs[8].getPipeReg() != this->REGS_decode[8].getPipeReg()){
        for(int i = 8; i < 32; i++){
            this->REGS_regs[i].propagateAlias(*(this->REGS_decode[i].getPipeReg()));
        }
    }
    if(this->REGS_decode[8].getPipeReg() != this->REGS_fetch[8].getPipeReg()){
        for(int i = 8; i < 32; i++){
            this->REGS_decode[i].propagateAlias(*(this->REGS_fetch[i].getPipeReg()));
        }
    }

    // Finally registers are unlocked, so that stalls due to data hazards can be resolved
    std::map<unsigned int, std::vector<Register *> >::iterator unlockQueueIter, unlockQueueEnd;
    for(unlockQueueIter = BasePipeStage::unlockQueue.begin(), unlockQueueEnd = BasePipeStage::unlockQueue.end(); \
        unlockQueueIter != unlockQueueEnd; unlockQueueIter++){
        std::vector<Register *>::iterator regToUnlockIter, regToUnlockEnd;
        if(unlockQueueIter->first == 0){
            for(regToUnlockIter = unlockQueueIter->second.begin(), regToUnlockEnd = unlockQueueIter->second.end(); \
                regToUnlockIter != regToUnlockEnd; regToUnlockIter++){
                (*regToUnlockIter)->unlock();
            }
        }
        else{
            for(regToUnlockIter = unlockQueueIter->second.begin(), regToUnlockEnd = unlockQueueIter->second.end(); \
                regToUnlockIter != regToUnlockEnd; regToUnlockIter++){
                (*regToUnlockIter)->unlock(unlockQueueIter->first);
            }
        }
        unlockQueueIter->second.clear();
    }
}

leon3_accat_trap::FETCH_PipeStage::FETCH_PipeStage( ToolsManager< unsigned int > \
    & toolManager, unsigned int & IRQ, sc_event & instrEndEvent, bool & instrExecuting, \
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
    * stage_wb, BasePipeStage * prevStage, BasePipeStage * succStage ) : sc_module(pipeName), \
    BasePipeStage(latency, stage_fetch, stage_decode, stage_regs, stage_execute, stage_memory, \
    stage_exception, stage_wb, prevStage, succStage), PSR(PSR), WIM(WIM), TBR(TBR), Y(Y), \
    NPC(NPC), GLOBAL(GLOBAL), WINREGS(WINREGS), ASR(ASR), FP_fetch(FP_fetch), LR_fetch(LR_fetch), \
    SP_fetch(SP_fetch), PCR_fetch(PCR_fetch), REGS_fetch(REGS_fetch), FP_decode(FP_decode), \
    LR_decode(LR_decode), SP_decode(SP_decode), PCR_decode(PCR_decode), REGS_decode(REGS_decode), \
    FP_regs(FP_regs), LR_regs(LR_regs), SP_regs(SP_regs), PCR_regs(PCR_regs), REGS_regs(REGS_regs), \
    FP_execute(FP_execute), LR_execute(LR_execute), SP_execute(SP_execute), PCR_execute(PCR_execute), \
    REGS_execute(REGS_execute), FP_memory(FP_memory), LR_memory(LR_memory), SP_memory(SP_memory), \
    PCR_memory(PCR_memory), REGS_memory(REGS_memory), FP_exception(FP_exception), LR_exception(LR_exception), \
    SP_exception(SP_exception), PCR_exception(PCR_exception), REGS_exception(REGS_exception), \
    FP_wb(FP_wb), LR_wb(LR_wb), SP_wb(SP_wb), PCR_wb(PCR_wb), REGS_wb(REGS_wb), instrMem(instrMem), \
    INSTRUCTIONS(INSTRUCTIONS), PC(PC), numInstructions(numInstructions), instrExecuting(instrExecuting), \
    instrEndEvent(instrEndEvent), IRQ(IRQ), toolManager(toolManager){
    SC_THREAD(behavior);
    end_module();
}

void leon3_accat_trap::DECODE_PipeStage::behavior(){
    this->curInstruction = this->NOPInstrInstance;
    this->nextInstruction = this->NOPInstrInstance;
    Instruction * nextStageInstruction;
    while(true){
        unsigned int numCycles = 0;
        bool flushAnnulled = false;

        // HERE WAIT FOR BEGIN OF ALL STAGES
        this->waitPipeBegin();

        this->curInstruction = this->nextInstruction;
        if(!this->chStalled){
            this->curInstruction->lockRegs_decode();
            try{
                numCycles = this->curInstruction->behavior_decode(BasePipeStage::unlockQueue);
            }
            catch(annull_exception &etc){
                flushAnnulled = this->curInstruction->flushPipeline;
                this->curInstruction->flushPipeline = false;
                this->curInstruction->getUnlock_decode(BasePipeStage::unlockQueue);

                if(this->curInstruction->toDestroy){
                    delete this->curInstruction;
                }
                else{
                    this->curInstruction->inPipeline = false;
                }
                this->curInstruction = this->NOPInstrInstance;
                numCycles = 0;
            }
            wait((numCycles + 1)*this->latency);
            // flushing current stage
            if(this->curInstruction->flushPipeline || flushAnnulled){
                this->curInstruction->flushPipeline = false;
                //Now I have to flush the preceding pipeline stages
                this->prevStage->flush();
            }
            if(this->hasToFlush){
                if(this->curInstruction->toDestroy){
                    delete this->curInstruction;
                }
                else{
                    this->curInstruction->inPipeline = false;
                }
                // First of all I have to free any used resource in case there are any
                this->curInstruction = this->NOPInstrInstance;
                this->nextInstruction = this->NOPInstrInstance;
                this->hasToFlush = false;
            }
            nextStageInstruction = this->curInstruction;
        }
        else{
            //The current stage is not doing anything since one of the following stages
            //is blocked to a data hazard.
            //Note that I need to return controll to the scheduler, otherwise
            //I will be impossible to procede, this thread will always execute
            wait(this->latency);
            if(this->hasToFlush){
                if(this->curInstruction->toDestroy){
                    delete this->curInstruction;
                }
                else{
                    this->curInstruction->inPipeline = false;
                }
                // First of all I have to free any used resource in case there are any
                this->curInstruction = this->NOPInstrInstance;
                this->nextInstruction = this->NOPInstrInstance;
                this->hasToFlush = false;
            }
            nextStageInstruction = this->NOPInstrInstance;
        }
        // HERE WAIT FOR END OF ALL STAGES
        this->waitPipeEnd();

        this->succStage->nextInstruction = nextStageInstruction;
    }
}

void leon3_accat_trap::DECODE_PipeStage::waitPipeBegin() throw(){
    this->stageBeginning = true;
    this->stageBeginningEv.notify();
    if(!this->stage_fetch->stageBeginning){
        wait(this->stage_fetch->stageBeginningEv);
    }
    if(!this->stage_regs->stageBeginning){
        wait(this->stage_regs->stageBeginningEv);
    }
    if(!this->stage_execute->stageBeginning){
        wait(this->stage_execute->stageBeginningEv);
    }
    if(!this->stage_memory->stageBeginning){
        wait(this->stage_memory->stageBeginningEv);
    }
    if(!this->stage_exception->stageBeginning){
        wait(this->stage_exception->stageBeginningEv);
    }
    if(!this->stage_wb->stageBeginning){
        wait(this->stage_wb->stageBeginningEv);
    }
    this->stageEnded = false;
}

void leon3_accat_trap::DECODE_PipeStage::waitPipeEnd() throw(){
    this->stageBeginning = false;
    this->stageEnded = true;
    this->stageEndedEv.notify();
    if(!this->stage_fetch->stageEnded){
        wait(this->stage_fetch->stageEndedEv);
    }
    if(!this->stage_regs->stageEnded){
        wait(this->stage_regs->stageEndedEv);
    }
    if(!this->stage_execute->stageEnded){
        wait(this->stage_execute->stageEndedEv);
    }
    if(!this->stage_memory->stageEnded){
        wait(this->stage_memory->stageEndedEv);
    }
    if(!this->stage_exception->stageEnded){
        wait(this->stage_exception->stageEndedEv);
    }
    if(!this->stage_wb->stageEnded){
        wait(this->stage_wb->stageEndedEv);
    }
}

leon3_accat_trap::DECODE_PipeStage::DECODE_PipeStage( sc_module_name pipeName, sc_time \
    & latency, BasePipeStage * stage_fetch, BasePipeStage * stage_decode, BasePipeStage \
    * stage_regs, BasePipeStage * stage_execute, BasePipeStage * stage_memory, BasePipeStage \
    * stage_exception, BasePipeStage * stage_wb, BasePipeStage * prevStage, BasePipeStage \
    * succStage ) : sc_module(pipeName), BasePipeStage(latency, stage_fetch, stage_decode, \
    stage_regs, stage_execute, stage_memory, stage_exception, stage_wb, prevStage, succStage){
    SC_THREAD(behavior);
    end_module();
}

void leon3_accat_trap::REGS_PipeStage::behavior(){
    this->curInstruction = this->NOPInstrInstance;
    this->nextInstruction = this->NOPInstrInstance;
    while(true){
        unsigned int numCycles = 0;
        bool flushAnnulled = false;

        // HERE WAIT FOR BEGIN OF ALL STAGES
        this->waitPipeBegin();

        this->curInstruction = this->nextInstruction;
        try{
            numCycles = this->curInstruction->behavior_regs(BasePipeStage::unlockQueue);
        }
        catch(annull_exception &etc){
            flushAnnulled = this->curInstruction->flushPipeline;
            this->curInstruction->flushPipeline = false;
            this->curInstruction->getUnlock_regs(BasePipeStage::unlockQueue);

            if(this->curInstruction->toDestroy){
                delete this->curInstruction;
            }
            else{
                this->curInstruction->inPipeline = false;
            }
            this->curInstruction = this->NOPInstrInstance;
            numCycles = 0;
        }
        wait((numCycles + 1)*this->latency);
        // flushing current stage
        if(this->curInstruction->flushPipeline || flushAnnulled){
            this->curInstruction->flushPipeline = false;
            //Now I have to flush the preceding pipeline stages
            this->prevStage->flush();
        }
        // HERE WAIT FOR END OF ALL STAGES
        this->waitPipeEnd();

        if(this->hasToFlush){
            if(this->curInstruction->toDestroy){
                delete this->curInstruction;
            }
            else{
                this->curInstruction->inPipeline = false;
            }
            // First of all I have to free any used resource in case there are any
            this->curInstruction = this->NOPInstrInstance;
            this->nextInstruction = this->NOPInstrInstance;
            this->hasToFlush = false;
        }
        this->succStage->nextInstruction = this->curInstruction;
    }
}

void leon3_accat_trap::REGS_PipeStage::waitPipeBegin() throw(){
    this->stageBeginning = true;
    this->stageBeginningEv.notify();
    if(!this->stage_fetch->stageBeginning){
        wait(this->stage_fetch->stageBeginningEv);
    }
    if(!this->stage_decode->stageBeginning){
        wait(this->stage_decode->stageBeginningEv);
    }
    if(!this->stage_execute->stageBeginning){
        wait(this->stage_execute->stageBeginningEv);
    }
    if(!this->stage_memory->stageBeginning){
        wait(this->stage_memory->stageBeginningEv);
    }
    if(!this->stage_exception->stageBeginning){
        wait(this->stage_exception->stageBeginningEv);
    }
    if(!this->stage_wb->stageBeginning){
        wait(this->stage_wb->stageBeginningEv);
    }
    this->stageEnded = false;
}

void leon3_accat_trap::REGS_PipeStage::waitPipeEnd() throw(){
    this->stageBeginning = false;
    this->stageEnded = true;
    this->stageEndedEv.notify();
    if(!this->stage_fetch->stageEnded){
        wait(this->stage_fetch->stageEndedEv);
    }
    if(!this->stage_decode->stageEnded){
        wait(this->stage_decode->stageEndedEv);
    }
    if(!this->stage_execute->stageEnded){
        wait(this->stage_execute->stageEndedEv);
    }
    if(!this->stage_memory->stageEnded){
        wait(this->stage_memory->stageEndedEv);
    }
    if(!this->stage_exception->stageEnded){
        wait(this->stage_exception->stageEndedEv);
    }
    if(!this->stage_wb->stageEnded){
        wait(this->stage_wb->stageEndedEv);
    }
}

leon3_accat_trap::REGS_PipeStage::REGS_PipeStage( sc_module_name pipeName, sc_time \
    & latency, BasePipeStage * stage_fetch, BasePipeStage * stage_decode, BasePipeStage \
    * stage_regs, BasePipeStage * stage_execute, BasePipeStage * stage_memory, BasePipeStage \
    * stage_exception, BasePipeStage * stage_wb, BasePipeStage * prevStage, BasePipeStage \
    * succStage ) : sc_module(pipeName), BasePipeStage(latency, stage_fetch, stage_decode, \
    stage_regs, stage_execute, stage_memory, stage_exception, stage_wb, prevStage, succStage){
    SC_THREAD(behavior);
    end_module();
}

void leon3_accat_trap::EXECUTE_PipeStage::behavior(){
    this->curInstruction = this->NOPInstrInstance;
    this->nextInstruction = this->NOPInstrInstance;
    while(true){
        unsigned int numCycles = 0;
        bool flushAnnulled = false;

        // HERE WAIT FOR BEGIN OF ALL STAGES
        this->waitPipeBegin();

        this->curInstruction = this->nextInstruction;
        try{
            numCycles = this->curInstruction->behavior_execute(BasePipeStage::unlockQueue);
        }
        catch(annull_exception &etc){
            flushAnnulled = this->curInstruction->flushPipeline;
            this->curInstruction->flushPipeline = false;
            this->curInstruction->getUnlock_execute(BasePipeStage::unlockQueue);

            if(this->curInstruction->toDestroy){
                delete this->curInstruction;
            }
            else{
                this->curInstruction->inPipeline = false;
            }
            this->curInstruction = this->NOPInstrInstance;
            numCycles = 0;
        }
        wait((numCycles + 1)*this->latency);
        // flushing current stage
        if(this->curInstruction->flushPipeline || flushAnnulled){
            this->curInstruction->flushPipeline = false;
            //Now I have to flush the preceding pipeline stages
            this->prevStage->flush();
        }
        // HERE WAIT FOR END OF ALL STAGES
        this->waitPipeEnd();

        if(this->hasToFlush){
            if(this->curInstruction->toDestroy){
                delete this->curInstruction;
            }
            else{
                this->curInstruction->inPipeline = false;
            }
            // First of all I have to free any used resource in case there are any
            this->curInstruction = this->NOPInstrInstance;
            this->nextInstruction = this->NOPInstrInstance;
            this->hasToFlush = false;
        }
        this->succStage->nextInstruction = this->curInstruction;
    }
}

void leon3_accat_trap::EXECUTE_PipeStage::waitPipeBegin() throw(){
    this->stageBeginning = true;
    this->stageBeginningEv.notify();
    if(!this->stage_fetch->stageBeginning){
        wait(this->stage_fetch->stageBeginningEv);
    }
    if(!this->stage_decode->stageBeginning){
        wait(this->stage_decode->stageBeginningEv);
    }
    if(!this->stage_regs->stageBeginning){
        wait(this->stage_regs->stageBeginningEv);
    }
    if(!this->stage_memory->stageBeginning){
        wait(this->stage_memory->stageBeginningEv);
    }
    if(!this->stage_exception->stageBeginning){
        wait(this->stage_exception->stageBeginningEv);
    }
    if(!this->stage_wb->stageBeginning){
        wait(this->stage_wb->stageBeginningEv);
    }
    this->stageEnded = false;
}

void leon3_accat_trap::EXECUTE_PipeStage::waitPipeEnd() throw(){
    this->stageBeginning = false;
    this->stageEnded = true;
    this->stageEndedEv.notify();
    if(!this->stage_fetch->stageEnded){
        wait(this->stage_fetch->stageEndedEv);
    }
    if(!this->stage_decode->stageEnded){
        wait(this->stage_decode->stageEndedEv);
    }
    if(!this->stage_regs->stageEnded){
        wait(this->stage_regs->stageEndedEv);
    }
    if(!this->stage_memory->stageEnded){
        wait(this->stage_memory->stageEndedEv);
    }
    if(!this->stage_exception->stageEnded){
        wait(this->stage_exception->stageEndedEv);
    }
    if(!this->stage_wb->stageEnded){
        wait(this->stage_wb->stageEndedEv);
    }
}

leon3_accat_trap::EXECUTE_PipeStage::EXECUTE_PipeStage( sc_module_name pipeName, \
    sc_time & latency, BasePipeStage * stage_fetch, BasePipeStage * stage_decode, BasePipeStage \
    * stage_regs, BasePipeStage * stage_execute, BasePipeStage * stage_memory, BasePipeStage \
    * stage_exception, BasePipeStage * stage_wb, BasePipeStage * prevStage, BasePipeStage \
    * succStage ) : sc_module(pipeName), BasePipeStage(latency, stage_fetch, stage_decode, \
    stage_regs, stage_execute, stage_memory, stage_exception, stage_wb, prevStage, succStage){
    SC_THREAD(behavior);
    end_module();
}

void leon3_accat_trap::MEMORY_PipeStage::behavior(){
    this->curInstruction = this->NOPInstrInstance;
    this->nextInstruction = this->NOPInstrInstance;
    while(true){
        unsigned int numCycles = 0;
        bool flushAnnulled = false;

        // HERE WAIT FOR BEGIN OF ALL STAGES
        this->waitPipeBegin();

        this->curInstruction = this->nextInstruction;
        try{
            numCycles = this->curInstruction->behavior_memory(BasePipeStage::unlockQueue);
        }
        catch(annull_exception &etc){
            flushAnnulled = this->curInstruction->flushPipeline;
            this->curInstruction->flushPipeline = false;
            this->curInstruction->getUnlock_memory(BasePipeStage::unlockQueue);

            if(this->curInstruction->toDestroy){
                delete this->curInstruction;
            }
            else{
                this->curInstruction->inPipeline = false;
            }
            this->curInstruction = this->NOPInstrInstance;
            numCycles = 0;
        }
        wait((numCycles + 1)*this->latency);
        // flushing current stage
        if(this->curInstruction->flushPipeline || flushAnnulled){
            this->curInstruction->flushPipeline = false;
            //Now I have to flush the preceding pipeline stages
            this->prevStage->flush();
        }
        // HERE WAIT FOR END OF ALL STAGES
        this->waitPipeEnd();

        if(this->hasToFlush){
            if(this->curInstruction->toDestroy){
                delete this->curInstruction;
            }
            else{
                this->curInstruction->inPipeline = false;
            }
            // First of all I have to free any used resource in case there are any
            this->curInstruction = this->NOPInstrInstance;
            this->nextInstruction = this->NOPInstrInstance;
            this->hasToFlush = false;
        }
        this->succStage->nextInstruction = this->curInstruction;
    }
}

void leon3_accat_trap::MEMORY_PipeStage::waitPipeBegin() throw(){
    this->stageBeginning = true;
    this->stageBeginningEv.notify();
    if(!this->stage_fetch->stageBeginning){
        wait(this->stage_fetch->stageBeginningEv);
    }
    if(!this->stage_decode->stageBeginning){
        wait(this->stage_decode->stageBeginningEv);
    }
    if(!this->stage_regs->stageBeginning){
        wait(this->stage_regs->stageBeginningEv);
    }
    if(!this->stage_execute->stageBeginning){
        wait(this->stage_execute->stageBeginningEv);
    }
    if(!this->stage_exception->stageBeginning){
        wait(this->stage_exception->stageBeginningEv);
    }
    if(!this->stage_wb->stageBeginning){
        wait(this->stage_wb->stageBeginningEv);
    }
    this->stageEnded = false;
}

void leon3_accat_trap::MEMORY_PipeStage::waitPipeEnd() throw(){
    this->stageBeginning = false;
    this->stageEnded = true;
    this->stageEndedEv.notify();
    if(!this->stage_fetch->stageEnded){
        wait(this->stage_fetch->stageEndedEv);
    }
    if(!this->stage_decode->stageEnded){
        wait(this->stage_decode->stageEndedEv);
    }
    if(!this->stage_regs->stageEnded){
        wait(this->stage_regs->stageEndedEv);
    }
    if(!this->stage_execute->stageEnded){
        wait(this->stage_execute->stageEndedEv);
    }
    if(!this->stage_exception->stageEnded){
        wait(this->stage_exception->stageEndedEv);
    }
    if(!this->stage_wb->stageEnded){
        wait(this->stage_wb->stageEndedEv);
    }
}

leon3_accat_trap::MEMORY_PipeStage::MEMORY_PipeStage( sc_module_name pipeName, sc_time \
    & latency, BasePipeStage * stage_fetch, BasePipeStage * stage_decode, BasePipeStage \
    * stage_regs, BasePipeStage * stage_execute, BasePipeStage * stage_memory, BasePipeStage \
    * stage_exception, BasePipeStage * stage_wb, BasePipeStage * prevStage, BasePipeStage \
    * succStage ) : sc_module(pipeName), BasePipeStage(latency, stage_fetch, stage_decode, \
    stage_regs, stage_execute, stage_memory, stage_exception, stage_wb, prevStage, succStage){
    SC_THREAD(behavior);
    end_module();
}

void leon3_accat_trap::EXCEPTION_PipeStage::behavior(){
    this->curInstruction = this->NOPInstrInstance;
    this->nextInstruction = this->NOPInstrInstance;
    while(true){
        unsigned int numCycles = 0;
        bool flushAnnulled = false;

        // HERE WAIT FOR BEGIN OF ALL STAGES
        this->waitPipeBegin();

        this->curInstruction = this->nextInstruction;
        try{
            numCycles = this->curInstruction->behavior_exception(BasePipeStage::unlockQueue);
        }
        catch(annull_exception &etc){
            flushAnnulled = this->curInstruction->flushPipeline;
            this->curInstruction->flushPipeline = false;
            this->curInstruction->getUnlock_exception(BasePipeStage::unlockQueue);

            if(this->curInstruction->toDestroy){
                delete this->curInstruction;
            }
            else{
                this->curInstruction->inPipeline = false;
            }
            this->curInstruction = this->NOPInstrInstance;
            numCycles = 0;
        }
        wait((numCycles + 1)*this->latency);
        // flushing current stage
        if(this->curInstruction->flushPipeline || flushAnnulled){
            this->curInstruction->flushPipeline = false;
            //Now I have to flush the preceding pipeline stages
            this->prevStage->flush();
        }
        // HERE WAIT FOR END OF ALL STAGES
        this->waitPipeEnd();

        if(this->hasToFlush){
            if(this->curInstruction->toDestroy){
                delete this->curInstruction;
            }
            else{
                this->curInstruction->inPipeline = false;
            }
            // First of all I have to free any used resource in case there are any
            this->curInstruction = this->NOPInstrInstance;
            this->nextInstruction = this->NOPInstrInstance;
            this->hasToFlush = false;
        }
        this->succStage->nextInstruction = this->curInstruction;
    }
}

void leon3_accat_trap::EXCEPTION_PipeStage::waitPipeBegin() throw(){
    this->stageBeginning = true;
    this->stageBeginningEv.notify();
    if(!this->stage_fetch->stageBeginning){
        wait(this->stage_fetch->stageBeginningEv);
    }
    if(!this->stage_decode->stageBeginning){
        wait(this->stage_decode->stageBeginningEv);
    }
    if(!this->stage_regs->stageBeginning){
        wait(this->stage_regs->stageBeginningEv);
    }
    if(!this->stage_execute->stageBeginning){
        wait(this->stage_execute->stageBeginningEv);
    }
    if(!this->stage_memory->stageBeginning){
        wait(this->stage_memory->stageBeginningEv);
    }
    if(!this->stage_wb->stageBeginning){
        wait(this->stage_wb->stageBeginningEv);
    }
    this->stageEnded = false;
}

void leon3_accat_trap::EXCEPTION_PipeStage::waitPipeEnd() throw(){
    this->stageBeginning = false;
    this->stageEnded = true;
    this->stageEndedEv.notify();
    if(!this->stage_fetch->stageEnded){
        wait(this->stage_fetch->stageEndedEv);
    }
    if(!this->stage_decode->stageEnded){
        wait(this->stage_decode->stageEndedEv);
    }
    if(!this->stage_regs->stageEnded){
        wait(this->stage_regs->stageEndedEv);
    }
    if(!this->stage_execute->stageEnded){
        wait(this->stage_execute->stageEndedEv);
    }
    if(!this->stage_memory->stageEnded){
        wait(this->stage_memory->stageEndedEv);
    }
    if(!this->stage_wb->stageEnded){
        wait(this->stage_wb->stageEndedEv);
    }
}

leon3_accat_trap::EXCEPTION_PipeStage::EXCEPTION_PipeStage( sc_module_name pipeName, \
    sc_time & latency, BasePipeStage * stage_fetch, BasePipeStage * stage_decode, BasePipeStage \
    * stage_regs, BasePipeStage * stage_execute, BasePipeStage * stage_memory, BasePipeStage \
    * stage_exception, BasePipeStage * stage_wb, BasePipeStage * prevStage, BasePipeStage \
    * succStage ) : sc_module(pipeName), BasePipeStage(latency, stage_fetch, stage_decode, \
    stage_regs, stage_execute, stage_memory, stage_exception, stage_wb, prevStage, succStage){
    SC_THREAD(behavior);
    end_module();
}

void leon3_accat_trap::WB_PipeStage::behavior(){
    this->curInstruction = this->NOPInstrInstance;
    this->nextInstruction = this->NOPInstrInstance;
    while(true){
        unsigned int numCycles = 0;
        bool flushAnnulled = false;

        // HERE WAIT FOR BEGIN OF ALL STAGES
        this->waitPipeBegin();

        this->curInstruction = this->nextInstruction;
        try{
            numCycles = this->curInstruction->behavior_wb(BasePipeStage::unlockQueue);
        }
        catch(annull_exception &etc){
            flushAnnulled = this->curInstruction->flushPipeline;
            this->curInstruction->flushPipeline = false;
            this->curInstruction->getUnlock_wb(BasePipeStage::unlockQueue);

            if(this->curInstruction->toDestroy){
                delete this->curInstruction;
            }
            else{
                this->curInstruction->inPipeline = false;
            }
            this->curInstruction = this->NOPInstrInstance;
            numCycles = 0;
        }
        wait((numCycles + 1)*this->latency);
        // flushing current stage
        if(this->curInstruction->flushPipeline || flushAnnulled){
            this->curInstruction->flushPipeline = false;
            //Now I have to flush the preceding pipeline stages
            this->prevStage->flush();
        }
        // HERE WAIT FOR END OF ALL STAGES
        this->waitPipeEnd();

        if(this->curInstruction->toDestroy){
            delete this->curInstruction;
        }
        else{
            this->curInstruction->inPipeline = false;
        }
    }
}

void leon3_accat_trap::WB_PipeStage::waitPipeBegin() throw(){
    this->stageBeginning = true;
    this->stageBeginningEv.notify();
    if(!this->stage_fetch->stageBeginning){
        wait(this->stage_fetch->stageBeginningEv);
    }
    if(!this->stage_decode->stageBeginning){
        wait(this->stage_decode->stageBeginningEv);
    }
    if(!this->stage_regs->stageBeginning){
        wait(this->stage_regs->stageBeginningEv);
    }
    if(!this->stage_execute->stageBeginning){
        wait(this->stage_execute->stageBeginningEv);
    }
    if(!this->stage_memory->stageBeginning){
        wait(this->stage_memory->stageBeginningEv);
    }
    if(!this->stage_exception->stageBeginning){
        wait(this->stage_exception->stageBeginningEv);
    }
    this->stageEnded = false;
}

void leon3_accat_trap::WB_PipeStage::waitPipeEnd() throw(){
    this->stageBeginning = false;
    this->stageEnded = true;
    this->stageEndedEv.notify();
    if(!this->stage_fetch->stageEnded){
        wait(this->stage_fetch->stageEndedEv);
    }
    if(!this->stage_decode->stageEnded){
        wait(this->stage_decode->stageEndedEv);
    }
    if(!this->stage_regs->stageEnded){
        wait(this->stage_regs->stageEndedEv);
    }
    if(!this->stage_execute->stageEnded){
        wait(this->stage_execute->stageEndedEv);
    }
    if(!this->stage_memory->stageEnded){
        wait(this->stage_memory->stageEndedEv);
    }
    if(!this->stage_exception->stageEnded){
        wait(this->stage_exception->stageEndedEv);
    }
}

leon3_accat_trap::WB_PipeStage::WB_PipeStage( sc_module_name pipeName, sc_time & \
    latency, BasePipeStage * stage_fetch, BasePipeStage * stage_decode, BasePipeStage \
    * stage_regs, BasePipeStage * stage_execute, BasePipeStage * stage_memory, BasePipeStage \
    * stage_exception, BasePipeStage * stage_wb, BasePipeStage * prevStage, BasePipeStage \
    * succStage ) : sc_module(pipeName), BasePipeStage(latency, stage_fetch, stage_decode, \
    stage_regs, stage_execute, stage_memory, stage_exception, stage_wb, prevStage, succStage){
    SC_THREAD(behavior);
    end_module();
}


