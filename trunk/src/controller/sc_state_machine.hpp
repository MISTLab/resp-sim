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

/*
 *   A SystemC controller module
 *
 *   It allows to load new components, attach them, start and stop the simulation
 *
*/

#ifndef SC_STATE_MACHINE_HPP
#define SC_STATE_MACHINE_HPP

#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/state.hpp>
#include <boost/statechart/event.hpp>
#include <boost/statechart/transition.hpp>
#include <boost/statechart/custom_reaction.hpp>
#include <boost/mpl/list.hpp>
#include <boost/timer.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/timer.hpp>

#include <systemc.h>

/**
 * Thread for running systemc in a separate thread: this allows
 * responsiveness of python, while still being able to
 * controll systemc. This structuure is used for the control
 * of interactive simulation
 */
struct controllerThread_interactive{
    boost::mutex & start_of_sim_mutex;
    boost::condition & start_of_sim_cond;

    controllerThread_interactive(boost::mutex & start_of_sim_mutex, boost::condition & start_of_sim_cond);

    void operator()();
};

// Lets first of all define the events at which this machine reacts:
// they are run, run_t, pause, timeout, stop

/// The run command is issued for an undefinite period of time
struct EvRun : boost::statechart::event< EvRun > {};
/// The run command is issued with a time t as argument: simulation
/// runs for, at most, time t
struct EvRun_t : boost::statechart::event< EvRun_t > {
    sc_time timeStep;
    EvRun_t(sc_time timeStep) : timeStep(timeStep){}
};
/// The pause command is issued
struct EvPause : boost::statechart::event< EvPause > {};
/// Simulation is stopped either because the sc_stop command
/// was called, or there are no more events in the quuee
struct EvStop : boost::statechart::event< EvStop > {};

// Now I have to define the state machine itself, specifying
// the initial state
struct Reset_st;
struct ControllerMachine : boost::statechart::state_machine<ControllerMachine, Reset_st> {
    // The state machine shall contain a reference to the
    // boost.timer object used to track elapsed time.
    boost::timer & timeTracker;
    double & accumulatedTime;
    // Event triggered to pause systemc simulation
    sc_event pauseEvent;
    // Mutex and condition used to pause simulation
    boost::mutex pause_mutex;
    boost::condition pause_condition;

    ControllerMachine(boost::timer & timeTracker, double & accumulatedTime);
};

// Now I have to actually define the different states and the outgoing
// transitions

/// Reset state: the simiulaton is reset because we
/// haven't started it yet.
struct Reset_st : boost::statechart::simple_state<Reset_st, ControllerMachine>{
    // Now I specify the reactions: from the stopped state I can only
    // start simulation; the start can be performed in two ways:
    // undefinitely or for a specified time t.
    // The state is also sensitive to the pause and stop events but
    // just to print warnings to the user (of course simulation can
    // neither be pause or stopped if it hasn't started yet).
    typedef boost::mpl::list<
        boost::statechart::custom_reaction<EvRun>,
        boost::statechart::custom_reaction<EvRun_t>,
        boost::statechart::custom_reaction<EvPause>,
        boost::statechart::custom_reaction<EvStop>
    > reactions;

    /// Reaction to the run undefinitely event; I have to actually
    /// start simulation using the sc_start call inside the
    /// controllerThread_interactive
    boost::statechart::result react(const EvRun &);
    /// Reaction to the run timed event; I have to actually
    /// start simulation using the sc_start call inside the
    /// controllerThread_interactive. Before starting the simulation
    /// I notify the stopped event at time t.
    boost::statechart::result react(const EvRun_t & event);
    ///These two events do not cause a transition, but they are simply
    /// used to print warnings to the user: simulation hasn't been
    ///started yet, of course it cannot be stopped or paused!!
    boost::statechart::result react(const EvPause &);
    boost::statechart::result react(const EvStop &);
};

/// Stopped state: the simiulaton is stopped because
/// sc_stop has been called or
/// the queue of events is empty
struct Stopped_st : boost::statechart::state<Stopped_st, ControllerMachine>{
    // Now I specify the reactions: actually from the stopped
    // state I cannot do anything, I cannot go into any other state,
    // these reactions are just used to print warning messages
    // to the user
    typedef boost::mpl::list<
        boost::statechart::custom_reaction<EvRun>,
        boost::statechart::custom_reaction<EvRun_t>
    > reactions;

    boost::statechart::result react(const EvRun &);
    boost::statechart::result react(const EvRun_t &);

    /// Constructor, called every time the
    /// status is entered. It mainly stops the timer and
    /// it records elapsed time
    Stopped_st(my_context ctx);
};

/// Run state: the simiulaton is running
struct Running_st : boost::statechart::state<Running_st, ControllerMachine>{
    // Now I specify the reactions: from the running state I can
    // either stop or pause simulation. Both these transitions can be
    // implicit (stop because the code called sc_stop or there are no
    // more event, puase because the specified time quantum has elapsed)
    // or explicit (the user expliitly issued commands to pause or stop simulation).
    // In order to centralize all these situations, all these transitions are
    // triggered from the simulation engine
    typedef boost::mpl::list<
        boost::statechart::custom_reaction<EvPause>,
        boost::statechart::custom_reaction<EvStop>
    > reactions;

    /// Reaction to the pause event.
    /// This transitions can be called both because the used explicitly requests
    /// to pause or because the current time quantum expired
    boost::statechart::result react(const EvPause &);
    /// Reaction to the stop timed event. This event can be called
    /// because the user explicitly requests to stop simulation,
    /// sc_stop is implicitly called in the code or the queue
    /// of events is empty
    boost::statechart::result react(const EvStop &);

    /// Constructor, called every time the
    /// status is entered. It mainly resets and
    /// re-starts the timer
    Running_st(my_context ctx);
};

/// Paused state: the simulation is paused, it is not running, no
/// delta cycles are being executed, still it is not stoppped,
/// there are still events in the event queue and sc_stop
/// hasn't been called yet.
struct Paused_st : boost::statechart::state<Paused_st, ControllerMachine>{
    // Now I specify the reactions: from the paused state I can
    // either re-start simulation or stop it.
    typedef boost::mpl::list<
        boost::statechart::custom_reaction<EvRun>,
        boost::statechart::custom_reaction<EvRun_t>,
        boost::statechart::custom_reaction<EvStop>
    > reactions;

    /// Reaction to the run undefinitely event; I have to actually
    /// resume simulation by notifying the boost condition
    boost::statechart::result react(const EvRun &);
    /// Reaction to the run timed event; I have to actually
    /// resume simulation by notifying the boost condition.
    /// Before that, I notify the stopped event at time t.
    boost::statechart::result react(const EvRun_t & event);
    /// Reaction to the stop timed event. This event can be called
    /// only because the user explicitly requests to stop simulation.
    /// I call sc_stop, and, immediately after, I notify the boost
    /// condition, so that also the simulation engine thread can
    /// correctly end.
    boost::statechart::result react(const EvStop &);

    /// Constructor, called every time the
    /// status is entered. It mainly stops the timer and
    /// it records elapsed time
    Paused_st(my_context ctx);
};

#endif
