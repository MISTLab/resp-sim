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

#include "concurrency_structures.hpp"

#include <systemc.h>

#include <map>

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
        ///variable pointing to the only instance of the concurrency manager
        static ConcurrencyManager * cmInstance;
        ///Private constructor of the CM: it is part of the singleton design
        ///pattern, meaning that it is not possible to create instances
        ///of this class from the external world and that only one
        ///instance will exists
        ConcurrencyManager();
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

        ///Returns an instance of the concurrency manager; if such instance does not
        ///exists yet it is created.
        static ConcurrencyManager& getManager();

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
        int createThread(thread_routine_t threadFun,  void * args, int attr = -1);
        void exitThread(unsigned int procId, void * retVal);

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
        void setSpecific(unsigned int procId, int key, void *memArea);
        void *getSpecific(unsigned int procId, int key);

        void join(int thId, unsigned int procId, int curThread_ = -1);
        void joinAll(unsigned int procId);

        std::pair<void *, void *> readTLS(unsigned int procId);
        void idleLoop(unsigned int procId);

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

///This class is in charge of performing initialization of all
///the necessary concurrency management structures, of the registration
///of the emulated routines, etc.
///Note that the executable must be compiled with the -specs=osemu.specs,
///Otherwise is will be impossible to use the Operating System
///Emulation features.
template<class wordSize> class ConcurrencyEmulator{
    private:
        ///Instance of the concurrency manager, used to actually implement the emulated concurrency-related
        ///functionalities.
        ConcurrencyManager& cm;

        ///In a multi-processor environment this function has to be called to
        ///enable the protection of the internal OS routines, ensuring their
        ///atomicity
        void initiReentrantEmu();
    public:
        ///Constructor of the class, it performs the preliminary initialization of all the necessary data structures
        ///and of the concurrency manager.
        ConcurrencyEmulator(unsigned int memSize, bool waitLoop = true, unsigned int defaultStackSize = 1024*20,
                                        std::map<std::string, sc_time> latencies = std::map<std::string, sc_time>()){
        }
        ///Registers the processor interface among the processors that
        ///will be manged by the concurrency manager.
        void addProcessorInterface(trap::ABIIf<wordSize> &processorInstance){
        }
        ///Marks a function to be executed as an interrupt service routine
        void registerISR(std::string isrFunctionName, int irqId, bool preemptive = false,
                                        int schedPolicy = ConcurrencyManager::SYSC_SCHED_FIFO,
                                                int priority = ConcurrencyManager::SYSC_PRIO_MAX);
        ///Sets thread properties, so that, when a thread is created, it is associated such properties
        ///independently from the thread attributes that were specified in the executeable file
        void setThreadInfo(std::string functionName, bool preemptive,
                           int schedPolicy, int priority, unsigned int deadline){
        }
        ///Resets the whole concurrency emulator, reinitializing it and preparing it for a new simulation
        void reset(){
        }
};

};


#endif
