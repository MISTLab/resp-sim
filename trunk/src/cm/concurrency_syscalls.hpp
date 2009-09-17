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

template<class wordSize> class pthread_cancelSysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_cancelSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                    trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_cleanup_popSysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_cleanup_popSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                            trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_cleanup_pushSysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_cleanup_pushSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                            trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_selfSysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_selfSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                        trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_mutex_destroySysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_mutex_destroySysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                                trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_mutex_initSysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_mutex_initSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                                trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_mutex_lockSysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_mutex_lockSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                        trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_mutex_unlockSysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_mutex_unlockSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                                trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_mutex_trylockSysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_mutex_trylockSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                                trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_mutexattr_destroySysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_mutexattr_destroySysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                                trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_mutexattr_initSysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_mutexattr_initSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                            trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_mutexattr_gettypeSysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_mutexattr_gettypeSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                                trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_mutexattr_settypeSysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_mutexattr_settypeSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                                trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_attr_getdetachstateSysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_attr_getdetachstateSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                                trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_attr_setdetachstateSysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_attr_setdetachstateSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                                trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_attr_destroySysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_attr_destroySysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                                trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_attr_initSysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_attr_initSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                                    trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_attr_getstacksizeSysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_attr_getstacksizeSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                                    trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_attr_setstacksizeSysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_attr_setstacksizeSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                                    trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_attr_getschedpolicySysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_attr_getschedpolicySysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                                    trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_attr_setschedpolicySysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_attr_setschedpolicySysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                                        trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_attr_getschedparamSysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_attr_getschedparamSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                                        trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_attr_setschedparamSysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_attr_setschedparamSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                                        trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_attr_getpreemptparamSysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_attr_getpreemptparamSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                                        trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_attr_setpreemptparamSysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_attr_setpreemptparamSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                                    trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_getschedparamSysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_getschedparamSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                                    trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_setschedparamSysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_setschedparamSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                                    trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_getspecificSysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_getspecificSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                                trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_setspecificSysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_setspecificSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                            trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_createSysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_createSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                    trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_exitSysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_exitSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                    trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_myexitSysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_myexitSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                    trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_key_createSysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_key_createSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                        trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class sem_initSysCall : public trap::SyscallCB<wordSize>{
    public:
    sem_initSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class sem_postSysCall : public trap::SyscallCB<wordSize>{
    public:
    sem_postSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class sem_destroySysCall : public trap::SyscallCB<wordSize>{
    public:
    sem_destroySysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                    trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class sem_waitSysCall : public trap::SyscallCB<wordSize>{
    public:
    sem_waitSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                            trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_joinSysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_joinSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                    trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_join_allSysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_join_allSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                    trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_cond_initSysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_cond_initSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                        trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_cond_signalSysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_cond_signalSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                        trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_cond_broadcastSysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_cond_broadcastSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                            trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_cond_waitSysCall : public trap::SyscallCB<wordSize>{
    public:
    pthread_cond_waitSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                            trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_cond_destroySysCall : public trap::SyscallCB<wordSize>{
public:
    pthread_cond_destroySysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                                trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class pthread_cond_timedwaitSysCall : public trap::SyscallCB<wordSize>{
public:
    pthread_cond_timedwaitSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                            trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class __malloc_lockSysCall : public trap::SyscallCB<wordSize>{
    public:
    __malloc_lockSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                    trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class __malloc_unlockSysCall : public trap::SyscallCB<wordSize>{
    public:
    __malloc_unlockSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                            trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class __sfp_lock_acquireSysCall : public trap::SyscallCB<wordSize>{
    public:
    __sfp_lock_acquireSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                        trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class __sfp_lock_releaseSysCall : public trap::SyscallCB<wordSize>{
    public:
    __sfp_lock_releaseSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                            trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class __sinit_lock_acquireSysCall : public trap::SyscallCB<wordSize>{
    public:
    __sinit_lock_acquireSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                            trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class __sinit_lock_releaseSysCall : public trap::SyscallCB<wordSize>{
    public:
    __sinit_lock_releaseSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                            trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class __fp_lockSysCall : public trap::SyscallCB<wordSize>{
    public:
    __fp_lockSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class __fp_unlockSysCall : public trap::SyscallCB<wordSize>{
    public:
    __fp_unlockSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class __aeabi_read_tpSysCall : public trap::SyscallCB<wordSize>{
    public:
    __aeabi_read_tpSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                        trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

template<class wordSize> class __nop_busy_loopSysCall : public trap::SyscallCB<wordSize>{
public:
    __nop_busy_loopSysCall(trap::ABIIf<wordSize> &processorInstance, sc_time latency = SC_ZERO_TIME) :
                                                        trap::SyscallCB<wordSize>(processorInstance, latency){}
    bool operator()(){
    }
};

#endif
