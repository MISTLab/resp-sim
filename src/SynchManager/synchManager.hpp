#ifndef SYNCHMANAGER_HPP
#define SYNCHMANAGER_HPP

#include "ProcessorIf.hpp"
#include "threadStruct.hpp"

#include <string>
#include <map>

#include <systemc.h>

#include <ctime>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#ifdef BOOST_NO_STDC_NAMESPACE
namespace std {
  using ::time;
}
#endif

/***********************************************************/
// A couple of data structures use to maintain the state
//of the system
/***********************************************************/

struct ThreadEmu;
struct Processor{
    resp::ProcessorIf *procIf;
    ThreadEmu * runThread;
    std::vector<std::pair<double, double> > idleTrace;
    double curIdleStart;

    // Idle event
    sc_event *idleEvent;

    Processor(resp::ProcessorIf *procIf) : procIf(procIf){
        idleEvent = new sc_event();
        runThread = NULL;
        curIdleStart = -1;
    }
    ~Processor(){
        //delete idleEvent;
        //For some strange reason deleting this event causes a segfault. Since
        // I do not care too much on it ... it is not much memory lost ...
        // we do not delete it
    }

    void schedule(ThreadEmu * thread);
    int deSchedule(bool saveStatus = true);
    void printIdleTrace();
    std::string getIdleTraceCSV();
};

struct DefaultThreadInfo{
    bool preemptive;
    int schedPolicy;
    int priority;
    unsigned int deadline;
    std::string funName;
};

extern unsigned int defaultStackSize;
extern unsigned int defaultTLSSize;
extern int mallocMutex;
extern int sfpMutex;
extern int sinitMutex;
extern int fpMutex;

/*************************************************/
// Here are the functions that will be exported for the use by python
/*************************************************/

namespace resp{
///Initializes the state of the processors;
///Note that the executable must be compiler with the -specs=osemu.specs,
///Otherwise is will be impossible to use the Operating System
///Emulation features. Note that this function must be called at the real
///end of the architecture instantiation,  if should in theory be the last
///command before the run_simulation instruction
void initializePthreadSystem(unsigned int memSize, bool waitLoop = true, unsigned int defaultStackSize = 1024*20, std::map<std::string, sc_time> latencies = std::map<std::string, sc_time>());
void setEmulationLatencies(sc_time schedulerRes = SC_ZERO_TIME, sc_time IRQRes = SC_ZERO_TIME);
///Registers the processor interface among the processors that
///will be manged by this synchronization unit; note that all the processor
///interfaces will have to be registered previous to calling the initializeSystem
void registerProcessorIf(ProcessorIf &procIf);
void initiReentrantEmu(std::map<std::string, sc_time> latencies = std::map<std::string, sc_time>());
void setEventLatency(sc_time eventLatency);
void initLatencies(sc_time schedLat, sc_time deSchedLat, sc_time schedChooseLat);
void registerISR(std::string isrFunctionName, int irqId, bool preemptive = false, int schedPolicy = resp::SYSC_SCHED_FIFO, int priority = resp::SYSC_PRIO_MAX);
void registerEvent(std::string eventFunctionName, unsigned int deadline, bool preemptive = true);
void setThreadInfo(std::string functionName, bool preemptive, int schedPolicy, int priority, unsigned int deadline);
void printThreadTraces();
std::vector<std::string> getThreadTraces();
std::map<int, double> getAverageLoads();
void printIdleTraces();
std::vector<std::string> getIdleTraces();
int getDeadlineMisses();
int getDeadlineOk();
/**** RESET ****/
void reset();
};

/***************************************************/
//and this finally is the interface towards the emulated system calls
/***************************************************/

typedef void *(*thread_routine_t)(void*);

/**** THREADS  ****/
///Creates a new thread
int createThread(thread_routine_t threadFun,  void * args, int attr = -1);
void exitThread(unsigned int procId, void * retVal);

int createThreadAttr();
void deleteThreadAttr(int attr);

void setStackSize(int attr, int stackSize);
unsigned int getStackSize(int attr);

void setPreemptive(int attr, int isPreemptive);
int getPreemptive(int attr);

void setSchedDeadline(int attr, unsigned int deadline);
unsigned int getSchedDeadline(int attr);

void setSchedPrio(int attr, int priority);
int getSchedPrio(int attr);

void setSchedPolicy(int attr, int policy);
int getSchedPolicy(int attr);

int getThreadSchePolicy(int threadId);
void setThreadSchePolicy(int threadId, int policy);

int getThreadSchedPrio(int threadId);
void setThreadSchedPrio(int threadId, int priority);

unsigned int getThreadSchedDeadline(int threadId);
void setThreadSchedDeadline(int threadId, unsigned int deadline);

int getThreadId(unsigned int procId);

int createKey();
void setSpecific(unsigned int procId, int key, void *memArea);
void *getSpecific(unsigned int procId, int key);

void join(int thId, unsigned int procId, int curThread_ = -1);
void joinAll(unsigned int procId);

std::pair<void *, void *> readTLS(unsigned int procId);
void idleLoop(unsigned int procId);

class TimeSliceClock : public sc_module{
  private:
    sc_time resolution;
  public:
    SC_HAS_PROCESS(TimeSliceClock);
    TimeSliceClock(const sc_time &resolution);
    void preempt();
};
class IRQClock : public sc_module{
  private:
    sc_time resolution;
    boost::minstd_rand generator;
  public:
    SC_HAS_PROCESS(IRQClock);
    IRQClock(const sc_time &resolution);
    void schedulerIRQ();
};
class EventClock : public sc_module{
  private:
    boost::minstd_rand generator;
  public:
    SC_HAS_PROCESS(EventClock);
    EventClock();
    void schedulerEvent();
};

/**** MUTEX  ****/
//Destroys a previously allocated mutex,  exception if the mutex does not exist
void destroyMutex(unsigned int procId, int mutex);
int createMutex(unsigned int procId);
//Locks the mutex,  deschedules the thread if the mutex is busy; in case
//nonbl == true,  the function behaves as a trylock
bool lockMutex(int mutex, unsigned int procId, bool nonbl = false);
//Releases the lock on the mutex
int unLockMutex(int mutex, unsigned int procId);

/**** SEMAPHORES ****/
int createSem(unsigned int procId, int initialValue = 0);
void destroySem(unsigned int procId, int sem);
void postSem(int sem, unsigned int procId);
void waitSem(int sem, unsigned int procId);

/**** CONDITION VARIABLE ****/
class TimeKeeper : public sc_module{
  public:
    sc_event startCount;
    sc_time accuracy;

    SC_HAS_PROCESS(TimeKeeper);

    TimeKeeper(sc_time accuracy);

    void countTime();
};
int createCond(unsigned int procId);
void destroyCond(unsigned int procId, int cond);
void signalCond(int cond, bool broadcast, unsigned int procId);
int waitCond(int cond, int mutex, double time, unsigned int procId);

#endif
