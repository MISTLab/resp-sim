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


#include "simulation_engine.hpp"
#include "controller.hpp"
#include "callback.hpp"
#include "sc_state_machine.hpp"

#include <iostream>
#include <string>
#include <systemc.h>
#include <exception>
#include <stdexcept>

#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/timer.hpp>

using namespace resp;

/**
 * Thread for running systemc in a separate thread: this allows
 * responsiveness of python, while still being able to
 * controll systemc. This structuure is used for the control
 * of non-interactive simulation, where it is not possible
 + to pause simulation
 */
void controllerThread_ninteractive::operator()(){
    //first of all, for correct synchronization with
    //the run_simulation method I have to acquire the
    //lock on the mutex
    boost::mutex::scoped_lock end_of_sim_lock(this->end_of_sim_mutex);

    // I have to check the timeSlice variable,
    // if equal to 0 I run undefinitely,
    // otherwise I go on for the specified amount
    // of time.
    if(this->timeSlice == SC_ZERO_TIME){
        sc_start();
    }
    else{
        sc_start(this->timeSlice);
    }
    // Finally simulation has ended: I can notify the condition
    // on which the run_smiulation method is blocked so that
    // I can go on...
    this->end_of_sim_cond.notify_all();
}

void my_terminate_handler(){
    #ifndef NDEBUG
    std::cerr << "Exception handled by controller!" << std::endl;
    #endif

    try{
        throw;
    }
    catch (std::exception &e){
        sc_controller::getController().errorMessage = e.what();
    }
    catch(std::string &e){
        sc_controller::getController().errorMessage = e;
    }
    catch (std::exception *e){
        sc_controller::getController().errorMessage = e->what();
    }
    catch(std::string *e){
        sc_controller::getController().errorMessage = *e;
    }
    catch(...){
        sc_controller::getController().errorMessage = "Unknown Exception";
    }

    try{
        std::cerr << std::endl << std::endl << "**** RECEIVED EXCEPTION --> " << sc_controller::getController().errorMessage << " <-- ***" <<std::endl << std::endl;
        if(sc_controller::getController().interactive)
            std::cerr << "\tCTRL^D to quit the simulator" << std::endl << std::endl;

        notifyErrorCallback();
    }
    catch(...){}

    try{
        /// I used this trick to prevent exiting from this function:
        /// this way it is possible to still used ReSP without
        /// having to close the whole ReSP process as the consequence of
        /// the exception
        boost::mutex termMutex;
        boost::condition termCond;
        boost::mutex::scoped_lock lk(termMutex);
        termCond.wait(lk);
    }
    catch(...){;}
}

///Resets the controller, destroying any object
///associated to it.
void sc_controller::reset_controller(){
    if(sc_controller::controllerInstance != NULL){
        delete sc_controller::controllerInstance;
        sc_controller::controllerInstance = NULL;
    }
}

/// Instance of the controller, used to manage simulation.
/// note that only one controller can exist in the system at a time
sc_controller * sc_controller::controllerInstance = NULL;

/// Constructor for the controller; it is private since the static method
/// createController should be
/// the only way of creating a new controller instance
sc_controller::sc_controller(bool interactive) : interactive(interactive),
            controllerMachine(timeTracker, accumulatedTime), se("sim_engine", controllerMachine){
    this->error = false;
    this->accumulatedTime = 0;
}

/// Static method for the creation of the controller class; note that this is the only way
///of creating a controller since the real constructor is private
sc_controller & sc_controller::getController(bool interactive){
    if(sc_controller::controllerInstance == NULL){
        sc_controller::controllerInstance = new sc_controller(interactive);
    }
    return *(sc_controller::controllerInstance);
}

/// Runs the simulation for a specified amount of time
void sc_controller::run_simulation(double simTime, sc_time_unit time_unit){
    run_simulation(sc_time(simTime, time_unit));
}

/// Runs the simulation for a specified amount of time
void sc_controller::run_simulation(sc_time simTime){
    //We have to distinguish two situations: interactive simulation, I use
    //the state machine and simply make a transition. non-interactive,
    //I directly start a new thread for the specified amount of
    //time.
    if(this->interactive){
        if(simTime >= 0){
            this->controllerMachine.process_event( EvRun_t(simTime) );
        }
        else{
            this->controllerMachine.process_event( EvRun() );
        }
    }
    else{
        controllerThread_ninteractive c;
        if(simTime >= 0)
            c.timeSlice = simTime;
        else
            c.timeSlice = SC_ZERO_TIME;
        //now I acquire the mutex, so that thread c cannot start before I have
        //completed all the necessary initialization
        boost::mutex::scoped_lock end_of_sim_lock(c.end_of_sim_mutex)
        //finally I start the thread and wait for its end
        this->timeKeeper.reset();
        boost::thread thrd (c);
        c.end_of_sim_cond.wait(end_of_sim_lock);
        this->accumulatedTime += this->timeKeeper.elapsed();
    }
}

/// Runs the simulation up to the specified simulation time
void sc_controller::run_up_to(double simTime, sc_time_unit time_unit){
    // it is more or less the same concept of before:
    // I actually can directly call the same run_simulation method
    // with the correct time
    sc_time difference = sc_time(simTime, time_unit) - sc_time_stamp();
    if(difference > 0)
        this->run_simulation(difference);
    else
        std::cerr << "Error, simulation time specified for method run_up_to must be greater than the current time" << std::endl;
}

/// Pauses the simulation; this is done with the help of
/// the simulation engine. Notify specifies whether registered
/// callbacks should be notified of the pausing event or not
void sc_controller::pause_simulation(bool notify){
    if(this->interactive){
        // I simply have to perform the transition in the state machine
        this->controllerMachine.process_event( EvPause() );
    }
    else{
        std:.cerr << "Unable to pause simulation in non-interactive mode" << std::endl;
    }
}

///Stops simulation (through a call to sc_stop)
void sc_controller::stop_simulation(){
    if(this->interactive){
        // I simply have to perform the transition in the state machine
        this->controllerMachine.process_event( EvStop() );
    }
    else{
        std:.cerr << "Unable to stop simulation in non-interactive mode" << std::endl;
    }
}

/// True is simulation has ended, i.e. sc_stop has been called
/// or there are no more events in SystemC's queue and it
/// has already been started
bool sc_controller::is_finished(){
}

/// True if simulation is running, i.e. if there are still events and
/// simulation has been started
bool sc_controller::is_running(){
}

/// True if simulation has ended, i.e.. there are no events and
/// and simulation has been started before
bool sc_controller::isEnded(){
}

/// True if simulation has already been started
bool sc_controller::hasStarted(){
}

/// Returns true if simulation is in the paused state
bool sc_controller::is_paused(){
}

/// Gets SystemC time using the specified time unit, ns are the default one
double sc_controller::get_simulated_time(sc_time_unit time_unit){
}

/// Print elapsed cpu time in seconds
std::string sc_controller::print_real_time(){
    std::string realTime = boost::lexical_cast<std::string>(this->accumulatedTime);
    return realTime;
}

/// Gets elapsed cpu time in seconds
double sc_controller::get_real_time(){
    return this->accumulatedTime;
}
