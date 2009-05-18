#ifndef SYSCALLS
#define SYSCALLS

#include <map>

#include "syscCallB.hpp"

#include <systemc.h>

namespace resp{
void registerPthreadEmu(std::map<std::string, sc_time> latencies);
void registerReentrantEmu(std::map<std::string, sc_time> latencies);

class pthread_cancelSysCall : public archc::SyscallCB{
    public:
    pthread_cancelSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_cleanup_popSysCall : public archc::SyscallCB{
    public:
    pthread_cleanup_popSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_cleanup_pushSysCall : public archc::SyscallCB{
    public:
    pthread_cleanup_pushSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_selfSysCall : public archc::SyscallCB{
    public:
    pthread_selfSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_mutex_destroySysCall : public archc::SyscallCB{
    public:
    pthread_mutex_destroySysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_mutex_initSysCall : public archc::SyscallCB{
    public:
    pthread_mutex_initSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_mutex_lockSysCall : public archc::SyscallCB{
    public:
    pthread_mutex_lockSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_mutex_unlockSysCall : public archc::SyscallCB{
    public:
    pthread_mutex_unlockSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_mutex_trylockSysCall : public archc::SyscallCB{
    public:
    pthread_mutex_trylockSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_mutexattr_destroySysCall : public archc::SyscallCB{
    public:
    pthread_mutexattr_destroySysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_mutexattr_initSysCall : public archc::SyscallCB{
    public:
    pthread_mutexattr_initSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_mutexattr_gettypeSysCall : public archc::SyscallCB{
    public:
    pthread_mutexattr_gettypeSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_mutexattr_settypeSysCall : public archc::SyscallCB{
    public:
    pthread_mutexattr_settypeSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_attr_getdetachstateSysCall : public archc::SyscallCB{
    public:
    pthread_attr_getdetachstateSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_attr_setdetachstateSysCall : public archc::SyscallCB{
    public:
    pthread_attr_setdetachstateSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_attr_destroySysCall : public archc::SyscallCB{
    public:
    pthread_attr_destroySysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_attr_initSysCall : public archc::SyscallCB{
    public:
    pthread_attr_initSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_attr_getstacksizeSysCall : public archc::SyscallCB{
    public:
    pthread_attr_getstacksizeSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_attr_setstacksizeSysCall : public archc::SyscallCB{
    public:
    pthread_attr_setstacksizeSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_attr_getschedpolicySysCall : public archc::SyscallCB{
    public:
    pthread_attr_getschedpolicySysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_attr_setschedpolicySysCall : public archc::SyscallCB{
    public:
    pthread_attr_setschedpolicySysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_attr_getschedparamSysCall : public archc::SyscallCB{
    public:
    pthread_attr_getschedparamSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_attr_setschedparamSysCall : public archc::SyscallCB{
    public:
    pthread_attr_setschedparamSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_attr_getpreemptparamSysCall : public archc::SyscallCB{
    public:
    pthread_attr_getpreemptparamSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_attr_setpreemptparamSysCall : public archc::SyscallCB{
    public:
    pthread_attr_setpreemptparamSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_getschedparamSysCall : public archc::SyscallCB{
    public:
    pthread_getschedparamSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_setschedparamSysCall : public archc::SyscallCB{
    public:
    pthread_setschedparamSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_getspecificSysCall : public archc::SyscallCB{
    public:
    pthread_getspecificSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_setspecificSysCall : public archc::SyscallCB{
    public:
    pthread_setspecificSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_createSysCall : public archc::SyscallCB{
    public:
    pthread_createSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_exitSysCall : public archc::SyscallCB{
    public:
    pthread_exitSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_myexitSysCall : public archc::SyscallCB{
    public:
    pthread_myexitSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_key_createSysCall : public archc::SyscallCB{
    public:
    pthread_key_createSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class sem_initSysCall : public archc::SyscallCB{
    public:
    sem_initSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class sem_postSysCall : public archc::SyscallCB{
    public:
    sem_postSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class sem_destroySysCall : public archc::SyscallCB{
    public:
    sem_destroySysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class sem_waitSysCall : public archc::SyscallCB{
    public:
    sem_waitSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_joinSysCall : public archc::SyscallCB{
    public:
    pthread_joinSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_join_allSysCall : public archc::SyscallCB{
    public:
    pthread_join_allSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_cond_initSysCall : public archc::SyscallCB{
    public:
    pthread_cond_initSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_cond_signalSysCall : public archc::SyscallCB{
    public:
    pthread_cond_signalSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_cond_broadcastSysCall : public archc::SyscallCB{
    public:
    pthread_cond_broadcastSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_cond_waitSysCall : public archc::SyscallCB{
    public:
    pthread_cond_waitSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_cond_destroySysCall : public archc::SyscallCB{
public:
    pthread_cond_destroySysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class pthread_cond_timedwaitSysCall : public archc::SyscallCB{
public:
    pthread_cond_timedwaitSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class __malloc_lockSysCall : public archc::SyscallCB{
    public:
    __malloc_lockSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class __malloc_unlockSysCall : public archc::SyscallCB{
    public:
    __malloc_unlockSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class __sfp_lock_acquireSysCall : public archc::SyscallCB{
    public:
    __sfp_lock_acquireSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class __sfp_lock_releaseSysCall : public archc::SyscallCB{
    public:
    __sfp_lock_releaseSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class __sinit_lock_acquireSysCall : public archc::SyscallCB{
    public:
    __sinit_lock_acquireSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class __sinit_lock_releaseSysCall : public archc::SyscallCB{
    public:
    __sinit_lock_releaseSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class __fp_lockSysCall : public archc::SyscallCB{
    public:
    __fp_lockSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class __fp_unlockSysCall : public archc::SyscallCB{
    public:
    __fp_unlockSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class __aeabi_read_tpSysCall : public archc::SyscallCB{
    public:
    __aeabi_read_tpSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

class __nop_busy_loopSysCall : public archc::SyscallCB{
public:
    __nop_busy_loopSysCall(sc_time latency = SC_ZERO_TIME) : SyscallCB(latency){}
    bool operator()(archc::ac_syscall_base &processorInstance);
};

};

#endif
