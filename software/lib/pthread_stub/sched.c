#include "sched.h"

int __attribute__ ((noinline)) sched_get_priority_max(int policy){
    return PRIO_MAX;
}

int __attribute__ ((noinline)) sched_get_priority_min(int policy){
    return PRIO_MIN;
}
