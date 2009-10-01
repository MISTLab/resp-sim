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

#include <systemc.h>

#include <list>
#include <vector>
#include <string>

///************************************************************************
///Here are the data structures use to represent threads, thread-attributes,
///etc.
///************************************************************************

///Thread attributes, determining the thread behavior
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

///High level, internal representation of a thread
struct ThreadEmu{
    struct ExecutionTrace{
        double startTime;
        double endTime;
        int processorId;
        unsigned int startAddress;
        unsigned int endAddress;
        ExecutionTrace();
        void clear();
    };
    enum status_t {READY, RUNNING, WAITING, DEAD};
    unsigned int thread_routine;
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

    ThreadEmu(int id, unsigned int thread_routine, unsigned int args, unsigned int stackBase, unsigned int tlsSize, AttributeEmu *attr);
    ~ThreadEmu();
    void printTrace();
    std::string getTrace();
};

///Default information associated to a thread, which can be specified from the command line.
///If a thread contains such information, it is
struct DefaultThreadInfo{
    bool preemptive;
    int schedPolicy;
    int priority;
    unsigned int deadline;
    std::string funName;
};

///Internal representation of a mutex
struct MutexEmu{
    enum status_t {LOCKED, FREE};
    std::list<ThreadEmu *> waitingTh;
    ThreadEmu * owner;
    status_t status;
    int recursiveIndex;

    MutexEmu();
    ~MutexEmu();
};

///Internal representation of a sempahore
struct SemaphoreEmu{
    unsigned int value;
    std::list<ThreadEmu *> waitingTh;

    SemaphoreEmu(int initialValue = 0);
    ~SemaphoreEmu();
};

///Internal representation of a condition variable
struct ConditionEmu{
    std::list<ThreadEmu *> waitingTh;
    unsigned int waitTime;
    int mutex;

    ConditionEmu();
    ~ConditionEmu();
};

#endif
