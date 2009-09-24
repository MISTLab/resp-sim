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

#include "concurrency_emulator.hpp"
#include "concurrency_manager.hpp"

// ***************** STATIC VARIABLES

///Instance of the concurrency manager, used to actually implement the emulated concurrency-related
///functionalities: note that this is a map since there might be different instances of the
///Manager, one for each emulation group
std::map<int, ConcurrencyManager * > resp::ConcurrencyEmulatorBase::cm;

// ***************** METHODS

///Sets the default properties of the threads,
void resp::ConcurrencyEmulatorBase::setThreadProps(bool waitLoop, unsigned int defaultStackSize){
    if(resp::ConcurrencyEmulatorBase::cm.size() > 0)
        THROW_EXCEPTION("Error, the setThreadProps method has to be called before creating any ConcurrencyEmulator instance");
    resp::ConcurrencyManager::busyWaitLoop = waitLoop;
    resp::ConcurrencyManager::threadStackSize = defaultStackSize;
}

///Sets thread properties, so that, when a thread is created, it is associated such properties
///independently from the thread attributes that were specified in the executeable file
void resp::ConcurrencyEmulatorBase::setThreadInfo(std::string functionName, bool preemptive,
                    int schedPolicy, int priority, unsigned int deadline){
    resp::ConcurrencyManager::defThreadInfo[functionName].preemptive = preemptive;
    resp::ConcurrencyManager::defThreadInfo[functionName].schedPolicy = schedPolicy;
    resp::ConcurrencyManager::defThreadInfo[functionName].priority = priority;
    resp::ConcurrencyManager::defThreadInfo[functionName].deadline = deadline;

}

                    ///Marks a function to be executed as an interrupt service routine
void resp::ConcurrencyEmulatorBase::registerISR(std::string isrFunctionName, int irqId, bool preemptive = false,
                                                            int schedPolicy = resp::ConcurrencyManager::SYSC_SCHED_FIFO,
                                                                    int priority = resp::ConcurrencyManager::SYSC_PRIO_MAX){
    resp::ConcurrencyManager::interruptServiceRoutines[irqId] = isrFunctionName;
    resp::ConcurrencyManager::defThreadInfo[isrFunctionName].preemptive = preemptive;
    resp::ConcurrencyManager::defThreadInfo[isrFunctionName].schedPolicy = schedPolicy;
    resp::ConcurrencyManager::defThreadInfo[isrFunctionName].priority = priority;
}
