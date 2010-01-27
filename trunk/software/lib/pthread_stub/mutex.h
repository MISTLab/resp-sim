#ifndef MUTEX_H
#define MUTEX_H

typedef int pthread_mutex_t;
typedef int pthread_mutexattr_t;

#define PTHREAD_MUTEX_INITIALIZER -1

#define PTHREAD_MUTEX_RECURSIVE 0

#ifdef __cplusplus
extern "C" {
#endif

int pthread_mutex_destroy(pthread_mutex_t *mutex);
int pthread_mutex_init(pthread_mutex_t *__restrict__ mutex, const pthread_mutexattr_t *__restrict__ attr);

int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
int pthread_mutex_trylock(pthread_mutex_t *mutex);

int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);
int pthread_mutexattr_init(pthread_mutexattr_t *attr);

int pthread_mutexattr_gettype(const pthread_mutexattr_t *__restrict__ attr, int *__restrict__ type);
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type);

#ifdef __cplusplus
}
#endif

#endif
