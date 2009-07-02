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
 *   ReSP is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *   or see <http://www.gnu.org/licenses/>.
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
    // First of all I have to signal it to the state machine
    this->controllerMachine.process_event( EvStop() );
    // Finally I signal to everyone that simulation has ended
    notifyEosCallback();
}

/// Simulation engine constructor
simulation_engine::simulation_engine(sc_module_name name, ControllerMachine &controllerMachine) :
                                            sc_module(name), controllerMachine(controllerMachine){
    SC_METHOD(pause);
    sensitive << this->controllerMachine.stopEvent;
    dont_initialize();
    end_module();
}

/// Blocks the SystemC execution by waiting on a boost::thread condition
void simulation_engine::pause(){
    // First of all I make the transitions in the state machine:
    this->controllerMachine.process_event( EvPause() );
    // I signal to all who registered that simulation
    // is being paused
    notifyPauseCallback();
    // finally I pause by waiting on the condition
    boost::mutex::scoped_lock lk(this->controllerMachine.pause_mutex);
    this->controllerMachine.pause_condition.wait(lk);
}
