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

#ifndef CONCURRENCY_MANAGER_HPP
#define CONCURRENCY_MANAGER_HPP

#include <ABIIf.hpp>

#include <systemc.h>

#include <map>
#include <string>
#include <list>
#include <deque>

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

#include "concurrency_structures.hpp"

///Variables necessary to implement a SystemC lock:
///the boolean variables is used to check the status of
///the lock, while the event is used to awake a waiting
///sc_thread.
struct SysCLock{
    private:
    sc_event awakeEvent;
    bool busy;
    public:
    SysCLock() : busy(false){}
    void lock(){
        while(this->busy)
            wait(this->awakeEvent);
        this->busy = true;
    }
    void unlock(){
        this->busy = false;
        this->awakeEvent.notify();
    }
};

///High level representation of a processor; it contains also the
///methods to manage the scheduling/unscheduling of the software running
///on it
template <class wordSize> struct Processor{
    trap::ABIIf<wordSize> &processorInstance;
    ThreadEmu * runThread;
    std::vector<std::pair<double, double> > idleTrace;
    double curIdleStart;
    
    // Idle event
    sc_event idleEvent;

    Processor(trap::ABIIf<wordSize> &processorInstance) : processorInstance(processorInstance){
        runThread = NULL;
        curIdleStart = -1;
    }
    ~Processor(){
    }

    void schedule(ThreadEmu * thread){
        std::vector< wordSize > args;
        args.push_back(thread->args);
        //TODO: ok, here, when scheduling the thread, we also have to remeber to
        //set the thread returtn value in case it was joined!!

        // Set thread state
        this->runThread = thread;
        this->runThread->status = ThreadEmu::RUNNING;
        
        // Schedule the thread
        // Case 1, new thread
        if( thread->state == NULL ) {
            this->processorInstance.setPC(thread->thread_routine);
            this->processorInstance.setArgs(args);
            this->processorInstance.setSP(thread->stackBase);
            this->processorInstance.setFP(thread->stackBase);
            this->processorInstance.setLR(thread->lastRetAddr);
        
        // Case 2, already existing thread
        } else {
            this->processorInstance.setState(thread->state);
            if( thread->setSyscRetVal ) {
                thread->setSyscRetVal = false;
                this->processorInstance.setRetVal(thread->syscallRetVal);
            }
        }

        // Notify halted processors
        this->idleEvent.notify();
        
    }
    int deSchedule(wordSize nop_loop_address, bool saveStatus = true){
        if( this->runThread == NULL ) {
            THROW_EXCEPTION("Trying to deschedule a NULL thread");
        }
        ThreadEmu *tempThread = this->runThread;
        this->runThread = NULL;
        
        if(saveStatus) {
            tempThread->status = ThreadEmu::WAITING;
            tempThread->state = this->processorInstance.getState();
            tempThread->lastPc = this->processorInstance.readPC();
            tempThread->lastRetAddr = this->processorInstance.readLR();
        }

        //this->processorInstance.clearState();
        this->processorInstance.setPC(nop_loop_address);
        this->processorInstance.setLR(nop_loop_address);

        return tempThread->id; 

    }
    void printIdleTrace(){
    }
    std::string getIdleTraceCSV(){
    }
};

///***************************************************************
/// Here we declare some classes which will be used as timers
///
///***************************************************************
class TimeSliceClock : public sc_module{
private:
    sc_time resolution;
public:
    SC_HAS_PROCESS(TimeSliceClock);
    TimeSliceClock(const sc_time &resolution);
    void preempt();
};

class IRQClock : public sc_module{
private:
    sc_time resolution;
    boost::minstd_rand generator;
public:
    SC_HAS_PROCESS(IRQClock);
    IRQClock(const sc_time &resolution);
    void schedulerIRQ();
};

class EventClock : public sc_module{
private:
    boost::minstd_rand generator;
public:
    SC_HAS_PROCESS(EventClock);
    EventClock();
    void schedulerEvent();
};

class CondVarClock : public sc_module{
public:
    sc_event startCount;
    sc_time accuracy;

    SC_HAS_PROCESS(CondVarClock);

    CondVarClock(sc_time accuracy);

    void countTime();
};

namespace resp{

///Main class of the concurrency manager: it is in charge of implementing the
///behavior of the thread related constructs, includuing thread creation, destruction,
///synchronization management (mutex, condition variables, semaphores, etc.)
class ConcurrencyManager{
    private:
        ///The name of the executable file which is managed by this version of the
        ///concurrency manager
        std::string execName;

        /// System memory size, used for allocating stacks and TLSs
        unsigned int memSize;
    public:
        ///Variables used for managing reentrant synchronization
        unsigned int mallocMutex;
        unsigned int sfpMutex;
        unsigned int sinitMutex;
        unsigned int fpMutex;

    private:
        ///Variables used to actually manage synchronization and scheduling
        ///among processes and threads

        ///TODO .... we are fixing this template, we should find an elegant way of
        ///setting the template dynamically
        std::map<unsigned int, Processor<unsigned int>* > managedProc;
        unsigned int maxProcId;
        
        ///Note that the last element of the readyQueue, the one containing
        ///the tasks with EDF schedule, must be ordered on the
        ///schedule deadline: the first element the one with closest
        ///deadline, the last one with the furthest one
        std::deque<ThreadEmu *> * readyQueue;
        
        ///Contains the allocated thread attributes
        std::map<int, AttributeEmu *> existingAttr;
        
        ///Contains the allocated threads; note that no thread
        ///is eliminated from the system (all threads can be joined
        ///and only one process is running, so we must keep track
        ///of all the threads forever)
        std::map<int, ThreadEmu *> existingThreads;

        /// Contains allocated mutexes
        std::map<int, MutexEmu *> existingMutex;
        
        ///Store address and size of the thread stacks (of course there is no
        ///way of determining if a thread has grown over its stack)
        std::map<unsigned int, unsigned int> stacks;
        
        ///Associates thread specific information with the threads
        std::map<std::pair<int, ThreadEmu *>, unsigned int> threadSpecific;
        
        ///Keeps track of all the keys used to associate a memory area with each thread (if needed)
        int keys;
        
        ///SystemC mutex variables used to maintain synchronization
        ///among the different sc_treads
        SysCLock schedLock;

        ///Some methods internally used to help concurrency management
        bool scheduleFreeProcessor(ThreadEmu *th);
        bool preemptLowerPrio(ThreadEmu *th);
        ThreadEmu * findReadyThread();
    public:
        /// Some constants
        static const int SYSC_SCHED_FIFO;
        static const int SYSC_SCHED_OTHER;
        static const int SYSC_SCHED_RR;
        static const int SYSC_SCHED_EDF;

        static const int SYSC_PRIO_MAX;
        static const int SYSC_PRIO_MIN;

        static const int SYSC_PREEMPTIVE;
        static const int SYSC_NON_PREEMPTIVE;

        /// Now some static stuff which is shared by all the emulation groups
        ///specifies whether blocked processor halts or keep on working in busy wait loops
        static bool busyWaitLoop;

        ///Specifies the stack size for each thread, defaults to 20 KB
        static unsigned int threadStackSize;
        
        ///Associates thred properties with a routine name: all threads
        ///created from that routine shall have such properties
        static std::map<std::string, DefaultThreadInfo> defThreadInfo;
        
        ///The registered interrupt service routines.
        static std::map<int, std::string> interruptServiceRoutines;
        
        ///The size and content of the thread-local-storage
        static unsigned int tlsSize;
        static unsigned char * tlsData;
        
        ///Default thread attribute, used to initialize threads which do not declare
        ///any specific attribute
        static AttributeEmu defaultAttr;
        
        ///Latencies of the scheduling operations, used to mimick the behavior
        ///of a real scheduler and to correctly keep track of time
        static sc_time schedLatency;
        static sc_time deSchedLatency;
        static sc_time schedChooseLatency;

        /// Standard (trapped) return address used to determine if threads have terminated
        unsigned int pthread_exit_address;
        
        /// Standard (trapped) busy loop address used to pause processors
        unsigned int nop_loop_address;

        /// Address for the main program, used to create the main thread
        unsigned int main_thread_address;

        /// HERE WE START WITH THE METHODS
        ConcurrencyManager(std::string execName, unsigned int memSize);
        ~ConcurrencyManager();

        ///Resets the CM to its original status as after the construction
        void reset();

        ///*******************************************************************
        /// Initialization functions
        ///*******************************************************************
        ///Initializes the mutexes used to guarantee mutual exclusion access to
        ///reentrant routines
        void initReentrantEmulation();

        ///Adds a processor to be managed by this concurrency
        ///concurrency emulator
        template <class wordSize> void addProcessor(trap::ABIIf<wordSize> &processorInstance){
            bool createMainThread = true;

            if(this->managedProc.find(processorInstance.getProcessorID()) != this->managedProc.end())
                THROW_EXCEPTION("A processor with ID = " << processorInstance.getProcessorID() << " has already been added to the concurrency emulator");

            if( managedProc.size() > 0 ) {
                createMainThread = false; 
            }

            this->managedProc[processorInstance.getProcessorID()] = new Processor<wordSize>(processorInstance);
            if(processorInstance.getProcessorID() + 1 > this->maxProcId)
                this->maxProcId = processorInstance.getProcessorID() + 1;
        
            if(createMainThread) {
                // Set processor to run the main thread
                ThreadEmu * th = new ThreadEmu(0, main_thread_address, NULL, memSize, tlsSize, &defaultAttr);
                th->status = ThreadEmu::RUNNING;
                existingThreads[0] = th;
                processorInstance.setSP(memSize - tlsSize);
                stacks[memSize] = threadStackSize;
                this->managedProc[processorInstance.getProcessorID()]->runThread = th;
            } else {
                // Set processor to busy loop        
                processorInstance.setPC(this->nop_loop_address);
                processorInstance.setLR(this->nop_loop_address);
                processorInstance.setSP(memSize - tlsSize);
                processorInstance.setFP(memSize - tlsSize);
            }

        }

        ///*******************************************************************
        /// Here are some functions for computing statistics on the
        /// execution
        ///*******************************************************************
        void printThreadTraces();
        std::vector<std::string> getThreadTraces();
        std::map<int, double> getAverageLoads();
        void printIdleTraces();
        std::vector<std::string> getIdleTraces();
        int getDeadlineMisses();
        int getDeadlineOk();

        ///*******************************************************************
        /// Finally we have the real core of the concurrency manager,
        /// it contains the functions emulating the functionalities of
        /// the emulated concurrency related primitives
        ///*******************************************************************
        ///*********** Thread related routines *******************
        int createThread(unsigned int procId, unsigned int threadFun,  unsigned int args, int attr = -1);
        void exitThread(unsigned int procId, unsigned int retVal);
        bool cancelThread(int threadId);

        int createThreadAttr();
        void deleteThreadAttr(int attr);

        void setStackSize(int attr, int stackSize);
        unsigned int getStackSize(int attr) const;

        void setPreemptive(int attr, int isPreemptive);
        int getPreemptive(int attr) const;

        void setSchedDeadline(int attr, unsigned int deadline);
        unsigned int getSchedDeadline(int attr) const;

        void setSchedPrio(int attr, int priority);
        int getSchedPrio(int attr) const;

        void setSchedPolicy(int attr, int policy);
        int getSchedPolicy(int attr) const;

        int getThreadSchePolicy(int threadId) const;
        void setThreadSchePolicy(int threadId, int policy);

        int getThreadSchedPrio(int threadId) const;
        void setThreadSchedPrio(int threadId, int priority);

        unsigned int getThreadSchedDeadline(int threadId) const;
        void setThreadSchedDeadline(int threadId, unsigned int deadline);

        int getThreadId(unsigned int procId) const;

        int createKey();
        void setSpecific(unsigned int procId, int key, unsigned int memArea);
        unsigned int getSpecific(unsigned int procId, int key) const;

        void join(int thId, unsigned int procId, unsigned int retValAddr);
        void joinAll(unsigned int procId);

        std::pair<unsigned int, unsigned int> readTLS(unsigned int procId) const;
        void idleLoop(unsigned int procId);

        void pushCleanupHandler(unsigned int procId, unsigned int routineAddress, unsigned int arg);
        void popCleanupHandler(unsigned int procId, bool execute);
        void execCleanupHandlerTop(unsigned int procId);

        ///*********** Mutex related routines *******************
        ///Destroys a previously allocated mutex, exception if the mutex does not exist
        void destroyMutex(unsigned int procId, int mutex);
        int createMutex();
        ///Locks the mutex, deschedules the thread if the mutex is busy; in case
        ///nonbl == true, the function behaves as a trylock
        bool lockMutex(int mutex, unsigned int procId, bool nonbl = false);
        ///Releases the lock on the mutex
        int unLockMutex(int mutex, unsigned int procId);

        ///*********** Semaphore related routines *******************
        int createSem(unsigned int procId, int initialValue = 0);
        void destroySem(unsigned int procId, int sem);
        void postSem(int sem, unsigned int procId);
        void waitSem(int sem, unsigned int procId);

        ///*********** Condition Variable related routines *******************
        int createCond(unsigned int procId);
        void destroyCond(unsigned int procId, int cond);
        void signalCond(int cond, bool broadcast, unsigned int procId);
        int waitCond(int cond, int mutex, double time, unsigned int procId);
};

};


#endif
