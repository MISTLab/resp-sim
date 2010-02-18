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

#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>

#include <systemc.h>

#include "sc_state_machine.hpp"
#include "simulation_engine.hpp"
#include "callback.hpp"

using namespace resp;

///Overloading of the end_of_simulation method; it can be used to execute methods
///at the end of the simulation
void simulation_engine::end_of_simulation(){
    // Simulation is being terminated: this method is automatically called by
    // the systemc kernel
    // First of all I have to signal it to the state machine; note that
    // there is a potential race condition: simulation ends while we
    // are still in the reset state (it can happen if simulation
    // is very fast and the state machine transition is very slow).
    // in order to prevent this I use the reset mutex: it is acquired while
    // are are in the reset status, and freed immediately after we go out:
    {
        boost::mutex::scoped_lock lk(this->controllerMachine.reset_mutex);
        this->controllerMachine.process_event( EvStop() );
    }
    // Finally I signal to everyone that simulation has ended
    notifyEosCallback();
    
    // Notify any waiting threads
    this->controllerMachine.end_condition.notify_all();
    
}

/// Simulation engine constructor
simulation_engine::simulation_engine(sc_module_name name, ControllerMachine &controllerMachine) :
                                            sc_module(name), controllerMachine(controllerMachine){
    SC_METHOD(pause);
    sensitive << this->controllerMachine.pauseEvent;
    dont_initialize();
    end_module();
}

/// Blocks the SystemC execution by waiting on a boost::thread condition
void simulation_engine::pause(){
    // in order to prevent very fast paused events (i.e. simulation is paused soon
    // after it is started), so fast the the machine hasn't moved to the running state
    // yet, I use this mutex 
 
    boost::mutex::scoped_lock lk_reset(this->controllerMachine.reset_mutex);
    // First of all I perform the transition to pause the state machine
    this->controllerMachine.process_event( EvPause() );
 
    // I signal to all who registered that simulation
    // is being paused
    notifyPauseCallback();
    // finally I pause by waiting on the condition 
    {
    boost::mutex::scoped_lock lk(this->controllerMachine.pause_mutex);
    this->controllerMachine.pause_condition.wait(lk);
    }
}
