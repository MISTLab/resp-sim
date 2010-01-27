#include <stdio.h>
#include "pthread.h"

void __attribute__ ((noinline)) pthread_myexit(void){
    fprintf(stderr, "%s %d\n", __PRETTY_FUNCTION__);
}

void __attribute__ ((noinline)) pthread_cleanup_push(void (*routine)(void *), void *arg){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
}

void __attribute__ ((noinline)) pthread_cleanup_pop(int execute){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
}

int __attribute__ ((noinline)) pthread_cancel(pthread_t thread){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return 0;
}

int __attribute__ ((noinline)) pthread_equal(pthread_t t1, pthread_t t2){
    return (t1 == t2);
}

pthread_t __attribute__ ((noinline)) pthread_self(void){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return 0;
}

int __attribute__ ((noinline)) pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return 0;
}

int __attribute__ ((noinline)) pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return 0;
}

int __attribute__ ((noinline)) pthread_attr_destroy(pthread_attr_t *attr){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return 0;
}

int __attribute__ ((noinline)) pthread_attr_init(pthread_attr_t *attr){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return 0;
}

int __attribute__ ((noinline)) pthread_attr_getstacksize(const pthread_attr_t *__restrict__ attr, size_t *__restrict__ stacksize){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return 0;
}

int __attribute__ ((noinline)) pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return 0;
}

void * __attribute__ ((noinline)) pthread_getspecific(pthread_key_t key){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return NULL;
}

int __attribute__ ((noinline)) pthread_setspecific(pthread_key_t key, const void *value){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return 0;
}

int __attribute__ ((noinline)) pthread_create(pthread_t *__restrict__ thread, const pthread_attr_t *__restrict__ attr, void *(*start_routine)(void*), void *__restrict__ arg){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    //Lets call pthread _exit: this way the pthread_exit symbol will not be discarded
    pthread_exit(NULL);
    pthread_myexit();
    return 0;
}

void __attribute__ ((noinline)) pthread_exit(void *value_ptr) {
    fprintf(stderr, "%s %d\n", __PRETTY_FUNCTION__);
}

int __attribute__ ((noinline)) pthread_key_create(pthread_key_t *key, void (*destructor)(void*)){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return 0;
}

int __attribute__ ((noinline)) pthread_join(pthread_t thread, void **value_ptr){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return 0;
}

void __attribute__ ((noinline)) pthread_join_all(){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
}

int __attribute__ ((noinline)) pthread_attr_getscope(const pthread_attr_t *__restrict__ attr, int *__restrict__ contentionscope){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return ENOSYS;
}

int __attribute__ ((noinline)) pthread_attr_setscope(pthread_attr_t *attr, int contentionscope){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return ENOSYS;
}

void* __attribute__ ((noinline))   __aeabi_read_tp(){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return NULL;
}

int __attribute__ ((noinline)) pthread_attr_getschedpolicy(const pthread_attr_t *__restrict__ attr, int *__restrict__ policy){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return ENOSYS;
}

int __attribute__ ((noinline)) pthread_attr_setschedpolicy(pthread_attr_t *__restrict__ attr, int policy){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return ENOSYS;
}

int __attribute__ ((noinline)) pthread_attr_getschedparam(const pthread_attr_t *__restrict__ attr, struct sched_param *__restrict__ param){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return ENOSYS;
}

int __attribute__ ((noinline)) pthread_attr_setschedparam(pthread_attr_t *__restrict__ attr, const struct sched_param *__restrict__ param){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return ENOSYS;
}

int __attribute__ ((noinline)) pthread_attr_getpreemptparam(const pthread_attr_t *__restrict__ attr, int *__restrict__ param){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return ENOSYS;
}

int __attribute__ ((noinline)) pthread_attr_setpreemptparam(pthread_attr_t *__restrict__ attr, int param){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return ENOSYS;
}

int __attribute__ ((noinline)) pthread_getschedparam(pthread_t thread, int *__restrict__ policy, struct sched_param *__restrict__ param){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return ENOSYS;
}

int __attribute__ ((noinline)) pthread_setschedparam(pthread_t thread, int policy, const struct sched_param *param){
    fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
    return ENOSYS;
}
