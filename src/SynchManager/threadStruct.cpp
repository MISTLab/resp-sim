#include "threadStruct.hpp"
#include "synchManager.hpp"

#include "utils.hpp"

#include <list>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>

AttributeEmu::AttributeEmu(){
    this->stackSize = defaultStackSize;
    this->detached = false;
    this->preemptive = true;
    this->schedPolicy = resp::SYSC_SCHED_FIFO;
    this->priority = resp::SYSC_PRIO_MIN;
    this->deadline = 0;
}

AttributeEmu::AttributeEmu(unsigned int stackSize){
    this->stackSize = stackSize;
    this->detached = false;
    this->preemptive = true;
    this->schedPolicy = resp::SYSC_SCHED_FIFO;
    this->priority = resp::SYSC_PRIO_MIN;
    this->deadline = 0;
}


ThreadEmu::ThreadEmu(int id, thread_routine_t thread_routine, void * args, unsigned int stackBase, unsigned int tlsSize, AttributeEmu *attr){
    this->id = id;
    this->thread_routine = thread_routine;
    this->args = args;
    this->setSyscRetVal = false;
    this->syscallRetVal = 0;
    this->attr = attr;
    if(attr->schedPolicy == resp::SYSC_SCHED_EDF){
        attr->deadline += (unsigned int)(sc_time_stamp().to_double()/1e3);
    }
    this->status = READY;
    this->stackBase = stackBase;
    this->tlsSize = tlsSize;
    this->retVal = NULL;
    this->numJoined = 0;
    this->lastPc = (unsigned int)(unsigned long)thread_routine;
    this->lastRetAddr = (unsigned int)(unsigned long)thread_routine;
    this->isIRQ = false;

    #ifndef NDEBUG
    if(this->attr->stackSize < tlsSize)
        THROW_EXCEPTION("The stack size chosen for thread " << id << " is smaller than its TLS size");
    #endif
}

ThreadEmu::~ThreadEmu(){
}

void ThreadEmu::printTrace(){
    std::cout << "Thread " << std::dec<< this->id << " trace:" << std::endl;
    std::vector<ExecutionTrace>::iterator traceEIter,  traceEnd;
    for(traceEIter = this->execTrace.begin(),  traceEnd = this->execTrace.end(); traceEIter !=  traceEnd; traceEIter++){
        std::cout << "Time: " << std::scientific << std::setprecision (7) << traceEIter->startTime << " - " << std::scientific << std::setprecision (7) << traceEIter->endTime << " Processor " << std::dec << traceEIter->processorId << " border functions " << std::showbase << std::hex << traceEIter->startAddress << " -- " << traceEIter->endAddress << std::dec << std::endl;
    }
    if(this->curTraceElem.processorId != -1)
        std::cout << "Current-execution element -- Time: " << std::scientific << std::setprecision (7) << this->curTraceElem.startTime << " Processor " << std::dec << this->curTraceElem.processorId << " border functions " << std::showbase << std::hex << this->curTraceElem.startAddress << std::dec << std::endl;
    else
        std::cout << "Currently this thread is not being executed" << std::endl;
}

std::string ThreadEmu::getTrace(){
    std::ostringstream temp;

    temp << "Thread " << std::dec<< this->id << " trace:" << std::endl;
    std::vector<ExecutionTrace>::iterator traceEIter,  traceEnd;
    for(traceEIter = this->execTrace.begin(),  traceEnd = this->execTrace.end(); traceEIter !=  traceEnd; traceEIter++){
        temp << "Time: " << std::scientific << std::setprecision (7) << traceEIter->startTime << " - " << std::scientific << std::setprecision (7) << traceEIter->endTime << " Processor " << std::dec << traceEIter->processorId << " border functions " << std::showbase << std::hex << traceEIter->startAddress << " -- " << traceEIter->endAddress << std::dec << std::endl;
    }

    return temp.str();
}

MutexEmu::MutexEmu(){
    this->owner = NULL;
    this->status = FREE;
    this->recursiveIndex = 0;
}

MutexEmu::~MutexEmu(){
}

SemaphoreEmu::SemaphoreEmu(int initialValue){
    this->value = initialValue;
}

SemaphoreEmu::~SemaphoreEmu(){
}
