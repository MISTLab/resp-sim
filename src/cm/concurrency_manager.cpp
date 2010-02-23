/***************************************************************************\
 *
 *
 *         ___           ___           ___           ___
 *        /  /\         /  /\         /  /\         /  /\
 *       /  /::\       /  /:/_       /  /:/_       /  /::\
 *      /  /:/\:\     /  /:/ /\     /  /:/ /\     /  /:/\:\
 *     /  /:/~/:/    /  /:/ /:/_   /  /:/ /::\   /  /:/~/:/
 *    /__/:/ /:/___ /__/:/ /:/ /\ /__/:/ /:/\:\ /__/:/ /:/
 *    \  \:\/:::::/ \  \:\/:/ /:/ \  \:\/:/~/:/ \  \:\/:/
 *     \  \::/~~~~   \  \::/ /:/   \  \::/ /:/   \  \::/
 *      \  \:\        \  \:\/:/     \__\/ /:/     \  \:\
 *       \  \:\        \  \::/        /__/:/       \  \:\
 *        \__\/         \__\/         \__\/         \__\/
 *
 *
 *
 *
 *   This file is part of ReSP.
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 *
 *   (c) Giovanni Beltrame, Luca Fossati
 *       Giovanni.Beltrame@esa.int fossati@elet.polimi.it
 *
\***************************************************************************/

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include <systemc.h>

#include "bfdWrapper.hpp"
#include "concurrency_manager.hpp"

#ifdef NDEBUG
#undef NDEBUG
#endif

//Initialization of some static variables
const int resp::ConcurrencyManager::SYSC_SCHED_FIFO = 0;
const int resp::ConcurrencyManager::SYSC_SCHED_OTHER = 1;
const int resp::ConcurrencyManager::SYSC_SCHED_RR = 2;
const int resp::ConcurrencyManager::SYSC_SCHED_EDF = 5;

const int resp::ConcurrencyManager::SYSC_PRIO_MAX = 255;
const int resp::ConcurrencyManager::SYSC_PRIO_MIN = 0;

const int resp::ConcurrencyManager::SYSC_PREEMPTIVE = 0;
const int resp::ConcurrencyManager::SYSC_NON_PREEMPTIVE = 1;

///specifies whether blocked processor halts or keep on working in busy wait loops
bool resp::ConcurrencyManager::busyWaitLoop = true;

///Specifies the stack size for each thread
unsigned int resp::ConcurrencyManager::threadStackSize = 1024*20;

///Associates thred properties with a routine name: all threads
///created from that routine shall have such properties
std::map<std::string, DefaultThreadInfo> resp::ConcurrencyManager::defThreadInfo;

///The registered interrupt service routines.
std::map<int, std::string> resp::ConcurrencyManager::interruptServiceRoutines;

///The size and content of the thread-local-storage
unsigned int resp::ConcurrencyManager::tlsSize = 0;
unsigned char * resp::ConcurrencyManager::tlsData = NULL;

///Default thread attribute, used to initialize threads which do not declare
///any specific attribute
AttributeEmu resp::ConcurrencyManager::defaultAttr;

///Latencies of the scheduling operations, used to mimick the behavior
///of a real scheduler and to correctly keep track of time
sc_time resp::ConcurrencyManager::schedLatency(SC_ZERO_TIME);
sc_time resp::ConcurrencyManager::deSchedLatency(SC_ZERO_TIME);
sc_time resp::ConcurrencyManager::schedChooseLatency(SC_ZERO_TIME);

///Function used to sort the tasks in the last ready queue according
///to their deadlines, the closest deadline first
bool deadlineSort(const ThreadEmu * a, const ThreadEmu * b){
    if(a->attr == NULL || b->attr == NULL)
        return false;
    return a->attr->deadline < b->attr->deadline;
}

resp::ConcurrencyManager::ConcurrencyManager(std::string execName) : execName(execName){
    //Constructor: there is not much to do
    //a part from se-setting the reentracy
    //variables
    this->keys = -1;
    this->mallocMutex = -1;
    this->sfpMutex = -1;
    this->sinitMutex = -1;
    this->fpMutex = -1;
    this->maxProcId = 0;
    this->readyQueue = new std::deque<ThreadEmu *>[resp::ConcurrencyManager::SYSC_PRIO_MAX + 2];
}

resp::ConcurrencyManager::~ConcurrencyManager(){
    if(resp::ConcurrencyManager::tlsData != NULL){
        delete [] resp::ConcurrencyManager::tlsData;
        resp::ConcurrencyManager::tlsData = NULL;
    }
    delete [] this->readyQueue;
}

///Resets the CM to its original status as after the construction
void resp::ConcurrencyManager::reset(){
    //Resetting static variables
    resp::ConcurrencyManager::busyWaitLoop = true;
    resp::ConcurrencyManager::threadStackSize = 1024*20;
    resp::ConcurrencyManager::defThreadInfo.clear();
    resp::ConcurrencyManager::interruptServiceRoutines.clear();
    resp::ConcurrencyManager::tlsSize = 0;
    if(resp::ConcurrencyManager::tlsData != NULL){
        delete [] resp::ConcurrencyManager::tlsData;
        resp::ConcurrencyManager::tlsData = NULL;
    }
    //Resetting instance variables
    this->keys = -1;
    this->mallocMutex = -1;
    this->sfpMutex = -1;
    this->sinitMutex = -1;
    this->fpMutex = -1;
    this->maxProcId = 0;
    this->existingAttr.clear();
    this->managedProc.clear();
    this->stacks.clear();
    this->threadSpecific.clear();
    for(int i = 0; i < resp::ConcurrencyManager::SYSC_PRIO_MAX + 2; i++){
        this->readyQueue[i].clear();
    }
}

///*******************************************************************
/// Helper methods used to aid concurrency management
///*******************************************************************
///Determines if there is an idle processor and, in case it
///schedules a thread for execution on that processor
bool resp::ConcurrencyManager::scheduleFreeProcessor(ThreadEmu *th){
    //Ok,  now I have to see if there is a free processor on which I can schedule the thread
    std::map<unsigned int, Processor<unsigned int>* >::iterator procIter, procIterEnd;
    for(procIter = this->managedProc.begin(), procIterEnd = this->managedProc.end(); procIter != procIterEnd; procIter++){
        if(procIter->second->runThread == NULL){
            //Here we are,  I have found the empty processor
            procIter->second->schedule(th);
            return true;
        }
    }
    return false;
}

///Determines the lowest priority thread which is currently running and it
///preepts it in case the current thread has a higher priority
bool resp::ConcurrencyManager::preemptLowerPrio(ThreadEmu *th){
    //First of all I loop all over the ready processors in order to determine
    //the ready ones
    std::map<unsigned int, Processor<unsigned int>* >::iterator procIter, procIterEnd;
    for(procIter = this->managedProc.begin(), procIterEnd = this->managedProc.end(); procIter != procIterEnd; procIter++){
        if(procIter->second->runThread == NULL){
            //Here we are, I have found the empty processor
            procIter->second->schedule(th);
            return true;
        }
    }
    //Here I have found no idle processor: I have to determine the processor running the lowest priority
    //thread and then, if it is lower than the current thread's priority, preempt it
    int curMinPrio = resp::ConcurrencyManager::SYSC_PRIO_MAX + 2;
    Processor<unsigned int> * minPriProc = NULL;
    for(procIter = this->managedProc.begin(), procIterEnd = this->managedProc.end(); procIter != procIterEnd; procIter++){
        int curPrio = 0;
        if(procIter->second->runThread->attr != NULL){
            if(procIter->second->runThread->attr->schedPolicy == resp::ConcurrencyManager::SYSC_SCHED_EDF){
                curPrio = resp::ConcurrencyManager::SYSC_PRIO_MAX + 1;
            }
            else
                curPrio = procIter->second->runThread->attr->priority;
        }
        else{
            std::cerr << "thread attribute is NULL??" << std::endl;
        }
        if(curPrio < curMinPrio){
            curMinPrio = curPrio;
            minPriProc = procIter->second;
        }
    }

    if(th->attr != NULL){
        int newPrio = 0;
        if(th->attr->schedPolicy == resp::ConcurrencyManager::SYSC_SCHED_EDF){
            newPrio = resp::ConcurrencyManager::SYSC_PRIO_MAX + 1;
        }
        else
            newPrio = th->attr->priority;

        if((curMinPrio < newPrio || (newPrio == (resp::ConcurrencyManager::SYSC_PRIO_MAX + 1) && th->attr->deadline < minPriProc->runThread->attr->deadline)) && (minPriProc->runThread->attr == NULL || minPriProc->runThread->attr->preemptive)){
            //Lets deschedule the old thread and schedule the new one.
            this->readyQueue[curMinPrio].push_back(minPriProc->runThread);
            if(curMinPrio == resp::ConcurrencyManager::SYSC_PRIO_MAX + 1)
                std::sort(this->readyQueue[curMinPrio].begin(), this->readyQueue[curMinPrio].end(), deadlineSort);
            int thId = minPriProc->deSchedule();
            this->existingThreads[thId]->status = ThreadEmu::READY;
            minPriProc->schedule(th);
            return true;
        }
    }
    else{
        std::cerr << "the thread being created has a NULL attribute???" << std::endl;
    }

    return false;
}

///This is the main function implementing the scheduling policy. In order to change the policy, this
///is the function which should be changed.
ThreadEmu * resp::ConcurrencyManager::findReadyThread(){
    //I get the thread of the first queue which is not empty
    wait(resp::ConcurrencyManager::schedChooseLatency);

    ThreadEmu * foundThread = NULL;
    int j = -1;
    for(int i = resp::ConcurrencyManager::SYSC_PRIO_MAX + 1; i >= 0; i--){
        if(this->readyQueue[i].size() > 0){
            foundThread = this->readyQueue[i].front();
            this->readyQueue[i].pop_front();
            j = i;
            break;
        }
    }
    //I have found no thread ready to be scheduled; I check that this is not a deadlock
    //condition (it is a deadlock condition if in the system we have threads and none
    //of them is ready or running)
    if(foundThread == NULL){
        bool foundRun = false;
        std::vector<ThreadEmu *> foundWait;
        std::map<int, ThreadEmu *>::const_iterator thIter, thEnd;
        for(thIter = existingThreads.begin(), thEnd = existingThreads.end(); thIter != thEnd; thIter++){
            if(thIter->second->status == ThreadEmu::RUNNING)
                foundRun = true;
            else if(thIter->second->status == ThreadEmu::WAITING)
                foundWait.push_back(thIter->second);
        }
        if(foundWait.size() > 0 && !foundRun){
            std::stringstream oss;
            for(unsigned int j = 0; j < foundWait.size(); j++)
                oss << foundWait[j]->id << " ";
            THROW_EXCEPTION("Deadlock: there are no ready threads in the system; waiting threads: " << oss.str());
        }
    }

    return foundThread;
}

///*******************************************************************
/// Initialization functions
///*******************************************************************
///Initializes the mutexes used to guarantee mutual exclusion access to
///reentrant routines
void resp::ConcurrencyManager::initReentrantEmulation(){
    this->mallocMutex = this->createMutex();
    this->sfpMutex = this->createMutex();
    this->sinitMutex = this->createMutex();
    this->fpMutex = this->createMutex();
}

///*******************************************************************
/// Here are some functions for computing statistics on the
/// execution
///*******************************************************************
void resp::ConcurrencyManager::printThreadTraces(){
    THROW_EXCEPTION(__PRETTY_FUNCTION__ << " not yet implemented");
}
std::vector<std::string> resp::ConcurrencyManager::getThreadTraces(){
    std::vector<std::string> traces;
    THROW_EXCEPTION(__PRETTY_FUNCTION__ << " not yet implemented");
    return traces;
}
std::map<int, double> resp::ConcurrencyManager::getAverageLoads(){
    std::map<int, double> avgLoad;
    THROW_EXCEPTION(__PRETTY_FUNCTION__ << " not yet implemented");
    return avgLoad;
}
void resp::ConcurrencyManager::printIdleTraces(){
    THROW_EXCEPTION(__PRETTY_FUNCTION__ << " not yet implemented");
}
std::vector<std::string> resp::ConcurrencyManager::getIdleTraces(){
    std::vector<std::string> idleTrace;
    THROW_EXCEPTION(__PRETTY_FUNCTION__ << " not yet implemented");
    return idleTrace;
}
int resp::ConcurrencyManager::getDeadlineMisses(){
    THROW_EXCEPTION(__PRETTY_FUNCTION__ << " not yet implemented");
    return 0;
}
int resp::ConcurrencyManager::getDeadlineOk(){
    THROW_EXCEPTION(__PRETTY_FUNCTION__ << " not yet implemented");
    return 0;
}

///*******************************************************************
/// Finally we have the real core of the concurrency manager,
/// it contains the functions emulating the functionalities of
/// the emulated concurrency related primitives
///*******************************************************************
///*********** Thread related routines *******************

int resp::ConcurrencyManager::createThread(unsigned int procId, unsigned int threadFun, unsigned int args, int attr){
    BFDWrapper &bfdFE = BFDWrapper::getInstance(this->execName);

    //I have to create the thread handle and add it to the list of
    //existing threads; in case there is an available processor,
    //I also start the execution of the thread on it; note that all the
    //data structures touched here are also used during scheduling,
    //so synchronization must be enforced with respect to those structures
    std::cout << __PRETTY_FUNCTION__ << std::endl;

    std::map<unsigned int, Processor<unsigned int>* >::iterator curProcIter = this->managedProc.find(procId);
    if(curProcIter == this->managedProc.end())
        THROW_EXCEPTION("Processor with ID = " << procId << " not found among the registered processors");
    Processor<unsigned int> & curProc = *(curProcIter->second);

    //Lets determine the lock of the schedule
    this->schedLock.lock();

    //First of all lets determine the stack size
    unsigned int curStackBase = 0;
    unsigned int curStackSize = 0;
    std::map<int, AttributeEmu *>::iterator currentAttrIter = this->existingAttr.find(attr);
    if(currentAttrIter == this->existingAttr.end()){
        curStackSize = resp::ConcurrencyManager::threadStackSize;
    }
    else{
        curStackSize = existingAttr[attr]->stackSize;
    }
    //I have to determine if there is space for the stack among two already existing ones
    //or if I have to position myself at the end of the stack
    std::map<unsigned int, unsigned int>::const_iterator stacksIter, stacksNext, stacksEnd;
    for(stacksIter = stacksNext = this->stacks.begin() ; stacksIter != stacks.end() ; stacksIter++){
        if( (++stacksNext) == stacks.end() ) break;
        if((stacksIter->first - stacksIter->second - stacksNext->first) > curStackSize){
            curStackBase = stacksIter->first - stacksIter->second;
            stacks[curStackBase] = curStackSize;
            break;
        }
    } 
    if(curStackBase == 0){
        //I haven't found any suitable stack, I have to position on the top
        //of the last stack (lowest address available)
        curStackBase = stacksIter->first - stacksIter->second;
        stacks[curStackBase] = curStackSize;
    }
    //Now I check that there is no collision with the executable code (actually we should check that
    //there is not collision with the heap, but it is difficult to get a reference to the current
    //stack)
    if(this->managedProc.empty()){
        THROW_EXCEPTION("Trying to create a thread while there is not processor interfacee registered");
    }
    unsigned int codeLimit = this->managedProc.begin()->second->processorInstance.getCodeLimit();
    if(curStackBase < codeLimit)
        THROW_EXCEPTION("Unable to allocate " << curStackSize << " bytes for the stack of thread " << existingThreads.size() << ": no more space in memory");

    //Actual thread creation
    int createdThId = this->existingThreads.size();
    ThreadEmu *th = NULL;
    AttributeEmu *curAttr = NULL;
    std::string routineName = bfdFE.symbolAt(threadFun);
    //Now I have to allocate the space for the thread local storage: I use the thread's stack.
    //I also have to copy the static initialization of the TLS into that location
    unsigned int curTlsAddress = 0;
    if(resp::ConcurrencyManager::tlsSize > 0){
        if(resp::ConcurrencyManager::tlsSize > curStackSize){
            THROW_EXCEPTION("Unable to allocate a TLS of size " << resp::ConcurrencyManager::tlsSize << " for thread " << threadFun << " since it is bigger than the stack size " << curStackSize);
        }
        curTlsAddress = curStackBase + curStackSize - 4;
        //Now I copy the content of the TLS; in order to do this
        //I use the memory as seen by the current processor
        for(int i = 0; i < resp::ConcurrencyManager::tlsSize; i++){
            curProc.processorInstance.writeCharMem(curTlsAddress + i, resp::ConcurrencyManager::tlsData[i]);
        }
    }
    if(currentAttrIter == this->existingAttr.end()){
        //I see if default properties were specified for this thread in ReSP's
        //command line
        if(resp::ConcurrencyManager::defThreadInfo.find(routineName) == resp::ConcurrencyManager::defThreadInfo.end()){
            th = new ThreadEmu(createdThId, threadFun, args, curStackBase, curTlsAddress, &resp::ConcurrencyManager::defaultAttr);
        }
        else{
            curAttr = this->existingAttr[this->createThreadAttr()];
            curAttr->preemptive = resp::ConcurrencyManager::defThreadInfo[routineName].preemptive;
            curAttr->schedPolicy = resp::ConcurrencyManager::defThreadInfo[routineName].schedPolicy;
            curAttr->priority = resp::ConcurrencyManager::defThreadInfo[routineName].priority;
            curAttr->deadline = resp::ConcurrencyManager::defThreadInfo[routineName].deadline;
            th = new ThreadEmu(createdThId, threadFun, args, curStackBase, curTlsAddress, curAttr);
        }
    }
    else{
        curAttr = currentAttrIter->second;
        //Again I have to check for default properties: in case there are, they even ovverride current
        //thread attributes
        if(resp::ConcurrencyManager::defThreadInfo.find(routineName) != resp::ConcurrencyManager::defThreadInfo.end()){
            curAttr->preemptive = resp::ConcurrencyManager::defThreadInfo[routineName].preemptive;
            curAttr->schedPolicy = resp::ConcurrencyManager::defThreadInfo[routineName].schedPolicy;
            curAttr->priority = resp::ConcurrencyManager::defThreadInfo[routineName].priority;
            curAttr->deadline = resp::ConcurrencyManager::defThreadInfo[routineName].deadline;
        }
        th = new ThreadEmu(createdThId, threadFun, args, curStackBase, curTlsAddress, curAttr);
    }
    this->existingThreads[createdThId] = th;

    //Since we want the system to respond as fast as possible, we try
    //to immediately schedule the thread in case an idle processor exists
    #ifndef NDEBUG
    std::cerr << "Trying to schedule thread " << createdThId << " on a free processor" << std::endl;
    #endif
    if(!this->scheduleFreeProcessor(th)){
        //Ok, I haven't found any free processor; I try to determine
        //if there is a processor running a lower priority thread and, in case,
        //I replace it
        #ifndef NDEBUG
        std::cerr << "Trying to schedule thread " << createdThId << " through preemption" << std::endl;
        #endif
        if(!this->preemptLowerPrio(th)){
            #ifndef NDEBUG
            std::cerr << "Unable to preempt any thread, adding " << createdThId << " to the ready queue" << std::endl;
            #endif
            //Ok, nothing else I can do: lets add the thread to the correct
            //ready queue
            int curPrio = 0;
            if(curAttr != NULL){
                if(curAttr->schedPolicy == resp::ConcurrencyManager::SYSC_SCHED_EDF)
                    curPrio = resp::ConcurrencyManager::SYSC_PRIO_MAX + 1;
                else
                    curPrio = curAttr->priority;
            }
            #ifndef NDEBUG
            if(curPrio < resp::ConcurrencyManager::SYSC_PRIO_MIN || curPrio > resp::ConcurrencyManager::SYSC_PRIO_MAX + 1){
                THROW_EXCEPTION("Non valid priority value " << curPrio);
            }
            #endif
            this->readyQueue[curPrio].push_back(th);
            if(curPrio == resp::ConcurrencyManager::SYSC_PRIO_MAX + 1)
                std::sort(this->readyQueue[curPrio].begin(), this->readyQueue[curPrio].end(), deadlineSort);
        }
    }

    #ifndef NDEBUG
    std::cerr << "Created thread Id = " << existingThreads.size() - 1 << \
                " stack size = " << curStackSize/1024 << "KB" << " stack base " << \
                std::showbase << std::hex << curStackBase << std::dec << " status " << \
                th->status << " address " << std::hex << std::showbase << threadFun <<  std::endl;
    #endif

    this->schedLock.unlock();

    return createdThId;
}
void resp::ConcurrencyManager::exitThread(unsigned int procId, unsigned int retVal){
    //I have to see if there are available threads and to schedule on the processor which is being left empty.
    //I also check that, in case there are none, that there are no threads waiting,  otherwise there is
    //a deadlock
    this->schedLock.lock();

    std::map<unsigned int, Processor<unsigned int>* >::iterator curProcIter = this->managedProc.find(procId);
    if(curProcIter == this->managedProc.end())
        THROW_EXCEPTION("Processor with ID = " << procId << " not found among the registered processors");
    Processor<unsigned int> & curProc = *(curProcIter->second);

    int th = curProc.deSchedule();
    // Update statistics for the just ended thread
/*    if(this->existingThreads[th]->attr->schedPolicy == resp::SYSC_SCHED_EDF){
        if(sc_time_stamp().to_double() > this->existingThreads[th]->attr->deadline*1e3 )
            deadlineMisses++;
        else
            deadlineOk++;
    }*/

    this->stacks.erase(existingThreads[th]->stackBase);
    this->existingThreads[th]->status = ThreadEmu::DEAD;

    if(!this->existingThreads[th]->isIRQ){
        this->existingThreads[th]->retVal = retVal;
        //Finally I have to awake all the threads which were wating on a join on this thread
        std::vector<ThreadEmu *>::iterator joinIter, joinEnd;
        for(joinIter = this->existingThreads[th]->joining.begin(), joinEnd = this->existingThreads[th]->joining.end(); joinIter != joinEnd; joinIter++){
            (*joinIter)->joinedRetVal = retVal;
            (*joinIter)->status = ThreadEmu::READY;
            //I add the thread to the queue of ready threads
            int curPrio = 0;
            if((*joinIter)->attr != NULL){
                if((*joinIter)->attr->schedPolicy == resp::ConcurrencyManager::SYSC_SCHED_EDF)
                    curPrio = resp::ConcurrencyManager::SYSC_PRIO_MAX + 1;
                else
                    curPrio = (*joinIter)->attr->priority;
            }
            this->readyQueue[curPrio].push_back(*joinIter);
            if(curPrio == resp::ConcurrencyManager::SYSC_PRIO_MAX + 1)
                std::sort(this->readyQueue[curPrio].begin(), this->readyQueue[curPrio].end(), deadlineSort);
        }
        this->existingThreads[th]->joining.clear();
    }
    else{
        //It is an IRQ thread, so I can freely deallocate it and the corresponding attribute
        this->deleteThreadAttr(this->existingThreads[th]->attr->id);
    }

    //Now I have to schedule a new thread on the just freed processor
    ThreadEmu * readTh = this->findReadyThread();
    if(readTh != NULL){
        curProc.schedule(readTh);
    }

    this->schedLock.unlock();
}

bool resp::ConcurrencyManager::cancelThread(int threadId){
    THROW_EXCEPTION("Error function " << __PRETTY_FUNCTION__ << " not yet implemented");
    return false;
}

int resp::ConcurrencyManager::createThreadAttr(){
    int newAttrId = 0;
    if(this->existingAttr.size() > 0)
        newAttrId = this->existingAttr.rbegin()->first + 1;
    this->existingAttr[newAttrId] = new AttributeEmu();
    this->existingAttr[newAttrId]->id = newAttrId;
    return newAttrId;
}
void resp::ConcurrencyManager::deleteThreadAttr(int attr){
    if(this->existingAttr.find(attr) == this->existingAttr.end()){
        THROW_EXCEPTION("Cannot destroy attribute " << attr << " unable to find it");
    }
}

void resp::ConcurrencyManager::setStackSize(int attr, int stackSize){
    std::map<int, AttributeEmu *>::iterator foundAttr = this->existingAttr.find(attr);
    if(foundAttr == this->existingAttr.end()){
        THROW_EXCEPTION("Cannot set stack for attribute " << attr << " unable to find it");
    }

    #ifndef NDEBUG
    std::cerr << "Setting stack " << stackSize << " for attribute " << attr << std::endl;
    #endif

    foundAttr->second->stackSize = stackSize;
}
unsigned int resp::ConcurrencyManager::getStackSize(int attr) const{
    std::map<int, AttributeEmu *>::const_iterator foundAttr = this->existingAttr.find(attr);
    if(foundAttr == this->existingAttr.end()){
        THROW_EXCEPTION("Cannot get stack for attribute " << attr << " unable to find it");
    }

    return foundAttr->second->stackSize;
}

void resp::ConcurrencyManager::setPreemptive(int attr, int isPreemptive){
    std::map<int, AttributeEmu *>::iterator foundAttr = this->existingAttr.find(attr);
    if(foundAttr == this->existingAttr.end()){
        THROW_EXCEPTION("Cannot set preemptive status for attribute " << attr << " unable to find it");
    }

    #ifndef NDEBUG
    std::cerr << "Setting preemptive status " << isPreemptive << " for attribute " << attr << std::endl;
    #endif

    foundAttr->second->preemptive = (isPreemptive == resp::ConcurrencyManager::SYSC_PREEMPTIVE);
}
int resp::ConcurrencyManager::getPreemptive(int attr) const{
    std::map<int, AttributeEmu *>::const_iterator foundAttr = this->existingAttr.find(attr);
    if(foundAttr == this->existingAttr.end()){
        THROW_EXCEPTION("Cannot get stack for attribute " << attr << " unable to find it");
    }

    return foundAttr->second->preemptive ? resp::ConcurrencyManager::SYSC_PREEMPTIVE : resp::ConcurrencyManager::SYSC_NON_PREEMPTIVE;
}

void resp::ConcurrencyManager::setSchedDeadline(int attr, unsigned int deadline){
    std::map<int, AttributeEmu *>::iterator foundAttr = this->existingAttr.find(attr);
    if(foundAttr == this->existingAttr.end()){
        THROW_EXCEPTION("Cannot set scheduling deadline for attribute " << attr << " unable to find it");
    }

    #ifndef NDEBUG
    std::cerr << "Setting scheduling deadline " << deadline << " for attribute " << attr << std::endl;
    #endif

    foundAttr->second->deadline = deadline;
}
unsigned int resp::ConcurrencyManager::getSchedDeadline(int attr) const{
    std::map<int, AttributeEmu *>::const_iterator foundAttr = this->existingAttr.find(attr);
    if(foundAttr == this->existingAttr.end()){
        THROW_EXCEPTION("Cannot get scheduling deadline for attribute " << attr << " unable to find it");
    }

    return foundAttr->second->deadline;
}

void resp::ConcurrencyManager::setSchedPrio(int attr, int priority){
    std::map<int, AttributeEmu *>::iterator foundAttr = this->existingAttr.find(attr);
    if(foundAttr == this->existingAttr.end()){
        THROW_EXCEPTION("Cannot set priority for attribute " << attr << " unable to find it");
    }

    #ifndef NDEBUG
    std::cerr << "Setting priority " << priority << " for attribute " << attr << std::endl;
    #endif

    foundAttr->second->priority = priority;
}
int resp::ConcurrencyManager::getSchedPrio(int attr) const{
    std::map<int, AttributeEmu *>::const_iterator foundAttr = this->existingAttr.find(attr);
    if(foundAttr == this->existingAttr.end()){
        THROW_EXCEPTION("Cannot get priority for attribute " << attr << " unable to find it");
    }

    return foundAttr->second->priority;
}

void resp::ConcurrencyManager::setSchedPolicy(int attr, int policy){
    std::map<int, AttributeEmu *>::iterator foundAttr = this->existingAttr.find(attr);
    if(foundAttr == this->existingAttr.end()){
        THROW_EXCEPTION("Cannot set scheduling policy for attribute " << attr << " unable to find it");
    }

    #ifndef NDEBUG
    std::cerr << "Setting scheduling policy " << policy << " for attribute " << attr << std::endl;
    #endif

    foundAttr->second->schedPolicy = policy;
}
int resp::ConcurrencyManager::getSchedPolicy(int attr) const{
    std::map<int, AttributeEmu *>::const_iterator foundAttr = this->existingAttr.find(attr);
    if(foundAttr == this->existingAttr.end()){
        THROW_EXCEPTION("Cannot get scheduling policy for attribute " << attr << " unable to find it");
    }

    return foundAttr->second->schedPolicy;
}

int resp::ConcurrencyManager::getThreadSchePolicy(int threadId) const{
    std::map<int, ThreadEmu *>::const_iterator foundThread = this->existingThreads.find(threadId);
    if(foundThread == this->existingThreads.end()){
        THROW_EXCEPTION("Cannot get scheduler policy for thread " << threadId << " unable to find it");
    }
    if(foundThread->second->attr != NULL){
        return foundThread->second->attr->schedPolicy;
    }
    else{
        return resp::ConcurrencyManager::SYSC_SCHED_RR;
    }
}
void resp::ConcurrencyManager::setThreadSchePolicy(int threadId, int policy){
    std::map<int, ThreadEmu *>::iterator foundThread = this->existingThreads.find(threadId);
    if(foundThread == this->existingThreads.end()){
        THROW_EXCEPTION("Cannot set scheduler policy for thread " << threadId << " unable to find it");
    }
    if(foundThread->second->attr != NULL){
        foundThread->second->attr->schedPolicy = policy;
    }
    else{
        // I have to create a new attribute and then I can set the scheduling
        // policy
        int newAttrId = 0;
        if(this->existingAttr.size() > 0)
            newAttrId = this->existingAttr.rbegin()->first + 1;
        AttributeEmu * newAttr = new AttributeEmu();
        foundThread->second->attr = newAttr;
        newAttr->schedPolicy = policy;
        this->existingAttr[newAttrId] = newAttr;
    }
}

int resp::ConcurrencyManager::getThreadSchedPrio(int threadId) const{
    std::map<int, ThreadEmu *>::const_iterator foundThread = this->existingThreads.find(threadId);
    if(foundThread == this->existingThreads.end()){
        THROW_EXCEPTION("Cannot get scheduler priority for thread " << threadId << " unable to find it");
    }
    if(foundThread->second->attr != NULL){
        return foundThread->second->attr->priority;
    }
    else{
        return resp::ConcurrencyManager::SYSC_PRIO_MIN;
    }
}
void resp::ConcurrencyManager::setThreadSchedPrio(int threadId, int priority){
    std::map<int, ThreadEmu *>::iterator foundThread = this->existingThreads.find(threadId);
    if(foundThread == this->existingThreads.end()){
        THROW_EXCEPTION("Cannot set scheduler priority for thread " << threadId << " unable to find it");
    }
    if(foundThread->second->attr != NULL){
        foundThread->second->attr->priority = priority;
    }
    else{
        // I have to create a new attribute and then I can set the scheduling
        // priority
        int newAttrId = 0;
        if(this->existingAttr.size() > 0)
            newAttrId = this->existingAttr.rbegin()->first + 1;
        AttributeEmu * newAttr = new AttributeEmu();
        foundThread->second->attr = newAttr;
        newAttr->priority = priority;
        this->existingAttr[newAttrId] = newAttr;
    }
}

unsigned int resp::ConcurrencyManager::getThreadSchedDeadline(int threadId) const{
    std::map<int, ThreadEmu *>::const_iterator foundThread = this->existingThreads.find(threadId);
    if(foundThread == this->existingThreads.end()){
        THROW_EXCEPTION("Cannot get deadline for thread " << threadId << " unable to find it");
    }
    if(foundThread->second->attr != NULL){
        return foundThread->second->attr->deadline;
    }
    else{
        return 0;
    }
}
void resp::ConcurrencyManager::setThreadSchedDeadline(int threadId, unsigned int deadline){
    std::map<int, ThreadEmu *>::iterator foundThread = this->existingThreads.find(threadId);
    if(foundThread == this->existingThreads.end()){
        THROW_EXCEPTION("Cannot set scheduler deadline for thread " << threadId << " unable to find it");
    }
    if(foundThread->second->attr != NULL){
        foundThread->second->attr->deadline = deadline;
    }
    else{
        // I have to create a new attribute and then I can set the scheduling
        // deadline
        int newAttrId = 0;
        if(this->existingAttr.size() > 0)
            newAttrId = this->existingAttr.rbegin()->first + 1;
        AttributeEmu * newAttr = new AttributeEmu();
        foundThread->second->attr = newAttr;
        newAttr->deadline = deadline;
        this->existingAttr[newAttrId] = newAttr;
    }
}

int resp::ConcurrencyManager::getThreadId(unsigned int procId) const{
    std::map<unsigned int, Processor<unsigned int>* >::const_iterator curProcIter = this->managedProc.find(procId);
    if(curProcIter == this->managedProc.end())
        THROW_EXCEPTION("Processor with ID = " << procId << " not found among the registered processors");

    return curProcIter->second->runThread->id;
}

int resp::ConcurrencyManager::createKey(){
    //TODO: per key destructors are not yet considered!!!!!!!
    this->keys++;
    return this->keys;
}
void resp::ConcurrencyManager::setSpecific(unsigned int procId, int key, unsigned int memArea){
    if(key > this->keys){
        THROW_EXCEPTION("Key " << key << " not existing");
    }

    std::map<unsigned int, Processor<unsigned int>* >::const_iterator curProcIter = this->managedProc.find(procId);
    if(curProcIter == this->managedProc.end())
        THROW_EXCEPTION("Processor with ID = " << procId << " not found among the registered processors");
    std::pair<int,  ThreadEmu *> keyId(key, curProcIter->second->runThread);
    this->threadSpecific[keyId] = memArea;
}
unsigned int resp::ConcurrencyManager::getSpecific(unsigned int procId, int key) const{
    if(key > this->keys){
        THROW_EXCEPTION("Key " << key << " not existing");
    }

    std::map<unsigned int, Processor<unsigned int>* >::const_iterator curProcIter = this->managedProc.find(procId);
    if(curProcIter == this->managedProc.end())
        THROW_EXCEPTION("Processor with ID = " << procId << " not found among the registered processors");

    std::pair<int,  ThreadEmu *> keyId(key, curProcIter->second->runThread);
    std::map<std::pair<int, ThreadEmu *>, unsigned int>::const_iterator foundKey = this->threadSpecific.find(keyId);
    if(foundKey == this->threadSpecific.end()){
        //If no key is found for the current thread NULL is returned
        return 0;
    }
    else{
        return foundKey->second;
    }
}

void resp::ConcurrencyManager::join(int thId, unsigned int procId, unsigned int retValAddr){
    //I have to check that thId has finished; in case I immediately return,  otherwise
    //I have to deschedule myself
    this->schedLock.lock();

    std::map<unsigned int, Processor<unsigned int>* >::iterator curProcIter = this->managedProc.find(procId);
    if(curProcIter == this->managedProc.end())
        THROW_EXCEPTION("Processor with ID = " << procId << " not found among the registered processors");

    if(this->existingThreads[thId]->status == ThreadEmu::DEAD){
        //There is nothing to do only set the thread return value
        if(retValAddr != 0){
            curProcIter->second->processorInstance.writeMem(retValAddr, this->existingThreads[thId]->retVal);
        }
    }
    else{
        //I have to deschedule this thread
        int curThread = curProcIter->second->deSchedule();
        //Now I have to reschedule the other threads in the system
        ThreadEmu * readTh = this->findReadyThread();
        if(readTh != NULL){
            curProcIter->second->schedule(readTh);
        }
        this->existingThreads[thId]->joining.push_back(this->existingThreads[curThread]);
        this->existingThreads[curThread]->joiningRetValAddr = retValAddr;
    }

    this->schedLock.unlock();
}

void resp::ConcurrencyManager::joinAll(unsigned int procId) {
    this->schedLock.lock();

    std::map<unsigned int, Processor<unsigned int>* >::iterator curProcIter = this->managedProc.find(procId);
    if(curProcIter == this->managedProc.end())
        THROW_EXCEPTION("Processor with ID = " << procId << " not found among the registered processors");
  
    // Deschedule the current thread
    int curThread = curProcIter->second->deSchedule();
    ThreadEmu * readTh = this->findReadyThread();
    if(readTh != NULL){
        curProcIter->second->schedule(readTh);
    }

    // Join with all other existing threads
    std::map<int, ThreadEmu *>::const_iterator thIter, thEnd;
    for(thIter = existingThreads.begin(), thEnd = existingThreads.end(); thIter != thEnd; thIter++){
        if(thIter->second->status != ThreadEmu::DEAD && thIter->first != curThread) {
            this->existingThreads[curThread]->joining.push_back(this->existingThreads[curThread]);
        }
    }

    this->schedLock.unlock();
}

std::pair<unsigned int, unsigned int> resp::ConcurrencyManager::readTLS(unsigned int procId) const{
    //I simply have to determine the TLS and return it; note that, at thread
    //creation, I have located the TLS in the upper part of the stack (lowest
    //addresses of the stack)
    if(resp::ConcurrencyManager::tlsSize == 0)
        THROW_EXCEPTION("Error, the current executable file does not have any TLS declared");

    std::map<unsigned int, Processor<unsigned int>* >::const_iterator curProcIter = this->managedProc.find(procId);
    if(curProcIter == this->managedProc.end())
        THROW_EXCEPTION("Processor with ID = " << procId << " not found among the registered processors");

    //The first element of the pair is the begin address of the TLS, the second element is the end address
    return std::pair<unsigned int, unsigned int>(curProcIter->second->runThread->stackBase,
                                curProcIter->second->runThread->stackBase - resp::ConcurrencyManager::tlsSize);
}
void resp::ConcurrencyManager::idleLoop(unsigned int procId){
    std::map<unsigned int, Processor<unsigned int>* >::iterator curProcIter = this->managedProc.find(procId);
    if(curProcIter == this->managedProc.end())
        THROW_EXCEPTION("Processor with ID = " << procId << " not found among the registered processors");

    wait(curProcIter->second->idleEvent);
}

void resp::ConcurrencyManager::pushCleanupHandler(unsigned int procId, unsigned int routineAddress, unsigned int arg){
    THROW_EXCEPTION("Error function " << __PRETTY_FUNCTION__ << " not yet implemented");
}
void resp::ConcurrencyManager::popCleanupHandler(unsigned int procId, bool execute){
    THROW_EXCEPTION("Error function " << __PRETTY_FUNCTION__ << " not yet implemented");
}
void resp::ConcurrencyManager::execCleanupHandlerTop(unsigned int procId){
    THROW_EXCEPTION("Error function " << __PRETTY_FUNCTION__ << " not yet implemented");
}

///*********** Mutex related routines *******************
///Destroys a previously allocated mutex, exception if the mutex does not exist
void resp::ConcurrencyManager::destroyMutex(unsigned int procId, int mutex){
/*    if(existingMutex.find(mutex) == existingMutex.end()){
        THROW_EXCEPTION("Cannot destroy mutex " << mutex << " unable to find it");
    }

    delete existingMutex[mutex];
    existingMutex.erase(mutex);*/
}
int resp::ConcurrencyManager::createMutex(){
//     int newMutId = 0;
//     if(existingMutex.size() > 0){
//         std::map<int, MutexEmu *>::iterator mutBeg, mutEnd;
//         for(mutBeg = existingMutex.begin(), mutEnd = existingMutex.end(); mutBeg != mutEnd; mutBeg++){
//             std::map<int, MutexEmu *>::iterator next = mutBeg;
//             next++;
//             if(next == mutEnd){
//                 newMutId = mutBeg->first + 1;
//                 break;
//             }
//             else if(mutBeg->first + 1 < next->first){
//                 newMutId = mutBeg->first + 1;
//                 break;
//             }
//         }
//     }
//     existingMutex[newMutId] = new MutexEmu();
//     #ifndef NDEBUG
//     if(procId != (unsigned int)-1)
//         std::cerr << "Creating mutex " << newMutId << " Thread " << findProcessor(procId).runThread->id << std::endl;
//     #endif
//
//     return newMutId;
}
///Locks the mutex, deschedules the thread if the mutex is busy; in case
///nonbl == true, the function behaves as a trylock
bool resp::ConcurrencyManager::lockMutex(int mutex, unsigned int procId, bool nonbl){
//     while(schedulingLockBusy)
//         wait(schedulingEvent);
//     schedulingLockBusy = true;
//
//     Processor &curProc = findProcessor(procId);
//
//     if(curProc.runThread == NULL)
//         THROW_EXCEPTION("Mutex " << mutex << ": Current processor " << procId << " is not running any thread");
//
//     if(existingMutex.find(mutex) == existingMutex.end()){
//         THROW_EXCEPTION(" Processor " << procId << " Thread " << curProc.runThread->id << " Cannot lock mutex " << mutex << " unable to find it");
//     }
//
//     if(existingMutex[mutex]->owner == curProc.runThread){
//         //Reentrant mutex, I have to increment the recursive index
//         existingMutex[mutex]->recursiveIndex++;
//         schedulingLockBusy = false;
//         schedulingEvent.notify();
//         return true;
//     }
//     if(existingMutex[mutex]->status == MutexEmu::FREE){
//         existingMutex[mutex]->status = MutexEmu::LOCKED;
//         existingMutex[mutex]->owner = curProc.runThread;
//         #ifndef NDEBUG
//         if(existingMutex[mutex]->recursiveIndex != 0)
//             THROW_EXCEPTION("Mutex " << mutex << " is free, but it has recursive index " << existingMutex[mutex]->recursiveIndex);
//         std::cerr << "Thread " << curProc.runThread->id << " locking free mutex " << mutex << std::endl;
//         #endif
//         schedulingLockBusy = false;
//         schedulingEvent.notify();
//         return true;
//     }
//     if(nonbl){
//         schedulingLockBusy = false;
//         schedulingEvent.notify();
//         return false;
//     }
//
//     #ifndef NDEBUG
//     if(existingMutex[mutex]->owner->status == ThreadEmu::DEAD)
//         THROW_EXCEPTION("Thread " << existingMutex[mutex]->owner->id << " died while being the owner of mutex " << mutex);
//     std::cerr << "Thread " << curProc.runThread->id << " waiting on mutex " << mutex << std::endl;
//     #endif
//     //Finally here I have to deschedule the current thread since the mutex is busy
//     int th = curProc.deSchedule();
//     existingMutex[mutex]->waitingTh.push_back(existingThreads[th]);
//     //Now I get the first ready thread and I schedule it on the processor
//     ThreadEmu * readTh = findReadyThread();
//     if(readTh != NULL){
//         curProc.schedule(readTh);
//     }
//     schedulingLockBusy = false;
//     schedulingEvent.notify();
//
//     return false;
}
///Releases the lock on the mutex
int resp::ConcurrencyManager::unLockMutex(int mutex, unsigned int procId){
//     while(schedulingLockBusy)
//         wait(schedulingEvent);
//     schedulingLockBusy = true;
//
//     if(existingMutex.find(mutex) == existingMutex.end()){
//         THROW_EXCEPTION("Cannot free mutex " << mutex << " unable to find it");
//     }
//
//     //If the mutex is not locked I simply return
//     if(existingMutex[mutex]->owner == NULL){
//         #ifndef NDEBUG
//         //I check that the status of the mutex is coherent
//         if(existingMutex[mutex]->status != MutexEmu::FREE || existingMutex[mutex]->waitingTh.size() > 0){
//             THROW_EXCEPTION("The mutex " << mutex << " is free, but its status is not coherent");
//         }
//         #endif
//
//         schedulingLockBusy = false;
//         schedulingEvent.notify();
//
//         return -1;
//     }
//     //I check that I'm the owner of the mutex, otherwise I raise an exception
//     Processor &curProc = findProcessor(procId);
//     #ifndef NDEBUG
//     if(curProc.runThread == NULL){
//         THROW_EXCEPTION("Free mutex: the current processor " << curProc.procIf->getId() << " is executing a NULL thread??");
//     }
//     #endif
//     if(existingMutex[mutex]->owner != curProc.runThread){
//         THROW_EXCEPTION("Cannot free mutex " << mutex << " the owner " << existingMutex[mutex]->owner->id << " is not the current thread " << curProc.runThread->id);
//     }
//     //Now I check to see if I'm in a recursive situation
//     if(existingMutex[mutex]->recursiveIndex > 0){
//         #ifndef NDEBUG
//         std::cerr << "Exiting recursive mutex, current index " << existingMutex[mutex]->recursiveIndex << std::endl;
//         #endif
//         existingMutex[mutex]->recursiveIndex--;
//
//         schedulingLockBusy = false;
//         schedulingEvent.notify();
//
//         return -1;
//     }
//     //Now I have to awake the first thread which is waiting on the mutex
//     if(existingMutex[mutex]->waitingTh.size() > 0){
//         ThreadEmu * toAwakeTh = existingMutex[mutex]->waitingTh.front();
//         existingMutex[mutex]->waitingTh.pop_front();
//         toAwakeTh->status = ThreadEmu::READY;
//         addReadyThread(toAwakeTh);
//         existingMutex[mutex]->owner = toAwakeTh;
//         #ifndef NDEBUG
//         std::cerr << "Thread " << curProc.runThread->id << " giving mutex " << mutex << " to thread " << toAwakeTh->id << std::endl;
//         #endif
//
//         schedulingLockBusy = false;
//         schedulingEvent.notify();
//
//         return toAwakeTh->id;
//     }
//     else{
//         #ifndef NDEBUG
//         std::cerr << "Thread " << curProc.runThread->id << " releasing mutex " << mutex << std::endl;
//         #endif
//         //Finally I can clear the mutex status
//         existingMutex[mutex]->status = MutexEmu::FREE;
//         existingMutex[mutex]->owner = NULL;
//         existingMutex[mutex]->waitingTh.clear();
//
//         schedulingLockBusy = false;
//         schedulingEvent.notify();
//
//         return -1;
//     }
//     schedulingLockBusy = false;
//     schedulingEvent.notify();
//     return -1;
}

///*********** Semaphore related routines *******************
int resp::ConcurrencyManager::createSem(unsigned int procId, int initialValue){
//     int newSemId = 0;
//     if(existingSem.size() > 0){
//         std::map<int, SemaphoreEmu *>::iterator semBeg, semEnd;
//         for(semBeg = existingSem.begin(), semEnd = existingSem.end(); semBeg != semEnd; semBeg++){
//             std::map<int, SemaphoreEmu *>::iterator next = semBeg;
//             next++;
//             if(next == semEnd){
//                 newSemId = semBeg->first + 1;
//                 break;
//             }
//             else if(semBeg->first + 1 < next->first){
//                 newSemId = semBeg->first + 1;
//                 break;
//             }
//         }
//     }
//     existingSem[newSemId] = new SemaphoreEmu(initialValue);
//     #ifndef NDEBUG
//     std::cerr << "Creating semaphore " << newSemId << " Thread " << findProcessor(procId).runThread->id << std::endl;
//     #endif
//     return newSemId;
}
void resp::ConcurrencyManager::destroySem(unsigned int procId, int sem){
//     if(existingSem.find(sem) == existingSem.end()){
//         THROW_EXCEPTION("Cannot destroy semaphore " << sem << " unable to find it");
//     }
//     #ifndef NDEBUG
//     if(existingSem[sem]->waitingTh.size() > 0)
//         THROW_EXCEPTION("Cannot destroy semaphore " << sem << " there are waiting threads");
//     std::cerr << "Destroying semaphore " << sem << " Thread " << findProcessor(procId).runThread->id << std::endl;
//     #endif
//     delete existingSem[sem];
//     existingSem.erase(sem);
}
void resp::ConcurrencyManager::postSem(int sem, unsigned int procId){
//     while(schedulingLockBusy)
//         wait(schedulingEvent);
//     schedulingLockBusy = true;
//
//     //I simply have to increment the value of the semaphore; I then check if there are
//     //threads waiting on the semaphore and I awake them in case
//     if(existingSem.find(sem) == existingSem.end()){
//         THROW_EXCEPTION("Cannot post semaphore " << sem << " unable to find it");
//     }
//
//     #ifndef NDEBUG
//     if(existingSem[sem]->waitingTh.size() > 0 && existingSem[sem]->value > 0){
//         THROW_EXCEPTION("Semaphore " << sem << " has value " << existingSem[sem]->value << " but there are waiting threads");
//     }
//     #endif
//     if(existingSem[sem]->waitingTh.size() > 0){
//         ThreadEmu * toAwakeTh = existingSem[sem]->waitingTh.front();
//         existingSem[sem]->waitingTh.pop_front();
//         toAwakeTh->status = ThreadEmu::READY;
//         addReadyThread(toAwakeTh);
//         #ifndef NDEBUG
//         Processor &curProc = findProcessor(procId);
//         std::cerr << "Thread " << curProc.runThread->id << " giving semaphore " << sem << " to thread " << toAwakeTh->id << std::endl;
//         #endif
//     }
//     else
//         existingSem[sem]->value++;
//     schedulingLockBusy = false;
//     schedulingEvent.notify();
}
void resp::ConcurrencyManager::waitSem(int sem, unsigned int procId){
//     while(schedulingLockBusy)
//         wait(schedulingEvent);
//     schedulingLockBusy = true;
//
//     //If the value of the semaphore is 0 I wait,  otherwise the value gets
//     //decremented and I can return
//     if(existingSem.find(sem) == existingSem.end()){
//         THROW_EXCEPTION("Cannot post semaphore " << sem << " unable to find it");
//     }
//
//     if(existingSem[sem]->value > 0)
//         existingSem[sem]->value--;
//     else{
//         //Finally here I have to deschedule the current thread since the semaphore is empty
//         Processor &curProc = findProcessor(procId);
//         int th = curProc.deSchedule();
//         existingSem[sem]->waitingTh.push_back(existingThreads[th]);
//         #ifndef NDEBUG
//         std::cerr << "DeScheduling thread " << th << " because semaphore " << sem << " has value " << existingSem[sem]->value << std::endl;
//         #endif
//         //Now I get the first ready thread and I schedule it on the processor
//         ThreadEmu * readTh = findReadyThread();
//         if(readTh != NULL){
//             curProc.schedule(readTh);
//         }
//     }
//     schedulingLockBusy = false;
//     schedulingEvent.notify();
}

///*********** Condition Variable related routines *******************
int resp::ConcurrencyManager::createCond(unsigned int procId){
//     int newCondId = 0;
//     if(existingCond.size() > 0){
//         std::map<int, ConditionEmu *>::iterator condBeg, condEnd;
//         for(condBeg = existingCond.begin(), condEnd = existingCond.end(); condBeg != condEnd; condBeg++){
//             std::map<int, ConditionEmu *>::iterator next = condBeg;
//             next++;
//             if(next == condEnd){
//                 newCondId = condBeg->first + 1;
//                 break;
//             }
//             else if(condBeg->first + 1 < next->first){
//                 newCondId = condBeg->first + 1;
//                 break;
//             }
//         }
//     }
//
//     existingCond[newCondId] = new ConditionEmu();
//     return newCondId;
}
void resp::ConcurrencyManager::destroyCond(unsigned int procId, int cond){
//     if(existingCond.find(cond) == existingCond.end()){
//         THROW_EXCEPTION("Cannot destroy condition variable " << cond << ": unable to find it");
//     }
//
//     delete existingCond[cond];
//     existingCond.erase(cond);
}
void resp::ConcurrencyManager::signalCond(int cond, bool broadcast, unsigned int procId){
//     //I simply have to awake the threads waiting on this condition;
//     //note that when the thread is awaken, it has to acquire the mutex
//     //associated with the condition variable; in case it cannot it
//     //is not awaken, but simply moved in the queue of the
//     //ccoresponding mutex
//     #ifndef NDEBUG
//     std::cerr << "waiting for lock to Signal condition variable " << cond << std::endl;
//     #endif
//
//     while(schedulingLockBusy)
//         wait(schedulingEvent);
//     schedulingLockBusy = true;
//
//     #ifndef NDEBUG
//     std::cerr << "Signaling condition variable " << cond << std::endl;
//     #endif
//
//     if(existingCond.find(cond) == existingCond.end()){
//         THROW_EXCEPTION("Cannot signal condition variable " << cond << ": unable to find it");
//     }
//
//     if(existingCond[cond]->waitingTh.size() > 0){
//         MutexEmu * mutex = NULL;
//         if(existingCond[cond]->mutex == -1)
//             THROW_EXCEPTION("Mutex associated to conditional variable " << cond << " is equal to -1");
//         else{
//             #ifndef NDEBUG
//             if(existingMutex.find(existingCond[cond]->mutex) == existingMutex.end()){
//                 THROW_EXCEPTION("Cannot get mutex " << existingCond[cond]->mutex << " unable to find it in condition variable " << cond);
//             }
//             #endif
//             mutex = existingMutex[existingCond[cond]->mutex];
//         }
//         if(broadcast){
//             std::list<ThreadEmu *>::iterator thIter, thEnd;
//             thIter = existingCond[cond]->waitingTh.begin();
//             std::map<ThreadEmu *, std::pair<double, int> >::iterator timedIter = timedThreads.find(*thIter);
//             if(timedIter != timedThreads.end())
//                 timedThreads.erase(timedIter);
//             //Now I have to check if the mutex is free for the first thread and
//             //lock it in case
//             returnFromCond(*thIter, mutex);
//
//             for(thIter++, thEnd = existingCond[cond]->waitingTh.end(); thIter != thEnd; thIter++){
//                 //Of course the mutex is busy for all the other threads since
//                 //the first one has just locked it: I simply put those
//                 //threads in the waiting queue of the mutex
//                 mutex->waitingTh.push_back((*thIter));
//                 timedIter = timedThreads.find(*thIter);
//                 if(timedIter != timedThreads.end())
//                     timedThreads.erase(timedIter);
//             }
//             existingCond[cond]->waitingTh.clear();
//             existingCond[cond]->mutex = -1;
//         }
//         else{
//             ThreadEmu * toAwakeTh = existingCond[cond]->waitingTh.front();
//             existingCond[cond]->waitingTh.pop_front();
//             std::map<ThreadEmu *, std::pair<double, int> >::iterator timedIter = timedThreads.find(toAwakeTh);
//             if(timedIter != timedThreads.end())
//                 timedThreads.erase(timedIter);
//
//             returnFromCond(toAwakeTh, mutex);
//
//             if(existingCond[cond]->waitingTh.size() == 0)
//                 existingCond[cond]->mutex = -1;
//
//             #ifndef NDEBUG
//             Processor &curProc = findProcessor(procId);
//             std::cerr << "Thread " << curProc.runThread->id << " awaking thread " << toAwakeTh->id << std::endl;
//             #endif
//         }
//     }
//     schedulingLockBusy = false;
//     schedulingEvent.notify();
}
int resp::ConcurrencyManager::waitCond(int cond, int mutex, double time, unsigned int procId){
//     while(schedulingLockBusy)
//         wait(schedulingEvent);
//     schedulingLockBusy = true;
//
//     #ifndef NDEBUG
//     if(time > 0 && tk == NULL)
//         THROW_EXCEPTION("Trying to do a timed wait on a condition variable while the Time Keeper is not existing");
//     #endif
//     if(existingCond.find(cond) == existingCond.end()){
//         THROW_EXCEPTION("Cannot wait on condition variable " << cond << ": unable to find it");
//     }
//     if(existingCond[cond]->mutex != -1 && existingCond[cond]->mutex != mutex){
//         #ifndef NDEBUG
//         std::cerr << "Condition variable " << cond << " has assigned mutex " << existingCond[cond]->mutex << " but proc " << procId << " is trying to use mutex " << mutex << std::endl;
//         #endif
//         return EINVAL;
//     }
//
//     if(existingCond[cond]->mutex == -1)
//         existingCond[cond]->mutex = mutex;
//     #ifndef NDEBUG
//     std::cerr << "Issn condition variable " << cond << " is going to unlock mutex " << existingCond[cond]->mutex << std::endl;
//     #endif
//     schedulingLockBusy = false;
//     unLockMutex(mutex, procId);
//     schedulingLockBusy = true;
//
//     Processor &curProc = findProcessor(procId);
//     int th = curProc.deSchedule();
//     if(time <= 0 || sc_time_stamp().to_double() >= time*1000)
//         existingCond[cond]->waitingTh.push_back(existingThreads[th]);
//     #ifndef NDEBUG
//     std::cerr << "DeScheduling thread " << th << " because waiting on condition variable " << cond << std::endl;
//     #endif
//     //Now I get the first ready thread and I schedule it on the processor
//     ThreadEmu * readTh = findReadyThread();
//     if(readTh != NULL){
//         curProc.schedule(readTh);
//     }
//
//     if(time > 0){
//         if(sc_time_stamp().to_double() >= time*1000){
//             #ifndef NDEBUG
//             std::cerr << "Thread " << th << " not waiting on condition variable " << cond << " beacause " << time << " has elapsed :-) " << std::endl;
//             #endif
//             //I set the error return value
//             existingThreads[th]->setSyscRetVal = true;
//             existingThreads[th]->syscallRetVal = ETIMEDOUT;
//             //And I move back the thread in the ready queue
//             returnFromCond(existingThreads[th], existingMutex[mutex]);
//         }
//         else{
//             #ifndef NDEBUG
//             std::cerr << "Thread " << th << " time waiting on condition variable " << cond << " time " << time << std::endl;
//             #endif
//             std::pair<ThreadEmu *, std::pair<double, int> > tTh(existingThreads[th], std::pair<double, int>(time*1000, mutex));
//             timedThreads.insert(tTh);
//             tk->startCount.notify();
//         }
//     }
//     schedulingLockBusy = false;
//     schedulingEvent.notify();
//
//     return 0;
}

///***************************************************************
/// Here we implement some classes which will be used as timers
///
///***************************************************************
TimeSliceClock::TimeSliceClock(const sc_time &resolution){
}
void TimeSliceClock::preempt(){
}
IRQClock::IRQClock(const sc_time &resolution){
}
void IRQClock::schedulerIRQ(){
}
EventClock::EventClock(){
}
void EventClock::schedulerEvent(){
}
CondVarClock::CondVarClock(sc_time accuracy){
}
void CondVarClock::countTime(){
}
