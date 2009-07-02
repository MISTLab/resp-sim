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
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/timer.hpp>

#include "sc_state_machine.hpp"

///Constructor of the overall machine
ControllerMachine::ControllerMachine(boost::timer & timeTracker, double & accumulatedTime) :
                                timeTracker(timeTracker), accumulatedTime(accumulatedTime) {}

/// Starts the simulation in a new boost thread; simulation is
/// always started with sc_start() without parameters, so
/// that simulation runs undefinitely. For pausing it,
/// the simulation engine is used.
void controllerThread_interactive::operator()(){
    // In addition to starting simulation in a new thread, I also
    // use a condition variable so that I can make sure that simulation
    // is really started before returning control to ReSP
    boost::mutex::scoped_lock end_of_sim_lock(this->start_of_sim_mutex);
    this->start_of_sim_cond.notify_all();
    sc_start();
}

/// *** Reset state: the simiulaton is reset because we haven't started it yet. ***
/// Reaction to the run undefinitely event; I have to actually
/// start simulation using the sc_start call inside the
/// controllerThread_interactive
boost::statechart::result Reset_st::react(const EvRun &){
}
/// Reaction to the run timed event; I have to actually
/// start simulation using the sc_start call inside the
/// controllerThread_interactive. Before starting the simulation
/// I notify the stopped event at time t.
boost::statechart::result Reset_st::react(const EvRun_t & event){
}
///These two events do not cause a transition, but they are simply
/// used to print warnings to the user: simulation hasn't been
///started yet, of course it cannot be stopped or paused!!
boost::statechart::result Reset_st::react(const EvPause &){
}
boost::statechart::result Reset_st::react(const EvStop &){
}

/// *** Stopped state: ***
///These events do not do anything; simply they are used to print
///warning to the user: simulation is stopped, it cannot be
///restarted
boost::statechart::result Stopped_st::react(const EvRun &){
}
boost::statechart::result Stopped_st::react(const EvRun_t &){
}
/// Constructor, called every time the
/// status is entered. It mainly stops the timer and
/// it records elapsed time
Stopped_st::Stopped_st(){}

/// *** Run state: the simiulaton is running ***
/// Reaction to the pause event: I have to cancel pending
/// stop notifications and explicitly notify the pause condition
/// This transitions can be called both because the used explicitly requests
/// to pause or because the current time quantum expired
boost::statechart::result Running_st::react(const EvPause &){
}
/// Reaction to the stop timed event. This event can be called
/// because the user explicitly requests to stop simulation,
/// sc_stop is implicitly called in the code or the queue
/// of events is empty
boost::statechart::result Running_st::react(const EvStop &){
}
/// Constructor, called every time the
/// status is entered. It mainly resets and
/// re-starts the timer
Running_st::Running_st(){
}

/// *** Paused state: the simulation is paused, it is not running ***
/// Reaction to the run undefinitely event; I have to actually
/// resume simulation by notifying the boost condition
boost::statechart::result Paused_st::react(const EvRun &){
}
/// Reaction to the run timed event; I have to actually
/// resume simulation by notifying the boost condition.
/// Before that, I notify the stopped event at time t.
boost::statechart::result Paused_st::react(const EvRun_t & event){
}
/// Reaction to the stop timed event. This event can be called
/// only because the user explicitly requests to stop simulation.
/// I call sc_stop, and, immediately after, I notify the boost
/// condition, so that also the simulation engine thread can
/// correctly end.
boost::statechart::result Paused_st::react(const EvStop &){
}
/// Constructor, called every time the
/// status is entered. It mainly stops the timer and
/// it records elapsed time
Paused_st::Paused_st(){
}
