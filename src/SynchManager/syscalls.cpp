#include "controller.hpp"
#include "ac_syscall.H"
#include "syscalls.hpp"
#include "synchManager.hpp"

#include <map>

#ifndef NDEBUG
#include <iostream>
#define DEBUG_PRINT(message) std::cerr << message << std::endl;
#else
#define DEBUG_PRINT(message)
#endif

using namespace resp;

void resp::registerPthreadEmu(std::map<std::string, sc_time> latencies){
    pthread_mutex_destroySysCall *a = new pthread_mutex_destroySysCall();
    if(!archc::register_syscall("pthread_mutex_destroy", *a, latencies, false))
       delete a;
    pthread_mutex_initSysCall *b = new pthread_mutex_initSysCall();
    if(!archc::register_syscall("pthread_mutex_init", *b, latencies, false))
       delete b;
    pthread_mutex_lockSysCall *c = new pthread_mutex_lockSysCall();
    if(!archc::register_syscall("pthread_mutex_lock", *c, latencies, false))
       delete c;
    pthread_mutex_unlockSysCall *d = new pthread_mutex_unlockSysCall();
    if(!archc::register_syscall("pthread_mutex_unlock", *d, latencies, false))
       delete d;
    pthread_mutex_trylockSysCall *e = new pthread_mutex_trylockSysCall();
    if(!archc::register_syscall("pthread_mutex_trylock", *e, latencies, false))
       delete e;
    pthread_mutexattr_destroySysCall *f = new pthread_mutexattr_destroySysCall();
    if(!archc::register_syscall("pthread_mutexattr_destroy", *f, latencies, false))
       delete f;
    pthread_mutexattr_initSysCall *g = new pthread_mutexattr_initSysCall();
    if(!archc::register_syscall("pthread_mutexattr_init", *g, latencies, false))
       delete g;
    pthread_mutexattr_gettypeSysCall *h = new pthread_mutexattr_gettypeSysCall();
    if(!archc::register_syscall("pthread_mutexattr_gettype", *h, latencies, false))
       delete h;
    pthread_mutexattr_settypeSysCall *i = new pthread_mutexattr_settypeSysCall();
    if(!archc::register_syscall("pthread_mutexattr_settype", *i, latencies, false))
       delete i;
    pthread_attr_getdetachstateSysCall *j = new pthread_attr_getdetachstateSysCall();
    if(!archc::register_syscall("pthread_attr_getdetachstate", *j, latencies, false))
       delete j;
    pthread_attr_setdetachstateSysCall *k = new pthread_attr_setdetachstateSysCall();
    if(!archc::register_syscall("pthread_attr_setdetachstate", *k, latencies, false))
       delete k;
    pthread_attr_destroySysCall *l = new pthread_attr_destroySysCall();
    if(!archc::register_syscall("pthread_attr_destroy", *l, latencies, false))
       delete l;
    pthread_attr_initSysCall *m = new pthread_attr_initSysCall();
    if(!archc::register_syscall("pthread_attr_init", *m, latencies, false))
       delete m;
    pthread_attr_getstacksizeSysCall *n = new pthread_attr_getstacksizeSysCall();
    if(!archc::register_syscall("pthread_attr_getstacksize", *n, latencies, false))
       delete n;
    pthread_attr_setstacksizeSysCall *o = new pthread_attr_setstacksizeSysCall();
    if(!archc::register_syscall("pthread_attr_setstacksize", *o, latencies, false))
       delete o;
    pthread_getspecificSysCall *p = new pthread_getspecificSysCall();
    if(!archc::register_syscall("pthread_getspecific", *p, latencies, false))
       delete p;
    pthread_setspecificSysCall *q = new pthread_setspecificSysCall();
    if(!archc::register_syscall("pthread_setspecific", *q, latencies, false))
       delete q;
    pthread_createSysCall *r = new pthread_createSysCall();
    if(!archc::register_syscall("pthread_create", *r, latencies, false))
       delete r;
    pthread_key_createSysCall *s = new pthread_key_createSysCall();
    if(!archc::register_syscall("pthread_key_create", *s, latencies, false))
       delete s;
    sem_initSysCall *t = new sem_initSysCall();
    if(!archc::register_syscall("sem_init", *t, latencies, false))
       delete t;
    sem_postSysCall *u = new sem_postSysCall();
    if(!archc::register_syscall("sem_post", *u, latencies, false))
       delete u;
    sem_destroySysCall *v = new sem_destroySysCall();
    if(!archc::register_syscall("sem_destroy", *v, latencies, false))
       delete v;
    pthread_exitSysCall *w = new pthread_exitSysCall();
    if(!archc::register_syscall("pthread_exit", *w, latencies, false))
       delete w;
    sem_waitSysCall *x = new sem_waitSysCall();
    if(!archc::register_syscall("sem_wait", *x, latencies, false))
       delete x;
    pthread_joinSysCall *y = new pthread_joinSysCall();
    if(!archc::register_syscall("pthread_join", *y, latencies, false))
       delete y;
    pthread_join_allSysCall *z = new pthread_join_allSysCall();
    if(!archc::register_syscall("pthread_join_all", *z, latencies, false))
       delete z;
    __aeabi_read_tpSysCall *A = new __aeabi_read_tpSysCall();
    if(!archc::register_syscall("__aeabi_read_tp", *A, latencies, false))
        delete A;
    pthread_cond_initSysCall *B = new pthread_cond_initSysCall();
    if(!archc::register_syscall("pthread_cond_init", *B, latencies, false))
       delete B;
    pthread_cond_signalSysCall *C = new pthread_cond_signalSysCall();
    if(!archc::register_syscall("pthread_cond_signal", *C, latencies, false))
       delete C;
    pthread_cond_broadcastSysCall *D = new pthread_cond_broadcastSysCall();
    if(!archc::register_syscall("pthread_cond_broadcast", *D, latencies, false))
       delete D;
    pthread_cond_waitSysCall *E = new pthread_cond_waitSysCall();
    if(!archc::register_syscall("pthread_cond_wait", *E, latencies, false))
       delete E;
    pthread_cond_destroySysCall *F = new pthread_cond_destroySysCall();
    if(!archc::register_syscall("pthread_cond_destroy", *F, latencies, false))
       delete F;
    pthread_cond_timedwaitSysCall *G = new pthread_cond_timedwaitSysCall();
    if(!archc::register_syscall("pthread_cond_timedwait", *G, latencies, false))
        delete G;
    pthread_attr_getschedpolicySysCall *H = new pthread_attr_getschedpolicySysCall();
    if(!archc::register_syscall("pthread_attr_getschedpolicy", *H, latencies, false))
        delete H;
    pthread_attr_setschedpolicySysCall *I = new pthread_attr_setschedpolicySysCall();
    if(!archc::register_syscall("pthread_attr_setschedpolicy", *I, latencies, false))
        delete I;
    pthread_attr_getschedparamSysCall *J = new pthread_attr_getschedparamSysCall();
    if(!archc::register_syscall("pthread_attr_getschedparam", *J, latencies, false))
        delete J;
    pthread_attr_setschedparamSysCall *K = new pthread_attr_setschedparamSysCall();
    if(!archc::register_syscall("pthread_attr_setschedparam", *K, latencies, false))
        delete K;
    pthread_attr_getpreemptparamSysCall *L = new pthread_attr_getpreemptparamSysCall();
    if(!archc::register_syscall("pthread_attr_getpreemptparam", *L, latencies, false))
        delete L;
    pthread_attr_setpreemptparamSysCall *M = new pthread_attr_setpreemptparamSysCall();
    if(!archc::register_syscall("pthread_attr_setpreemptparam", *M, latencies, false))
        delete M;
    pthread_getschedparamSysCall *N = new pthread_getschedparamSysCall();
    if(!archc::register_syscall("pthread_getschedparam", *N, latencies, false))
        delete N;
    pthread_setschedparamSysCall *O = new pthread_setschedparamSysCall();
    if(!archc::register_syscall("pthread_setschedparam", *O, latencies, false))
        delete O;
    pthread_selfSysCall *P = new pthread_selfSysCall();
    if(!archc::register_syscall("pthread_self", *P, latencies, false))
       delete P;
    pthread_cancelSysCall *Q = new pthread_cancelSysCall();
    if(!archc::register_syscall("pthread_cancel", *Q, latencies, false))
       delete Q;
    pthread_cleanup_popSysCall *R = new pthread_cleanup_popSysCall();
    if(!archc::register_syscall("pthread_cleanup_pop", *R, latencies, false))
       delete R;
    pthread_cleanup_pushSysCall *S = new pthread_cleanup_pushSysCall();
    if(!archc::register_syscall("pthread_cleanup_push", *S, latencies, false))
       delete S;
    pthread_myexitSysCall *T = new pthread_myexitSysCall();
    if(!archc::register_syscall("pthread_myexit", *T, latencies, false))
       delete T;

}

void resp::registerReentrantEmu(std::map<std::string, sc_time> latencies){
    __malloc_lockSysCall *a = new __malloc_lockSysCall();
    if(!archc::register_syscall("__malloc_lock", *a, latencies))
       delete a;
    __malloc_unlockSysCall *b = new __malloc_unlockSysCall();
    if(!archc::register_syscall("__malloc_unlock", *b, latencies))
       delete b;
    __sfp_lock_acquireSysCall *c = new __sfp_lock_acquireSysCall();
    if(!archc::register_syscall("__sfp_lock_acquire", *c, latencies))
       delete c;
    __sfp_lock_releaseSysCall *d = new __sfp_lock_releaseSysCall();
    if(!archc::register_syscall("__sfp_lock_release", *d, latencies))
       delete d;
    __sinit_lock_acquireSysCall *e = new __sinit_lock_acquireSysCall();
    if(!archc::register_syscall("__sinit_lock_acquire", *e, latencies))
       delete e;
    __sinit_lock_releaseSysCall *f = new __sinit_lock_releaseSysCall();
    if(!archc::register_syscall("__sinit_lock_release", *f, latencies))
       delete f;
    __fp_lockSysCall *g = new __fp_lockSysCall();
    if(!archc::register_syscall("__fp_lock", *g, latencies))
       delete g;
    __fp_unlockSysCall *h = new __fp_unlockSysCall();
    if(!archc::register_syscall("__fp_unlock", *h, latencies))
       delete h;
}

bool pthread_mutex_destroySysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    int mutexId = 0;
    processorInstance.get_buffer_endian(0, (unsigned char *)&mutexId, processorInstance.getWordSize());
    destroyMutex(processorInstance.get_proc_id(), mutexId);
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();
    sc_controller::disableLatency = false;

    return false;
}

bool pthread_mutex_initSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    int mutexId = createMutex(processorInstance.get_proc_id());
    processorInstance.set_buffer_endian(0, (unsigned char *)&mutexId, processorInstance.getWordSize());
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();
    sc_controller::disableLatency = false;

    return false;
}

bool pthread_mutex_lockSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
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

    sc_controller::disableLatency = false;
    return false;
}

bool pthread_mutex_unlockSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    int mutexId = 0;
    processorInstance.get_buffer_endian(0, (unsigned char *)&mutexId, processorInstance.getWordSize());
    unLockMutex(mutexId, processorInstance.get_proc_id());
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();
    sc_controller::disableLatency = false;

    return false;
}

bool pthread_mutex_trylockSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
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
    sc_controller::disableLatency = false;

    return false;
}

bool pthread_mutexattr_destroySysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();
    sc_controller::disableLatency = false;

    return false;
}

bool pthread_mutexattr_initSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();
    sc_controller::disableLatency = false;

    return false;
}

bool pthread_mutexattr_gettypeSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();
    sc_controller::disableLatency = false;

    return false;
}

bool pthread_mutexattr_settypeSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();
    sc_controller::disableLatency = false;

    return false;
}

bool pthread_attr_getdetachstateSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();
    sc_controller::disableLatency = false;

    return false;
}

bool pthread_attr_setdetachstateSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();
    sc_controller::disableLatency = false;

    return false;
}

bool pthread_attr_destroySysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    int attrId = 0;
    processorInstance.get_buffer_endian(0, (unsigned char *)&attrId, processorInstance.getWordSize());
    deleteThreadAttr(attrId);
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();
    sc_controller::disableLatency = false;

    return false;
}

bool pthread_attr_initSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    int attrId = createThreadAttr();
    processorInstance.set_buffer_endian(0, (unsigned char *)&attrId, processorInstance.getWordSize());
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();
    sc_controller::disableLatency = false;

    return false;
}

bool pthread_attr_getstacksizeSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    int attrId = 0;
    processorInstance.get_buffer_endian(0, (unsigned char *)&attrId, processorInstance.getWordSize());
    int stackSize = getStackSize(attrId);
    processorInstance.set_buffer_endian(1, (unsigned char *)&stackSize, processorInstance.getWordSize());
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();
    sc_controller::disableLatency = false;

    return false;
}

bool pthread_attr_setstacksizeSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    int attrId = 0;
    processorInstance.get_buffer_endian(0, (unsigned char *)&attrId, processorInstance.getWordSize());
    int stackSize = 0;
    processorInstance.get_buffer_endian(1, (unsigned char *)&stackSize, processorInstance.getWordSize());
    setStackSize(attrId, stackSize);
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();
    sc_controller::disableLatency = false;

    return false;
}

bool pthread_getspecificSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    int key = processorInstance.get_arg(0);
    unsigned int memArea = (unsigned int)(unsigned long)getSpecific(processorInstance.get_proc_id(), key);
    processorInstance.set_retVal(0, memArea);
    processorInstance.return_from_syscall();
    sc_controller::disableLatency = false;

    return false;
}

bool pthread_setspecificSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    int key = processorInstance.get_arg(0);
    void * memArea = (void *)processorInstance.get_arg(1);
    setSpecific(processorInstance.get_proc_id(), key, memArea);
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();
    sc_controller::disableLatency = false;

    return false;
}

bool pthread_createSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
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
    sc_controller::disableLatency = false;

    return false;
}

bool pthread_selfSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    processorInstance.set_retVal(0, getThreadId(processorInstance.get_proc_id()));
    sc_controller::disableLatency = false;

    return false;
}

bool pthread_cleanup_pushSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    THROW_EXCEPTION("pthread_cleanup_push system call not yet completely implemented");
    sc_controller::disableLatency = false;

    return false;
}

bool pthread_cleanup_popSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    THROW_EXCEPTION("pthread_cleanup_pop system call not yet completely implemented");
    sc_controller::disableLatency = false;

    return false;
}

bool pthread_cancelSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    THROW_EXCEPTION("pthread_cancel system call not yet completely implemented");
    sc_controller::disableLatency = false;

    return false;
}

bool pthread_exitSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    void * retVal = (void *)processorInstance.get_arg(0);

    exitThread(processorInstance.get_proc_id(), retVal);

    //There is not need for the call to processorInstance.return_from_syscall(); since I should never
    //return from this function,  I should always resume a new execution path (a new thread)
    sc_controller::disableLatency = false;

    return false;
}

bool pthread_myexitSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    void * retVal = (void *)processorInstance.get_retVal(0);

    exitThread(processorInstance.get_proc_id(), retVal);

    //There is not need for the call to processorInstance.return_from_syscall(); since I should never
    //return from this function,  I should always resume a new execution path (a new thread)
    sc_controller::disableLatency = false;

    return false;
}

bool pthread_key_createSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    int keyId = createKey();
    processorInstance.set_buffer_endian(0, (unsigned char *)&keyId, processorInstance.getWordSize());
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();
    sc_controller::disableLatency = false;

    return false;
}

bool sem_initSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    int semId = createSem(processorInstance.get_proc_id(), processorInstance.get_arg(2));
    processorInstance.set_buffer_endian(0, (unsigned char *)&semId, processorInstance.getWordSize());
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();
    sc_controller::disableLatency = false;

    return false;
}

bool sem_postSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    int semId = 0;
    processorInstance.get_buffer_endian(0, (unsigned char *)&semId, processorInstance.getWordSize());
    postSem(semId, processorInstance.get_proc_id());
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();
    sc_controller::disableLatency = false;

    return false;
}

bool sem_destroySysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    int semId = 0;
    processorInstance.get_buffer_endian(0, (unsigned char *)&semId, processorInstance.getWordSize());
    destroySem(processorInstance.get_proc_id(), semId);
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();
    sc_controller::disableLatency = false;

    return false;
}

bool sem_waitSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    int semId = 0;
    processorInstance.get_buffer_endian(0, (unsigned char *)&semId, processorInstance.getWordSize());
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();

    waitSem(semId, processorInstance.get_proc_id());
    sc_controller::disableLatency = false;

    return false;
}

bool pthread_joinSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

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

    return false;
}

bool pthread_join_allSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    processorInstance.return_from_syscall();

    joinAll(processorInstance.get_proc_id());
    sc_controller::disableLatency = false;

    return false;
}

bool pthread_cond_initSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    int condId = createCond(processorInstance.get_proc_id());
    processorInstance.set_buffer_endian(0, (unsigned char *)&condId, processorInstance.getWordSize());
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();
    sc_controller::disableLatency = false;

    return false;
}

bool pthread_cond_signalSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
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
    sc_controller::disableLatency = false;

    return false;
}

bool pthread_cond_broadcastSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
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
    sc_controller::disableLatency = false;

    return false;
}

bool pthread_cond_waitSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
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

    sc_controller::disableLatency = false;
    return false;
}

bool pthread_cond_destroySysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);

    if(this->latency.to_double() > 0 )
        wait(latency);

    sc_controller::disableLatency = true;
    int condId = 0;
    processorInstance.get_buffer_endian(0, (unsigned char *)&condId, processorInstance.getWordSize());
    destroyCond(processorInstance.get_proc_id(), condId);
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();
    sc_controller::disableLatency = false;

    return false;
}

bool pthread_cond_timedwaitSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);

    if(this->latency.to_double() > 0 )
        wait(latency);

    sc_controller::disableLatency = true;
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
    sc_controller::disableLatency = false;

    return false;
}

bool __aeabi_read_tpSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    processorInstance.set_retVal(0, (unsigned int)(unsigned long)readTLS(processorInstance.get_proc_id()).second);
    processorInstance.return_from_syscall();
    sc_controller::disableLatency = false;

    return false;
}

bool __nop_busy_loopSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);

    if(this->latency.to_double() > 0 )
        wait(latency);

    idleLoop(processorInstance.get_proc_id());
    return false;
}

bool __malloc_lockSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    processorInstance.return_from_syscall();
    lockMutex(mallocMutex, processorInstance.get_proc_id(), false);

    sc_controller::disableLatency = false;
    return false;
}

bool __malloc_unlockSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    processorInstance.return_from_syscall();
    unLockMutex(mallocMutex, processorInstance.get_proc_id());

    sc_controller::disableLatency = false;
    return false;
}

bool __sfp_lock_acquireSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    processorInstance.return_from_syscall();
    lockMutex(sfpMutex, processorInstance.get_proc_id(), false);

    sc_controller::disableLatency = false;
    return false;
}

bool __sfp_lock_releaseSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    processorInstance.return_from_syscall();
    unLockMutex(sfpMutex, processorInstance.get_proc_id());

    sc_controller::disableLatency = false;
    return false;
}

bool __sinit_lock_acquireSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    processorInstance.return_from_syscall();
    lockMutex(sinitMutex, processorInstance.get_proc_id(), false);

    sc_controller::disableLatency = false;
    return false;
}

bool __sinit_lock_releaseSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    processorInstance.return_from_syscall();
    unLockMutex(sinitMutex, processorInstance.get_proc_id());

    sc_controller::disableLatency = false;
    return false;
}

bool __fp_lockSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    processorInstance.return_from_syscall();
    lockMutex(fpMutex, processorInstance.get_proc_id(), false);

    sc_controller::disableLatency = false;
    return false;
}

bool __fp_unlockSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    processorInstance.return_from_syscall();
    unLockMutex(fpMutex, processorInstance.get_proc_id());

    sc_controller::disableLatency = false;
    return false;
}

bool pthread_attr_getschedpolicySysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    int attrId = 0;
    processorInstance.get_buffer_endian(0, (unsigned char *)&attrId, processorInstance.getWordSize());
    int policy = getSchedPolicy(attrId);
    processorInstance.set_buffer_endian(1, (unsigned char *)&policy, processorInstance.getWordSize());
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();
    sc_controller::disableLatency = false;

    return false;
}

bool pthread_attr_setschedpolicySysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    int attrId = 0;
    processorInstance.get_buffer_endian(0, (unsigned char *)&attrId, processorInstance.getWordSize());
    int policy = processorInstance.get_arg(1);
    setSchedPolicy(attrId, policy);
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();
    sc_controller::disableLatency = false;

    return false;
}

bool pthread_attr_getschedparamSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
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
    sc_controller::disableLatency = false;

    return false;
}

bool pthread_attr_setschedparamSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
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
    sc_controller::disableLatency = false;

    return false;
}

bool pthread_attr_getpreemptparamSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    int attrId = 0;
    processorInstance.get_buffer_endian(0, (unsigned char *)&attrId, processorInstance.getWordSize());
    int isPreemptive = getPreemptive(attrId);
    processorInstance.set_buffer_endian(1, (unsigned char *)&isPreemptive, processorInstance.getWordSize());
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();
    sc_controller::disableLatency = false;

    return false;
}

bool pthread_attr_setpreemptparamSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
    int attrId = 0;
    processorInstance.get_buffer_endian(0, (unsigned char *)&attrId, processorInstance.getWordSize());
    int isPreemptive = processorInstance.get_arg(1);
    setPreemptive(attrId, isPreemptive);
    processorInstance.set_retVal(0, 0);
    processorInstance.return_from_syscall();
    sc_controller::disableLatency = false;

    return false;
}

bool pthread_getschedparamSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
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
    sc_controller::disableLatency = false;

    return false;
}

bool pthread_setschedparamSysCall::operator()(archc::ac_syscall_base &processorInstance){
    DEBUG_PRINT(__PRETTY_FUNCTION__);
    if(this->latency.to_double() > 0)
        wait(this->latency);

    sc_controller::disableLatency = true;
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
    sc_controller::disableLatency = false;

    return false;
}
