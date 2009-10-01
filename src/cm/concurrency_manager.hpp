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
    }
    int deSchedule(bool saveStatus = true){
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
        ///Variables used for managing reentrant synchronization
        unsigned int mallocMutex;
        unsigned int sfpMutex;
        unsigned int sinitMutex;
        unsigned int fpMutex;
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

        ConcurrencyManager();

        ///Resets the CM to its original status as after the construction
        void reset();

        ///*******************************************************************
        /// Initialization functions
        ///*******************************************************************
        ///Initializes the mutexes used to guarantee mutual exclusion access to
        ///reentrant routines
        void initReentrantEmulation();
        template <class wordSize> void addProcessor(trap::ABIIf<wordSize> &processorInstance){
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
        int createThread(unsigned int threadFun,  unsigned int args, int attr = -1);
        void exitThread(unsigned int procId, unsigned int retVal);
        bool cancelThread(int threadId);

        int createThreadAttr();
        void deleteThreadAttr(int attr);

        void setStackSize(int attr, int stackSize);
        unsigned int getStackSize(int attr);

        void setPreemptive(int attr, int isPreemptive);
        int getPreemptive(int attr);

        void setSchedDeadline(int attr, unsigned int deadline);
        unsigned int getSchedDeadline(int attr);

        void setSchedPrio(int attr, int priority);
        int getSchedPrio(int attr);

        void setSchedPolicy(int attr, int policy);
        int getSchedPolicy(int attr);

        int getThreadSchePolicy(int threadId);
        void setThreadSchePolicy(int threadId, int policy);

        int getThreadSchedPrio(int threadId);
        void setThreadSchedPrio(int threadId, int priority);

        unsigned int getThreadSchedDeadline(int threadId);
        void setThreadSchedDeadline(int threadId, unsigned int deadline);

        int getThreadId(unsigned int procId);

        int createKey();
        void setSpecific(unsigned int procId, int key, unsigned int memArea);
        unsigned int getSpecific(unsigned int procId, int key);

        void join(int thId, unsigned int procId, int curThread_ = -1);
        void joinAll(unsigned int procId);

        std::pair<unsigned int, unsigned int> readTLS(unsigned int procId);
        void idleLoop(unsigned int procId);

        void pushCleanupHandler(unsigned int procId, unsigned int routineAddress, unsigned int arg);
        void popCleanupHandler(unsigned int procId, bool execute);
        void execCleanupHandlerTop(unsigned int procId);

        ///*********** Mutex related routines *******************
        ///Destroys a previously allocated mutex, exception if the mutex does not exist
        void destroyMutex(unsigned int procId, int mutex);
        int createMutex(unsigned int procId);
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
