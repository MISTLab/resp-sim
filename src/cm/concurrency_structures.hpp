/***************************************************************************\
 *
 *
 *         ___           ___           ___           ___
 *        /  /\         /  /\         /  /\         /  /\
 *       /  /::\       /  /:/_       /  /:/_       /  /::\
 *      /  /:/\:\     /  /:/ /\     /  /:/ /\     /  /:/\:\
 *     /  /:/~/:/    /  /:/ /:/_   /  /:/ /::\   /  /:/~/:/
 *    /__/:/ /:/___ /__/:/ /:/ /\ /__/:/ /:/\:\ /__/:/ /:/
 *    \  \:\/:::::/ \  \:\/:/ /:/ \  \:\/:/~/:/ \  \:\/:/
 *     \  \::/~~~~   \  \::/ /:/   \  \::/ /:/   \  \::/
 *      \  \:\        \  \:\/:/     \__\/ /:/     \  \:\
 *       \  \:\        \  \::/        /__/:/       \  \:\
 *        \__\/         \__\/         \__\/         \__\/
 *
 *
 *
 *
 *   This file is part of ReSP.
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 *
 *   (c) Giovanni Beltrame, Luca Fossati
 *       Giovanni.Beltrame@esa.int fossati@elet.polimi.it
 *
\***************************************************************************/

#ifndef CONCURRENCY_STRUCTURES_HPP
#define CONCURRENCY_STRUCTURES_HPP

#include <list>
#include <vector>
#include <string>

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

struct Processor{
    trap::ABIIf<wordSize> &processorInstance;
    ThreadEmu * runThread;
    std::vector<std::pair<double, double> > idleTrace;
    double curIdleStart;

    // Idle event
    sc_event idleEvent;

    Processor(trap::ABIIf<wordSize> &processorInstance);
    ~Processor();

    void schedule(ThreadEmu * thread);
    int deSchedule(bool saveStatus = true);
    void printIdleTrace();
    std::string getIdleTraceCSV();
};

#endif
