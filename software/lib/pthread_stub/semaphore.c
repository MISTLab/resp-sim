#include <stdio.h>
#include "semaphore.h"

int __attribute__ ((noinline)) sem_init(sem_t *sem, int pshared, unsigned int value){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return 0;
}

int __attribute__ ((noinline)) sem_post(sem_t *sem){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return 0;
}

int __attribute__ ((noinline)) sem_destroy(sem_t *sem){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return 0;
}

int __attribute__ ((noinline)) sem_wait(sem_t *sem){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return 0;
}
