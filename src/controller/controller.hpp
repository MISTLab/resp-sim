/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *                                                                         *
 ***************************************************************************/


/*
 *   A SystemC controller module
 *
 *   It allows to load new components, attach them, start and stop the simulation
 *
*/

#ifndef SC_CONTROLLER_H
#define SC_CONTROLLER_H

#include <systemc.h>
#include <string>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>

#include "callback.hpp"
#include "simulation_engine.hpp"

extern bool interactiveSimulation;

namespace resp{

///Registers a callback which is called when the simulation is manually
///paused by the user using sc_controller.pause_simulation
void register_PAUSE_callback(PauseCallback &callBack);
///Registers a callback which is called when the simulation ends
///because of a timeout (i.e. simulation was started for 10 NS and
///this time has expired)
void register_TIMEOUT_callback(TimeoutCallback &callBack);
void register_ERROR_callback(ErrorCallback &callBack);

class sc_controller;

/**
 * A SystemC controller module
 *
 * It allows to start and stop the simulation
 */
struct caller {
    ///Variables used to keep track of the duration of the simulation in real time (not systemc time)
    unsigned long *now;
    unsigned long *accumulatedTime;
    ///being set when the current simulation slot has ended
    bool *ended;
    ///being set just before the current simulation slot starts
    bool *started;
    ///The length of the current simulation slot
    double *simTime;
    ///Specifies whether when the current simulation slot has finished we have to launch
    ///another one
    bool *hasToContinue;
    sc_controller *ctrl;

    void operator()();
};

extern sc_controller * controllerInstance;
extern bool GDBEnabled;

class sc_controller {
  private:
    ///In case we start a simulation for k NS,  stop after b < k and restart for j
    ///this variable holds the value k = b
    double residualSimTime;
    ///Keeps track of the desired simulation length
    double requestedSimLength;
    ///Simulation time at the last run
    double simTimeAtLastRun;
    ///Contains the current state of the simulation,  if it is automatic (started with sc_start(-1)) or timed
    ///started with sc_start(a_value_>_0)
    enum State{NONE,  AUTO,  TIMED};
    State curState;
    ///being set just before the current simulation slot starts
    bool started;
    ///True if simulation is interactive (threads are used for managing simulation)
    bool paused;
    ///The simulation engine (i.e. the systemc component used to pause and restart simulation)
    simulation_engine *se;
    int pid;
    ///Variable used to control simulation thread,  use to specify if simulation has to go on after
    ///the end of the current simulation slot
    bool hasToContinue;
    ///The length of the current simulation
    double simTime;

    /// Constructor for the controller; it is private since the static method createController should be
    ///the only way of creating a new controller instance
    sc_controller(int pid, bool interactive);

public:
    /**
     * Callback management
     */
    void register_delta_callback( DeltaCallback& c );
    void remove_delta_callback( DeltaCallback& c );

    static boost::mutex global_mutex;
    static boost::condition stopped_condition;
    static sc_event stop;
    static bool error;
    static std::string errorMessage;
    static bool disableLatency;
    static sc_event syscall_lock;
    static bool locked;
    ///True if the simulation slot has ended
    bool ended;
    ///Variables used to keep track of the duration of the simulation in real time (not systemc time)
    unsigned long now;
    unsigned long accumulatedTime;
    bool interactive;
    ///True if simulation is in a paused state

    /// Static method for the creation of the controller class; note that this is the only way
    ///of creating a controller since the real constructor is private
    static sc_controller &createController(int pid, bool interactive);

    /// Runs the simulation for a specified amount of time
    void run_simulation(double simTime = -1);
    /// Runs the simulation up to the specified simulation time
    void run_up_to(double simTime = 0);

    /// Pauses the simulation, waiting for external control
    void pause_simulation(bool notify = true);

    ///Stops simulation (through a call to sc_stop)
    void stop_simulation();

    // Gets simulation status
    bool is_finished();

    bool is_running();

    bool isEnded();

    bool hasStarted();

    void kill();

    /// Gets SystemC time
    double get_simulated_time();

    bool is_paused();

    std::string print_real_time();

    // Gests SystemC real time
    unsigned long get_real_time_ms();

    /// Setuo the controller and its callbacks for stuff that can't be handled in the constructor
    void setup_callbacks();

    /// Resets the controller
    void reset();

    int get_pid();

    void set_interactive(bool interactive);
};

}

#endif /* SC_CONTROLLER_H */
