#include <stdio.h>
#include "mutex.h"

int __attribute__ ((noinline)) pthread_mutex_destroy(pthread_mutex_t *mutex){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return 0;
}

int __attribute__ ((noinline)) pthread_mutex_init(pthread_mutex_t *__restrict__ mutex, const pthread_mutexattr_t *__restrict__ attr){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return 0;
}

int __attribute__ ((noinline)) pthread_mutex_lock(pthread_mutex_t *mutex){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return 0;
}

int __attribute__ ((noinline)) pthread_mutex_unlock(pthread_mutex_t *mutex){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return 0;
}

int __attribute__ ((noinline)) pthread_mutex_trylock(pthread_mutex_t *mutex){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return 0;
}

int __attribute__ ((noinline)) pthread_mutexattr_destroy(pthread_mutexattr_t *attr){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return 0;
}

int __attribute__ ((noinline)) pthread_mutexattr_init(pthread_mutexattr_t *attr){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return 0;
}

int __attribute__ ((noinline)) pthread_mutexattr_gettype(const pthread_mutexattr_t *__restrict__ attr, int *__restrict__ type){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return 0;
}

int __attribute__ ((noinline)) pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return 0;
}

