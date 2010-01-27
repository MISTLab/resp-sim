#ifndef PTHREAD_H
#define PTHREAD_H

#include <sys/types.h>

#include "mutex.h"
#include "cond.h"
#include "semaphore.h"
#include "sched.h"

typedef int pthread_attr_t;
typedef int pthread_key_t;
typedef int pthread_t;

#define PTHREAD_CREATE_DETACHED 1
#define PTHREAD_CREATE_JOINABLE 0
#define PTHREAD_SCOPE_PROCESS 1
#define PTHREAD_SCOPE_SYSTEM 0
#ifndef ENOSYS
#  define ENOSYS 140     /* Semi-arbitrary value */
#endif

#ifdef __cplusplus
extern "C" {
#endif

void pthread_cleanup_push(void (*routine)(void *), void *arg);
void pthread_cleanup_pop(int execute);
int pthread_cancel(pthread_t thread);

int pthread_equal(pthread_t t1, pthread_t t2);
pthread_t pthread_self(void);

int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate);
int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);

int pthread_attr_destroy(pthread_attr_t *attr);
int pthread_attr_init(pthread_attr_t *attr);

int pthread_attr_getstacksize(const pthread_attr_t *__restrict__ attr, size_t *__restrict__ stacksize);
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);

void *pthread_getspecific(pthread_key_t key);
int pthread_setspecific(pthread_key_t key, const void *value);

int pthread_create(pthread_t *__restrict__ thread, const pthread_attr_t *__restrict__ attr, void *(*start_routine)(void*), void *__restrict__ arg);
void pthread_exit(void *value_ptr);

int pthread_key_create(pthread_key_t *key, void (*destructor)(void*));

int pthread_join(pthread_t thread, void **value_ptr);
void pthread_join_all();

int pthread_attr_getscope(const pthread_attr_t *__restrict__ attr, int *__restrict__ contentionscope);
int pthread_attr_setscope(pthread_attr_t *attr, int contentionscope);

int pthread_attr_getschedpolicy(const pthread_attr_t *__restrict__ attr, int *__restrict__ policy);
int pthread_attr_setschedpolicy(pthread_attr_t *__restrict__ attr, int policy);

int pthread_attr_getschedparam(const pthread_attr_t *__restrict__ attr, struct sched_param *__restrict__ param);
int pthread_attr_setschedparam(pthread_attr_t *__restrict__ attr, const struct sched_param *__restrict__ param);

int pthread_attr_getpreemptparam(const pthread_attr_t *__restrict__ attr, int *__restrict__ param);
int pthread_attr_setpreemptparam(pthread_attr_t *__restrict__ attr, int param);

int pthread_getschedparam(pthread_t thread, int *__restrict__ policy, struct sched_param *__restrict__ param);
int pthread_setschedparam(pthread_t thread, int policy, const struct sched_param *param);

void * __aeabi_read_tp();

#ifdef __cplusplus
}
#endif

#endif
