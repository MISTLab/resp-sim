#include <stdio.h>
#include <time.h>
#include "mutex.h"
#include "cond.h"

int __attribute__ ((noinline)) pthread_cond_init(pthread_cond_t * __restrict__ cond, pthread_condattr_t * __restrict__ cond_attr){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return 0;
}

int __attribute__ ((noinline)) pthread_cond_signal(pthread_cond_t * __restrict__ cond){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return 0;
}

int __attribute__ ((noinline)) pthread_cond_broadcast(pthread_cond_t * __restrict__ cond){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return 0;
}

int __attribute__ ((noinline)) pthread_cond_wait(pthread_cond_t * __restrict__ cond, pthread_mutex_t * __restrict__ mutex){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return 0;
}

int __attribute__ ((noinline)) pthread_cond_timedwait(pthread_cond_t * __restrict__ cond, pthread_mutex_t * __restrict__ mutex, const struct timespec * __restrict__ abstime){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return 0;
}

int __attribute__ ((noinline)) pthread_cond_destroy(pthread_cond_t * __restrict__ cond){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return 0;
}
