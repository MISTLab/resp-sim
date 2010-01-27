#ifndef COND_H
#define COND_H

#include <time.h>
#include "mutex.h"

typedef int pthread_cond_t;
typedef int pthread_condattr_t;

#define PTHREAD_COND_INITIALIZER -1

#ifdef __cplusplus
extern "C" {
#endif

int pthread_cond_init(pthread_cond_t * __restrict__ cond, pthread_condattr_t * __restrict__ cond_attr);

int pthread_cond_signal(pthread_cond_t * __restrict__ cond);

int pthread_cond_broadcast(pthread_cond_t * __restrict__ cond);

int pthread_cond_wait(pthread_cond_t * __restrict__ cond, pthread_mutex_t * __restrict__ mutex);

int pthread_cond_timedwait(pthread_cond_t * __restrict__ cond, pthread_mutex_t * __restrict__ mutex, const struct timespec * __restrict__ abstime);

int pthread_cond_destroy(pthread_cond_t * __restrict__ cond);

#ifdef __cplusplus
}
#endif

#endif
