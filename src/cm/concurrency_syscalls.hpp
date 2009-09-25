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

#include <sysCallB.hpp>
#include <ABIIf.hpp>

#include "concurrency_manager.hpp"

///*********************************************************
// Calls related to thread destruction and cancellation
///*********************************************************

template<class wordSize> class pthread_cancelSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_cancelSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                    trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
        THROW_EXCEPTION("pthread_cancel system call not yet completely implemented");
    }
};

template<class wordSize> class pthread_cleanup_popSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_cleanup_popSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                            trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
        THROW_EXCEPTION("pthread_cleanup_pop system call not yet completely implemented");
    }
};

template<class wordSize> class pthread_cleanup_pushSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_cleanup_pushSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                            trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
        THROW_EXCEPTION("pthread_cleanup_push system call not yet completely implemented");
    }
};

template<class wordSize> class pthread_selfSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_selfSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                        trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            processorInstance.return_from_syscall();
    processorInstance.set_retVal(0, getThreadId(processorInstance.get_proc_id()));
    processorInstance.return_from_syscall();

    }
};

///*********************************************************
// Calls related to mutex management
///*********************************************************

template<class wordSize> class pthread_mutex_destroySysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_mutex_destroySysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
        int mutexId = 0;
        processorInstance.get_buffer_endian(0, (unsigned char *)&mutexId, processorInstance.getWordSize());
        destroyMutex(processorInstance.get_proc_id(), mutexId);
        processorInstance.set_retVal(0, 0);
        processorInstance.return_from_syscall();
    }
};

template<class wordSize> class pthread_mutex_initSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_mutex_initSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            int mutexId = createMutex(processorInstance.get_proc_id());
    processorInstance.set_buffer_endian(0, (unsigned char *)&mutexId, processorInstance.getWordSize());
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();

    }
};

template<class wordSize> class pthread_mutex_lockSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_mutex_lockSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                        trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            int mutexId = 0;
    processorInstance.get_buffer_endian(0, (unsigned char *)&mutexId, processorInstance.getWordSize());
    if(mutexId == -1){
        // Mutex initialized with PTHREAD_MUTEX_INITIALIZER,
        //I have to create a new mutex
        mutexId = createMutex(processorInstance.get_proc_id());
        processorInstance.set_buffer_endian(0, (unsigned char *)&mutexId, processorInstance.getWordSize());
    }
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();

    lockMutex(mutexId, processorInstance.get_proc_id(), false);

    }
};

template<class wordSize> class pthread_mutex_unlockSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_mutex_unlockSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            int mutexId = 0;
    processorInstance.get_buffer_endian(0, (unsigned char *)&mutexId, processorInstance.getWordSize());
    unLockMutex(mutexId, processorInstance.get_proc_id());
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();

    }
};

template<class wordSize> class pthread_mutex_trylockSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_mutex_trylockSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
    int mutexId = 0;
    processorInstance.get_buffer_endian(0, (unsigned char *)&mutexId, processorInstance.getWordSize());
    if(mutexId == -1){
        // Mutex initialized with PTHREAD_MUTEX_INITIALIZER,
        //I have to create a new mutex
        mutexId = createMutex(processorInstance.get_proc_id());
        processorInstance.set_buffer_endian(0, (unsigned char *)&mutexId, processorInstance.getWordSize());
    }
    if(lockMutex(mutexId, processorInstance.get_proc_id(), true))
        processorInstance.set_retVal(0, 0);
    else
        processorInstance.set_retVal(0, EBUSY);
    processorInstance.return_from_syscall();
    }
};

///*********************************************************
// Calls related to mutex attributes
///*********************************************************

template<class wordSize> class pthread_mutexattr_destroySysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_mutexattr_destroySysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();

    }
};

template<class wordSize> class pthread_mutexattr_initSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_mutexattr_initSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                            trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();

    }
};

template<class wordSize> class pthread_mutexattr_gettypeSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_mutexattr_gettypeSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();

    }
};

template<class wordSize> class pthread_mutexattr_settypeSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_mutexattr_settypeSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();

    }
};

///*********************************************************
// Calls related to thread attributes
///*********************************************************

template<class wordSize> class pthread_attr_getdetachstateSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_attr_getdetachstateSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();

    }
};

template<class wordSize> class pthread_attr_setdetachstateSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_attr_setdetachstateSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();

    }
};

template<class wordSize> class pthread_attr_destroySysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_attr_destroySysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            int attrId = 0;
    processorInstance.get_buffer_endian(0, (unsigned char *)&attrId, processorInstance.getWordSize());
    deleteThreadAttr(attrId);
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();

    }
};

template<class wordSize> class pthread_attr_initSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_attr_initSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                    trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            int attrId = createThreadAttr();
    processorInstance.set_buffer_endian(0, (unsigned char *)&attrId, processorInstance.getWordSize());
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();

    }
};

template<class wordSize> class pthread_attr_getstacksizeSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_attr_getstacksizeSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                    trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            int attrId = 0;
    processorInstance.get_buffer_endian(0, (unsigned char *)&attrId, processorInstance.getWordSize());
    int stackSize = getStackSize(attrId);
    processorInstance.set_buffer_endian(1, (unsigned char *)&stackSize, processorInstance.getWordSize());
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();

    }
};

template<class wordSize> class pthread_attr_setstacksizeSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_attr_setstacksizeSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                    trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            int attrId = 0;
    processorInstance.get_buffer_endian(0, (unsigned char *)&attrId, processorInstance.getWordSize());
    int stackSize = 0;
    processorInstance.get_buffer_endian(1, (unsigned char *)&stackSize, processorInstance.getWordSize());
    setStackSize(attrId, stackSize);
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();

    }
};

///*********************************************************
// Calls related to thread attributes: real-time management
///*********************************************************

template<class wordSize> class pthread_attr_getschedpolicySysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_attr_getschedpolicySysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                    trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            int attrId = 0;
    processorInstance.get_buffer_endian(0, (unsigned char *)&attrId, processorInstance.getWordSize());
    int policy = getSchedPolicy(attrId);
    processorInstance.set_buffer_endian(1, (unsigned char *)&policy, processorInstance.getWordSize());
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();

    }
};

template<class wordSize> class pthread_attr_setschedpolicySysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_attr_setschedpolicySysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                        trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            int attrId = 0;
    processorInstance.get_buffer_endian(0, (unsigned char *)&attrId, processorInstance.getWordSize());
    int policy = processorInstance.get_arg(1);
    setSchedPolicy(attrId, policy);
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();

    }
};

template<class wordSize> class pthread_attr_getschedparamSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_attr_getschedparamSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                        trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            int attrId = 0;
    processorInstance.get_buffer_endian(0, (unsigned char *)&attrId, processorInstance.getWordSize());
    int retAddr = processorInstance.get_arg(1);
    int priority = getSchedPrio(attrId);
    processorInstance.set_buffer_endian(1, (unsigned char *)&priority, processorInstance.getWordSize());
    processorInstance.set_arg(1, retAddr + processorInstance.getWordSize());
    unsigned int deadline = getSchedDeadline(attrId);
    processorInstance.set_buffer_endian(1, (unsigned char *)&deadline, processorInstance.getWordSize());
    processorInstance.set_arg(1, retAddr);
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();

    }
};

template<class wordSize> class pthread_attr_setschedparamSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_attr_setschedparamSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                        trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            int attrId = 0;
    processorInstance.get_buffer_endian(0, (unsigned char *)&attrId, processorInstance.getWordSize());
    int retAddr = processorInstance.get_arg(1);
    int priority = 0;
    processorInstance.get_buffer_endian(1, (unsigned char *)&priority, processorInstance.getWordSize());
    setSchedPrio(attrId, priority);
    unsigned int deadline = 0;
    processorInstance.set_arg(1, retAddr + processorInstance.getWordSize());
    processorInstance.get_buffer_endian(1, (unsigned char *)&deadline, processorInstance.getWordSize());
    setSchedDeadline(attrId, deadline);
    processorInstance.set_arg(1, retAddr);
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();

    }
};

template<class wordSize> class pthread_attr_getpreemptparamSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_attr_getpreemptparamSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                        trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            int attrId = 0;
    processorInstance.get_buffer_endian(0, (unsigned char *)&attrId, processorInstance.getWordSize());
    int isPreemptive = getPreemptive(attrId);
    processorInstance.set_buffer_endian(1, (unsigned char *)&isPreemptive, processorInstance.getWordSize());
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();

    }
};

template<class wordSize> class pthread_attr_setpreemptparamSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_attr_setpreemptparamSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                    trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            int attrId = 0;
    processorInstance.get_buffer_endian(0, (unsigned char *)&attrId, processorInstance.getWordSize());
    int isPreemptive = processorInstance.get_arg(1);
    setPreemptive(attrId, isPreemptive);
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();

    }
};

template<class wordSize> class pthread_getschedparamSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_getschedparamSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                    trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            int threadId = processorInstance.get_arg(0);
    int policy = getThreadSchePolicy(threadId);
    processorInstance.set_buffer_endian(1, (unsigned char *)&policy, processorInstance.getWordSize());
    int retAddr = processorInstance.get_arg(2);
    int priority = getThreadSchedPrio(threadId);
    processorInstance.set_buffer_endian(2, (unsigned char *)&priority, processorInstance.getWordSize());
    processorInstance.set_arg(2, retAddr + processorInstance.getWordSize());
    unsigned int deadline = getThreadSchedDeadline(threadId);
    processorInstance.set_buffer_endian(2, (unsigned char *)&deadline, processorInstance.getWordSize());
    processorInstance.set_arg(2, retAddr);
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();

    }
};

template<class wordSize> class pthread_setschedparamSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_setschedparamSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                    trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            int threadId = processorInstance.get_arg(0);
    int policy = processorInstance.get_arg(1);
    setThreadSchePolicy(threadId, policy);
    int retAddr = processorInstance.get_arg(2);
    int priority = 0;
    processorInstance.get_buffer_endian(2, (unsigned char *)&priority, processorInstance.getWordSize());
    setThreadSchedPrio(threadId, priority);
    unsigned int deadline = 0;
    processorInstance.set_arg(2, retAddr + processorInstance.getWordSize());
    processorInstance.get_buffer_endian(2, (unsigned char *)&deadline, processorInstance.getWordSize());
    setThreadSchedDeadline(threadId, deadline);
    processorInstance.set_arg(2, retAddr);
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();

    }
};

///*********************************************************
// Calls related to thread specific storage (not TLS)
///*********************************************************

template<class wordSize> class pthread_getspecificSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_getspecificSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            int key = processorInstance.get_arg(0);
    unsigned int memArea = (unsigned int)(unsigned long)getSpecific(processorInstance.get_proc_id(), key);
    processorInstance.set_retVal(0, memArea);
    processorInstance.return_from_syscall();

    }
};

template<class wordSize> class pthread_setspecificSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_setspecificSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                            trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            int key = processorInstance.get_arg(0);
    void * memArea = (void *)processorInstance.get_arg(1);
    setSpecific(processorInstance.get_proc_id(), key, memArea);
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();

    }
};

///**************************************************************
// Calls related to thread management (creation/destruction ...)
///**************************************************************

template<class wordSize> class pthread_createSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_createSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                    trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            thread_routine_t threadFun = (thread_routine_t)processorInstance.get_arg(2);
    void *args = (void *)processorInstance.get_arg(3);
    int attrId = 0;
    processorInstance.get_buffer_endian(1, (unsigned char *)&attrId, processorInstance.getWordSize());
    processorInstance.return_from_syscall();
    int thIdAddress = processorInstance.get_arg(0);
    int thId = createThread(threadFun, args, attrId);
    processorInstance.set_arg(0, thIdAddress);
    processorInstance.set_buffer_endian(0, (unsigned char *)&thId, processorInstance.getWordSize());
    processorInstance.set_retVal(0, 0);

    }
};

template<class wordSize> class pthread_exitSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_exitSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                    trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            void * retVal = (void *)processorInstance.get_arg(0);

    exitThread(processorInstance.get_proc_id(), retVal);

    //There is not need for the call to processorInstance.return_from_syscall(); since I should never
    //return from this function,  I should always resume a new execution path (a new thread)

    }
};

template<class wordSize> class pthread_myexitSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_myexitSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                    trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            void * retVal = (void *)processorInstance.get_retVal(0);

    exitThread(processorInstance.get_proc_id(), retVal);

    //There is not need for the call to processorInstance.return_from_syscall(); since I should never
    //return from this function,  I should always resume a new execution path (a new thread)

    }
};

template<class wordSize> class pthread_joinSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_joinSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                    trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            int pthreadId = processorInstance.get_arg(0);

    // Note that these instructions set the return value of the thread before the join operation actually
    // happens: I can do this only because I discard the thread return value
    if(processorInstance.get_arg(1) != 0){
        int retVal  = 0;
        sc_controller::disableLatency = true;
        processorInstance.set_buffer_endian(1, (unsigned char *)&retVal, processorInstance.getWordSize());
        sc_controller::disableLatency = false;
    }
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();
    // end of the setting of the return value

    join(pthreadId, processorInstance.get_proc_id());

    }
};

template<class wordSize> class pthread_join_allSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_join_allSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                    trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            processorInstance.return_from_syscall();

    joinAll(processorInstance.get_proc_id());

    }
};

template<class wordSize> class pthread_key_createSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_key_createSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                        trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            int keyId = createKey();
    processorInstance.set_buffer_endian(0, (unsigned char *)&keyId, processorInstance.getWordSize());
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();

    }
};

///**************************************************************
// Calls related to semaphore management
///**************************************************************

template<class wordSize> class sem_initSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    sem_initSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            int semId = createSem(processorInstance.get_proc_id(), processorInstance.get_arg(2));
    processorInstance.set_buffer_endian(0, (unsigned char *)&semId, processorInstance.getWordSize());
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();

    }
};

template<class wordSize> class sem_postSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    sem_postSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            int semId = 0;
    processorInstance.get_buffer_endian(0, (unsigned char *)&semId, processorInstance.getWordSize());
    postSem(semId, processorInstance.get_proc_id());
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();

    }
};

template<class wordSize> class sem_destroySysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    sem_destroySysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                    trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            int semId = 0;
    processorInstance.get_buffer_endian(0, (unsigned char *)&semId, processorInstance.getWordSize());
    destroySem(processorInstance.get_proc_id(), semId);
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();

    }
};

template<class wordSize> class sem_waitSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    sem_waitSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                            trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            int semId = 0;
    processorInstance.get_buffer_endian(0, (unsigned char *)&semId, processorInstance.getWordSize());
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();

    waitSem(semId, processorInstance.get_proc_id());

    }
};

///**************************************************************
// Calls related to condition-variables management
///**************************************************************

template<class wordSize> class pthread_cond_initSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_cond_initSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                        trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            int condId = createCond(processorInstance.get_proc_id());
    processorInstance.set_buffer_endian(0, (unsigned char *)&condId, processorInstance.getWordSize());
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();

    }
};

template<class wordSize> class pthread_cond_signalSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_cond_signalSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                        trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            int condId = 0;
    processorInstance.get_buffer_endian(0, (unsigned char *)&condId, processorInstance.getWordSize());
    if(condId == -1){
        // Condition variable was initialized with the PTHREAD_COND_INITIALIZER
        // macro
        condId = createCond(processorInstance.get_proc_id());
        processorInstance.set_buffer_endian(0, (unsigned char *)&condId, processorInstance.getWordSize());
    }
    signalCond(condId, false, processorInstance.get_proc_id());
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();

    }
};

template<class wordSize> class pthread_cond_broadcastSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_cond_broadcastSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                            trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            int condId = 0;
    processorInstance.get_buffer_endian(0, (unsigned char *)&condId, processorInstance.getWordSize());
    if(condId == -1){
        // Condition variable was initialized with the PTHREAD_COND_INITIALIZER
        // macro
        condId = createCond(processorInstance.get_proc_id());
        processorInstance.set_buffer_endian(0, (unsigned char *)&condId, processorInstance.getWordSize());
    }
    signalCond(condId, true, processorInstance.get_proc_id());
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();

    }
};

template<class wordSize> class pthread_cond_waitSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    pthread_cond_waitSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                            trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            int condId = 0;
    int mutId = 0;
    processorInstance.get_buffer_endian(0, (unsigned char *)&condId, processorInstance.getWordSize());
    if(condId == -1){
        // Condition variable was initialized with the PTHREAD_COND_INITIALIZER
        // macro
        condId = createCond(processorInstance.get_proc_id());
        processorInstance.set_buffer_endian(0, (unsigned char *)&condId, processorInstance.getWordSize());
    }
    processorInstance.get_buffer_endian(1, (unsigned char *)&mutId, processorInstance.getWordSize());
    if(mutId == -1){
        // Mutex initialized with PTHREAD_MUTEX_INITIALIZER,
        //I have to create a new mutex
        mutId = createMutex(processorInstance.get_proc_id());
        processorInstance.set_buffer_endian(0, (unsigned char *)&mutId, processorInstance.getWordSize());
    }
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();

    int retVal = waitCond(condId, mutId, 0, processorInstance.get_proc_id());
    if(retVal != 0)
        processorInstance.set_retVal(0, retVal);

    }
};

template<class wordSize> class pthread_cond_destroySysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
public:
    pthread_cond_destroySysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                                trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            int condId = 0;
    processorInstance.get_buffer_endian(0, (unsigned char *)&condId, processorInstance.getWordSize());
    destroyCond(processorInstance.get_proc_id(), condId);
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();

    }
};

template<class wordSize> class pthread_cond_timedwaitSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
public:
    pthread_cond_timedwaitSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                            trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            int condId = 0;
    int mutId = 0;
    double time = 0;
    int timeTemp = 0;
    processorInstance.get_buffer_endian(0, (unsigned char *)&condId, processorInstance.getWordSize());
    if(condId == -1){
        // Condition variable was initialized with the PTHREAD_COND_INITIALIZER
        // macro
        condId = createCond(processorInstance.get_proc_id());
        processorInstance.set_buffer_endian(0, (unsigned char *)&condId, processorInstance.getWordSize());
    }
    processorInstance.get_buffer_endian(1, (unsigned char *)&mutId, processorInstance.getWordSize());
    if(mutId == -1){
        // Mutex initialized with PTHREAD_MUTEX_INITIALIZER,
        //I have to create a new mutex
        mutId = createMutex(processorInstance.get_proc_id());
        processorInstance.set_buffer_endian(0, (unsigned char *)&mutId, processorInstance.getWordSize());
    }
    int baseAddr = processorInstance.get_arg(2);
    if(baseAddr != 0){
        processorInstance.get_buffer_endian(2, (unsigned char *)&timeTemp, 4);
        time = timeTemp*1.0e+9;
        processorInstance.set_arg(2, baseAddr + 4);
        processorInstance.get_buffer_endian(2, (unsigned char *)&timeTemp, 4);
        time += timeTemp;
        processorInstance.set_arg(2, baseAddr);
    }
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();

    waitCond(condId, mutId, time, processorInstance.get_proc_id());

    }
};

///**************************************************************
// Calls related to mutual exclusion in newlib reentrant calls
///**************************************************************

template<class wordSize> class __malloc_lockSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    __malloc_lockSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                    trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            processorInstance.return_from_syscall();
    lockMutex(mallocMutex, processorInstance.get_proc_id(), false);

    }
};

template<class wordSize> class __malloc_unlockSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    __malloc_unlockSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                            trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            processorInstance.return_from_syscall();
    unLockMutex(mallocMutex, processorInstance.get_proc_id());

    }
};

template<class wordSize> class __sfp_lock_acquireSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    __sfp_lock_acquireSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                        trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            processorInstance.return_from_syscall();
    lockMutex(sfpMutex, processorInstance.get_proc_id(), false);

    }
};

template<class wordSize> class __sfp_lock_releaseSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    __sfp_lock_releaseSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                            trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            processorInstance.return_from_syscall();
    unLockMutex(sfpMutex, processorInstance.get_proc_id());

    }
};

template<class wordSize> class __sinit_lock_acquireSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    __sinit_lock_acquireSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                            trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            processorInstance.return_from_syscall();
    lockMutex(sinitMutex, processorInstance.get_proc_id(), false);

    }
};

template<class wordSize> class __sinit_lock_releaseSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    __sinit_lock_releaseSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                            trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            processorInstance.return_from_syscall();
    unLockMutex(sinitMutex, processorInstance.get_proc_id());

    }
};

template<class wordSize> class __fp_lockSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    __fp_lockSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            processorInstance.return_from_syscall();
    lockMutex(fpMutex, processorInstance.get_proc_id(), false);

    }
};

template<class wordSize> class __fp_unlockSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    __fp_unlockSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
            processorInstance.return_from_syscall();
    unLockMutex(fpMutex, processorInstance.get_proc_id());

    }
};

template<class wordSize> class __aeabi_read_tpSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
    public:
    __aeabi_read_tpSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                        trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
    processorInstance.set_retVal(0, (unsigned int)(unsigned long)readTLS(processorInstance.get_proc_id()).second);
    processorInstance.return_from_syscall();

    }
};

template<class wordSize> class __nop_busy_loopSysCall : public trap::SyscallCB<wordSize>{
    private:
        resp::ConcurrencyManager * cm;
public:
    __nop_busy_loopSysCall(trap::ABIIf<wordSize> &processorInstance, resp::ConcurrencyManager * cm, sc_time latency = SC_ZERO_TIME) :
                                                        trap::SyscallCB<wordSize>(processorInstance, latency), cm(cm){}
    bool operator()(){
        idleLoop(processorInstance.get_proc_id());
    }
};

#endif
