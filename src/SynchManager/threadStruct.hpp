#ifndef THREADSTRUCT_HPP
#define THREADSTRUCT_HPP

#include <list>
#include <vector>
#include <string>

namespace resp{
const int SYSC_SCHED_FIFO = 0;
const int SYSC_SCHED_OTHER = 1;
const int SYSC_SCHED_RR = 2;
const int SYSC_SCHED_EDF = 5;

const int SYSC_PRIO_MAX = 255;
const int SYSC_PRIO_MIN = 0;

const int SYSC_PREEMPTIVE = 1;
const int SYSC_NON_PREEMPTIVE = 0;
};

typedef void *(*thread_routine_t)(void*);

struct AttributeEmu{
    unsigned int stackSize;
    bool detached;
    bool preemptive;
    int schedPolicy;
    int priority;
    unsigned int deadline;
    int id;

    AttributeEmu();
    AttributeEmu(unsigned int stackSize);
};

struct ThreadEmu{
    struct ExecutionTrace{
        double startTime;
        double endTime;
        int processorId;
        unsigned int startAddress;
        unsigned int endAddress;
        ExecutionTrace(){
            this->startTime = -1;
            this->endTime = -1;
            this->processorId = -1;
            this->startAddress = 0;
            this->endAddress = 0;
        }
        void clear(){
            this->startTime = -1;
            this->endTime = -1;
            this->processorId = -1;
            this->startAddress = 0;
            this->endAddress = 0;
        }
    };
    enum status_t {READY, RUNNING, WAITING, DEAD};
    thread_routine_t thread_routine;
    AttributeEmu *attr;
    void * args;
    status_t status;
    unsigned int stackBase;
    unsigned int tlsSize;
    std::vector<unsigned char> state;
    int id;
    bool isIRQ;

    int syscallRetVal;
    bool setSyscRetVal;

    void * retVal;
    std::vector<ThreadEmu *> joining;
    int numJoined;
    std::vector<ExecutionTrace> execTrace;
    ExecutionTrace curTraceElem;
    //These two variables contains the value of the link register and the program counter when
    //the thread was last descheduled
    unsigned int lastPc;
    unsigned int lastRetAddr;

    ThreadEmu(int id, thread_routine_t thread_routine, void * args, unsigned int stackBase, unsigned int tlsSize, AttributeEmu *attr);
    ~ThreadEmu();
    void printTrace();
    std::string getTrace();
};

struct MutexEmu{
    enum status_t {LOCKED, FREE};
    std::list<ThreadEmu *> waitingTh;
    ThreadEmu * owner;
    status_t status;
    int recursiveIndex;

    MutexEmu();
    ~MutexEmu();
};

struct SemaphoreEmu{
    unsigned int value;
    std::list<ThreadEmu *> waitingTh;

    SemaphoreEmu(int initialValue = 0);
    ~SemaphoreEmu();
};

struct ConditionEmu{
    std::list<ThreadEmu *> waitingTh;
    unsigned int waitTime;
    int mutex;

    ConditionEmu(){
        this->waitTime = 0;
        this->mutex = -1;
    }
    ~ConditionEmu(){}
};

#endif
