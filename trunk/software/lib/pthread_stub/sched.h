#ifndef SCHED_H
#define SCHED_H

struct sched_param{
   int sched_priority;
   unsigned int deadline; // specified in us
};

#define SCHED_FIFO 0
#define SCHED_OTHER 1
#define SCHED_RR 2
#define SCHED_EDF 5

#define PRIO_MAX 255
#define PRIO_MIN 0

#define PREEMPTIVE 1
#define NON_PREEMPTIVE 0

int sched_get_priority_max(int policy);
int sched_get_priority_min(int policy);

#endif
