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

#include <vector>
#include <map>

#include <systemc.h>

#include "concurrency_manager.hpp"

//Initialization of some static variables
const int resp::ConcurrencyManager::SYSC_SCHED_FIFO = 0;
const int resp::ConcurrencyManager::SYSC_SCHED_OTHER = 1;
const int resp::ConcurrencyManager::SYSC_SCHED_RR = 2;
const int resp::ConcurrencyManager::SYSC_SCHED_EDF = 5;

const int resp::ConcurrencyManager::SYSC_PRIO_MAX = 255;
const int resp::ConcurrencyManager::SYSC_PRIO_MIN = 0;

const int resp::ConcurrencyManager::SYSC_PREEMPTIVE = 1;
const int resp::ConcurrencyManager::SYSC_NON_PREEMPTIVE = 0;

///specifies whether blocked processor halts or keep on working in busy wait loops
bool resp::ConcurrencyManager::busyWaitLoop = true;
///Specifies the stack size for each thread
unsigned int resp::ConcurrencyManager::threadStackSize = 1024*20;
///Associates thred properties with a routine name: all threads
///created from that routine shall have such properties
std::map<std::string, DefaultThreadInfo> resp::ConcurrencyManager::defThreadInfo;
///The registered interrupt service routines.
std::map<int, std::string> resp::ConcurrencyManager::interruptServiceRoutines;
///The size and content of the thread-local-storage
unsigned int resp::ConcurrencyManager::tlsSize = 0;
unsigned char * resp::ConcurrencyManager::tlsData = NULL;

resp::ConcurrencyManager::ConcurrencyManager(){
}

///Resets the CM to its original status as after the construction
void resp::ConcurrencyManager::reset(){
}

///*******************************************************************
/// Initialization functions
///*******************************************************************
///Initializes the mutexes used to guarantee mutual exclusion access to
///reentrant routines
void resp::ConcurrencyManager::initReentrantEmulation(){
}

///*******************************************************************
/// Here are some functions for computing statistics on the
/// execution
///*******************************************************************
void resp::ConcurrencyManager::printThreadTraces(){
}
std::vector<std::string> resp::ConcurrencyManager::getThreadTraces(){
}
std::map<int, double> resp::ConcurrencyManager::getAverageLoads(){
}
void resp::ConcurrencyManager::printIdleTraces(){
}
std::vector<std::string> resp::ConcurrencyManager::getIdleTraces(){
}
int resp::ConcurrencyManager::getDeadlineMisses(){
}
int resp::ConcurrencyManager::getDeadlineOk(){
}

///*******************************************************************
/// Finally we have the real core of the concurrency manager,
/// it contains the functions emulating the functionalities of
/// the emulated concurrency related primitives
///*******************************************************************
///*********** Thread related routines *******************
int resp::ConcurrencyManager::createThread(unsigned int threadFun,  unsigned int args, int attr){
}
void resp::ConcurrencyManager::exitThread(unsigned int procId, unsigned int retVal){
}
bool resp::ConcurrencyManager::cancelThread(int threadId){
}

int resp::ConcurrencyManager::createThreadAttr(){
}
void resp::ConcurrencyManager::deleteThreadAttr(int attr){
}

void resp::ConcurrencyManager::setStackSize(int attr, int stackSize){
}
unsigned int resp::ConcurrencyManager::getStackSize(int attr){
}

void resp::ConcurrencyManager::setPreemptive(int attr, int isPreemptive){
}
int resp::ConcurrencyManager::getPreemptive(int attr){
}

void resp::ConcurrencyManager::setSchedDeadline(int attr, unsigned int deadline){
}
unsigned int resp::ConcurrencyManager::getSchedDeadline(int attr){
}

void resp::ConcurrencyManager::setSchedPrio(int attr, int priority){
}
int resp::ConcurrencyManager::getSchedPrio(int attr){
}

void resp::ConcurrencyManager::setSchedPolicy(int attr, int policy){
}
int resp::ConcurrencyManager::getSchedPolicy(int attr){
}

int resp::ConcurrencyManager::getThreadSchePolicy(int threadId){
}
void resp::ConcurrencyManager::setThreadSchePolicy(int threadId, int policy){
}

int resp::ConcurrencyManager::getThreadSchedPrio(int threadId){
}
void resp::ConcurrencyManager::setThreadSchedPrio(int threadId, int priority){
}

unsigned int resp::ConcurrencyManager::getThreadSchedDeadline(int threadId){
}
void resp::ConcurrencyManager::setThreadSchedDeadline(int threadId, unsigned int deadline){
}

int resp::ConcurrencyManager::getThreadId(unsigned int procId){
}

int resp::ConcurrencyManager::createKey(){
}
void resp::ConcurrencyManager::setSpecific(unsigned int procId, int key, unsigned int memArea){
}
unsigned int resp::ConcurrencyManager::getSpecific(unsigned int procId, int key){
}

void resp::ConcurrencyManager::join(int thId, unsigned int procId, int curThread_){
}
void resp::ConcurrencyManager::joinAll(unsigned int procId){
}

std::pair<unsigned int, unsigned int> resp::ConcurrencyManager::readTLS(unsigned int procId){
}
void resp::ConcurrencyManager::idleLoop(unsigned int procId){
}

void resp::ConcurrencyManager::pushCleanupHandler(unsigned int procId, unsigned int routineAddress, unsigned int arg){
}
void resp::ConcurrencyManager::popCleanupHandler(unsigned int procId, bool execute){
}
void resp::ConcurrencyManager::execCleanupHandlerTop(unsigned int procId){
}

///*********** Mutex related routines *******************
///Destroys a previously allocated mutex, exception if the mutex does not exist
void resp::ConcurrencyManager::destroyMutex(unsigned int procId, int mutex){
}
int resp::ConcurrencyManager::createMutex(unsigned int procId){
}
///Locks the mutex, deschedules the thread if the mutex is busy; in case
///nonbl == true, the function behaves as a trylock
bool resp::ConcurrencyManager::lockMutex(int mutex, unsigned int procId, bool nonbl){
}
///Releases the lock on the mutex
int resp::ConcurrencyManager::unLockMutex(int mutex, unsigned int procId){
}

///*********** Semaphore related routines *******************
int resp::ConcurrencyManager::createSem(unsigned int procId, int initialValue){
}
void resp::ConcurrencyManager::destroySem(unsigned int procId, int sem){
}
void resp::ConcurrencyManager::postSem(int sem, unsigned int procId){
}
void resp::ConcurrencyManager::waitSem(int sem, unsigned int procId){
}

///*********** Condition Variable related routines *******************
int resp::ConcurrencyManager::createCond(unsigned int procId){
}
void resp::ConcurrencyManager::destroyCond(unsigned int procId, int cond){
}
void resp::ConcurrencyManager::signalCond(int cond, bool broadcast, unsigned int procId){
}
int resp::ConcurrencyManager::waitCond(int cond, int mutex, double time, unsigned int procId){
}

///***************************************************************
/// Here we declare some classes which will be used as timers
///
///***************************************************************
TimeSliceClock::TimeSliceClock(const sc_time &resolution){
}
void TimeSliceClock::preempt(){
}
IRQClock::IRQClock(const sc_time &resolution){
}
void IRQClock::schedulerIRQ(){
}
EventClock::EventClock(){
}
void EventClock::schedulerEvent(){
}
CondVarClock::CondVarClock(sc_time accuracy){
}
void CondVarClock::countTime(){
}
