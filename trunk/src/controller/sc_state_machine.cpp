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

#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/event.hpp>
#include <boost/statechart/transition.hpp>
#include <boost/statechart/custom_reaction.hpp>
#include <boost/mpl/list.hpp>
#include <boost/timer.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/timer.hpp>

#include "sc_state_machine.hpp"

///Constructor of the overall machine
ControllerMachine::ControllerMachine(boost::timer & timeTracker, double & accumulatedTime) :
                                timeTracker(timeTracker), accumulatedTime(accumulatedTime) {}

controllerThread_interactive::controllerThread_interactive(boost::mutex & start_of_sim_mutex, boost::condition & start_of_sim_cond) :
                            start_of_sim_mutex(start_of_sim_mutex), start_of_sim_cond(start_of_sim_cond) {}

/// Starts the simulation in a new boost thread; simulation is
/// always started with sc_start() without parameters, so
/// that simulation runs undefinitely. For pausing it,
/// the simulation engine is used.
void controllerThread_interactive::operator()(){
    // In addition to starting simulation in a new thread, I also
    // use a condition variable so that I can make sure that simulation
    // is really started before returning control to ReSP
    boost::mutex::scoped_lock start_of_sim_lock(this->start_of_sim_mutex);
    this->start_of_sim_cond.notify_all();
    sc_start();
}

/// *** Reset state: the simiulaton is reset because we haven't started it yet. ***
/// Reaction to the run undefinitely event; I have to actually
/// start simulation using the sc_start call inside the
/// controllerThread_interactive
boost::statechart::result Reset_st::react(const EvRun &){
    boost::mutex start_of_sim_mutex;
    boost::condition start_of_sim_cond;

    // I have to start the thread. In order to be sure that when I finish the transition
    // simulation has really started, I use the start_of_sim_cond condition variable
    controllerThread_interactive c(start_of_sim_mutex, start_of_sim_cond);
    boost::mutex::scoped_lock start_of_sim_lock(start_of_sim_mutex);
    boost::thread thrd (c);
    start_of_sim_cond.wait(start_of_sim_lock);

    return transit< Running_st >();
}
/// Reaction to the run timed event; I have to actually
/// start simulation using the sc_start call inside the
/// controllerThread_interactive. Before starting the simulation
/// I notify the stopped event at time t.
boost::statechart::result Reset_st::react(const EvRun_t & event){
    boost::mutex start_of_sim_mutex;
    boost::condition start_of_sim_cond;

    // I have to start the thread. In order to be sure that when I finish the transition
    // simulation has really started, I use the start_of_sim_cond condition variable
    controllerThread_interactive c(start_of_sim_mutex, start_of_sim_cond);
    boost::mutex::scoped_lock start_of_sim_lock(start_of_sim_mutex);
    boost::thread thrd (c);
    this->outermost_context().pauseEvent.notify(event.timeStep);
    start_of_sim_cond.wait(start_of_sim_lock);

    return transit< Running_st >();
}
///These two events do not cause a transition, but they are simply
/// used to print warnings to the user: simulation hasn't been
///started yet, of course it cannot be stopped or paused!!
boost::statechart::result Reset_st::react(const EvPause &){
    std::cerr << "Simulation hasn't been started yet, it cannot be paused" << std::endl;

    return discard_event();
}
boost::statechart::result Reset_st::react(const EvStop &){
    std::cerr << "Simulation hasn't been started yet, it cannot be stopped" << std::endl;

    return discard_event();
}

/// *** Stopped state: ***
///These events do not do anything; simply they are used to print
///warning to the user: simulation is stopped, it cannot be
///restarted
boost::statechart::result Stopped_st::react(const EvRun &){
    std::cerr << "Simulation is stopped, it cannot be run; plese reset the simulator to restart" << std::endl;

    return discard_event();
}
boost::statechart::result Stopped_st::react(const EvRun_t &){
    std::cerr << "Simulation is stopped, it cannot be run; plese reset the simulator to restart" << std::endl;

    return discard_event();
}
/// Constructor, called every time the
/// status is entered. It mainly stops the timer and
/// it records elapsed time
Stopped_st::Stopped_st(my_context ctx) : boost::statechart::state<Stopped_st, ControllerMachine>(ctx){
    this->outermost_context().accumulatedTime += this->outermost_context().timeTracker.elapsed();
}

/// *** Run state: the simiulaton is running ***
/// Reaction to the pause event.
/// This transitions can be called both because the used explicitly requests
/// to pause or because the current time quantum expired
boost::statechart::result Running_st::react(const EvPause &){
    this->outermost_context().pauseEvent.cancel();
    this->outermost_context().pauseEvent.notify();

    return transit< Paused_st >();
}
/// Reaction to the stop timed event. This event can be called
/// because the user explicitly requests to stop simulation,
/// sc_stop is implicitly called in the code or the queue
/// of events is empty
boost::statechart::result Running_st::react(const EvStop &){
    return transit< Stopped_st >();
}
/// Constructor, called every time the
/// status is entered. It mainly resets and
/// re-starts the timer
Running_st::Running_st(my_context ctx) : boost::statechart::state<Running_st, ControllerMachine>(ctx){
    this->outermost_context().timeTracker.restart();
}

/// *** Paused state: the simulation is paused, it is not running ***
/// Reaction to the run undefinitely event; I have to actually
/// resume simulation by notifying the boost condition
boost::statechart::result Paused_st::react(const EvRun &){
    this->outermost_context().pauseEvent.cancel();
    this->outermost_context().pause_condition.notify_all();

    return transit< Running_st >();
}
/// Reaction to the run timed event; I have to actually
/// resume simulation by notifying the boost condition.
/// Before that, I notify the stopped event at time t.
boost::statechart::result Paused_st::react(const EvRun_t & event){
    this->outermost_context().pauseEvent.cancel();
    this->outermost_context().pauseEvent.notify(event.timeStep);
    this->outermost_context().pause_condition.notify_all();

    return transit< Running_st >();
}
/// Reaction to the stop timed event. This event can be called
/// only because the user explicitly requests to stop simulation.
boost::statechart::result Paused_st::react(const EvStop &){
    this->outermost_context().pause_condition.notify_all();

    return transit< Stopped_st >();
}
/// Constructor, called every time the
/// status is entered. It mainly stops the timer and
/// it records elapsed time
Paused_st::Paused_st(my_context ctx) : boost::statechart::state<Paused_st, ControllerMachine>(ctx){
    this->outermost_context().accumulatedTime += this->outermost_context().timeTracker.elapsed();
}
