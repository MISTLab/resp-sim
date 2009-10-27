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

//Initialization of some static variables
const int resp::ConcurrencyManager::SYSC_SCHED_FIFO = 0;
const int resp::ConcurrencyManager::SYSC_SCHED_OTHER = 1;
const int resp::ConcurrencyManager::SYSC_SCHED_RR = 2;
const int resp::ConcurrencyManager::SYSC_SCHED_EDF = 5;

const int resp::ConcurrencyManager::SYSC_PRIO_MAX = 255;
const int resp::ConcurrencyManager::SYSC_PRIO_MIN = 0;

const int resp::ConcurrencyManager::SYSC_PREEMPTIVE = 1;
const int resp::ConcurrencyManager::SYSC_NON_PREEMPTIVE = 0;

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

resp::ConcurrencyManager::ConcurrencyManager(){
    //Constructor: there is not much to do
    //a part from se-setting the reentracy
    //variables
    this->mallocMutex = -1;
    this->sfpMutex = -1;
    this->sinitMutex = -1;
    this->fpMutex = -1;
    this->maxProcId = 0;
    this->readyQueue = new std::deque<ThreadEmu *>[resp::ConcurrencyManager::SYSC_PRIO_MAX + 2];
}

resp::ConcurrencyManager::~ConcurrencyManager(){
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
    resp::ConcurrencyManager::tlsData = NULL;
    //Resetting instance variables
    this->mallocMutex = -1;
    this->sfpMutex = -1;
    this->sinitMutex = -1;
    this->fpMutex = -1;
    this->maxProcId = 0;
    this->existingAttr.clear();
    this->managedProc.clear();
    this->stacks.clear();
}

///*******************************************************************
/// Helper methods used to aid concurrency management
///*******************************************************************
///Determines if there is an idle processor and, in case it
///schedules a thread for execution on that processor
bool resp::ConcurrencyManager::scheduleFreeProcessor(ThreadEmu *th){
    //Ok,  now I have to see if there is a free processor on which I can schedule the thread
    std::map<unsigned int, Processor<unsigned int> >::iterator procIter, procIterEnd;
    for(procIter = this->managedProc.begin(), procIterEnd = this->managedProc.end(); procIter != procIterEnd; procIter++){
        if(procIter->second.runThread == NULL){
            //Here we are,  I have found the empty processor
            procIter->second.schedule(th);
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
    std::map<unsigned int, Processor<unsigned int> >::iterator procIter, procIterEnd;
    for(procIter = this->managedProc.begin(), procIterEnd = this->managedProc.end(); procIter != procIterEnd; procIter++){
        if(procIter->second.runThread == NULL){
            //Here we are, I have found the empty processor
            procIter->second.schedule(th);
            return true;
        }
    }
    //Here I have found no idle processor: I have to determine the processor running the lowest priority
    //thread and then, if it is lower than the current thread's priority, preempt it
    int curMinPrio = resp::ConcurrencyManager::SYSC_PRIO_MAX + 2;
    Processor<unsigned int> * minPriProc = NULL;
    for(procIter = this->managedProc.begin(), procIterEnd = this->managedProc.end(); procIter != procIterEnd; procIter++){
        int curPrio = 0;
        if(procIter->second.runThread->attr != NULL){
            if(procIter->second.runThread->attr->schedPolicy == resp::ConcurrencyManager::SYSC_SCHED_EDF){
                curPrio = resp::ConcurrencyManager::SYSC_PRIO_MAX + 1;
            }
            else
                curPrio = procIter->second.runThread->attr->priority;
        }
        else{
            std::cerr << "thread attribute is NULL??" << std::endl;
        }
        if(curPrio < curMinPrio){
            curMinPrio = curPrio;
            minPriProc = &(procIter->second);
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

//TODO: HERE WE ALSO HAVE TO DEAL WITH THE TLS

int resp::ConcurrencyManager::createThread(unsigned int threadFun, unsigned int args, int attr){
    BFDWrapper &bfdFE = BFDWrapper::getInstance();

    //I have to create the thread handle and add it to the list of
    //existing threads; in case there is an available processor,
    //I also start the execution of the thread on it; note that all the
    //data structures touched here are also used during scheduling,
    //so synchronization must be enforced with respect to those structures

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
    for(stacksIter = this->stacks.begin(), stacksNext = stacksIter, stacksNext++, stacksEnd = this->stacks.end();
                                                                        stacksNext != stacksEnd; stacksIter++, stacksNext++){
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
    unsigned int codeLimit = this->managedProc.begin()->second.processorInstance.getCodeLimit();
    if(curStackBase < codeLimit)
        THROW_EXCEPTION("Unable to allocate " << curStackSize << " bytes for the stack of thread " << existingThreads.size() << ": no more space in memory");

    //Actual thread creation
    int createdThId = this->existingThreads.size();
    ThreadEmu *th = NULL;
    AttributeEmu *curAttr = NULL;
    std::string routineName = bfdFE.symbolAt(threadFun);
    if(currentAttrIter == this->existingAttr.end()){
        //I see if default properties were specified for this thread in ReSP's
        //command line
        if(resp::ConcurrencyManager::defThreadInfo.find(routineName) == resp::ConcurrencyManager::defThreadInfo.end()){
            th = new ThreadEmu(createdThId, threadFun, args, curStackBase, resp::ConcurrencyManager::tlsSize, &resp::ConcurrencyManager::defaultAttr);
        }
        else{
            curAttr = this->existingAttr[this->createThreadAttr()];
            curAttr->preemptive = resp::ConcurrencyManager::defThreadInfo[routineName].preemptive;
            curAttr->schedPolicy = resp::ConcurrencyManager::defThreadInfo[routineName].schedPolicy;
            curAttr->priority = resp::ConcurrencyManager::defThreadInfo[routineName].priority;
            curAttr->deadline = resp::ConcurrencyManager::defThreadInfo[routineName].deadline;
            th = new ThreadEmu(createdThId, threadFun, args, curStackBase, resp::ConcurrencyManager::tlsSize, curAttr);
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
        th = new ThreadEmu(createdThId, threadFun, args, curStackBase, resp::ConcurrencyManager::tlsSize, curAttr);
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

    std::map<unsigned int, Processor<unsigned int> >::iterator curProcIter = this->managedProc.find(procId);
    if(curProcIter == this->managedProc.end())
        THROW_EXCEPTION("Processor with ID = " << procId << " not found among the registered processors");
    Processor<unsigned int> & curProc = curProcIter->second;

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
            #ifndef NDEBUG
            std::cerr << "Checking joined " << (*joinIter)->id << " joined on " << th << " waiting for the end of " << (*joinIter)->numJoined-1 << " others" << std::endl;
            #endif
            (*joinIter)->numJoined--;
            if((*joinIter)->numJoined == 0){
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
        }
        this->existingThreads[th]->joining.clear();

        #ifndef NDEBUG
        std::cerr << "Exiting thread " << th << " Return Value " << retVal << std::endl;
        #endif
    }
    else{
        //It is an IRQ thread, so I can freely deallocate it and the corresponding attribute
        this->deleteThreadAttr(this->existingThreads[th]->attr->id);
        #ifndef NDEBUG
        std::cerr << "Exiting IRQ thread " << th << std::endl;
        #endif
    }

    //Now I have to schedule a new thread on the just freed processor
    ThreadEmu * readTh = this->findReadyThread();
    if(readTh != NULL){
        curProc.schedule(readTh);
    }

    this->schedLock.unlock();
}

bool resp::ConcurrencyManager::cancelThread(int threadId){
}

int resp::ConcurrencyManager::createThreadAttr(){
}
void resp::ConcurrencyManager::deleteThreadAttr(int attr){
}

void resp::ConcurrencyManager::setStackSize(int attr, int stackSize){
}
unsigned int resp::ConcurrencyManager::getStackSize(int attr){
}

void resp::ConcurrencyManager::setPreemptive(int attr, int isPreemptive){
}
int resp::ConcurrencyManager::getPreemptive(int attr){
}

void resp::ConcurrencyManager::setSchedDeadline(int attr, unsigned int deadline){
}
unsigned int resp::ConcurrencyManager::getSchedDeadline(int attr){
}

void resp::ConcurrencyManager::setSchedPrio(int attr, int priority){
}
int resp::ConcurrencyManager::getSchedPrio(int attr){
}

void resp::ConcurrencyManager::setSchedPolicy(int attr, int policy){
}
int resp::ConcurrencyManager::getSchedPolicy(int attr){
}

int resp::ConcurrencyManager::getThreadSchePolicy(int threadId){
}
void resp::ConcurrencyManager::setThreadSchePolicy(int threadId, int policy){
}

int resp::ConcurrencyManager::getThreadSchedPrio(int threadId){
}
void resp::ConcurrencyManager::setThreadSchedPrio(int threadId, int priority){
}

unsigned int resp::ConcurrencyManager::getThreadSchedDeadline(int threadId){
}
void resp::ConcurrencyManager::setThreadSchedDeadline(int threadId, unsigned int deadline){
}

int resp::ConcurrencyManager::getThreadId(unsigned int procId){
}

int resp::ConcurrencyManager::createKey(){
}
void resp::ConcurrencyManager::setSpecific(unsigned int procId, int key, unsigned int memArea){
}
unsigned int resp::ConcurrencyManager::getSpecific(unsigned int procId, int key){
}

void resp::ConcurrencyManager::join(int thId, unsigned int procId, int curThread_){
}
void resp::ConcurrencyManager::joinAll(unsigned int procId){
}

std::pair<unsigned int, unsigned int> resp::ConcurrencyManager::readTLS(unsigned int procId){
}
void resp::ConcurrencyManager::idleLoop(unsigned int procId){
}

void resp::ConcurrencyManager::pushCleanupHandler(unsigned int procId, unsigned int routineAddress, unsigned int arg){
}
void resp::ConcurrencyManager::popCleanupHandler(unsigned int procId, bool execute){
}
void resp::ConcurrencyManager::execCleanupHandlerTop(unsigned int procId){
}

///*********** Mutex related routines *******************
///Destroys a previously allocated mutex, exception if the mutex does not exist
void resp::ConcurrencyManager::destroyMutex(unsigned int procId, int mutex){
}
int resp::ConcurrencyManager::createMutex(){
}
///Locks the mutex, deschedules the thread if the mutex is busy; in case
///nonbl == true, the function behaves as a trylock
bool resp::ConcurrencyManager::lockMutex(int mutex, unsigned int procId, bool nonbl){
}
///Releases the lock on the mutex
int resp::ConcurrencyManager::unLockMutex(int mutex, unsigned int procId){
}

///*********** Semaphore related routines *******************
int resp::ConcurrencyManager::createSem(unsigned int procId, int initialValue){
}
void resp::ConcurrencyManager::destroySem(unsigned int procId, int sem){
}
void resp::ConcurrencyManager::postSem(int sem, unsigned int procId){
}
void resp::ConcurrencyManager::waitSem(int sem, unsigned int procId){
}

///*********** Condition Variable related routines *******************
int resp::ConcurrencyManager::createCond(unsigned int procId){
}
void resp::ConcurrencyManager::destroyCond(unsigned int procId, int cond){
}
void resp::ConcurrencyManager::signalCond(int cond, bool broadcast, unsigned int procId){
}
int resp::ConcurrencyManager::waitCond(int cond, int mutex, double time, unsigned int procId){
}

///***************************************************************
/// Here we declare some classes which will be used as timers
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
