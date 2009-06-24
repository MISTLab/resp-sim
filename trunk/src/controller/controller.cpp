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
 *   It allows to start and stop the simulation
 *
 */


#include "simulation_engine.hpp"
#include "controller.hpp"
#include "callback.hpp"
#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/lexical_cast.hpp>
#ifndef STATIC_PLATFORM
#include <Python.h>
// #include <boost/python/exec.hpp>
// #include <boost/python/import.hpp>
// #include <boost/python/object.hpp>
#endif
#include <iostream>
#include <string>
#include <systemc.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <csignal>
#include <exception>
#include <stdexcept>

#include <boost/timer.hpp>

extern void (*delta_callback)(void);

namespace resp{

bool GDBEnabled = false;
boost::mutex sc_controller::global_mutex;
boost::condition sc_controller::stopped_condition;
sc_event sc_controller::stop;

boost::mutex controller_mutex;
boost::mutex endOfSimWait;

bool sc_controller::error = false;
bool sc_controller::disableLatency = false;
sc_event sc_controller::syscall_lock;
bool sc_controller::locked = false;
std::string sc_controller::errorMessage;

sc_controller * controllerInstance = NULL;

std::vector<PauseCallback *> pause_callbacks;
std::vector<TimeoutCallback *> timeout_callbacks;
std::vector<ErrorCallback *> error_callbacks;

#ifndef STATIC_PLATFORM
bool interactive = true;
bool have_callbacks=false;

/*********************************************************************
    Callbacks related stuff
**********************************************************************/
// boost::python::object resp_ns;
std::list<DeltaCallback *> callbacks;

void check_callbacks() {

    // Do the callback magic
    if( have_callbacks ) {
        for( std::list<DeltaCallback *>::iterator i = callbacks.begin() ; i != callbacks.end() ; i++ ) {
            (*(*i))();
        }
    }

}
#endif

/*********************************************************************
    END Callbacks related stuff
**********************************************************************/

///Registers a callback which is called when the simulation is manually
///paused by the user using sc_controller.pause_simulation
void register_PAUSE_callback(PauseCallback &callBack){
    pause_callbacks.push_back(&callBack);
}

///Registers a callback which is called when the simulation ends
///because of a timeout (i.e. simulation was started for 10 NS and
///this time has expired)
void register_TIMEOUT_callback(TimeoutCallback &callBack){
    timeout_callbacks.push_back(&callBack);
}

void register_ERROR_callback(ErrorCallback &callBack){
    error_callbacks.push_back(&callBack);
}

static boost::mutex blockMutex;
static boost::condition blockCond;

void my_terminate_handler(){
    #ifndef NDEBUG
    std::cerr << "Exception handled by controller!" << std::endl;
    #endif

    sc_controller::error = true;
    controllerInstance->accumulatedTime += controllerInstance->timeTracker.elapsed();
    controllerInstance->ended = true;
    controllerInstance->stop_simulation();
    try{
        throw;
    }
    catch (std::exception &e){
        sc_controller::errorMessage = e.what();
    }
    catch(std::string &e){
        sc_controller::errorMessage = e;
    }
    catch (std::exception *e){
        sc_controller::errorMessage = e->what();
    }
    catch(std::string *e){
        sc_controller::errorMessage = *e;
    }
    catch(...){
        sc_controller::errorMessage = "Unknown Exception";
    }

    try{
        std::cerr << std::endl << std::endl << "**** RECEIVED EXCEPTION --> " << sc_controller::errorMessage << " <-- ***" <<std::endl << std::endl;
        if(controllerInstance->interactive)
            std::cerr << "\tCTRL^D to quit the simulator" << std::endl << std::endl;

        std::vector<ErrorCallback *>::iterator cbIter, cbIterEnd;
        for(cbIter = error_callbacks.begin(), cbIterEnd = error_callbacks.end();
                                                        cbIter != cbIterEnd; cbIter++){
            #ifndef NDEBUG
            std::cerr << __PRETTY_FUNCTION__ << " Calling Error callback" << std::endl;
            #endif
            (*(*cbIter))();
        }
    }
    catch(...){}

    kill(controllerInstance->get_pid(), 10);

    try{
        blockCond.notify_all();
        static boost::mutex termMutex;
        static boost::condition termCond;
        boost::mutex::scoped_lock lk(termMutex);
        termCond.wait(lk);
    }
    catch(...){;}
}

/// Static method for the creation of the controller class; note that this is the only way
///of creating a controller since the real constructor is private
void sc_controller::reset() {
    se = new simulation_engine("se", pid, interactive, paused);
    this->residualSimTime = 0;
    this->requestedSimLength = 0;
    this->curState = NONE;
    this->started = false;
    this->paused = false;
    this->hasToContinue = false;
    this->simTime = 0;
    this->ended = false;
    this->accumulatedTime = 0;
    this->timeTracker.restart();
}

/// Static method for the creation of the controller class; note that this is the only way
///of creating a controller since the real constructor is private
sc_controller & sc_controller::createController(int pid, bool interactive){
    if(controllerInstance == NULL){
        controllerInstance = new sc_controller(pid,  interactive);
    }
    return *controllerInstance;
}

/// Controller class constructor
sc_controller::sc_controller(int pid, bool interactive) : residualSimTime(0), requestedSimLength(0),  curState(NONE),
        started(false), paused(false), pid(pid), hasToContinue(false), simTime(0), ended(false),
                                                accumulatedTime(0), interactive(interactive) {
    se = new simulation_engine("se", pid, interactive, paused);
    interactiveSimulation = interactive;
    #ifndef STATIC_PLATFORM
    PyEval_InitThreads();
    #endif
    sc_set_stop_mode(SC_STOP_IMMEDIATE);
    if(controllerInstance != NULL){
        std::cerr << __PRETTY_FUNCTION__ << " - Warning: An instance of the controller already exists" << std::endl;
    }
    std::set_terminate(resp::my_terminate_handler);
}

/// Runs the simulation up to the specified simulation time
void sc_controller::run_up_to(double simTime){
    if((simTime*1000 - sc_time_stamp().to_double()) > 0)
        this->run_simulation((simTime*1000 - sc_time_stamp().to_double()));
}

/// Runs the simulation for a specified amount of time
void sc_controller::run_simulation(double simTime){
    #ifndef NDEBUG
    std::cerr << __PRETTY_FUNCTION__ << " acquiring blockMutex " << std::endl;
    #endif
    boost::mutex::scoped_lock lb(resp::blockMutex);
    #ifndef NDEBUG
    std::cerr << __PRETTY_FUNCTION__ << " acquiring controller mutex" << std::endl;
    #endif
    boost::mutex::scoped_lock lock(controller_mutex);
    if(sc_controller::error){
        std::cerr << __PRETTY_FUNCTION__ << " An error occurred during simulation, so it can't be restarted" << std::endl;
        return;
    }
    this->hasToContinue = false;
    sc_controller::disableLatency = false;
    if(this->curState == NONE || (this->curState == TIMED && this->ended)){

        //Ok,  It's the first time, I haven't started simulation yet
        #ifndef NDEBUG
        std::cerr << __PRETTY_FUNCTION__ << " first time interactive - simTime " << simTime << std::endl;
        #endif
        caller c;
        c.accumulatedTime = &this->accumulatedTime;
        c.timeTracker = &this->timeTracker;
        c.ended = &this->ended;
        c.started = &this->started;
        c.hasToContinue = &this->hasToContinue;
        c.simTime = &this->simTime;
        c.ctrl = this;
        this->simTime = simTime;
        if( simTime <= 0 ) {
            this->curState = AUTO;
        } else { // Simulation has't started yet and I specify it to run for a particular ammount of time
            this->simTimeAtLastRun = sc_time_stamp().to_double();
            this->requestedSimLength = simTime*1000;
            this->curState = TIMED;
            this->ended = false;
        }
        boost::thread thrd (c);
        //Now I wait until simulation has really started
        if(!this->interactive){
          #ifndef NDEBUG
          std::cerr << __PRETTY_FUNCTION__ << " waiting for simulation to end - 238" << std::endl;
          #endif
          resp::blockCond.wait(lb);
        } else{
          while(!this->started)
            usleep(100);
        }

       #ifndef NDEBUG
       std::cerr << __PRETTY_FUNCTION__ << " Simulation started" << std::endl;
       #endif

   }  else{
        //OK,  simulation has already been started: it is either in AUTO or TIMED mode;
        //if it is in AUTO I simply have to notify the stop condition and the simulation engine
        //will restart simulation; I then schedule the STOP condition to happen in the future
        if(this->curState == TIMED){
             //I'm in TIMED state: this means that I have to restart simulation and
             //finish the time that was missing; in case the simulation time I specified is longer
             //than the time which is missing,  I have to schedule another run for that time. Note
             //that it might be that in all moments we can be paused again.
             if(this->residualSimTime < simTime){
                #ifndef NDEBUG
                std::cerr << __PRETTY_FUNCTION__ << " already started timed - simTime " << simTime << " residual " << this->residualSimTime << std::endl;
                #endif
                 //I have to restart also another simulation slot,  because the current one is
                 //not necessary to complete the request
                 this->hasToContinue = true;
                 this->simTime = simTime - this->residualSimTime;
             }
        }
         #ifndef NDEBUG
         std::cerr << __PRETTY_FUNCTION__ << " Resuming - simTime " << simTime << " this->sim-time " << this->simTime << " remaining time " << this->residualSimTime << std::endl;
         #endif
        //Now I can simply restart simulation
        if(simTime > 0 && (this->curState == AUTO || (this->curState == TIMED && this->residualSimTime > simTime)))
            stop.notify(simTime, SC_NS);
         #ifndef NDEBUG
         std::cerr << __PRETTY_FUNCTION__ << " Notifying stop condition" << std::endl;
         #endif
        stopped_condition.notify_all();
         #ifndef NDEBUG
         std::cerr << __PRETTY_FUNCTION__ << " Notified stop condition" << std::endl;
         #endif
        this->timeTracker.restart();
        if(!this->interactive && !this->ended){
          #ifndef NDEBUG
          std::cerr << __PRETTY_FUNCTION__ << " waiting for simulation to end - 284" << std::endl;
          #endif
          resp::blockCond.wait(lb);
        }
    }
    this->paused = false;
    #ifndef NDEBUG
    std::cerr << __PRETTY_FUNCTION__ << " exited from run_simulation" << std::endl;
    #endif
}

void sc_controller::kill(){
    this->se->isKilled = true;
    this->stop_simulation();
}

void sc_controller::stop_simulation(){
    if(sc_controller::error){
         #ifndef NDEBUG
         std::cerr << __PRETTY_FUNCTION__ << " there was an error, so I do not do anything" << std::endl;
         #endif
        return;
    }
    else{
         sc_controller::disableLatency = true;
         #ifndef NDEBUG
         std::cerr << __PRETTY_FUNCTION__ << " Trying to acquire --> controller_mutex" << std::endl;
         #endif
         boost::mutex::scoped_lock * lock = NULL;
         if(this->interactive)
             lock = new boost::mutex::scoped_lock(controller_mutex);
         #ifndef NDEBUG
         std::cerr << __PRETTY_FUNCTION__ << " Stopping simulation - started " << this->started << " - ended " << this->ended << std::endl;
         #endif
         //Note that we perform the operations to stop only if simulation has already started
        if(this->started && ((!this->ended && sc_is_running()))){ // || this->curState == TIMED)){
             this->hasToContinue = false;
             #ifndef NDEBUG
             std::cerr << __PRETTY_FUNCTION__ << " calling sc_stop" << std::endl;
             #endif
             sc_stop();
             #ifndef NDEBUG
             std::cerr << __PRETTY_FUNCTION__ << " sc_stop returned - paused " << this->paused << std::endl;
             #endif
             if(this->paused)
                stopped_condition.notify_all();
            if(this->interactive && !sc_controller::error){
                 #ifndef NDEBUG
                 std::cerr << __PRETTY_FUNCTION__ << " Waiting for the end of the simulation" << std::endl;
                 #endif
                boost::mutex::scoped_lock lock2(endOfSimWait);
            }
         }
         #ifndef NDEBUG
         std::cerr << __PRETTY_FUNCTION__ << " Simulation stopped" << std::endl;
         #endif
         this->curState = NONE;
         if(lock != NULL)
             delete lock;
     }
}

bool sc_controller::is_paused(){
    return this->paused;
}

/// Pauses the simulation, waiting for external control
void sc_controller::pause_simulation(bool notify){
    this->accumulatedTime += this->timeTracker.elapsed();
    if(sc_controller::error){
        std::cerr << __PRETTY_FUNCTION__ << " An error occurred during simulation, so it is already stopped" << std::endl;
        return;
    }
    if(!this->interactive){
        std::cerr << __PRETTY_FUNCTION__ << " Simulation running in non-interactive mode cannot be stopped" << std::endl;
        return;
    }
    sc_controller::disableLatency = true;
    #ifndef NDEBUG
    std::cerr << __PRETTY_FUNCTION__ << " acquiring lock on controller_mutex" << std::endl;
    #endif
    boost::mutex::scoped_lock lock(controller_mutex);
    #ifndef NDEBUG
    std::cerr << __PRETTY_FUNCTION__ << " lock acquired on controller_mutex" << std::endl;
    #endif
    if(this->paused || (this->residualSimTime > 0 && this->curState == TIMED)){
        #ifndef NDEBUG
        std::cerr << __PRETTY_FUNCTION__ << " trying to pause though I'm already paused" << std::endl;
        #endif
        return;
    }
    if(this->curState == NONE){
        std::cerr << "Warning: before pausing simulation you should try to start it :-)" << std::endl;
        return;
    }
    bool retry = false;
    se->goingToPause = true;
    do{
        try{
            stop.notify();
            #ifndef NDEBUG
            std::cerr << __PRETTY_FUNCTION__ << " stop condition notified" << std::endl;
            #endif
            retry = false;
        }
        catch(...){
            #ifndef NDEBUG
            std::cerr << __PRETTY_FUNCTION__ << " error in the notification of the stop condition: retrying" << std::endl;
            #endif
            retry = true;
        }
    }while(retry);
    this->paused = true;
    if(this->curState == TIMED){
        this->residualSimTime = this->requestedSimLength - (sc_time_stamp().to_double() - this->simTimeAtLastRun);
    }

    //Calling the callbacks for signaling the whole world that I'm paused
    if(notify){
       std::vector<PauseCallback *>::iterator cbIter, cbIterEnd;
       for(cbIter = pause_callbacks.begin(), cbIterEnd = pause_callbacks.end();
                                                            cbIter != cbIterEnd; cbIter++){
          #ifndef NDEBUG
          std::cerr << __PRETTY_FUNCTION__ << " Calling Pause callback" << std::endl;
          #endif
          (*(*cbIter))();
       }
   }
}

/// Gets SystemC time
double sc_controller::get_simulated_time(){
    return sc_time_stamp().to_double();
}

std::string sc_controller::print_real_time(){
    return boost::lexical_cast<std::string>(this->accumulatedTime) + "s";
}

double sc_controller::get_real_time(){
    return this->accumulatedTime;
}

bool sc_controller::hasStarted(){
    return this->curState != NONE;
}

bool sc_controller::is_finished(){
    return !sc_is_running() && get_simulated_time()>0;
}

bool sc_controller::is_running(){
    return sc_is_running() && get_simulated_time()>0;
}

bool sc_controller::isEnded(){
    return this->ended;
}

void caller::operator()(){
    do{
        #ifndef NDEBUG
        std::cerr << __PRETTY_FUNCTION__ << " controller_thread ** this->sim-time " << *this->simTime << " has to continue " << *this->hasToContinue << std::endl;
        #endif
        bool manual = false;
       *(this->hasToContinue) = false;
       *(this->ended) = false;
       *(this->started) = true;
       if( !ctrl->interactive ) {
            #ifndef NDEBUG
            std::cerr << __PRETTY_FUNCTION__ << " controller_thread ** acquiring blockMutex" << std::endl;
            #endif
            boost::mutex::scoped_lock lb(resp::blockMutex);
       }

       {
           #ifndef NDEBUG
           std::cerr << __PRETTY_FUNCTION__ << " controller_thread ** acquiring endOfSimWait" << std::endl;
           #endif
           boost::mutex::scoped_lock lock(endOfSimWait);
           if(*this->simTime == -1){
               #ifndef NDEBUG
               std::cerr << __PRETTY_FUNCTION__ << " Starting to run undefinitely" << std::endl;
               #endif
               this->timeTracker->restart();
               sc_start();
               #ifndef NDEBUG
               std::cerr << __PRETTY_FUNCTION__ << " I'm done" << std::endl;
               #endif
           } else {
                sc_time t(*this->simTime, SC_NS);
                this->timeTracker->restart();
                manual = true;
                sc_start(t);
            }

            *this->accumulatedTime += this->timeTracker->elapsed();
            *this->ended = true;
        }
        if(manual){
            std::vector<TimeoutCallback *>::iterator cbIter, cbIterEnd;
            for(cbIter = timeout_callbacks.begin(), cbIterEnd = timeout_callbacks.end();
                                                            cbIter != cbIterEnd; cbIter++){
                #ifndef NDEBUG
                std::cerr << __PRETTY_FUNCTION__ << " Calling Timeout callback" << std::endl;
                #endif
                (*(*cbIter))();
            }
        }
        #ifndef NDEBUG
        std::cerr << __PRETTY_FUNCTION__ << " controller_thread ** waking up blocked running_simulation" << std::endl;
        #endif
        resp::blockCond.notify_all();
    }while(*(this->hasToContinue) && *this->simTime > 0);
    #ifndef NDEBUG
    std::cerr << __PRETTY_FUNCTION__ << " controller_thread ** ending thread  " << *this->hasToContinue << "," << *this->simTime << std::endl;
    #endif
}


void sc_controller::setup_callbacks() {
#ifndef STATIC_PLATFORM
    delta_callback = &resp::check_callbacks;
#endif
}

void sc_controller::register_delta_callback( DeltaCallback& c ) {
#ifndef STATIC_PLATFORM
    if( !have_callbacks ) {
        this->setup_callbacks();
        have_callbacks=true;
    }
    callbacks.push_back(&c);
#endif
}

void sc_controller::remove_delta_callback( DeltaCallback& c ) {
#ifndef STATIC_PLATFORM
    callbacks.remove(&c);
    if( callbacks.size() == 0 ) have_callbacks=false;
#endif
}

int sc_controller::get_pid() {
	return this->pid;
}

void sc_controller::set_interactive(bool interactive) {
  this->interactive = interactive;
}

}
