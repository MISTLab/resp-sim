#include "utils.hpp"

#include "threadStruct.hpp"
#include "ProcessorIf.hpp"
#include "synchManager.hpp"
#include "bfdFrontend.hpp"
#include "syscalls.hpp"
#include "ac_syscall.H"

#include "controller.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <map>
#include <list>
#include <deque>
#include <string>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <pthread.h>

#include <systemc.h>

#include <ctime>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#ifdef BOOST_NO_STDC_NAMESPACE
namespace std {
  using ::time;
}
#endif


/***********************************************************************************/
//The following variables are used to keep track of what is happening in the system,
//what is its current status
/***********************************************************************************/
std::list<Processor> existingProc;

std::map<int, AttributeEmu *> existingAttr;

std::map<int, ThreadEmu *> existingThreads;

std::map<int, MutexEmu *> existingMutex;

std::map<int, ConditionEmu *> existingCond;

std::map<int, SemaphoreEmu *> existingSem;

int keys = -1;
std::map<std::pair<int, ThreadEmu *>, void *> threadSpecific;

TimeKeeper *tk = NULL;
std::map<ThreadEmu *, std::pair<double, int> > timedThreads;

TimeSliceClock *timeSlicer = NULL;
IRQClock *irqKeeperSlicer = NULL;
EventClock *eventKeeperSlicer = NULL;

ThreadEmu * mainThread = NULL;

AttributeEmu defaultAttr;

bool initDone = false;

std::map<unsigned int, unsigned int> stacks;

unsigned int defaultStackSize = 1024*20;
unsigned int defaultTLSSize = 1024*2;

unsigned int nopLoppAddr = 0;
extern unsigned int nopLoopAddr;
unsigned int exitThreadAddr = 0;
bool waitLoopEnabled = true;

int mallocMutex = -1;
int sfpMutex = -1;
int sinitMutex = -1;
int fpMutex = -1;

//Latencies respectively for scheduling a task on the processor,
//descheduling that task and for determining what is the next
//task to be chosen
sc_time schedLatency = SC_ZERO_TIME;
sc_time deSchedLatency = SC_ZERO_TIME;
sc_time schedChooseLatency = SC_ZERO_TIME;

//Note that the last element of the readyQueue, the one containing
//the tasks with EDF schedule, must be ordered on the
//schedule deadline: the first element the one with closest
//deadline, the last one with the furthest one
std::deque<ThreadEmu *> readyQueue[resp::SYSC_PRIO_MAX + 2];

//interrup service routines which are called when the corresponding
//interrupts (the index of the map) happen
std::map<int, unsigned int> isrAddresses;

std::vector<unsigned int> eventAddresses;

//Function used to sort the tasks in the last ready queue according
//to their deadlines, the closest deadline first
bool deadlineSort(ThreadEmu * a, ThreadEmu * b){
    if(a->attr == NULL || b->attr == NULL)
        return false;
    return a->attr->deadline < b->attr->deadline;
}

std::map<std::string, DefaultThreadInfo> defThreadInfo;

bool schedulingLockBusy = false;
sc_event schedulingEvent;
bool enableWait = true;

extern sc_event armProcReadEvent;
extern bool *readyARMProc;
unsigned int maxProcId = 0;

bool timeSliceing = false;
sc_event doingTimeSlice;

sc_time eventFreq = SC_ZERO_TIME;

void Processor::schedule(ThreadEmu * thread){
    //Schedules a thread on the current processor
    #ifndef NDEBUG
    if(this->runThread != NULL){
        THROW_EXCEPTION("Trying to schedule thread " << thread->id << " on processor " << this->procIf->getId() << " which is already executing another thread");
    }
    #endif
    if(enableWait)
        wait(schedLatency);
    this->runThread = thread;
    thread->status = ThreadEmu::RUNNING;

    //Now I have to set the status of the processor to the new thread: when the system call returns
    //the processor will start executing the new code; note that the action depends from the
    //fact that the thread is just starting or if it had already been scheduled
    if(thread->state.size() == 0){
        //This thread has never been scheduled before
        this->procIf->setPC((unsigned long long)thread->thread_routine);
        this->procIf->setFunctionArg(0, thread->args);
        this->procIf->setStack(thread->stackBase - thread->tlsSize);
        this->procIf->setReturnAddr(exitThreadAddr);
        this->procIf->setTLS(std::pair<void *, void *>((void*)thread->stackBase, (void*)((thread->stackBase - thread->tlsSize))));
        #ifndef NDEBUG
        std::cerr << "Scheduling for the first time thread on processor " << this->procIf->getId() << " thread id = " << thread->id << " with start address " << std::hex << (unsigned long)thread->thread_routine << std::dec << std::endl;
        #endif
    }
    else{
        #ifndef NDEBUG
        std::cerr << "Scheduling thread on processor " << this->procIf->getId() << " thread id = " << thread->id << std::endl;
        #endif
        this->procIf->setStatus(thread->state);

        if(thread->setSyscRetVal){
            #ifndef NDEBUG
            std::cerr << "Thread " << thread->id << " - Changing return value of last called function to " << thread->syscallRetVal << std::endl;
            #endif
            thread->setSyscRetVal = false;
            this->procIf->setRetVal(thread->syscallRetVal);
        }
    }
    #ifndef NDEBUG
    if(thread->curTraceElem.processorId != -1 || thread->curTraceElem.startTime != -1 || thread->curTraceElem.endTime != -1)
        THROW_EXCEPTION("Thread " << thread->id << " is already being executed by processor " << thread->curTraceElem.processorId << " starting at time " << thread->curTraceElem.startTime << " when trying to schedule it on processor " << this->procIf->getId());
    #endif
    thread->curTraceElem.processorId = this->procIf->getId();
    thread->curTraceElem.startTime = sc_time_stamp().to_double();
    thread->curTraceElem.startAddress = this->procIf->getPC();
    if(this->curIdleStart != sc_time_stamp().to_double() && this->curIdleStart != -1){
        this->idleTrace.push_back(std::pair<double, double>(this->curIdleStart,  sc_time_stamp().to_double()));
    }
    this->curIdleStart = -1;

    if(::waitLoopEnabled)
        this->idleEvent->notify();
}

int Processor::deSchedule(bool saveStatus){
    //Deschedules current thread putting the processor in the busy loop
    #ifndef NDEBUG
    if(this->runThread == NULL){
        THROW_EXCEPTION("Trying to deschedule thread " << this->runThread->id << " from processor " << this->procIf->getId() << " which has no running threads");
    }
    #endif
    if(enableWait)
        wait(deSchedLatency);
    ThreadEmu * thTemp = this->runThread;
    this->runThread = NULL;
    if(saveStatus){
        thTemp->status = ThreadEmu::WAITING;
        thTemp->state = this->procIf->getStatus();
        thTemp->lastPc = this->procIf->getPC();
        thTemp->lastRetAddr = this->procIf->getReturnAddr();
    }

    #ifndef NDEBUG
    if(nopLoppAddr == 0){
        THROW_EXCEPTION("nopLoppAddr has never been initialized");
    }
    #endif
    this->procIf->clearStatus();
    this->procIf->setPC(nopLoppAddr);
    this->procIf->setReturnAddr(nopLoppAddr);
    #ifndef NDEBUG
    if(this->curIdleStart != -1)
        THROW_EXCEPTION("I'm starting a new idle time but it seems that the start idle time is already set");
    #endif
    this->curIdleStart = sc_time_stamp().to_double();

    #ifndef NDEBUG
    std::cerr << "DeScheduling thread " << thTemp->id << " from processor " << this->procIf->getId()  << std::endl;
    #endif

    #ifndef NDEBUG
    if(thTemp->curTraceElem.processorId == -1 || thTemp->curTraceElem.startTime == -1)
        THROW_EXCEPTION("Thread " << thTemp->id << " is not beeing executed by any processor when trying to deschedule from " << this->procIf->getId());
    if(thTemp->curTraceElem.endTime != -1)
        THROW_EXCEPTION("Thread " << thTemp->id << " has already an endtime even though it hasn't been descheduled yet");
    if(thTemp->curTraceElem.processorId != (int)this->procIf->getId())
        THROW_EXCEPTION("Thread " << thTemp->id << " started execution of current slot on processor " << thTemp->curTraceElem.processorId << " and is ending on processor " << this->procIf->getId());
    #endif
    thTemp->curTraceElem.endTime = sc_time_stamp().to_double();
    thTemp->curTraceElem.endAddress = thTemp->lastPc;
    thTemp->execTrace.push_back(thTemp->curTraceElem);
    thTemp->curTraceElem.clear();

    return thTemp->id;
}

/***********************************************************************/
// These functions are used to keep and print statistics on system load
/***********************************************************************/


void Processor::printIdleTrace(){
    double totalTime = 0;
    std::cout << "Processor " << this->procIf->getId() << std::endl;
    std::vector<std::pair<double, double> >::iterator traceIter, traceEnd;
    for(traceIter = this->idleTrace.begin(), traceEnd = this->idleTrace.end(); traceIter != traceEnd; traceIter++){
        std::cout << std::scientific << std::setprecision (7) << traceIter->first << " - " << std::scientific << std::setprecision (7) << traceIter->second << std::endl;
        totalTime += traceIter->second - traceIter->first;
    }
    if(this->curIdleStart != -1)
        std::cout << "Currently in an idle period started at time " << std::scientific << std::setprecision (7) << this->curIdleStart << std::endl;
    std::cout << "Total Time: " << std::scientific << std::setprecision (7) << totalTime << std::endl;
}

std::string Processor::getIdleTraceCSV(){
    double totalTime = 0;
    std::ostringstream temp;
    std::vector<std::pair<double, double> >::iterator traceIter, traceEnd;
    for(traceIter = this->idleTrace.begin(), traceEnd = this->idleTrace.end(); traceIter != traceEnd; traceIter++){
        temp << std::scientific << std::setprecision (7) << traceIter->first << "," << std::scientific << std::setprecision (7) << traceIter->second << std::endl;
        totalTime += traceIter->second - traceIter->first;
    }
    if( this->curIdleStart != -1 ) {
        temp << this->curIdleStart << "," << sc_time_stamp().to_double() <<  std::endl;
    }
    return temp.str();
}

void resp::printThreadTraces(){
    std::map<int, ThreadEmu *>::iterator thIter,  thEnd;
    for(thIter = existingThreads.begin(),  thEnd = existingThreads.end(); thIter !=  thEnd; thIter++){
        thIter->second->printTrace();
        std::cout << std::endl;
    }
}

std::vector<std::string> resp::getThreadTraces(){
    double totalTime = 0;
    std::vector<std::string> result;
    std::list<Processor>::iterator procIter,  procEnd;
    std::map<int, ThreadEmu *>::iterator thIter,  thEnd;

    for(thIter = existingThreads.begin(),  thEnd = existingThreads.end(); thIter !=  thEnd; thIter++){
        result.push_back(thIter->second->getTrace());
    }

    for(procIter = existingProc.begin(),  procEnd = existingProc.end(); procIter != procEnd; procIter++){
        std::ostringstream temp;
        temp << "Processor " << procIter->procIf->getId() << std::endl;
        std::vector<std::pair<double, double> >::iterator traceIter, traceEnd;
        for(traceIter = procIter->idleTrace.begin(), traceEnd = procIter->idleTrace.end(); traceIter != traceEnd; traceIter++){
            temp << std::scientific << std::setprecision (7) << traceIter->first << ";" << std::scientific << std::setprecision (7) << traceIter->second << std::endl;
            totalTime += traceIter->second - traceIter->first;
        }
        if(procIter->curIdleStart != -1) {
            temp << std::scientific << std::setprecision (7) << procIter->curIdleStart  << std::scientific << std::setprecision (7) << sc_time_stamp().to_double() << std::endl;
            totalTime += sc_time_stamp().to_double() - procIter->curIdleStart;
        }
        temp << "Total Time: " << std::scientific << std::setprecision (7) << totalTime << std::endl;
        result.push_back(temp.str());
        totalTime = 0;
    }

    return result;
}

std::map<int, double> resp::getAverageLoads(){
    double totalTime = 0;
    std::map<int,double> result;
    std::list<Processor>::iterator procIter,  procEnd;

    for(procIter = existingProc.begin(),  procEnd = existingProc.end(); procIter != procEnd; procIter++){
        std::vector<std::pair<double, double> >::iterator traceIter, traceEnd;
        for(traceIter = procIter->idleTrace.begin(), traceEnd = procIter->idleTrace.end(); traceIter != traceEnd; traceIter++){
            totalTime += traceIter->second - traceIter->first;
        }

        // If the processor is still in Idle add the remaining time
        if(procIter->curIdleStart != -1)
            totalTime += sc_time_stamp().to_double() - procIter->curIdleStart;

        result[procIter->procIf->getId()] = 1-totalTime/sc_time_stamp().to_double();
        totalTime = 0;
    }

    return result;
}


void resp::printIdleTraces(){
    std::list<Processor>::iterator procIter,  procEnd;
    for(procIter = existingProc.begin(),  procEnd = existingProc.end(); procIter != procEnd; procIter++){
        procIter->printIdleTrace();
        std::cout << std::endl;
    }
}

std::vector<std::string> resp::getIdleTraces(){
    std::list<Processor>::iterator procIter,  procEnd;
    std::vector<std::string> ret;
    for(procIter = existingProc.begin(),  procEnd = existingProc.end(); procIter != procEnd; procIter++){
        ret.push_back(procIter->getIdleTraceCSV());
    }
    return ret;
}


/******************************************************************************/
// These functions are used to initialize the pthread and reentrancy emulation.
/******************************************************************************/

void resp::setEventLatency(sc_time eventLatency){
    eventFreq = eventLatency;
    if(eventFreq.to_double() > 0){
        eventKeeperSlicer = new EventClock();
        #ifndef NDEBUG
        std::cerr << "Initialized the event scheduler with an accuracy of " << eventFreq.to_double()/10e3 << " NS" << std::endl;
        #endif
    }
}

void resp::initiReentrantEmu(std::map<std::string, sc_time> latencies){
    if(bfdFE == NULL){
        THROW_EXCEPTION("Normal system calls should be initialized with function \"initSysCalls\" before calling initiReentrantEmu");
    }
    resp::registerReentrantEmu(latencies);
    mallocMutex = ::createMutex((unsigned int)-1);
    sfpMutex = ::createMutex((unsigned int)-1);
    sinitMutex = ::createMutex((unsigned int)-1);
    fpMutex = ::createMutex((unsigned int)-1);
}

void resp::registerISR(std::string isrFunctionName, int irqId, bool preemptive, int schedPolicy, int priority){
    if(bfdFE == NULL){
        THROW_EXCEPTION("Normal system calls should be initialized with function \"initSysCalls\" before calling registerISR");
    }
    bool valid = false;
    valid = false;
    isrAddresses[irqId] = bfdFE->getSymAddr(isrFunctionName, valid);
    if(!valid){
        THROW_EXCEPTION("No symbol for interrupt " << isrFunctionName << " found in executable " << bfdFE->getExecName());
    }
    defThreadInfo[isrFunctionName].preemptive = preemptive;
    defThreadInfo[isrFunctionName].schedPolicy = schedPolicy;
    defThreadInfo[isrFunctionName].priority = priority;
    #ifndef NDEBUG
    std::cerr << "Registering ISR " << isrFunctionName << " with ID " << irqId << " at address " << std::hex << std::showbase << isrAddresses[irqId] << std::endl;
    #endif
}

void resp::registerEvent(std::string eventFunctionName, unsigned int deadline, bool preemptive){
    if(bfdFE == NULL){
        THROW_EXCEPTION("Normal system calls should be initialized with function \"initSysCalls\" before calling registerEvent");
    }
    bool valid = false;
    valid = false;
    eventAddresses.push_back(bfdFE->getSymAddr(eventFunctionName, valid));
    if(!valid){
        THROW_EXCEPTION("No symbol for interrupt " << eventFunctionName << " found in executable " << bfdFE->getExecName());
    }
    defThreadInfo[eventFunctionName].preemptive = preemptive;
    defThreadInfo[eventFunctionName].schedPolicy = resp::SYSC_SCHED_EDF;
    defThreadInfo[eventFunctionName].deadline = deadline;
    #ifndef NDEBUG
    std::cerr << "Registering Event " << eventFunctionName << std::endl;
    #endif
}

void resp::initializePthreadSystem(unsigned int memSize, bool waitLoop, unsigned int defaultStackSize, std::map<std::string, sc_time> latencies){
    //I have to regsiter all the pthread system calls,  and,  finally, initialize
    //the status of the processors. In particular,  the main processor will
    //be initialized to the entry point,  while the other processors will point
    //to the NOP loop. Also the stacks of the processors will be initialized
    if(bfdFE == NULL){
        THROW_EXCEPTION("Normal system calls should be initialized with function \"initSysCalls\" before calling initializePthreadSystem");
    }
    ::defaultStackSize = defaultStackSize;
    initDone = true;
    resp::registerPthreadEmu(latencies);
    bool valid = false;
    nopLoppAddr = bfdFE->getSymAddr(".__nop_busy_loop", valid);
    nopLoopAddr = nopLoppAddr;
    if(!valid){
        THROW_EXCEPTION("No symbol .__nop_busy_loop found in executable " << bfdFE->getExecName());
    }
    valid = false;
    exitThreadAddr = bfdFE->getSymAddr("pthread_myexit", valid);
    if(!valid){
        THROW_EXCEPTION("No symbol pthread_myexit found in executable " << bfdFE->getExecName());
    }
    std::list<Processor>::iterator procIter = existingProc.begin(), procEnd = existingProc.end();
    if(procIter == procEnd){
        THROW_EXCEPTION("No processors registered: unable to initialize pthread emulation");
    }
    ::waitLoopEnabled = waitLoop;
    if(waitLoop){
        resp::__nop_busy_loopSysCall *a = new resp::__nop_busy_loopSysCall();
        archc::register_syscall(".__nop_busy_loop", *a, latencies);
    }
    //I have to set the stack for the main thread,  the one which is being executed by
    //the first processor and which contains the main routine. I also have to create
    //this main thread
    valid = false;
    unsigned int mainRoutineAddr = bfdFE->getSymAddr("main", valid);
    if(!valid){
        THROW_EXCEPTION("No symbol main found in executable " << bfdFE->getExecName());
    }
    //Now I also have to check if the pthread_cond_timedwait is present: if it is I have to instantiate the
    //dummy systemc component which keeps track of time
    valid = false;
    bfdFE->getSymAddr("pthread_cond_timedwait", valid);
    if(valid){
        tk = new TimeKeeper(sc_time(10, SC_NS));
        #ifndef NDEBUG
        std::cerr << "Initialized the timekeeper with an accuracy of 10 NS" << std::endl;
        #endif
    }

    ThreadEmu * th = new ThreadEmu(0, (thread_routine_t)mainRoutineAddr, NULL, memSize, defaultTLSSize, &defaultAttr);
    th->status = ThreadEmu::RUNNING;
    existingThreads[0] = th;
    mainThread = existingThreads[0];
    procIter->runThread = mainThread;
    procIter->procIf->setStack(memSize - defaultTLSSize);
    stacks[memSize] = defaultStackSize;
    mainThread->curTraceElem.processorId = procIter->procIf->getId();
    mainThread->curTraceElem.startTime = sc_time_stamp().to_double();
    mainThread->curTraceElem.startAddress = mainRoutineAddr;
    for(procIter++; procIter != procEnd; procIter++){
        //For each processor I set the stack and I initialize the program counter
        //to the NOP loop
        procIter->procIf->setPC(nopLoppAddr);
        procIter->procIf->setReturnAddr(nopLoppAddr);
        procIter->curIdleStart = sc_time_stamp().to_double();
    }

    readyARMProc = new bool[maxProcId];
    for(int i = 0; i < maxProcId; i++)
        readyARMProc[i] = false;

    #ifndef NDEBUG
    std::cerr << "Max Processor ID " << maxProcId << std::endl;
    std::cerr << "Completed pthread emulation initialization; main processor Id = " << existingProc.front().procIf->getId() << \
                " Default stack size = " << defaultStackSize/1024 << "KB" << std::endl;
    #endif

}

void resp::setEmulationLatencies(sc_time schedulerRes, sc_time IRQRes){
    //Initialization of the time-slice scheduler
    if(schedulerRes.to_double() > 0){
        timeSlicer = new TimeSliceClock(schedulerRes);
        #ifndef NDEBUG
        std::cerr << "Initialized the slicing scheduler with an accuracy of " << schedulerRes.to_double()/10e3 << " NS" << std::endl;
        #endif
    }
    if(IRQRes.to_double() > 0){
        irqKeeperSlicer = new IRQClock(IRQRes);
        #ifndef NDEBUG
        std::cerr << "Initialized the IRQ scheduler with an accuracy of " << IRQRes.to_double()/10e3 << " NS" << std::endl;
        #endif
    }
}

void resp::initLatencies(sc_time schedLat, sc_time deSchedLat, sc_time schedChooseLat){
    schedLatency = schedLat;
    deSchedLatency = deSchedLat;
    schedChooseLatency = schedChooseLat;
}

///Sets the thread informations from python so that when a thread is created, these peroperties are applied
///to the thread. Note that, in case an attribute for the thread does not esists it is created and initialized
///with these values. If an attribute already exists its values are overwritten
void resp::setThreadInfo(std::string functionName, bool preemptive, int schedPolicy, int priority, unsigned int deadline){
    defThreadInfo[functionName].preemptive = preemptive;
    defThreadInfo[functionName].schedPolicy = schedPolicy;
    defThreadInfo[functionName].priority = priority;
    defThreadInfo[functionName].deadline = deadline;
    #ifndef NDEBUG
    std::cerr << "Setting information for thread " << functionName << " preemptive = " << preemptive << " schedPolicy = " << schedPolicy << " priority = " << priority << " deadline = " << deadline << std::endl;
    #endif
}

void resp::registerProcessorIf(ProcessorIf &procIf){
    if(initDone){
        THROW_EXCEPTION("The registerProcessorIf function cannot be called after the initializeSystem");
    }
    Processor newProc(&procIf);
    std::list<Processor>::iterator procIter,  procEnd;
    for(procIter = existingProc.begin(),  procEnd = existingProc.end(); procIter != procEnd; procIter++){
        if(procIter->procIf->getId() == procIf.getId())
            THROW_EXCEPTION("There is another processor already registered with ID = " << procIf.getId());
    }
    existingProc.push_back(newProc);
    if(procIf.getId() + 1 > maxProcId)
        maxProcId = procIf.getId() + 1;
}

void resp::reset() {
    std::map<int, AttributeEmu *>::iterator existingAttrIter, existingAttrEnd;
    for(existingAttrIter = existingAttr.begin(), existingAttrEnd = existingAttr.end(); existingAttrIter != existingAttrEnd; existingAttrIter++){
        delete existingAttrIter->second;
    }
    std::map<int, ThreadEmu *>::iterator existingThreadsIter, existingThreadsEnd;
    for(existingThreadsIter = existingThreads.begin(), existingThreadsEnd = existingThreads.end(); existingThreadsIter != existingThreadsEnd; existingThreadsIter++){
        delete existingThreadsIter->second;
    }
    std::map<int, MutexEmu *>::iterator existingMutexIter, existingMutexEnd;
    for(existingMutexIter = existingMutex.begin(), existingMutexEnd = existingMutex.end(); existingMutexIter != existingMutexEnd; existingMutexIter++){
        delete existingMutexIter->second;
    }
    std::map<int, ConditionEmu *>::iterator existingCondIter, existingCondEnd;
    for(existingCondIter = existingCond.begin(), existingCondEnd = existingCond.end(); existingCondIter != existingCondEnd; existingCondIter++){
        delete existingCondIter->second;
    }
    std::map<int, SemaphoreEmu *>::iterator existingSemIter, existingSemEnd;
    for(existingSemIter = existingSem.begin(), existingSemEnd = existingSem.end(); existingSemIter != existingSemEnd; existingSemIter++){
        delete existingSemIter->second;
    }

    existingProc.clear();
    existingAttr.clear();
    existingThreads.clear();
    existingMutex.clear();
    existingCond.clear();
    existingSem.clear();
    keys = -1;
    threadSpecific.clear();
    timedThreads.clear();
    stacks.clear();
    eventAddresses.clear();

    if(tk != NULL)
        delete tk;
    tk = NULL;
    mainThread = NULL;

    defaultStackSize = 1024*20;
    defaultTLSSize = 1024*2;

    maxProcId = 0;
    nopLoppAddr = 0;
    exitThreadAddr = 0;
    waitLoopEnabled = true;

    schedLatency = SC_ZERO_TIME;
    deSchedLatency = SC_ZERO_TIME;
    schedChooseLatency = SC_ZERO_TIME;
    eventFreq = SC_ZERO_TIME;

    defThreadInfo.clear();

    mallocMutex = -1;
    sfpMutex = -1;
    sinitMutex = -1;
    fpMutex = -1;
    initDone = false;
    if(timeSlicer != NULL)
        delete timeSlicer;
    if(irqKeeperSlicer != NULL)
        delete irqKeeperSlicer;
    if(readyARMProc != NULL)
        delete readyARMProc;
    if(eventKeeperSlicer != NULL)
        delete eventKeeperSlicer;
    readyARMProc = NULL;

    if( bfdFE != NULL ) {
        delete bfdFE;
        bfdFE = NULL;
    }
}

bool scheduleFreeProcessor(ThreadEmu *th){
    //Ok,  now I have to see if there is a free processor on which I can schedule the thread
    std::list<Processor>::iterator procIter, procIterEnd;
    for(procIter = existingProc.begin(), procIterEnd = existingProc.end(); procIter != procIterEnd; procIter++){
        if(procIter->runThread == NULL){
            //Here we are,  I have found the empty processor
            procIter->schedule(th);
            return true;
        }
    }
    return false;
}

void addReadyThread(ThreadEmu *toAwakeTh){
    if(!scheduleFreeProcessor(toAwakeTh)){
        int curPrio = 0;
        if(toAwakeTh->attr != NULL){
            if(toAwakeTh->attr->schedPolicy == resp::SYSC_SCHED_EDF)
                curPrio = resp::SYSC_PRIO_MAX + 1;
            else
                curPrio = toAwakeTh->attr->priority;
        }
        readyQueue[curPrio].push_back(toAwakeTh);
        if(curPrio == resp::SYSC_PRIO_MAX + 1)
            sort(readyQueue[curPrio].begin(), readyQueue[curPrio].end(), deadlineSort);
    }
}

bool preemptLowerPrio(ThreadEmu *th){
    std::list<Processor>::iterator procIter, procIterEnd;
    for(procIter = existingProc.begin(), procIterEnd = existingProc.end(); procIter != procIterEnd; procIter++){
        if(procIter->runThread == NULL){
            //Here we are, I have found the empty processor
            procIter->schedule(th);
            return true;
        }
        else{
            //Lets check the priority level of the task: if it is lower than the current task, then
            //I preempt the task which is currently running on that processor
            int curPrio = 0;
            unsigned int deadline = 0;
            if(procIter->runThread->attr != NULL){
                if(procIter->runThread->attr->schedPolicy == resp::SYSC_SCHED_EDF){
                    curPrio = resp::SYSC_PRIO_MAX + 1;
                    deadline = procIter->runThread->attr->deadline;
                }
                else
                    curPrio = procIter->runThread->attr->priority;
            }
            if(th->attr != NULL){
                int newPrio = 0;
                if(th->attr->schedPolicy == resp::SYSC_SCHED_EDF){
                    newPrio = resp::SYSC_PRIO_MAX + 1;
                }
                else
                    newPrio = th->attr->priority;
                #ifndef NDEBUG
                std::cerr << "trying to preempt task " << procIter->runThread->id << " (prio=" << curPrio << ",deadline=" << deadline << ") with " << th->id << " (prio=" << newPrio << ",deadline=" << th->attr->deadline << ")" << std::endl;
                #endif
                if((curPrio < newPrio || (newPrio == (resp::SYSC_PRIO_MAX + 1) && th->attr->deadline < deadline)) && (procIter->runThread->attr == NULL || procIter->runThread->attr->preemptive)){
                    #ifndef NDEBUG
                    std::cerr << "preempting" << std::endl;
                    #endif
                    //Lets deschedule the old thread and schedule the new one.
                    readyQueue[curPrio].push_back(procIter->runThread);
                    if(curPrio == resp::SYSC_PRIO_MAX + 1)
                        sort(readyQueue[curPrio].begin(), readyQueue[curPrio].end(), deadlineSort);
                    int thId = procIter->deSchedule();
                    existingThreads[thId]->status = ThreadEmu::READY;
                    procIter->schedule(th);
                    return true;
                }
            }
        }
    }
    return false;
}

Processor &findProcessor(unsigned int procId){
    std::list<Processor>::iterator procBeg, procEnd;
    for(procBeg = existingProc.begin(), procEnd = existingProc.end(); procBeg != procEnd; procBeg++){
        if(procBeg->procIf->getId() == procId)
            return *procBeg;
    }
    #ifndef NDEBUG
    if(procBeg == procEnd){
        THROW_EXCEPTION("Processor " << procId << " not found among the available processors");
    }
    #endif
    return *procBeg;
}

///This is the main function implementing the scheduling policy. In order to change the policy, this
///is the function which should be changed.
ThreadEmu * findReadyThread(){
    //I get the thread of the first queue which is not empty
    if(enableWait)
        wait(schedChooseLatency);
    ThreadEmu * foundThread = NULL;
    int j = -1;
    for(int i = resp::SYSC_PRIO_MAX + 1; i >= 0; i--){
        if(readyQueue[i].size() > 0){
            foundThread = readyQueue[i].front();
            readyQueue[i].pop_front();
            j = i;
            break;
        }
    }
    if(foundThread == NULL){
        bool foundRun = false;
        std::vector<ThreadEmu *> foundWait;
        std::map<int, ThreadEmu *>::iterator thIter, thEnd;
        for(thIter = existingThreads.begin(), thEnd = existingThreads.end(); thIter != thEnd; thIter++){
            if(thIter->second->status == ThreadEmu::RUNNING)
                foundRun = true;
            else if(thIter->second->status == ThreadEmu::WAITING)
                foundWait.push_back(thIter->second);
        }
        if(foundWait.size() > 0 && !foundRun){
            std::cerr << "waiting threads ";
            for(unsigned int j = 0; j < foundWait.size(); j++)
                std::cerr << foundWait[j]->id << " ";
            std::cerr << std::endl;
            THROW_EXCEPTION("Deadlock: there are no ready threads in the system");
        }
    }

    return foundThread;
}

/***********************************************************************/
// List of functions used to interface the OS management component with
// the system calls
/***********************************************************************/

///Creates a task and it assigns it
int createThread(thread_routine_t threadFun,  void * args, int attr){
    //I have to create the thread handle and add it to the list of
    //existing threads; in case there is an available processor,
    //I also start the execution of the thread on it

    while(schedulingLockBusy)
        wait(schedulingEvent);
    schedulingLockBusy = true;
    //First of all lets determine the stack size
    unsigned int curStackBase = 0;
    unsigned int curStackSize = 0;
    if(existingAttr.find(attr) == existingAttr.end()){
        curStackSize = defaultStackSize;
    }
    else{
        curStackSize = existingAttr[attr]->stackSize;
    }
    //I have to determine if there is space for
    //my stack among two already existing ones
    //or if I have to position myself at the end of
    //the stack
    std::map<unsigned int, unsigned int>::iterator stacksIter, stacksNext, stacksEnd;
    for(stacksIter = stacks.begin(), stacksNext = stacksIter, stacksNext++, stacksEnd = stacks.end();
                                                                        stacksNext != stacksEnd; stacksIter++, stacksNext++){
        if((stacksIter->first - stacksIter->second - stacksNext->first) > curStackSize){
            curStackBase = stacksIter->first - stacksIter->second;
            stacks[curStackBase] = curStackSize;
            break;
        }
    }
    if(curStackBase == 0){
        //I haven't found any suitable stack
        curStackBase = stacksIter->first - stacksIter->second;
        stacks[curStackBase] = curStackSize;
    }
    if(curStackBase < archc::ac_heap_ptr)
        THROW_EXCEPTION("Unable to allocate " << curStackSize << " bytes for the stack of thread " << existingThreads.size() << ": no more space in memory");
    ThreadEmu *th = NULL;
    AttributeEmu *curAttr = NULL;
    std::string routineName = bfdFE->symbolAt((unsigned long)threadFun).front();
    if(existingAttr.find(attr) == existingAttr.end()){
        if(defThreadInfo.find(routineName) == defThreadInfo.end()){
            th = new ThreadEmu(existingThreads.size(), threadFun, args, curStackBase, defaultTLSSize, &defaultAttr);
        }
        else{
            curAttr = existingAttr[createThreadAttr()];
            curAttr->preemptive = defThreadInfo[routineName].preemptive;
            curAttr->schedPolicy = defThreadInfo[routineName].schedPolicy;
            curAttr->priority = defThreadInfo[routineName].priority;
            curAttr->deadline = defThreadInfo[routineName].deadline;
            th = new ThreadEmu(existingThreads.size(), threadFun, args, curStackBase, defaultTLSSize, curAttr);
        }
    }
    else{
        curAttr = existingAttr[attr];
        if(defThreadInfo.find(routineName) != defThreadInfo.end()){
            curAttr->preemptive = defThreadInfo[routineName].preemptive;
            curAttr->schedPolicy = defThreadInfo[routineName].schedPolicy;
            curAttr->priority = defThreadInfo[routineName].priority;
            curAttr->deadline = defThreadInfo[routineName].deadline;
        }
        th = new ThreadEmu(existingThreads.size(), threadFun, args, curStackBase, defaultTLSSize, curAttr);
    }
    existingThreads[existingThreads.size()] = th;

    //I schedule the thread if I can find a free processor
    #ifndef NDEBUG
    std::cerr << "Trying to schedule thread " << existingThreads.size() - 1 << " on a free processor" << std::endl;
    #endif
    if(!scheduleFreeProcessor(existingThreads[existingThreads.size() - 1])){
        //Ok, I haven't found any free processor; I try to determine
        //if there is a processor running a lower priority thread and, in case,
        //I replace it
        #ifndef NDEBUG
        std::cerr << "Trying to schedule thread " << existingThreads.size() - 1 << " through preemption" << std::endl;
        #endif
        if(!preemptLowerPrio(existingThreads[existingThreads.size() - 1])){
            #ifndef NDEBUG
            std::cerr << "Unable to preempt any thread, adding " << existingThreads.size() - 1 << " to the ready queue" << std::endl;
            #endif
            //Ok, nothing else I can do: lets add the thread to the correct
            //ready queue
            int curPrio = 0;
            if(curAttr != NULL){
                if(curAttr->schedPolicy == resp::SYSC_SCHED_EDF)
                    curPrio = resp::SYSC_PRIO_MAX + 1;
                else
                    curPrio = curAttr->priority;
            }
            #ifndef NDEBUG
            if(curPrio < resp::SYSC_PRIO_MIN || curPrio > resp::SYSC_PRIO_MAX + 1){
                THROW_EXCEPTION("Non valid priority value " << curPrio);
            }
            #endif
            readyQueue[curPrio].push_back(existingThreads[existingThreads.size() - 1]);
            if(curPrio == resp::SYSC_PRIO_MAX + 1)
                sort(readyQueue[curPrio].begin(), readyQueue[curPrio].end(), deadlineSort);
        }
    }

    #ifndef NDEBUG
    std::cerr << "Created thread Id = " << existingThreads.size() - 1 << \
                " stack size = " << curStackSize/1024 << "KB" << " stack base " << \
                std::showbase << std::hex << curStackBase << std::dec << " status " << \
                th->status << " address " << std::hex << std::showbase << threadFun <<  std::endl;
    #endif

    schedulingLockBusy = false;
    schedulingEvent.notify();

    return (existingThreads.size() - 1);
}

int deadlineMisses = 0;
int deadlineOk = 0;

int resp::getDeadlineMisses() {
    return deadlineMisses;
}

int resp::getDeadlineOk() {
    return deadlineOk;
}

void exitThread(unsigned int procId, void * retVal){
    //I have to see if there are available threads and to schedule on the processor which is being left empty.
    //I also check that, in case there are none, that there are no threads waiting,  otherwise there is
    //a deadlock
    while(schedulingLockBusy)
        wait(schedulingEvent);
    schedulingLockBusy = true;

    Processor & curProc = findProcessor(procId);

    int th = curProc.deSchedule();
    // Update statistics
    if(existingThreads[th]->attr->schedPolicy == resp::SYSC_SCHED_EDF){
        if( sc_time_stamp().to_double() > existingThreads[th]->attr->deadline*1e3 )
            deadlineMisses++;
        else
            deadlineOk++;
    }
    //I have to free the stack which was used by this thread
    #ifndef NDEBUG
    if(stacks.find(existingThreads[th]->stackBase) == stacks.end()){
        THROW_EXCEPTION("Unable to find stack " << std::showbase << std::hex << existingThreads[th]->stackBase << std::dec << " for thread " << th << " among the stacks");
    }
    #endif
    stacks.erase(existingThreads[th]->stackBase);
    existingThreads[th]->status = ThreadEmu::DEAD;

    if(!existingThreads[th]->isIRQ){
        existingThreads[th]->retVal = retVal;
        //Finally I have to awake all the threads which were wating on a join on this thread
        std::vector<ThreadEmu *>::iterator joinIter, joinEnd;
        for(joinIter = existingThreads[th]->joining.begin(),  joinEnd = existingThreads[th]->joining.end(); joinIter != joinEnd; joinIter++){
            #ifndef NDEBUG
            std::cerr << "Checking joined " << (*joinIter)->id << " joined on " << th << " waiting for the end of " << (*joinIter)->numJoined-1 << " others" << std::endl;
            #endif
            (*joinIter)->numJoined--;
            if((*joinIter)->numJoined == 0){
                (*joinIter)->status = ThreadEmu::READY;
                //I add the thread to the queue of ready threads
                int curPrio = 0;
                if((*joinIter)->attr != NULL){
                    if((*joinIter)->attr->schedPolicy == resp::SYSC_SCHED_EDF)
                        curPrio = resp::SYSC_PRIO_MAX + 1;
                    else
                        curPrio = (*joinIter)->attr->priority;
                }
                readyQueue[curPrio].push_back(*joinIter);
                if(curPrio == resp::SYSC_PRIO_MAX + 1)
                    sort(readyQueue[curPrio].begin(), readyQueue[curPrio].end(), deadlineSort);
            }
        }
        existingThreads[th]->joining.clear();

        #ifndef NDEBUG
        std::cerr << "Exiting thread " << th << " Return Value " << retVal << std::endl;
        #endif
    }
    else{
        //It is an IRQ thread, so I can freely deallocate it and the corresponding attribute
        deleteThreadAttr(existingThreads[th]->attr->id);
        #ifndef NDEBUG
        std::cerr << "Exiting IRQ thread " << th << std::endl;
        #endif
    }

    //Now I have to schedule a new thread on the just freed processor
    ThreadEmu * readTh = findReadyThread();
    if(readTh != NULL){
        curProc.schedule(readTh);
    }
    schedulingLockBusy = false;
    schedulingEvent.notify();
}

int createThreadAttr(){
    int newAttrId = 0;
    if(existingAttr.size() > 0)
        newAttrId = existingAttr.rbegin()->first + 1;
    existingAttr[newAttrId] = new AttributeEmu();
    existingAttr[newAttrId]->id = newAttrId;
    return newAttrId;
}

void deleteThreadAttr(int attr){
    if(existingAttr.find(attr) == existingAttr.end()){
        THROW_EXCEPTION("Cannot destroy attribute " << attr << " unable to find it");
    }
    delete existingAttr[attr];
    existingAttr.erase(attr);
}

void setStackSize(int attr, int stackSize){
    if(existingAttr.find(attr) == existingAttr.end()){
        THROW_EXCEPTION("Cannot set stack for attribute " << attr << " unable to find it");
    }

    #ifndef NDEBUG
    std::cerr << "Setting stack " << stackSize << " for attribute " << attr << std::endl;
    #endif

    existingAttr[attr]->stackSize = stackSize;
}

unsigned int getStackSize(int attr){
    if(existingAttr.find(attr) == existingAttr.end()){
        THROW_EXCEPTION("Cannot get stack for attribute " << attr << " unable to find it");
    }
    return existingAttr[attr]->stackSize;
}

int createKey(){
    keys++;
    return keys;
}

void setSpecific(unsigned int procId, int key, void *memArea){
    if(key > keys){
        THROW_EXCEPTION("Key " << key << " not existing");
    }

    Processor &curProc = findProcessor(procId);
    std::pair<int,  ThreadEmu *> keyId(key, curProc.runThread);
    threadSpecific[keyId] = memArea;
    #ifndef NDEBUG
    std::cerr << "Setting memory area " << std::hex << memArea << std::dec << " for key " << key << " thread " << curProc.runThread->id << std::endl;
    #endif
}

int getThreadId(unsigned int procId){
    Processor &curProc = findProcessor(procId);
    return curProc.runThread->id;
}

void *getSpecific(unsigned int procId, int key){
    if(key > keys){
        THROW_EXCEPTION("Key " << key << " not existing");
    }

    Processor &curProc = findProcessor(procId);
    std::pair<int,  ThreadEmu *> keyId(key, curProc.runThread);
    #ifndef NDEBUG
    if(threadSpecific.find(keyId) == threadSpecific.end()){
        THROW_EXCEPTION("Specific for association (key, thread) = (" << key << "," << curProc.runThread->id << ") not existing");
    }
    #endif
    return threadSpecific[keyId];
}

void join(int thId, unsigned int procId, int curThread_){
    //I have to check that thId has finished; in case I immediately return,  otherwise
    //I have to deschedule myself
    while(schedulingLockBusy)
        wait(schedulingEvent);
    schedulingLockBusy = true;
    #ifndef NDEBUG
    std::cerr << "Trying to join thread on " << thId << std::endl;
    #endif

    #ifndef NDEBUG
    if(existingThreads.find(thId) == existingThreads.end()){
        THROW_EXCEPTION("Unable to find thread " << thId << " on which I have to join");
    }
    #endif
    #ifndef NDEBUG
    if(existingThreads[thId] == NULL){
        THROW_EXCEPTION("Thread " << thId << " on which I have to join is existing but NULL");
    }
    #endif
    if(existingThreads[thId]->status == ThreadEmu::DEAD){
        //There is nothing to do since I do not consider the return value so far
        #ifndef NDEBUG
        std::cerr << "Thread " << thId << " is dead, so I do not block" << std::endl;
        #endif
        ;
    }
    else{
        //I have to deschedule this thread
        int curThread = 0;
        if(curThread_ < 0){
            curThread = findProcessor(procId).deSchedule();
            #ifndef NDEBUG
            if(curThread == thId)
                THROW_EXCEPTION("Trying to join thread " << thId << " on itself");
            #endif
            //Now I have to reschedule the other threads in the system
            ThreadEmu * readTh = findReadyThread();
            if(readTh != NULL){
                findProcessor(procId).schedule(readTh);
            }
        }
        else
            curThread = curThread_;
        #ifndef NDEBUG
        std::cerr << "Joining thread " << curThread << " waiting for the end of thread " << thId << std::endl;
        if(curThread < 0){
            THROW_EXCEPTION("Unable to find processor " << procId << " on which the current thread is running");
        }
        if(existingThreads.find(curThread) == existingThreads.end()){
            THROW_EXCEPTION("Unable to find thread " << curThread << " being run by the current processor");
        }
        #endif
        existingThreads[thId]->joining.push_back(existingThreads[curThread]);
        existingThreads[curThread]->numJoined++;
    }
    schedulingLockBusy = false;
    schedulingEvent.notify();
}

void joinAll(unsigned int procId){
    //I have to wait for all the threads in the system
    while(schedulingLockBusy)
        wait(schedulingEvent);
    schedulingLockBusy = true;

    int curThread = findProcessor(procId).deSchedule();
    ThreadEmu * readTh = findReadyThread();
    if(readTh != NULL){
        findProcessor(procId).schedule(readTh);
    }

    std::map<int, ThreadEmu *>::iterator allThIter,  allThEnd;
    for(allThIter = existingThreads.begin(),  allThEnd = existingThreads.end();
                                                                allThIter != allThEnd; allThIter++){
        if(allThIter->second->status != ThreadEmu::DEAD && allThIter->first != curThread){
            schedulingLockBusy = false;
            join(allThIter->first, procId, curThread);
            schedulingLockBusy = true;
        }
    }
    schedulingLockBusy = false;
    schedulingEvent.notify();
}

std::pair<void *, void *> readTLS(unsigned int procId){
    //I simply have to determine the TLS and return it;
    //I assume that the local storage for each thread is
    //located in its stack; in particular I leave the lower
    //2KB of the thread stack for the local storage
    Processor &curProc = findProcessor(procId);
    #ifndef NDEBUG
    if(curProc.runThread == NULL)
        THROW_EXCEPTION("Current processor " << procId << " is not running any thread");
    std::cerr << "Reading TLS for thread " << curProc.runThread->id << " TLS limit " << std::showbase << std::hex << (unsigned long)(curProc.runThread->stackBase - curProc.runThread->tlsSize) << std::dec << std::endl;
    #endif
    return std::pair<void *, void *>((void*)curProc.runThread->stackBase, (void*)(curProc.runThread->stackBase - curProc.runThread->tlsSize));
}

//Destroys a previously allocated mutex,  exception if the mutex does not exist
void destroyMutex(unsigned int procId, int mutex){
    if(existingMutex.find(mutex) == existingMutex.end()){
        THROW_EXCEPTION("Cannot destroy mutex " << mutex << " unable to find it");
    }

    #ifndef NDEBUG
    if(existingMutex[mutex]->status == MutexEmu::LOCKED || existingMutex[mutex]->waitingTh.size() > 0)
        THROW_EXCEPTION("Cannot destroy mutex " << mutex << " it is still locked");
    std::cerr << "Destroying mutex " << mutex << " Thread " << findProcessor(procId).runThread->id << std::endl;
    #endif

    delete existingMutex[mutex];
    existingMutex.erase(mutex);
}

int createMutex(unsigned int procId){
    int newMutId = 0;
    if(existingMutex.size() > 0){
        std::map<int, MutexEmu *>::iterator mutBeg, mutEnd;
        for(mutBeg = existingMutex.begin(), mutEnd = existingMutex.end(); mutBeg != mutEnd; mutBeg++){
            std::map<int, MutexEmu *>::iterator next = mutBeg;
            next++;
            if(next == mutEnd){
                newMutId = mutBeg->first + 1;
                break;
            }
            else if(mutBeg->first + 1 < next->first){
                newMutId = mutBeg->first + 1;
                break;
            }
        }
    }
    existingMutex[newMutId] = new MutexEmu();
    #ifndef NDEBUG
    if(procId != (unsigned int)-1)
        std::cerr << "Creating mutex " << newMutId << " Thread " << findProcessor(procId).runThread->id << std::endl;
    #endif

    return newMutId;
}

//Locks the mutex,  deschedules the thread if the mutex is busy; in case
//nonbl == true,  the function behaves as a trylock
bool lockMutex(int mutex, unsigned int procId, bool nonbl){
    while(schedulingLockBusy)
        wait(schedulingEvent);
    schedulingLockBusy = true;

    Processor &curProc = findProcessor(procId);

    if(curProc.runThread == NULL)
        THROW_EXCEPTION("Mutex " << mutex << ": Current processor " << procId << " is not running any thread");

    if(existingMutex.find(mutex) == existingMutex.end()){
        THROW_EXCEPTION(" Processor " << procId << " Thread " << curProc.runThread->id << " Cannot lock mutex " << mutex << " unable to find it");
    }

    if(existingMutex[mutex]->owner == curProc.runThread){
        //Reentrant mutex, I have to increment the recursive index
        existingMutex[mutex]->recursiveIndex++;
        schedulingLockBusy = false;
        schedulingEvent.notify();
        return true;
    }
    if(existingMutex[mutex]->status == MutexEmu::FREE){
        existingMutex[mutex]->status = MutexEmu::LOCKED;
        existingMutex[mutex]->owner = curProc.runThread;
        #ifndef NDEBUG
        if(existingMutex[mutex]->recursiveIndex != 0)
            THROW_EXCEPTION("Mutex " << mutex << " is free, but it has recursive index " << existingMutex[mutex]->recursiveIndex);
        std::cerr << "Thread " << curProc.runThread->id << " locking free mutex " << mutex << std::endl;
        #endif
        schedulingLockBusy = false;
        schedulingEvent.notify();
        return true;
    }
    if(nonbl){
        schedulingLockBusy = false;
        schedulingEvent.notify();
        return false;
    }

    #ifndef NDEBUG
    if(existingMutex[mutex]->owner->status == ThreadEmu::DEAD)
        THROW_EXCEPTION("Thread " << existingMutex[mutex]->owner->id << " died while being the owner of mutex " << mutex);
    std::cerr << "Thread " << curProc.runThread->id << " waiting on mutex " << mutex << std::endl;
    #endif
    //Finally here I have to deschedule the current thread since the mutex is busy
    int th = curProc.deSchedule();
    existingMutex[mutex]->waitingTh.push_back(existingThreads[th]);
    //Now I get the first ready thread and I schedule it on the processor
    ThreadEmu * readTh = findReadyThread();
    if(readTh != NULL){
        curProc.schedule(readTh);
    }
    schedulingLockBusy = false;
    schedulingEvent.notify();

    return false;
}

//Releases the lock on the mutex
int unLockMutex(int mutex, unsigned int procId){
    while(schedulingLockBusy)
        wait(schedulingEvent);
    schedulingLockBusy = true;

    if(existingMutex.find(mutex) == existingMutex.end()){
        THROW_EXCEPTION("Cannot free mutex " << mutex << " unable to find it");
    }

    //If the mutex is not locked I simply return
    if(existingMutex[mutex]->owner == NULL){
        #ifndef NDEBUG
        //I check that the status of the mutex is coherent
        if(existingMutex[mutex]->status != MutexEmu::FREE || existingMutex[mutex]->waitingTh.size() > 0){
            THROW_EXCEPTION("The mutex " << mutex << " is free, but its status is not coherent");
        }
        #endif

        schedulingLockBusy = false;
        schedulingEvent.notify();

        return -1;
    }
    //I check that I'm the owner of the mutex, otherwise I raise an exception
    Processor &curProc = findProcessor(procId);
    #ifndef NDEBUG
    if(curProc.runThread == NULL){
        THROW_EXCEPTION("Free mutex: the current processor " << curProc.procIf->getId() << " is executing a NULL thread??");
    }
    #endif
    if(existingMutex[mutex]->owner != curProc.runThread){
        THROW_EXCEPTION("Cannot free mutex " << mutex << " the owner " << existingMutex[mutex]->owner->id << " is not the current thread " << curProc.runThread->id);
    }
    //Now I check to see if I'm in a recursive situation
    if(existingMutex[mutex]->recursiveIndex > 0){
        #ifndef NDEBUG
        std::cerr << "Exiting recursive mutex, current index " << existingMutex[mutex]->recursiveIndex << std::endl;
        #endif
        existingMutex[mutex]->recursiveIndex--;

        schedulingLockBusy = false;
        schedulingEvent.notify();

        return -1;
    }
    //Now I have to awake the first thread which is waiting on the mutex
    if(existingMutex[mutex]->waitingTh.size() > 0){
        ThreadEmu * toAwakeTh = existingMutex[mutex]->waitingTh.front();
        existingMutex[mutex]->waitingTh.pop_front();
        toAwakeTh->status = ThreadEmu::READY;
        addReadyThread(toAwakeTh);
        existingMutex[mutex]->owner = toAwakeTh;
        #ifndef NDEBUG
        std::cerr << "Thread " << curProc.runThread->id << " giving mutex " << mutex << " to thread " << toAwakeTh->id << std::endl;
        #endif

        schedulingLockBusy = false;
        schedulingEvent.notify();

        return toAwakeTh->id;
    }
    else{
        #ifndef NDEBUG
        std::cerr << "Thread " << curProc.runThread->id << " releasing mutex " << mutex << std::endl;
        #endif
        //Finally I can clear the mutex status
        existingMutex[mutex]->status = MutexEmu::FREE;
        existingMutex[mutex]->owner = NULL;
        existingMutex[mutex]->waitingTh.clear();

        schedulingLockBusy = false;
        schedulingEvent.notify();

        return -1;
    }
    schedulingLockBusy = false;
    schedulingEvent.notify();
    return -1;
}

int createSem(unsigned int procId, int initialValue){
    int newSemId = 0;
    if(existingSem.size() > 0){
        std::map<int, SemaphoreEmu *>::iterator semBeg, semEnd;
        for(semBeg = existingSem.begin(), semEnd = existingSem.end(); semBeg != semEnd; semBeg++){
            std::map<int, SemaphoreEmu *>::iterator next = semBeg;
            next++;
            if(next == semEnd){
                newSemId = semBeg->first + 1;
                break;
            }
            else if(semBeg->first + 1 < next->first){
                newSemId = semBeg->first + 1;
                break;
            }
        }
    }
    existingSem[newSemId] = new SemaphoreEmu(initialValue);
    #ifndef NDEBUG
    std::cerr << "Creating semaphore " << newSemId << " Thread " << findProcessor(procId).runThread->id << std::endl;
    #endif
    return newSemId;
}

void destroySem(unsigned int procId, int sem){
    if(existingSem.find(sem) == existingSem.end()){
        THROW_EXCEPTION("Cannot destroy semaphore " << sem << " unable to find it");
    }
    #ifndef NDEBUG
    if(existingSem[sem]->waitingTh.size() > 0)
        THROW_EXCEPTION("Cannot destroy semaphore " << sem << " there are waiting threads");
    std::cerr << "Destroying semaphore " << sem << " Thread " << findProcessor(procId).runThread->id << std::endl;
    #endif
    delete existingSem[sem];
    existingSem.erase(sem);
}

void postSem(int sem, unsigned int procId){
    while(schedulingLockBusy)
        wait(schedulingEvent);
    schedulingLockBusy = true;

    //I simply have to increment the value of the semaphore; I then check if there are
    //threads waiting on the semaphore and I awake them in case
    if(existingSem.find(sem) == existingSem.end()){
        THROW_EXCEPTION("Cannot post semaphore " << sem << " unable to find it");
    }

    #ifndef NDEBUG
    if(existingSem[sem]->waitingTh.size() > 0 && existingSem[sem]->value > 0){
        THROW_EXCEPTION("Semaphore " << sem << " has value " << existingSem[sem]->value << " but there are waiting threads");
    }
    #endif
    if(existingSem[sem]->waitingTh.size() > 0){
        ThreadEmu * toAwakeTh = existingSem[sem]->waitingTh.front();
        existingSem[sem]->waitingTh.pop_front();
        toAwakeTh->status = ThreadEmu::READY;
        addReadyThread(toAwakeTh);
        #ifndef NDEBUG
        Processor &curProc = findProcessor(procId);
        std::cerr << "Thread " << curProc.runThread->id << " giving semaphore " << sem << " to thread " << toAwakeTh->id << std::endl;
        #endif
    }
    else
        existingSem[sem]->value++;
    schedulingLockBusy = false;
    schedulingEvent.notify();
}

void waitSem(int sem, unsigned int procId){
    while(schedulingLockBusy)
        wait(schedulingEvent);
    schedulingLockBusy = true;

    //If the value of the semaphore is 0 I wait,  otherwise the value gets
    //decremented and I can return
    if(existingSem.find(sem) == existingSem.end()){
        THROW_EXCEPTION("Cannot post semaphore " << sem << " unable to find it");
    }

    if(existingSem[sem]->value > 0)
        existingSem[sem]->value--;
    else{
        //Finally here I have to deschedule the current thread since the semaphore is empty
        Processor &curProc = findProcessor(procId);
        int th = curProc.deSchedule();
        existingSem[sem]->waitingTh.push_back(existingThreads[th]);
        #ifndef NDEBUG
        std::cerr << "DeScheduling thread " << th << " because semaphore " << sem << " has value " << existingSem[sem]->value << std::endl;
        #endif
        //Now I get the first ready thread and I schedule it on the processor
        ThreadEmu * readTh = findReadyThread();
        if(readTh != NULL){
            curProc.schedule(readTh);
        }
    }
    schedulingLockBusy = false;
    schedulingEvent.notify();
}

void returnFromCond(ThreadEmu *thread, MutexEmu *mutex){
    if(mutex->owner == thread){
        //Reentrant mutex, I have to increment the recursive index
        mutex->recursiveIndex++;
        thread->status = ThreadEmu::READY;
        addReadyThread(thread);
    }
    else if(mutex->status == MutexEmu::FREE){
        // Free mutex
        mutex->status = MutexEmu::LOCKED;
        mutex->owner = thread;
        #ifndef NDEBUG
        std::cerr << "Thread " << mutex->owner->id << std::endl;
        #endif
        thread->status = ThreadEmu::READY;
        addReadyThread(thread);
    }
    else{
        #ifndef NDEBUG
        std::cerr << "Thread " << thread->id << std::endl;
        #endif
        //Mutex already locked
        mutex->waitingTh.push_back(thread);
    }
}


TimeKeeper::TimeKeeper(sc_time accuracy) : sc_module("TimeKeeper"), accuracy(accuracy){
    SC_THREAD(countTime);
    end_module();
}
void TimeKeeper::countTime(){
    while(true){
        while(timedThreads.size() == 0)
            wait(this->startCount);

        //I have to check whether there are expired conditions and
        //in case make them return with an error
        std::map<ThreadEmu *, std::pair<double, int> > newList;
        std::map<ThreadEmu *, std::pair<double, int> >::iterator thIter, thEnd;
        for(thIter = timedThreads.begin(), thEnd = timedThreads.end(); thIter != thEnd; thIter++){
            if(thIter->second.first > sc_time_stamp().to_double()){
                while(schedulingLockBusy)
                    wait(schedulingEvent);
                schedulingLockBusy = true;
                thIter->first->setSyscRetVal = true;
                thIter->first->syscallRetVal = ETIMEDOUT;
                //I also have to move the thread back in the ready queue
                returnFromCond(thIter->first, existingMutex[thIter->second.second]);
                schedulingLockBusy = false;
                schedulingEvent.notify();
            }
            else
                newList.insert(*thIter);
        }
        timedThreads = newList;

        wait(this->accuracy);
    }
}

int createCond(unsigned int procId){
    int newCondId = 0;
    if(existingCond.size() > 0){
        std::map<int, ConditionEmu *>::iterator condBeg, condEnd;
        for(condBeg = existingCond.begin(), condEnd = existingCond.end(); condBeg != condEnd; condBeg++){
            std::map<int, ConditionEmu *>::iterator next = condBeg;
            next++;
            if(next == condEnd){
                newCondId = condBeg->first + 1;
                break;
            }
            else if(condBeg->first + 1 < next->first){
                newCondId = condBeg->first + 1;
                break;
            }
        }
    }

    existingCond[newCondId] = new ConditionEmu();
    return newCondId;
}

void destroyCond(unsigned int procId, int cond){
    if(existingCond.find(cond) == existingCond.end()){
        THROW_EXCEPTION("Cannot destroy condition variable " << cond << ": unable to find it");
    }

    delete existingCond[cond];
    existingCond.erase(cond);
}

void signalCond(int cond, bool broadcast, unsigned int procId){
    //I simply have to awake the threads waiting on this condition;
    //note that when the thread is awaken, it has to acquire the mutex
    //associated with the condition variable; in case it cannot it
    //is not awaken, but simply moved in the queue of the
    //ccoresponding mutex
    #ifndef NDEBUG
    std::cerr << "waiting for lock to Signal condition variable " << cond << std::endl;
    #endif

    while(schedulingLockBusy)
        wait(schedulingEvent);
    schedulingLockBusy = true;

    #ifndef NDEBUG
    std::cerr << "Signaling condition variable " << cond << std::endl;
    #endif

    if(existingCond.find(cond) == existingCond.end()){
        THROW_EXCEPTION("Cannot signal condition variable " << cond << ": unable to find it");
    }

    if(existingCond[cond]->waitingTh.size() > 0){
        MutexEmu * mutex = NULL;
        if(existingCond[cond]->mutex == -1)
            THROW_EXCEPTION("Mutex associated to conditional variable " << cond << " is equal to -1");
        else{
            #ifndef NDEBUG
            if(existingMutex.find(existingCond[cond]->mutex) == existingMutex.end()){
                THROW_EXCEPTION("Cannot get mutex " << existingCond[cond]->mutex << " unable to find it in condition variable " << cond);
            }
            #endif
            mutex = existingMutex[existingCond[cond]->mutex];
        }
        if(broadcast){
            std::list<ThreadEmu *>::iterator thIter, thEnd;
            thIter = existingCond[cond]->waitingTh.begin();
            std::map<ThreadEmu *, std::pair<double, int> >::iterator timedIter = timedThreads.find(*thIter);
            if(timedIter != timedThreads.end())
                timedThreads.erase(timedIter);
            //Now I have to check if the mutex is free for the first thread and
            //lock it in case
            returnFromCond(*thIter, mutex);

            for(thIter++, thEnd = existingCond[cond]->waitingTh.end(); thIter != thEnd; thIter++){
                //Of course the mutex is busy for all the other threads since
                //the first one has just locked it: I simply put those
                //threads in the waiting queue of the mutex
                mutex->waitingTh.push_back((*thIter));
                timedIter = timedThreads.find(*thIter);
                if(timedIter != timedThreads.end())
                    timedThreads.erase(timedIter);
            }
            existingCond[cond]->waitingTh.clear();
            existingCond[cond]->mutex = -1;
        }
        else{
            ThreadEmu * toAwakeTh = existingCond[cond]->waitingTh.front();
            existingCond[cond]->waitingTh.pop_front();
            std::map<ThreadEmu *, std::pair<double, int> >::iterator timedIter = timedThreads.find(toAwakeTh);
            if(timedIter != timedThreads.end())
                timedThreads.erase(timedIter);

            returnFromCond(toAwakeTh, mutex);

            if(existingCond[cond]->waitingTh.size() == 0)
                existingCond[cond]->mutex = -1;

            #ifndef NDEBUG
            Processor &curProc = findProcessor(procId);
            std::cerr << "Thread " << curProc.runThread->id << " awaking thread " << toAwakeTh->id << std::endl;
            #endif
        }
    }
    schedulingLockBusy = false;
    schedulingEvent.notify();
}

int waitCond(int cond, int mutex, double time, unsigned int procId){
    while(schedulingLockBusy)
        wait(schedulingEvent);
    schedulingLockBusy = true;

    #ifndef NDEBUG
    if(time > 0 && tk == NULL)
        THROW_EXCEPTION("Trying to do a timed wait on a condition variable while the Time Keeper is not existing");
    #endif
    if(existingCond.find(cond) == existingCond.end()){
        THROW_EXCEPTION("Cannot wait on condition variable " << cond << ": unable to find it");
    }
    if(existingCond[cond]->mutex != -1 && existingCond[cond]->mutex != mutex){
        #ifndef NDEBUG
        std::cerr << "Condition variable " << cond << " has assigned mutex " << existingCond[cond]->mutex << " but proc " << procId << " is trying to use mutex " << mutex << std::endl;
        #endif
        return EINVAL;
    }

    if(existingCond[cond]->mutex == -1)
        existingCond[cond]->mutex = mutex;
    #ifndef NDEBUG
    std::cerr << "Issn condition variable " << cond << " is going to unlock mutex " << existingCond[cond]->mutex << std::endl;
    #endif
    schedulingLockBusy = false;
    unLockMutex(mutex, procId);
    schedulingLockBusy = true;

    Processor &curProc = findProcessor(procId);
    int th = curProc.deSchedule();
    if(time <= 0 || sc_time_stamp().to_double() >= time*1000)
        existingCond[cond]->waitingTh.push_back(existingThreads[th]);
    #ifndef NDEBUG
    std::cerr << "DeScheduling thread " << th << " because waiting on condition variable " << cond << std::endl;
    #endif
    //Now I get the first ready thread and I schedule it on the processor
    ThreadEmu * readTh = findReadyThread();
    if(readTh != NULL){
        curProc.schedule(readTh);
    }

    if(time > 0){
        if(sc_time_stamp().to_double() >= time*1000){
            #ifndef NDEBUG
            std::cerr << "Thread " << th << " not waiting on condition variable " << cond << " beacause " << time << " has elapsed :-) " << std::endl;
            #endif
            //I set the error return value
            existingThreads[th]->setSyscRetVal = true;
            existingThreads[th]->syscallRetVal = ETIMEDOUT;
            //And I move back the thread in the ready queue
            returnFromCond(existingThreads[th], existingMutex[mutex]);
        }
        else{
            #ifndef NDEBUG
            std::cerr << "Thread " << th << " time waiting on condition variable " << cond << " time " << time << std::endl;
            #endif
            std::pair<ThreadEmu *, std::pair<double, int> > tTh(existingThreads[th], std::pair<double, int>(time*1000, mutex));
            timedThreads.insert(tTh);
            tk->startCount.notify();
        }
    }
    schedulingLockBusy = false;
    schedulingEvent.notify();

    return 0;
}

void idleLoop(unsigned int procId){
    readyARMProc[procId] = true;
    wait(*(findProcessor(procId).idleEvent));
    readyARMProc[procId] = false;
}

void setPreemptive(int attr, int isPreemptive){
    if(existingAttr.find(attr) == existingAttr.end()){
        THROW_EXCEPTION("Cannot set preemptive status for attribute " << attr << " unable to find it");
    }

    #ifndef NDEBUG
    std::cerr << "Setting preemptive status " << isPreemptive << " for attribute " << attr << std::endl;
    #endif

    existingAttr[attr]->preemptive = (isPreemptive == resp::SYSC_PREEMPTIVE);
}

int getPreemptive(int attr){
    if(existingAttr.find(attr) == existingAttr.end()){
        THROW_EXCEPTION("Cannot get preemptive status for attribute " << attr << " unable to find it");
    }
    return existingAttr[attr]->preemptive ? resp::SYSC_PREEMPTIVE : resp::SYSC_NON_PREEMPTIVE;
}

void setSchedDeadline(int attr, unsigned int deadline){
    if(existingAttr.find(attr) == existingAttr.end()){
        THROW_EXCEPTION("Cannot set deadline for attribute " << attr << " unable to find it");
    }

    #ifndef NDEBUG
    std::cerr << "Setting deadline " << deadline << " for attribute " << attr << std::endl;
    #endif

    existingAttr[attr]->deadline = deadline;
}

unsigned int getSchedDeadline(int attr){
    if(existingAttr.find(attr) == existingAttr.end()){
        THROW_EXCEPTION("Cannot get deadline for attribute " << attr << " unable to find it");
    }
    return existingAttr[attr]->deadline;
}

void setSchedPrio(int attr, int priority){
    if(existingAttr.find(attr) == existingAttr.end()){
        THROW_EXCEPTION("Cannot set priority for attribute " << attr << " unable to find it");
    }

    #ifndef NDEBUG
    std::cerr << "Setting priority " << priority << " for attribute " << attr << std::endl;
    #endif

    existingAttr[attr]->priority = priority;
}

int getSchedPrio(int attr){
    if(existingAttr.find(attr) == existingAttr.end()){
        THROW_EXCEPTION("Cannot get deadline for attribute " << attr << " unable to find it");
    }
    return existingAttr[attr]->priority;
}

void setSchedPolicy(int attr, int policy){
    if(existingAttr.find(attr) == existingAttr.end()){
        THROW_EXCEPTION("Cannot set scheduler policy for attribute " << attr << " unable to find it");
    }

    #ifndef NDEBUG
    std::cerr << "Setting scheduler policy " << policy << " for attribute " << attr << std::endl;
    #endif

    existingAttr[attr]->schedPolicy = policy;
}

int getSchedPolicy(int attr){
    if(existingAttr.find(attr) == existingAttr.end()){
        THROW_EXCEPTION("Cannot get scheduler policy for attribute " << attr << " unable to find it");
    }
    return existingAttr[attr]->schedPolicy;
}

int getThreadSchePolicy(int threadId){
    if(existingThreads.find(threadId) == existingThreads.end()){
        THROW_EXCEPTION("Cannot get scheduler policy for thread " << threadId << " unable to find it");
    }
    if(existingThreads[threadId]->attr != NULL){
        return existingThreads[threadId]->attr->schedPolicy;
    }
    else{
        return resp::SYSC_SCHED_RR;
    }
}

int getThreadSchedPrio(int threadId){
    if(existingThreads.find(threadId) == existingThreads.end()){
        THROW_EXCEPTION("Cannot get scheduler priority for thread " << threadId << " unable to find it");
    }
    if(existingThreads[threadId]->attr != NULL){
        return existingThreads[threadId]->attr->priority;
    }
    else{
        return resp::SYSC_PRIO_MIN;
    }
}

unsigned int getThreadSchedDeadline(int threadId){
    if(existingThreads.find(threadId) == existingThreads.end()){
        THROW_EXCEPTION("Cannot get scheduler deadline for thread " << threadId << " unable to find it");
    }
    if(existingThreads[threadId]->attr != NULL){
        return existingThreads[threadId]->attr->deadline;
    }
    else{
        return 0;
    }
}

void setThreadSchePolicy(int threadId, int policy){
    if(existingThreads.find(threadId) == existingThreads.end()){
        THROW_EXCEPTION("Cannot set scheduler policy for thread " << threadId << " unable to find it");
    }
    if(existingThreads[threadId]->attr != NULL){
        existingThreads[threadId]->attr->schedPolicy = policy;
    }
    else{
        // I have to create a new attribute and then I can set the scheduling
        // policy
        int newAttrId = 0;
        if(existingAttr.size() > 0)
            newAttrId = existingAttr.rbegin()->first + 1;
        existingAttr[newAttrId] = new AttributeEmu();
        existingThreads[threadId]->attr = existingAttr[newAttrId];
        existingAttr[newAttrId]->schedPolicy = policy;
    }
}

void setThreadSchedPrio(int threadId, int priority){
    if(existingThreads.find(threadId) == existingThreads.end()){
        THROW_EXCEPTION("Cannot set scheduler priority for thread " << threadId << " unable to find it");
    }
    if(existingThreads[threadId]->attr != NULL){
        existingThreads[threadId]->attr->priority = priority;
    }
    else{
        // I have to create a new attribute and then I can set the scheduling
        // policy
        int newAttrId = 0;
        if(existingAttr.size() > 0)
            newAttrId = existingAttr.rbegin()->first + 1;
        existingAttr[newAttrId] = new AttributeEmu();
        existingThreads[threadId]->attr = existingAttr[newAttrId];
        existingAttr[newAttrId]->priority = priority;
    }
}

void setThreadSchedDeadline(int threadId, unsigned int deadline){
    if(existingThreads.find(threadId) == existingThreads.end()){
        THROW_EXCEPTION("Cannot set scheduler deadline for thread " << threadId << " unable to find it");
    }
    if(existingThreads[threadId]->attr != NULL){
        existingThreads[threadId]->attr->deadline = deadline;
    }
    else{
        // I have to create a new attribute and then I can set the scheduling
        // policy
        int newAttrId = 0;
        if(existingAttr.size() > 0)
            newAttrId = existingAttr.rbegin()->first + 1;
        existingAttr[newAttrId] = new AttributeEmu();
        existingThreads[threadId]->attr = existingAttr[newAttrId];
        existingAttr[newAttrId]->deadline = deadline;
    }
}

TimeSliceClock::TimeSliceClock(const sc_time &resolution) : sc_module("TimeSliceClock"), resolution(resolution) {
    SC_THREAD(preempt);
    end_module();
}
void TimeSliceClock::preempt(){
    while(true){
        wait(this->resolution);

        bool isProcReady = true;
        for(int i = 0; i < maxProcId; i++){
            #ifndef NDEBUG
            std::cerr << "ts readyARMProc[" << i << "] = " << readyARMProc[i] << " PC = " << findProcessor(i).procIf->getPC() << std::endl;
            #endif
            isProcReady &= readyARMProc[i];
        }
        while(!isProcReady){
            #ifndef NDEBUG
            std::cerr << "timeslicing: not preempting because a processor is busy" << std::endl;
            #endif
            wait(armProcReadEvent);
            isProcReady = true;
            for(int i = 0; i < maxProcId; i++){
                #ifndef NDEBUG
                std::cerr << "ts readyARMProc[" << i << "] = " << readyARMProc[i] << " PC = " << findProcessor(i).procIf->getPC() << std::endl;
                #endif
                isProcReady &= readyARMProc[i];
            }
        }

        while(timeSliceing)
            wait(doingTimeSlice);
        timeSliceing = true;

        while(schedulingLockBusy)
            wait(schedulingEvent);
        schedulingLockBusy = true;

        #ifndef NDEBUG
        //std::cerr << "TimeSliceing" << std::endl;
        #endif
        enableWait = false;
        //Ok, the specified timeslice has elapsed: I have to preempt all round-robin
        //tasks and then re-schedule the tasks on the just freed processors.
        std::list<Processor>::iterator procIter, procIterEnd;
        for(procIter = existingProc.begin(), procIterEnd = existingProc.end(); procIter != procIterEnd; procIter++){
            if(procIter->runThread != NULL){
                //I preempt a task if it has the round-robin policy and or if no policy was set
                //I consider that it is preemptive
                if(procIter->runThread->attr == NULL || procIter->runThread->attr->schedPolicy == resp::SYSC_SCHED_RR || procIter->runThread->attr->schedPolicy == resp::SYSC_SCHED_EDF){
                    //Lets deschedule the old thread and schedule the new one.
                    int curPrio = 0;
                    if(procIter->runThread->attr != NULL){
                        if(procIter->runThread->attr->schedPolicy == resp::SYSC_SCHED_EDF)
                            curPrio = resp::SYSC_PRIO_MAX + 1;
                        else
                            curPrio = procIter->runThread->attr->priority;
                    }
                    readyQueue[curPrio].push_back(procIter->runThread);
                    if(curPrio == resp::SYSC_PRIO_MAX + 1)
                        sort(readyQueue[curPrio].begin(), readyQueue[curPrio].end(), deadlineSort);
                    int thId = procIter->deSchedule();
                    existingThreads[thId]->status = ThreadEmu::READY;
                    #ifndef NDEBUG
                    std::cerr << "Descheduling " << thId << " " << readyQueue[resp::SYSC_PRIO_MAX + 1].size() << " elements in deadline queue" << std::endl;
                    #endif
                }
            }
        }
        // Now I can restart with scheduling the processes on the idle processors
        for(procIter = existingProc.begin(), procIterEnd = existingProc.end(); procIter != procIterEnd; procIter++){
            if(procIter->runThread == NULL){
                #ifndef NDEBUG
                std::cerr << "Looking for ready threads" << std::endl;
                #endif
                ThreadEmu * readTh = findReadyThread();
                if(readTh != NULL){
                    #ifndef NDEBUG
                    std::cerr << "found thread " << readTh->id << std::endl;
                    #endif
                    procIter->schedule(readTh);
                }
            }
        }
        enableWait = true;

        schedulingLockBusy = false;
        schedulingEvent.notify();

        timeSliceing = false;
        doingTimeSlice.notify();
    }
}

IRQClock::IRQClock(const sc_time &resolution) : sc_module("IRQClock"), resolution(resolution), generator(static_cast<int>(std::time(NULL))){
    SC_THREAD(schedulerIRQ);
    end_module();
}
void IRQClock::schedulerIRQ(){
    wait(this->resolution/2);
    while(true){
        wait(this->resolution);
        if(isrAddresses.size() == 0)
            continue;

        bool isProcReady = true;
        for(int i = 0; i < maxProcId; i++){
            #ifndef NDEBUG
            std::cerr << "irq eadyARMProc[" << i << "] = " << readyARMProc[i] << " PC = " << findProcessor(i).procIf->getPC() << std::endl;
            #endif
            isProcReady &= readyARMProc[i];
        }
        while(!isProcReady){
            #ifndef NDEBUG
            std::cerr << "IRQ: not throwing because a processor is busy" << std::endl;
            #endif
            wait(armProcReadEvent);
            isProcReady = true;
            for(int i = 0; i < maxProcId; i++){
                #ifndef NDEBUG
                std::cerr << "irq readyARMProc[" << i << "] = " << readyARMProc[i] << " PC = " << findProcessor(i).procIf->getPC() << std::endl;
                #endif
                isProcReady &= readyARMProc[i];
            }
        }
        while(timeSliceing)
            wait(doingTimeSlice);
        timeSliceing = true;
        // I simply have to randomly find a number, corresponding to the
        // current interrupt id. Then I create a new task with the main
        // function corresponding to the function representing the interrupt
        // Eventually the interrupt will be scheduled on a processor
        // Note that I also use the attributes already set for this
        // task
        enableWait = false;
        #ifndef NDEBUG
        std::cerr << "Throwing IRQ" << std::endl;
        #endif
        std::map<int, unsigned int>::iterator isrAddressesIter = isrAddresses.begin();
        boost::uniform_int<> degen_dist(0, isrAddresses.size() - 1);
        boost::variate_generator<boost::minstd_rand&, boost::uniform_int<> > deg(this->generator, degen_dist);
        int randomVal = deg();
        for(int i = 0; i < randomVal; i++, isrAddressesIter++);
        int newIrqTaskId = createThread((thread_routine_t)(isrAddressesIter->second), NULL, -1);
        existingThreads[newIrqTaskId]->isIRQ = true;
        #ifndef NDEBUG
        std::cerr << "Created IRQ " << isrAddressesIter->first << " corresponding task ID " << newIrqTaskId << std::endl;
        #endif
        enableWait = true;
        timeSliceing = false;
        doingTimeSlice.notify();
    }
}

EventClock::EventClock() : sc_module("EventClock"), generator(static_cast<unsigned int>(std::time(NULL))){
    SC_THREAD(schedulerEvent);
    end_module();
}
void EventClock::schedulerEvent(){
    while(true){
        //Here I choose the random number
        boost::uniform_real<> uni_dist(0.5, 1.5);
        boost::variate_generator<boost::minstd_rand&, boost::uniform_real<> > uni(this->generator, uni_dist);
        wait(uni()*eventFreq);
        if(eventAddresses.size() == 0)
            continue;

        bool isProcReady = true;
        for(unsigned int i = 0; i < maxProcId; i++){
            #ifndef NDEBUG
            std::cerr << "irq eadyARMProc[" << i << "] = " << readyARMProc[i] << " PC = " << findProcessor(i).procIf->getPC() << std::endl;
            #endif
            isProcReady &= readyARMProc[i];
        }
        while(!isProcReady){
            #ifndef NDEBUG
            std::cerr << "IRQ: not throwing because a processor is busy" << std::endl;
            #endif
            wait(armProcReadEvent);
            isProcReady = true;
            for(int i = 0; i < maxProcId; i++){
                #ifndef NDEBUG
                std::cerr << "irq readyARMProc[" << i << "] = " << readyARMProc[i] << " PC = " << findProcessor(i).procIf->getPC() << std::endl;
                #endif
                isProcReady &= readyARMProc[i];
            }
        }
        while(timeSliceing)
            wait(doingTimeSlice);
        timeSliceing = true;
        // I simply have to randomly find a number, corresponding to the
        // current interrupt id. Then I create a new task with the main
        // function corresponding to the function representing the interrupt
        // Eventually the interrupt will be scheduled on a processor
        // Note that I also use the attributes already set for this
        // task
        enableWait = false;
        #ifndef NDEBUG
        std::cerr << "Throwing Event" << std::endl;
        #endif
        boost::uniform_int<> degen_dist(0, eventAddresses.size() - 1);
        boost::variate_generator<boost::minstd_rand&, boost::uniform_int<> > deg(this->generator, degen_dist);
        int randomVal = deg();
        int newIrqTaskId = createThread((thread_routine_t)(eventAddresses[deg()]), NULL, -1);
        existingThreads[newIrqTaskId]->isIRQ = true;
        enableWait = true;
        timeSliceing = false;
        doingTimeSlice.notify();
    }
}
