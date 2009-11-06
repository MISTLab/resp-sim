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


#ifndef SC_CONTROLLER_HPP
#define SC_CONTROLLER_HPP

#include <systemc.h>
#include <string>

#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/timer.hpp>

#include "callback.hpp"
#include "simulation_engine.hpp"
#include "sc_state_machine.hpp"


/**
 * Thread for running systemc in a separate thread: this allows
 * responsiveness of python, while still being able to
 * controll systemc. This structuure is used for the control
 * of non-interactive simulation, where it is not possible
 + to pause simulation
 */
struct controllerThread_ninteractive{
    sc_time timeSlice;

    void operator()();
};

namespace resp{

/**
 * Controller class: it is responsible to pausing, starting, stopping
 * simulation and also for setting the callbacks which are invoked
 * every time an event happens (simulation paused, stopped, etc.)
 */
class sc_controller{
  private:
    /// Simulation engine used to pause simulation in interactive mode
    simulation_engine * se;
    /// State machine helping in the management of simulation, it keeps track of
    /// the current simulation status
    ControllerMachine controllerMachine;
    /// Constructor for the controller; it is private since the static method
    /// createController should be
    /// the only way of creating a new controller instance
    sc_controller(bool interactive);
    /// Instance of the controller, used to manage simulation.
    /// note that only one controller can exist in the system at a time
    static sc_controller * controllerInstance;
public:
    ///Variables used to keep track of errors happening during simulation
    bool error;
    std::string errorMessage;
    ///Variables used to keep track of the duration of the simulation in real time (not systemc time)
    boost::timer timeTracker;
    double accumulatedTime;
    ///True if simulation is interactive, so if it is managed through
    ///ReSP's console
    const bool interactive;

    ~sc_controller();

    /// Static method for the creation of the controller class; note that this is the only way
    ///of creating a controller since the real constructor is private
    static sc_controller & getController();
    static sc_controller & getController(bool interactive);

    ///Resets the controller, destroying any object
    ///associated to it.
    void reset_controller();

    /// Runs the simulation for a specified amount of time
    void run_simulation(double simTime = 0, sc_time_unit time_unit = SC_NS);
    void run_simulation(sc_time simTime);
    /// Runs the simulation up to the specified simulation time
    void run_up_to(double simTime = 0, sc_time_unit time_unit = SC_NS);

    /// Pauses the simulation; this is done with the help of
    /// the simulation engine. Notify specifies whether registered
    /// callbacks should be notified of the pausing event or not
    void pause_simulation();

    ///Stops simulation (through a call to sc_stop)
    void stop_simulation();

    /// True is simulation has ended, i.e. sc_stop has been called
    /// or there are no more events in SystemC's queue and it
    /// has already been started
    bool is_finished();

    /// True if simulation is running, i.e. if there are still events and
    /// simulation has been started
    bool is_running();

    /// True if simulation has ended, i.e.. there are no events and
    /// and simulation has been started before
    bool is_ended();

    /// True if simulation has already been started
    bool has_started();

    /// Gets SystemC time using the specified time unit, ns are the default one
    double get_simulated_time(sc_time_unit time_unit = SC_NS);

    /// Returns true if simulation is in the paused state
    bool is_paused();

    /// Print elapsed cpu time in seconds
    std::string print_real_time();

    /// Gets elapsed cpu time in seconds
    double get_real_time();
};

}

#endif
