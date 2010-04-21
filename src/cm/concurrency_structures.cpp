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

#include <systemc.h>

#include <list>
#include <vector>
#include <string>

#include "concurrency_structures.hpp"

AttributeEmu::AttributeEmu(){
    priority = 0;
}
AttributeEmu::AttributeEmu(unsigned int stackSize) : stackSize(stackSize) {
    priority = 0;
}

ThreadEmu::ThreadEmu(int id, unsigned int thread_routine, unsigned int args, unsigned int stackBase, unsigned int tlsAddress, AttributeEmu *attr) : id(id), thread_routine(thread_routine), args(args), stackBase(stackBase), tlsAddress(tlsAddress), attr(attr) {
    this->state = NULL;
    this->isIRQ = false;
    this->setSyscRetVal = false;
    this->status = READY;
}
ThreadEmu::~ThreadEmu(){
}

ThreadEmu::ExecutionTrace::ExecutionTrace(){
    this->startTime = -1;
    this->endTime = -1;
    this->processorId = -1;
    this->startAddress = 0;
    this->endAddress = 0;
}
void ThreadEmu::ExecutionTrace::clear(){
    this->startTime = -1;
    this->endTime = -1;
    this->processorId = -1;
    this->startAddress = 0;
    this->endAddress = 0;
}

MutexEmu::MutexEmu() : waitingTh() {
    status = FREE;
    owner = NULL;
    recursiveIndex = 0;
}
MutexEmu::~MutexEmu() {}

SemaphoreEmu::SemaphoreEmu(int initialValue){
    this->value = initialValue;
    this->owner = NULL;
}

SemaphoreEmu::~SemaphoreEmu(){
}

ConditionEmu::ConditionEmu(){
    this->waitTime = 0;
    this->mutex = -1;
}
ConditionEmu::~ConditionEmu(){
}
