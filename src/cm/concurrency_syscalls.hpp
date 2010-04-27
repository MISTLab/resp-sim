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

#ifndef CONCURRENCY_SYSCALLS_HPP
#define CONCURRENCY_SYSCALLS_HPP

#include <systemc.h>

#include <syscCallB.hpp>
#include <ABIIf.hpp>

#include "concurrency_manager.hpp"

///*********************************************************
// Calls related to thread destruction and cancellation
///*********************************************************

///Cancels the execution of thread whose ID is the argument of the call
template<class wordSize> class pthread_cancelSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_cancelSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                        trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            if(this->cm->cancelThread(this->processorInstance.readArgs()[0]))
                this->processorInstance.setRetVal(0);
            else
                this->processorInstance.setRetVal(-1);
            this->processorInstance.returnFromCall();

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///Pops one of the cleanup handlers from the top of the clean-up stack for the
///current thread
template<class wordSize> class pthread_cleanup_popSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_cleanup_popSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            this->cm->popCleanupHandler(this->processorInstance.getProcessorID(), this->processorInstance.readArgs()[0] != 0);

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///Adds a cleanup-handler to the stack of handlers allocated for the current thread
template<class wordSize> class pthread_cleanup_pushSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_cleanup_pushSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            std::vector< wordSize > callArgs = this->processorInstance.readArgs();
            this->cm->pushCleanupHandler(this->processorInstance.getProcessorID(), callArgs[0], callArgs[1]);

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///Returns the identifier of the current thread
template<class wordSize> class pthread_selfSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_selfSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                            trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            this->processorInstance.setRetVal(this->cm->getThreadId(this->processorInstance.getProcessorID()));
            this->processorInstance.returnFromCall();

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///*********************************************************
// Calls related to mutex management
///*********************************************************

///Destroys an existing mutex
template<class wordSize> class pthread_mutex_destroySysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_mutex_destroySysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                    trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            int mutexId = 0;

            this->processorInstance.preCall();

            std::vector< wordSize > callArgs = this->processorInstance.readArgs();
            mutexId = this->processorInstance.readMem(callArgs[0]);
            this->cm->destroyMutex(this->processorInstance.getProcessorID(), mutexId);
            this->processorInstance.setRetVal(0);
            this->processorInstance.returnFromCall();

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///Initializes a mutex to a given value
template<class wordSize> class pthread_mutex_initSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_mutex_initSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                    trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            int mutexId = this->cm->createMutex();

            this->processorInstance.preCall();

            this->processorInstance.writeMem(this->processorInstance.readArgs()[0], mutexId);
            this->processorInstance.setRetVal(0);
            this->processorInstance.returnFromCall();

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///Locks a mutex
template<class wordSize> class pthread_mutex_lockSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_mutex_lockSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                            trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            int mutexId = 0;

            this->processorInstance.preCall();

            std::vector< wordSize > callArgs = this->processorInstance.readArgs();
            mutexId = this->processorInstance.readMem(callArgs[0]);

            if(mutexId == -1){
                // Mutex initialized with PTHREAD_MUTEX_INITIALIZER,
                //I have to create a new mutex
                mutexId = this->cm->createMutex();
                this->processorInstance.writeMem(callArgs[0], mutexId);
            }
            this->processorInstance.setRetVal(0);
            this->processorInstance.returnFromCall();
            this->processorInstance.postCall();

            this->cm->lockMutex(mutexId, this->processorInstance.getProcessorID(), false);

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///unlocks a mutex
template<class wordSize> class pthread_mutex_unlockSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_mutex_unlockSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                    trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            int mutexId = 0;

            this->processorInstance.preCall();

            std::vector< wordSize > callArgs = this->processorInstance.readArgs();
            mutexId = this->processorInstance.readMem(callArgs[0]);
            this->cm->unLockMutex(mutexId, this->processorInstance.getProcessorID());
            this->processorInstance.setRetVal(0);
            this->processorInstance.returnFromCall();

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///tests whether a mutex is unlocked: in case it locks it, otherwise it returns an error
template<class wordSize> class pthread_mutex_trylockSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_mutex_trylockSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                    trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            int mutexId = 0;

            this->processorInstance.preCall();

            std::vector< wordSize > callArgs = this->processorInstance.readArgs();
            mutexId = this->processorInstance.readMem(callArgs[0]);
            if(mutexId == -1){
                // Mutex initialized with PTHREAD_MUTEX_INITIALIZER,
                //I have to create a new mutex
                mutexId = this->cm->createMutex();
                this->processorInstance.writeMem(callArgs[0], mutexId);
            }
            if(this->cm->lockMutex(mutexId, this->processorInstance.getProcessorID(), true))
                this->processorInstance.setRetVal(0);
            else
                this->processorInstance.setRetVal(EBUSY);
            this->processorInstance.returnFromCall();

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///*********************************************************
// Calls related to mutex attributes; note that all these calls
// are empty calls since we do not use mutex attributes
///*********************************************************

///Destroys an attribute allocated for a mutex:empty call
template<class wordSize> class pthread_mutexattr_destroySysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_mutexattr_destroySysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                    trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            this->processorInstance.setRetVal(0);
            this->processorInstance.returnFromCall();

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///Initializes an attribute allocated for a mutex:empty call
template<class wordSize> class pthread_mutexattr_initSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_mutexattr_initSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            this->processorInstance.setRetVal(0);
            this->processorInstance.returnFromCall();

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///Queries the type of an attribute allocated for a mutex:empty call
template<class wordSize> class pthread_mutexattr_gettypeSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_mutexattr_gettypeSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                    trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            this->processorInstance.setRetVal(0);
            this->processorInstance.returnFromCall();

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///Sets the type of an attribute allocated for a mutex:empty call
template<class wordSize> class pthread_mutexattr_settypeSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_mutexattr_settypeSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                    trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            this->processorInstance.setRetVal(0);
            this->processorInstance.returnFromCall();

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///*********************************************************
// Calls related to thread attributes
///*********************************************************

///Queries the detached state of a thread: empty call
template<class wordSize> class pthread_attr_getdetachstateSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_attr_getdetachstateSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                    trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            this->processorInstance.setRetVal(0);
            this->processorInstance.returnFromCall();

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///Sets the detached state of a thread: empty call
template<class wordSize> class pthread_attr_setdetachstateSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_attr_setdetachstateSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                    trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            this->processorInstance.setRetVal(0);
            this->processorInstance.returnFromCall();

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///Destroys a previosly allcoated thread attribute
template<class wordSize> class pthread_attr_destroySysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_attr_destroySysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                    trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            std::vector< wordSize > callArgs = this->processorInstance.readArgs();
            int attrId = this->processorInstance.readMem(callArgs[0]);
            this->cm->deleteThreadAttr(attrId);
            this->processorInstance.setRetVal(0);
            this->processorInstance.returnFromCall();

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///Initializes a thread attribute
template<class wordSize> class pthread_attr_initSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_attr_initSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                        trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            int attrId = this->cm->createThreadAttr();

            this->processorInstance.preCall();

            this->processorInstance.writeMem(this->processorInstance.readArgs()[0], attrId);
            this->processorInstance.setRetVal(0);
            this->processorInstance.returnFromCall();

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///Queries the stack size associated to the current thread attribute
template<class wordSize> class pthread_attr_getstacksizeSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_attr_getstacksizeSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                        trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            int attrId = 0;

            this->processorInstance.preCall();

            std::vector< wordSize > callArgs = this->processorInstance.readArgs();
            attrId = this->processorInstance.readMem(callArgs[0]);
            int stackSize = this->cm->getStackSize(attrId);
            this->processorInstance.writeMem(callArgs[1], stackSize);
            this->processorInstance.setRetVal(0);
            this->processorInstance.returnFromCall();

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///Finally sets the thread attribute associated to the current
template<class wordSize> class pthread_attr_setstacksizeSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_attr_setstacksizeSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                        trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            int attrId = 0;

            this->processorInstance.preCall();

            std::vector< wordSize > callArgs = this->processorInstance.readArgs();
            attrId = this->processorInstance.readMem(callArgs[0]);
            int stackSize = this->processorInstance.readMem(callArgs[1]);
            this->cm->setStackSize(attrId, stackSize);
            this->processorInstance.setRetVal(0);
            this->processorInstance.returnFromCall();

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///*********************************************************
// Calls related to thread attributes: real-time management
///*********************************************************

///Gets the scheduling policy associated with this thread attribute
template<class wordSize> class pthread_attr_getschedpolicySysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_attr_getschedpolicySysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                        trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            int attrId = 0;

            this->processorInstance.preCall();

            std::vector< wordSize > callArgs = this->processorInstance.readArgs();
            attrId = this->processorInstance.readMem(callArgs[0]);
            int policy = this->cm->getSchedPolicy(attrId);
            this->processorInstance.writeMem(callArgs[1], policy);
            this->processorInstance.setRetVal(0);
            this->processorInstance.returnFromCall();

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///Sets the scheduling policy associated with this thread attribute
template<class wordSize> class pthread_attr_setschedpolicySysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_attr_setschedpolicySysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                            trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            int attrId = 0;

            this->processorInstance.preCall();

            std::vector< wordSize > callArgs = this->processorInstance.readArgs();
            attrId = this->processorInstance.readMem(callArgs[0]);
            int policy = callArgs[1];
            this->cm->setSchedPolicy(attrId, policy);
            this->processorInstance.setRetVal(0);
            this->processorInstance.returnFromCall();

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///Gets the scheduling parameter associated to the thread attribute
template<class wordSize> class pthread_attr_getschedparamSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_attr_getschedparamSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                            trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            int attrId = 0;

            this->processorInstance.preCall();

            std::vector< wordSize > callArgs = this->processorInstance.readArgs();
            attrId = this->processorInstance.readMem(callArgs[0]);
            int retAddr = callArgs[1];
            int priority = this->cm->getSchedPrio(attrId);
            this->processorInstance.writeMem(callArgs[1], priority);
            unsigned int deadline = this->cm->getSchedDeadline(attrId);
            this->processorInstance.writeMem(callArgs[1] + sizeof(wordSize), deadline);
            this->processorInstance.setRetVal(0);
            this->processorInstance.returnFromCall();

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///Sets the scheduling parameter associated to the thread attribute
template<class wordSize> class pthread_attr_setschedparamSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_attr_setschedparamSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                            trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            int attrId = 0;

            this->processorInstance.preCall();

            std::vector< wordSize > callArgs = this->processorInstance.readArgs();
            attrId = this->processorInstance.readMem(callArgs[0]);
            int priority = this->processorInstance.readMem(callArgs[1]);
            this->cm->setSchedPrio(attrId, priority);
            unsigned int deadline = this->processorInstance.readMem(callArgs[1] + sizeof(wordSize));
            this->cm->setSchedDeadline(attrId, deadline);
            this->processorInstance.setRetVal(0);
            this->processorInstance.returnFromCall();

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///Queries the preemption staus of the current attribute
template<class wordSize> class pthread_attr_getpreemptparamSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_attr_getpreemptparamSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                            trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            int attrId = 0;

            this->processorInstance.preCall();

            std::vector< wordSize > callArgs = this->processorInstance.readArgs();
            attrId = this->processorInstance.readMem(callArgs[0]);
            int isPreemptive = this->cm->getPreemptive(attrId);
            this->processorInstance.writeMem(callArgs[1], isPreemptive);
            this->processorInstance.setRetVal(0);
            this->processorInstance.returnFromCall();

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///Sets the preemption status of the current attribute
template<class wordSize> class pthread_attr_setpreemptparamSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_attr_setpreemptparamSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                        trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            int attrId = 0;

            this->processorInstance.preCall();
            std::vector< wordSize > callArgs = this->processorInstance.readArgs();
            attrId = this->processorInstance.readMem(callArgs[0]);
            this->cm->setPreemptive(attrId, callArgs[1]);
            this->processorInstance.setRetVal(0);
            this->processorInstance.returnFromCall();

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///Queries the scheduling parameter of the current attribute
template<class wordSize> class pthread_getschedparamSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_getschedparamSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                        trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            std::vector< wordSize > callArgs = this->processorInstance.readArgs();
            int threadId = callArgs[0];
            int policy = this->cm->getThreadSchePolicy(threadId);
            this->processorInstance.writeMem(callArgs[1], policy);
            int priority = this->cm->getThreadSchedPrio(threadId);
            this->processorInstance.writeMem(callArgs[2], priority);
            unsigned int deadline = this->cm->getThreadSchedDeadline(threadId);
            this->processorInstance.writeMem(callArgs[2] + sizeof(wordSize), deadline);
            this->processorInstance.setRetVal(0);
            this->processorInstance.returnFromCall();

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///Sets the scheduling parameter of the current attribute
template<class wordSize> class pthread_setschedparamSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_setschedparamSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                        trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            std::vector< wordSize > callArgs = this->processorInstance.readArgs();
            int threadId = callArgs[0];
            this->cm->setThreadSchePolicy(threadId, callArgs[1]);
            int priority = this->processorInstance.readMem(callArgs[2]);
            this->cm->setThreadSchedPrio(threadId, priority);
            unsigned int deadline = this->processorInstance.readMem(callArgs[2] + sizeof(wordSize));
            this->cm->setThreadSchedDeadline(threadId, deadline);
            this->processorInstance.setRetVal(0);
            this->processorInstance.returnFromCall();

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///*********************************************************
// Calls related to thread specific storage (not TLS)
///*********************************************************

///Gets the address of the thread specific storage
template<class wordSize> class pthread_getspecificSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_getspecificSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                    trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            std::vector< wordSize > callArgs = this->processorInstance.readArgs();
            unsigned int memArea = (unsigned int)this->cm->getSpecific(this->processorInstance.getProcessorID(), callArgs[0]);
            this->processorInstance.setRetVal(memArea);
            this->processorInstance.returnFromCall();

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///Sets the address of the thread specific storage
template<class wordSize> class pthread_setspecificSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_setspecificSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            std::vector< wordSize > callArgs = this->processorInstance.readArgs();
            this->cm->setSpecific(this->processorInstance.getProcessorID(), callArgs[0], callArgs[1]);
            this->processorInstance.setRetVal(0);
            this->processorInstance.returnFromCall();

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///**************************************************************
// Calls related to thread management (creation/destruction ...)
///**************************************************************

///Creation of the system call
template<class wordSize> class pthread_createSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_createSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                        trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            std::vector< wordSize > callArgs = this->processorInstance.readArgs();
            unsigned int threadFun = callArgs[2];
            unsigned int args = callArgs[3];
            int attrId = this->processorInstance.readMem(callArgs[1]);
            this->processorInstance.returnFromCall();
            int thIdAddress = callArgs[0];
            int thId = this->cm->createThread(this->processorInstance.getProcessorID(), threadFun, args, attrId);
            this->processorInstance.writeMem(thIdAddress, thId);
            this->processorInstance.setRetVal(0);

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///Exists the current thread; note that when the thread exists, it also calls the
///cancellation handlers
template<class wordSize> class pthread_exitSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_exitSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                        trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            std::vector< wordSize > callArgs = this->processorInstance.readArgs();
            this->cm->exitThread(this->processorInstance.getProcessorID(), callArgs[0]);
            //There is no need for the call to processorInstance.return_from_syscall(); since I should never
            //return from this function,  I should always resume a new execution path (a new thread)
            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///Halts the execution of a thread waiting
template<class wordSize> class pthread_joinSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_joinSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                        trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            std::vector< wordSize > callArgs = this->processorInstance.readArgs();
            int pthreadId = callArgs[0];

            this->processorInstance.setRetVal(0);
            this->processorInstance.returnFromCall();

            //Note that in case the current thread is blocked waiting for the termination of
            //'pthreadId', when thread is resumed, I have to set the thread return value correctly
            //and I also have to execute the postCall routine.
            this->cm->join(pthreadId, this->processorInstance.getProcessorID(), callArgs[1]);
            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///Joins on all the threads executing in the current process (of course all but the current thread)
template<class wordSize> class pthread_join_allSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_join_allSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                        trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            this->processorInstance.setRetVal(0);
            this->processorInstance.returnFromCall();
            this->cm->joinAll(this->processorInstance.getProcessorID());

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///Creates a thread-unique key
template<class wordSize> class pthread_key_createSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_key_createSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                            trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            int keyId = this->cm->createKey();

            this->processorInstance.preCall();

            this->processorInstance.writeMem(this->processorInstance.readArgs()[0], keyId);
            this->processorInstance.setRetVal(0);
            this->processorInstance.returnFromCall();

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///**************************************************************
// Calls related to semaphore management
///**************************************************************

///Initialized a semaphore
template<class wordSize> class sem_initSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        sem_initSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                    trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            std::vector< wordSize > callArgs = this->processorInstance.readArgs();
            int semId = this->cm->createSem(this->processorInstance.getProcessorID(), callArgs[2]);
            this->processorInstance.writeMem(callArgs[0], semId);
            this->processorInstance.setRetVal(0);
            this->processorInstance.returnFromCall();

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///Atomically increments the value of a semaphore
template<class wordSize> class sem_postSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        sem_postSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                    trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            std::vector< wordSize > callArgs = this->processorInstance.readArgs();
            int semId = this->processorInstance.readMem(callArgs[0]);
            this->cm->postSem(semId, this->processorInstance.getProcessorID());
            this->processorInstance.setRetVal(0);
            this->processorInstance.returnFromCall();

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///Atomically decrements the value of a semaphore and it stops thread execution in case the
///value is smaller than 0
template<class wordSize> class sem_waitSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        sem_waitSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            std::vector< wordSize > callArgs = this->processorInstance.readArgs();
            int semId = this->processorInstance.readMem(callArgs[0]);
            this->processorInstance.setRetVal(0);
            this->processorInstance.returnFromCall();
            this->processorInstance.postCall();

            this->cm->waitSem(semId, this->processorInstance.getProcessorID());

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///Destroys a semaphore
template<class wordSize> class sem_destroySysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        sem_destroySysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                        trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            std::vector< wordSize > callArgs = this->processorInstance.readArgs();
            int semId = this->processorInstance.readMem(callArgs[0]);
            this->cm->destroySem(this->processorInstance.getProcessorID(), semId);
            this->processorInstance.setRetVal(0);
            this->processorInstance.returnFromCall();

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///**************************************************************
// Calls related to condition-variables management
///**************************************************************

///Initializes a condition variable
template<class wordSize> class pthread_cond_initSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_cond_initSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                            trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            int condId = this->cm->createCond(this->processorInstance.getProcessorID());

            this->processorInstance.preCall();

            std::vector< wordSize > callArgs = this->processorInstance.readArgs();
            this->processorInstance.writeMem(callArgs[0], condId);
            this->processorInstance.setRetVal(0);
            this->processorInstance.returnFromCall();

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///Signals a condition variable
template<class wordSize> class pthread_cond_signalSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_cond_signalSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                            trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            std::vector< wordSize > callArgs = this->processorInstance.readArgs();
            int condId = this->processorInstance.readMem(callArgs[0]);
            if(condId == -1){
                // Condition variable was initialized with the PTHREAD_COND_INITIALIZER
                // macro
                condId = this->cm->createCond(this->processorInstance.getProcessorID());
                this->processorInstance.writeMem(callArgs[0], condId);
            }
            this->cm->signalCond(condId, false, this->processorInstance.getProcessorID());
            this->processorInstance.setRetVal(0);
            this->processorInstance.returnFromCall();

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///Singlas in broadcast a condition variable
template<class wordSize> class pthread_cond_broadcastSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_cond_broadcastSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            std::vector< wordSize > callArgs = this->processorInstance.readArgs();
            int condId = this->processorInstance.readMem(callArgs[0]);
            if(condId == -1){
                // Condition variable was initialized with the PTHREAD_COND_INITIALIZER
                // macro
                condId = this->cm->createCond(this->processorInstance.getProcessorID());
                this->processorInstance.writeMem(callArgs[0], condId);
            }
            this->cm->signalCond(condId, true, this->processorInstance.getProcessorID());
            this->processorInstance.setRetVal(0);
            this->processorInstance.returnFromCall();

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///Waits for the signaling of a condition variable
template<class wordSize> class pthread_cond_waitSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        pthread_cond_waitSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            std::vector< wordSize > callArgs = this->processorInstance.readArgs();
            int condId = this->processorInstance.readMem(callArgs[0]);
            if(condId == -1){
                // Condition variable was initialized with the PTHREAD_COND_INITIALIZER
                // macro
                condId = this->cm->createCond(this->processorInstance.getProcessorID());
                this->processorInstance.writeMem(callArgs[0], condId);
            }
            int mutId = this->processorInstance.readMem(callArgs[1]);
            if(mutId == -1){
                // Mutex initialized with PTHREAD_MUTEX_INITIALIZER,
                //I have to create a new mutex
                mutId = this->cm->createMutex();
                this->processorInstance.writeMem(callArgs[1], condId);
            }
            this->processorInstance.setRetVal(0);
            this->processorInstance.returnFromCall();
            this->processorInstance.postCall();

            int retVal = this->cm->waitCond(condId, mutId, 0, this->processorInstance.getProcessorID());
            if(retVal != 0){
                this->processorInstance.preCall();
                this->processorInstance.setRetVal(retVal);
                this->processorInstance.postCall();
            }

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///Destroys a condition variable
template<class wordSize> class pthread_cond_destroySysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
public:
        pthread_cond_destroySysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                    trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            std::vector< wordSize > callArgs = this->processorInstance.readArgs();
            int condId = this->processorInstance.readMem(callArgs[0]);
            this->cm->destroyCond(this->processorInstance.getProcessorID(), condId);
            this->processorInstance.setRetVal(0);
            this->processorInstance.returnFromCall();

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///Waits on a condition variable; timed wait in the sense that if the condition is not
///notified in a specified amount of time, the thread resumes with an error
template<class wordSize> class pthread_cond_timedwaitSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
public:
        pthread_cond_timedwaitSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            std::vector< wordSize > callArgs = this->processorInstance.readArgs();
            int condId = this->processorInstance.readMem(callArgs[0]);
            if(condId == -1){
                // Condition variable was initialized with the PTHREAD_COND_INITIALIZER
                // macro
                condId = this->cm->createCond(this->processorInstance.getProcessorID());
                this->processorInstance.writeMem(callArgs[0], condId);
            }
            int mutId = this->processorInstance.readMem(callArgs[1]);
            if(mutId == -1){
                // Mutex initialized with PTHREAD_MUTEX_INITIALIZER,
                //I have to create a new mutex
                mutId = this->cm->createMutex();
                this->processorInstance.writeMem(callArgs[1], condId);
            }
            double time = 0;
            if(callArgs[2] != 0){
                int timeTemp = 0;
                timeTemp = this->processorInstance.readMem(callArgs[2]);
                time = timeTemp*1.0e+9;
                timeTemp = this->processorInstance.readMem(callArgs[2] + sizeof(wordSize));
                time += timeTemp;
            }
            this->processorInstance.setRetVal(0);
            this->processorInstance.returnFromCall();
            this->processorInstance.postCall();

            int retVal = this->cm->waitCond(condId, mutId, time, this->processorInstance.getProcessorID());
            if(retVal != 0){
                this->processorInstance.preCall();
                this->processorInstance.setRetVal(retVal);
                this->processorInstance.postCall();
            }

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///**************************************************************
// Calls related to mutual exclusion in newlib reentrant calls
///**************************************************************

///Locks the malloc mutex, protecting reentrancy emulation
template<class wordSize> class __malloc_lockSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        __malloc_lockSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                        trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            this->processorInstance.returnFromCall();
            this->processorInstance.postCall();
            this->cm->lockMutex(this->cm->mallocMutex, this->processorInstance.getProcessorID(), false);

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

template<class wordSize> class __malloc_unlockSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        __malloc_unlockSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            this->processorInstance.returnFromCall();
            this->processorInstance.postCall();
            this->cm->unLockMutex(this->cm->mallocMutex, this->processorInstance.getProcessorID());

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

template<class wordSize> class __sfp_lock_acquireSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        __sfp_lock_acquireSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                            trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            this->processorInstance.returnFromCall();
            this->processorInstance.postCall();
            this->cm->lockMutex(this->cm->sfpMutex, this->processorInstance.getProcessorID(), false);

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

template<class wordSize> class __sfp_lock_releaseSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        __sfp_lock_releaseSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            this->processorInstance.returnFromCall();
            this->processorInstance.postCall();
            this->cm->unLockMutex(this->cm->sfpMutex, this->processorInstance.getProcessorID());

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

template<class wordSize> class __sinit_lock_acquireSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        __sinit_lock_acquireSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            this->processorInstance.returnFromCall();
            this->processorInstance.postCall();
            this->cm->lockMutex(this->cm->sinitMutex, this->processorInstance.getProcessorID(), false);

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

template<class wordSize> class __sinit_lock_releaseSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        __sinit_lock_releaseSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            this->processorInstance.returnFromCall();
            this->processorInstance.postCall();
            this->cm->unLockMutex(this->cm->sinitMutex, this->processorInstance.getProcessorID());

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

template<class wordSize> class __fp_lockSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        __fp_lockSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                    trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            this->processorInstance.returnFromCall();
            this->processorInstance.postCall();
            this->cm->lockMutex(this->cm->fpMutex, this->processorInstance.getProcessorID(), false);

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

template<class wordSize> class __fp_unlockSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        __fp_unlockSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                    trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            this->processorInstance.returnFromCall();
            this->processorInstance.postCall();
            this->cm->unLockMutex(this->cm->fpMutex, this->processorInstance.getProcessorID());

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///**************************************************************
// Call used to read the base address of the Thread-Local storage (TLS)
///**************************************************************

///Returns the address of the thread local storage for the current
///thread
template<class wordSize> class __aeabi_read_tpSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
        __aeabi_read_tpSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                            trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
        bool operator()(){
            this->processorInstance.preCall();

            this->processorInstance.setRetVal(this->cm->readTLS(this->processorInstance.getProcessorID()).second);
            this->processorInstance.returnFromCall();

            this->processorInstance.postCall();

            if(this->latency.to_double() > 0)
                wait(this->latency);
            return true;
        }
};

///**************************************************************
// Call used to signal that the busy loop is entered
///**************************************************************

///Called when the busy loop is encountered
template<class wordSize> class __nop_busy_loopSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
public:
    __nop_busy_loopSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                        trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
        //std::cout << "Processor " << this->processorInstance.getProcessorID() << " busy looping" << std::endl;
        this->cm->idleLoop(this->processorInstance.getProcessorID());
        //std::cout << "Processor " << this->processorInstance.getProcessorID() << " wakeup " << std::endl;
        return true;
    }
};

#endif
