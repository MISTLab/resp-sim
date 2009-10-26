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

#ifndef CONCURRENCY_EMULATOR_HPP
#define CONCURRENCY_EMULATOR_HPP

#include <systemc.h>

#include <map>
#include <string>

#ifdef __GNUC__
#ifdef __GNUC_MINOR__
#if (__GNUC__ >= 4 && __GNUC_MINOR__ >= 3)
#include <tr1/unordered_map>
#define template_map std::tr1::unordered_map
#else
#include <ext/hash_map>
#define  template_map __gnu_cxx::hash_map
#endif
#else
#include <ext/hash_map>
#define  template_map __gnu_cxx::hash_map
#endif
#else
#ifdef _WIN32
#include <hash_map>
#define  template_map stdext::hash_map
#else
#include <map>
#define  template_map std::map
#endif
#endif

#include "bfdWrapper.hpp"
#include "concurrency_manager.hpp"
#include "concurrency_syscalls.hpp"

#include <ABIIf.hpp>
#include <ToolsIf.hpp>
#include <syscCallB.hpp>
#include <instructionBase.hpp>

namespace resp{

///Base class of the ConcurrencyEmulator, used to declare some static
///components (which cannot be declared directly inside the ConcurrencyEmulator,
///being it a templated class)
class ConcurrencyEmulatorBase{
    protected:
        ///Instance of the concurrency manager, used to actually implement the emulated concurrency-related
        ///functionalities: note that this is a map since there might be different instances of the
        ///Manager, one for each emulation group
        static std::map<int, ConcurrencyManager * > cm;
    public:
        ///Sets the default properties of the threads; this method has to be called before
        ///creating instances of the ConcurrencyEmulator
        static void setThreadProps(bool waitLoop, unsigned int defaultStackSize);
        ///Sets thread properties, so that, when a thread is created, it is associated such properties
        ///independently from the thread attributes that were specified in the executeable file
        static void setThreadInfo(const std::string functionName, bool preemptive,
                           int schedPolicy, int priority, unsigned int deadline);
        ///Marks a function to be executed as an interrupt service routine
        static void registerISR(const std::string isrFunctionName, int irqId, bool preemptive = false,
                                        int schedPolicy = ConcurrencyManager::SYSC_SCHED_FIFO,
                                                int priority = ConcurrencyManager::SYSC_PRIO_MAX);
        ///Sets the thread-local-storage data which is going to be used
        ///for the thread private storage
        static void setTLSdata(const std::vector<unsigned char> tlsData);
};

///This class is in charge of performing initialization of all
///the necessary concurrency management structures, of the registration
///of the emulated routines, etc.
///Note that the executable must be compiled with the -specs=osemu.specs,
///Otherwise is will be impossible to use the Operating System
///Emulation features.
template<class wordSize> class ConcurrencyEmulator: public trap::ToolsIf<issueWidth>, ConcurrencyEmulatorBase{
    private:
        ///routine offset
        int routineOffset;
        ///map holding the addresses of the emulated routines
        template_map<issueWidth, trap::SyscallCB<issueWidth>* > syscCallbacks;
        typename template_map<issueWidth, trap::SyscallCB<issueWidth>* >::const_iterator syscCallbacksEnd;
        ///instance of the processor interface
        trap::ABIIf<issueWidth> &processorInstance;

        ///Associates an emulated functionality with a routine name
        ///Returns true if the association was successfully performed
        bool register_syscall(const std::string funName, trap::SyscallCB<issueWidth> &callBack){
            BFDWrapper &bfdFE = BFDWrapper::getInstance();
            bool valid = false;
            unsigned int symAddr = bfdFE.getSymAddr(funName, valid) + this->routineOffset;
            if(!valid){
                return false;
            }

            typename template_map<issueWidth, trap::SyscallCB<issueWidth>* >::iterator foundSysc = this->syscCallbacks.find(symAddr);
            if(foundSysc != this->syscCallbacks.end()){
                int numMatch = 0;
                typename template_map<issueWidth, trap::SyscallCB<issueWidth>* >::iterator allCallIter, allCallEnd;
                for(allCallIter = this->syscCallbacks.begin(), allCallEnd = this->syscCallbacks.end(); allCallIter != allCallEnd; allCallIter++){
                    if(allCallIter->second == foundSysc->second)
                        numMatch++;
                }
                if(numMatch <= 1){
                    delete foundSysc->second;
                }
            }

            this->syscCallbacks[symAddr] = &callBack;
            this->syscCallbacksEnd = this->syscCallbacks.end();

            return true;
        }

        ///Associates an emulated functionality with an address
        ///Returns true if the association was successfully performed
        bool register_syscall(issueWidth address, trap::SyscallCB<issueWidth> &callBack){
            typename template_map<issueWidth, trap::SyscallCB<issueWidth>* >::iterator foundSysc = this->syscCallbacks.find(address);
            if(foundSysc != this->syscCallbacks.end()){
                int numMatch = 0;
                typename template_map<issueWidth, trap::SyscallCB<issueWidth>* >::iterator allCallIter, allCallEnd;
                for(allCallIter = this->syscCallbacks.begin(), allCallEnd = this->syscCallbacks.end(); allCallIter != allCallEnd; allCallIter++){
                    if(allCallIter->second == foundSysc->second)
                        numMatch++;
                }
                if(numMatch <= 1){
                    delete foundSysc->second;
                }
            }

            this->syscCallbacks[address] = &callBack;
            this->syscCallbacksEnd = this->syscCallbacks.end();

            return true;
        }
        ///In a multi-processor environment this function has to be called to
        ///enable the protection of the internal OS routines, ensuring their
        ///atomicity
        ///The emulation group also needs to be specified
        void initiReentrantEmu(int group = 0){
            //First of all I have to initialize the reentrant emulation in the concurrency
            //manager
            if(ConcurrencyEmulatorBase::cm.find(group) == ConcurrencyEmulatorBase::cm.end())
                THROW_EXCEPTION("The current emulation group " << group << " has not been initialized yet: have you correctly called the initSysCalls method?");
            ConcurrencyManager * curCm  = ConcurrencyEmulatorBase::cm[group];
            curCm->initReentrantEmulation();

            //Now I can register the routines managing reentrant emulation
            __malloc_lockSysCall *a = new __malloc_lockSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("__malloc_lock", *a))
                delete a;
            __malloc_unlockSysCall *b = new __malloc_unlockSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("__malloc_unlock", *b))
                delete b;
            __sfp_lock_acquireSysCall *c = new __sfp_lock_acquireSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("__sfp_lock_acquire", *c))
                delete c;
            __sfp_lock_releaseSysCall *d = new __sfp_lock_releaseSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("__sfp_lock_release", *d))
                delete d;
            __sinit_lock_acquireSysCall *e = new __sinit_lock_acquireSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("__sinit_lock_acquire", *e))
                delete e;
            __sinit_lock_releaseSysCall *f = new __sinit_lock_releaseSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("__sinit_lock_release", *f))
                delete f;
            __fp_lockSysCall *g = new __fp_lockSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("__fp_lock", *g))
                delete g;
            __fp_unlockSysCall *h = new __fp_unlockSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("__fp_unlock", *h))
                delete h;
        }
    public:
        ///Constructor of the Emulator: its main task consists of creating an instance of the emulator
        ConcurrencyEmulator(ABIIf<issueWidth> &processorInstance, int routineOffset = 0) :
                                processorInstance(processorInstance), routineOffset(routineOffset){
            this->syscCallbacksEnd = this->syscCallbacks.end();
        }
        ///Returns the list of the routines marked for emulation insisde the this manager
        std::set<std::string> getRegisteredFunctions(){
            BFDWrapper &bfdFE = BFDWrapper::getInstance();
            std::set<std::string> registeredFunctions;
            typename template_map<issueWidth, trap::SyscallCB<issueWidth>* >::iterator emuIter, emuEnd;
            for(emuIter = this->syscCallbacks.begin(), emuEnd = this->syscCallbacks.end(); emuIter != emuEnd; emuIter++){
                registeredFunctions.insert(bfdFE.symbolAt(emuIter->first));
            }
            return registeredFunctions;
        }
        void initSysCalls(std::string execName, bool reentrant = false, int group = 0){
            std::map<std::string, sc_time> emptyLatMap;
            this->initSysCalls(execName, emptyLatMap, reentrant, group);
        }
        ///Initializes the system calls for emulation; it registers all the PTHREAD routines which need
        ///emulation. The group parameter specifies the current emulation group
        void initSysCalls(const std::string execName, const std::map<std::string, sc_time> &latencies, bool reentrant = false, int group = 0){
            //First of all I have to make sure that a concurrency manager for the current group
            //does not exist yet
            if(ConcurrencyEmulatorBase::cm.find(group) == ConcurrencyEmulatorBase::cm.end()){
                ConcurrencyEmulatorBase::cm[group] = new ConcurrencyManager();
            }
            ConcurrencyManager * curCm  = ConcurrencyEmulatorBase::cm[group];
            std::map<std::string, sc_time>::const_iterator curLatency;
            //Lets add a processor to the current concurrency manager
            curCm->addProcessor(this->processorInstance);
            //Now I initialized the BFD library with an instance of the current executable file
            BFDWrapper::getInstance(execName);
            //Now I perform the registration of the emulated pthread-Calls
            pthread_mutex_destroySysCall *a = NULL;
            curLatency = latencies.find("pthread_mutex_destroy");
            if(curLatency != latencies.end())
                a = new pthread_mutex_destroySysCall(this->processorInstance, curCm, curLatency);
            else
                a = new pthread_mutex_destroySysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_mutex_destroy", *a))
                delete a;
            pthread_mutex_initSysCall *b = NULL;
            curLatency = latencies.find("pthread_mutex_init");
            if(curLatency != latencies.end())
                b = new pthread_mutex_initSysCall(this->processorInstance, curCm, curLatency);
            else
                b = new pthread_mutex_initSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_mutex_init", *b))
                delete b;
            pthread_mutex_lockSysCall *c = NULL;
            curLatency = latencies.find("pthread_mutex_lock");
            if(curLatency != latencies.end())
                c = new pthread_mutex_lockSysCall(this->processorInstance, curCm, curLatency);
            else
                c = new pthread_mutex_lockSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_mutex_lock", *c))
                delete c;
            pthread_mutex_unlockSysCall *d = NULL;
            curLatency = latencies.find("pthread_mutex_unlock");
            if(curLatency != latencies.end())
                d = new pthread_mutex_unlockSysCall(this->processorInstance, curCm, curLatency);
            else
                d = new pthread_mutex_unlockSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_mutex_unlock", *d))
                delete d;
            pthread_mutex_trylockSysCall *e = NULL;
            curLatency = latencies.find("pthread_mutex_trylock");
            if(curLatency != latencies.end())
                e = new pthread_mutex_trylockSysCall(this->processorInstance, curCm, curLatency);
            else
                e = new pthread_mutex_trylockSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_mutex_trylock", *e))
                delete e;
            pthread_mutexattr_destroySysCall *f = NULL;
            curLatency = latencies.find("pthread_mutexattr_destroy");
            if(curLatency != latencies.end())
                f = new pthread_mutexattr_destroySysCall(this->processorInstance, curCm, curLatency);
            else
                f = new pthread_mutexattr_destroySysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_mutexattr_destroy", *f))
                delete f;
            pthread_mutexattr_initSysCall *g = NULL;
            curLatency = latencies.find("pthread_mutexattr_init");
            if(curLatency != latencies.end())
                g = new pthread_mutexattr_initSysCall(this->processorInstance, curCm, curLatency);
            else
                g = new pthread_mutexattr_initSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_mutexattr_init", *g))
                delete g;
            pthread_mutexattr_gettypeSysCall *h = NULL;
            curLatency = latencies.find("pthread_mutexattr_gettype");
            if(curLatency != latencies.end())
                h = new pthread_mutexattr_gettypeSysCall(this->processorInstance, curCm, curLatency);
            else
                h = new pthread_mutexattr_gettypeSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_mutexattr_gettype", *h))
                delete h;
            pthread_mutexattr_settypeSysCall *i = NULL;
            curLatency = latencies.find("pthread_mutexattr_settype");
            if(curLatency != latencies.end())
                i = new pthread_mutexattr_settypeSysCall(this->processorInstance, curCm, curLatency);
            else
                i = new pthread_mutexattr_settypeSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_mutexattr_settype", *i))
                delete i;
            pthread_attr_getdetachstateSysCall *j = NULL;
            curLatency = latencies.find("pthread_attr_getdetachstate");
            if(curLatency != latencies.end())
                j = new pthread_attr_getdetachstateSysCall(this->processorInstance, curCm, curLatency);
            else
                j = new pthread_attr_getdetachstateSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_attr_getdetachstate", *j))
                delete j;
            pthread_attr_setdetachstateSysCall *k = NULL;
            curLatency = latencies.find("pthread_attr_setdetachstate");
            if(curLatency != latencies.end())
                k = new pthread_attr_setdetachstateSysCall(this->processorInstance, curCm, curLatency);
            else
                k = new pthread_attr_setdetachstateSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_attr_setdetachstate", *k))
                delete k;
            pthread_attr_destroySysCall *l = NULL;
            curLatency = latencies.find("pthread_attr_destroy");
            if(curLatency != latencies.end())
                l = new pthread_attr_destroySysCall(this->processorInstance, curCm, curLatency);
            else
                l = new pthread_attr_destroySysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_attr_destroy", *l))
                delete l;
            pthread_attr_initSysCall *m = NULL;
            curLatency = latencies.find("pthread_attr_init");
            if(curLatency != latencies.end())
                m = new pthread_attr_initSysCall(this->processorInstance, curCm, curLatency);
            else
                m = new pthread_attr_initSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_attr_init", *m))
                delete m;
            pthread_attr_getstacksizeSysCall *n = NULL;
            curLatency = latencies.find("pthread_attr_getstacksize");
            if(curLatency != latencies.end())
                n = new pthread_attr_getstacksizeSysCall(this->processorInstance, curCm, curLatency);
            else
                n = new pthread_attr_getstacksizeSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_attr_getstacksize", *n))
                delete n;
            pthread_attr_setstacksizeSysCall *o = NULL;
            curLatency = latencies.find("pthread_attr_setstacksize");
            if(curLatency != latencies.end())
                o = new pthread_attr_setstacksizeSysCall(this->processorInstance, curCm, curLatency);
            else
                o = new pthread_attr_setstacksizeSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_attr_setstacksize", *o))
                delete o;
            pthread_getspecificSysCall *p = NULL;
            curLatency = latencies.find("pthread_getspecific");
            if(curLatency != latencies.end())
                p = new pthread_getspecificSysCall(this->processorInstance, curCm, curLatency);
            else
                p = new pthread_getspecificSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_getspecific", *p))
                delete p;
            pthread_setspecificSysCall *q = NULL;
            curLatency = latencies.find("pthread_setspecific");
            if(curLatency != latencies.end())
                q = new pthread_setspecificSysCall(this->processorInstance, curCm, curLatency);
            else
                q = new pthread_setspecificSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_setspecific", *q))
                delete q;
            pthread_createSysCall *r = NULL;
            curLatency = latencies.find("pthread_create");
            if(curLatency != latencies.end())
                r = new pthread_createSysCall(this->processorInstance, curCm, curLatency);
            else
                r = new pthread_createSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_create", *r))
                delete r;
            pthread_key_createSysCall *s = NULL;
            curLatency = latencies.find("pthread_key_create");
            if(curLatency != latencies.end())
                s = new pthread_key_createSysCall(this->processorInstance, curCm, curLatency);
            else
                s = new pthread_key_createSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_key_create", *s))
                delete s;
            sem_initSysCall *t = NULL;
            curLatency = latencies.find("sem_init");
            if(curLatency != latencies.end())
                t = new sem_initSysCall(this->processorInstance, curCm, curLatency);
            else
                t = new sem_initSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("sem_init", *t))
                delete t;
            sem_postSysCall *u = NULL;
            curLatency = latencies.find("sem_post");
            if(curLatency != latencies.end())
                u = new sem_postSysCall(this->processorInstance, curCm, curLatency);
            else
                u = new sem_postSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("sem_post", *u))
                delete u;
            sem_destroySysCall *v = NULL;
            curLatency = latencies.find("sem_destroy");
            if(curLatency != latencies.end())
                v = new sem_destroySysCall(this->processorInstance, curCm, curLatency);
            else
                v = new sem_destroySysCall(this->processorInstance, curCm);
            if(!this->register_syscall("sem_destroy", *v))
                delete v;
            pthread_exitSysCall *w = NULL;
            curLatency = latencies.find("pthread_exit");
            if(curLatency != latencies.end())
                w = new pthread_exitSysCall(this->processorInstance, curCm, curLatency->second);
            else
                w = new pthread_exitSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_exit", *w))
                delete w;
            sem_waitSysCall *x = NULL;
            curLatency = latencies.find("sem_wait");
            if(curLatency != latencies.end())
                x = new sem_waitSysCall(this->processorInstance, curCm, curLatency->second);
            else
                x = new sem_waitSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("sem_wait", *x))
                delete x;
            pthread_joinSysCall *y = NULL;
            curLatency = latencies.find("pthread_join");
            if(curLatency != latencies.end())
                y = new pthread_joinSysCall(this->processorInstance, curCm, curLatency->second);
            else
                y = new pthread_joinSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_join", *y))
                delete y;
            pthread_join_allSysCall *z = NULL;
            curLatency = latencies.find("pthread_join_all");
            if(curLatency != latencies.end())
                z = new pthread_join_allSysCall(this->processorInstance, curCm, curLatency->second);
            else
                z = new pthread_join_allSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_join_all", *z))
                delete z;
            __aeabi_read_tpSysCall *A = NULL;
            curLatency = latencies.find("__aeabi_read_tp");
            if(curLatency != latencies.end())
                A = new __aeabi_read_tpSysCall(this->processorInstance, curCm, curLatency->second);
            else
                A = new __aeabi_read_tpSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("__aeabi_read_tp", *A))
                delete A;
            pthread_cond_initSysCall *B = NULL;
            curLatency = latencies.find("pthread_cond_init");
            if(curLatency != latencies.end())
                B = new pthread_cond_initSysCall(this->processorInstance, curCm, curLatency->second);
            else
                B = new pthread_cond_initSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_cond_init", *B))
                delete B;
            pthread_cond_signalSysCall *C = NULL;
            curLatency = latencies.find("pthread_cond_signal");
            if(curLatency != latencies.end())
                C = new pthread_cond_signalSysCall(this->processorInstance, curCm, curLatency->second);
            else
                C = new pthread_cond_signalSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_cond_signal", *C))
                delete C;
            pthread_cond_broadcastSysCall *D = NULL;
            curLatency = latencies.find("pthread_cond_broadcast");
            if(curLatency != latencies.end())
                D = new pthread_cond_broadcastSysCall(this->processorInstance, curCm, curLatency->second);
            else
                D = new pthread_cond_broadcastSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_cond_broadcast", *D))
                delete D;
            pthread_cond_waitSysCall *E = NULL;
            curLatency = latencies.find("pthread_cond_wait");
            if(curLatency != latencies.end())
                E = new pthread_cond_waitSysCall(this->processorInstance, curCm, curLatency->second);
            else
                E = new pthread_cond_waitSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_cond_wait", *E))
                delete E;
            pthread_cond_destroySysCall *F = NULL;
            curLatency = latencies.find("pthread_cond_destroy");
            if(curLatency != latencies.end())
                F = new pthread_cond_destroySysCall(this->processorInstance, curCm, curLatency->second);
            else
                F = new pthread_cond_destroySysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_cond_destroy", *F))
                delete F;
            pthread_cond_timedwaitSysCall *G = NULL;
            curLatency = latencies.find("pthread_cond_timedwait");
            if(curLatency != latencies.end())
                G = new pthread_cond_timedwaitSysCall(this->processorInstance, curCm, curLatency->second);
            else
                G = new pthread_cond_timedwaitSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_cond_timedwait", *G))
                delete G;
            pthread_attr_getschedpolicySysCall *H = NULL;
            curLatency = latencies.find("pthread_attr_getschedpolicy");
            if(curLatency != latencies.end())
                H = new pthread_attr_getschedpolicySysCall(this->processorInstance, curCm, curLatency->second);
            else
                H = new pthread_attr_getschedpolicySysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_attr_getschedpolicy", *H))
                delete H;
            pthread_attr_setschedpolicySysCall *I = NULL;
            curLatency = latencies.find("pthread_attr_setschedpolicy");
            if(curLatency != latencies.end())
                I = new pthread_attr_setschedpolicySysCall(this->processorInstance, curCm, curLatency->second);
            else
                I = new pthread_attr_setschedpolicySysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_attr_setschedpolicy", *I))
                delete I;
            pthread_attr_getschedparamSysCall *J = NULL;
            curLatency = latencies.find("pthread_attr_getschedparam");
            if(curLatency != latencies.end())
                J = new pthread_attr_getschedparamSysCall(this->processorInstance, curCm, curLatency->second);
            else
                J = new pthread_attr_getschedparamSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_attr_getschedparam", *J))
                delete J;
            pthread_attr_setschedparamSysCall *K = NULL;
            curLatency = latencies.find("pthread_attr_setschedparam");
            if(curLatency != latencies.end())
                K = new pthread_attr_setschedparamSysCall(this->processorInstance, curCm, curLatency->second);
            else
                K = new pthread_attr_setschedparamSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_attr_setschedparam", *K))
                delete K;
            pthread_attr_getpreemptparamSysCall *L = NULL;
            curLatency = latencies.find("pthread_attr_getpreemptparam");
            if(curLatency != latencies.end())
                L = new pthread_attr_getpreemptparamSysCall(this->processorInstance, curCm, curLatency->second);
            else
                L = new pthread_attr_getpreemptparamSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_attr_getpreemptparam", *L))
                delete L;
            pthread_attr_setpreemptparamSysCall *M = NULL;
            curLatency = latencies.find("pthread_attr_setpreemptparam");
            if(curLatency != latencies.end())
                M = new pthread_attr_setpreemptparamSysCall(this->processorInstance, curCm, curLatency->second);
            else
                M = new pthread_attr_setpreemptparamSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_attr_setpreemptparam", *M))
                delete M;
            pthread_getschedparamSysCall *N = NULL;
            curLatency = latencies.find("pthread_getschedparam");
            if(curLatency != latencies.end())
                N = new pthread_getschedparamSysCall(this->processorInstance, curCm, curLatency->second);
            else
                N = new pthread_getschedparamSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_getschedparam", *N))
                delete N;
            pthread_setschedparamSysCall *O = NULL;
            curLatency = latencies.find("pthread_setschedparam");
            if(curLatency != latencies.end())
                O = new pthread_setschedparamSysCall(this->processorInstance, curCm, curLatency->second);
            else
                O = new pthread_setschedparamSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_setschedparam", *O))
                delete O;
            pthread_selfSysCall *P = NULL;
            curLatency = latencies.find("pthread_self");
            if(curLatency != latencies.end())
                P = new pthread_selfSysCall(this->processorInstance, curCm, curLatency->second);
            else
                P = new pthread_selfSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_self", *P))
                delete P;
            pthread_cancelSysCall *Q = NULL;
            curLatency = latencies.find("pthread_cancel");
            if(curLatency != latencies.end())
                Q = new pthread_cancelSysCall(this->processorInstance, curCm, curLatency->second);
            else
                Q = new pthread_cancelSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_cancel", *Q))
                delete Q;
            pthread_cleanup_popSysCall *R = NULL;
            curLatency = latencies.find("pthread_cleanup_pop");
            if(curLatency != latencies.end())
                R = new pthread_cleanup_popSysCall(this->processorInstance, curCm, curLatency->second);
            else
                R = new pthread_cleanup_popSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_cleanup_pop", *R))
                delete R;
            pthread_cleanup_pushSysCall *S = NULL;
            curLatency = latencies.find("pthread_cleanup_push");
            if(curLatency != latencies.end())
                S = new pthread_cleanup_pushSysCall(this->processorInstance, curCm, curLatency->second);
            else
                S = new pthread_cleanup_pushSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_cleanup_push", *S))
                delete S;
            pthread_exitSysCall *T = NULL;
            curLatency = latencies.find("pthread_myexit");
            if(curLatency != latencies.end())
                T = new pthread_exitSysCall(this->processorInstance, curCm, curLatency->second);
            else
                T = new pthread_exitSysCall(this->processorInstance, curCm);
            if(!this->register_syscall("pthread_myexit", *T))
                delete T;

            if(waitLoop){
                __nop_busy_loopSysCall *U = new __nop_busy_loopSysCall(this->processorInstance, curCm);
                if(!this->register_syscall(".__nop_busy_loop", *U))
                    THROW_EXCEPTION(".__nop_busy_loop symbol not found in the executable, unable to initialize pthread-emulation system");
            }
            if(reentrant)
                this->initiReentrantEmu(group);
        }

        ///Resets the whole concurrency emulator, reinitializing it and preparing it for a new simulation
        void reset(){
            this->syscCallbacks.clear();
            std::map<int, ConcurrencyManager * >::iterator cmIter, cmEnd;
            for(cmIter = ConcurrencyEmulatorBase::cm.begin(), cmEnd = ConcurrencyEmulatorBase::cm.end(); cmIter != cmEnd; cmIter++){
                cmIter->second->reset();
            }
            ConcurrencyEmulatorBase::cm.clear();
        }

        ///Method called at every instruction issue by the processor: we have to determine if
        ///the current program counter corresponds to an emulated call and call that
        ///call in case
        bool newIssue(const issueWidth &curPC, const InstructionBase *curInstr) throw(){
            //I have to go over all the registered system calls and check if there is one
            //that matches the current program counter. In case I simply call the corresponding
            //callback.
            typename template_map<issueWidth, SyscallCB<issueWidth>* >::const_iterator foundSysc = this->syscCallbacks.find(curPC + stageOffset);
            if(foundSysc != this->syscCallbacksEnd){
                return (*(foundSysc->second))();
            }
            return false;
        }

};

};


#endif
