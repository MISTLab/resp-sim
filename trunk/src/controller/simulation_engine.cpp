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
 *   TRAP is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
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


/**
 *   The simulation engine
 *
 *   It implements the simulation start and stop mechanism
 *
 */

#include <vector>

#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>

#include "simulation_engine.hpp"
#include "controller.hpp"
#include "callback.hpp"

#include <sys/types.h>
#include <signal.h>

namespace resp{
std::vector<EOScallback *> end_of_sim_callbacks;

void register_EOS_callback(EOScallback &callBack){
    end_of_sim_callbacks.push_back(&callBack);
}

extern std::vector<TimeoutCallback *> timeout_callbacks;
}

using namespace resp;

///Overloading of the end_of_simulation method; it can be used to execute methods
///at the end of the simulation
void simulation_engine::end_of_simulation(){
   std::vector<EOScallback *>::iterator cbIter, cbIterEnd;
   for(cbIter = end_of_sim_callbacks.begin(), cbIterEnd = end_of_sim_callbacks.end();
                                                        cbIter != cbIterEnd; cbIter++){
      #ifndef NDEBUG
      std::cerr << __PRETTY_FUNCTION__ << " Calling EOS callback" << std::endl;
      #endif
      (*(*cbIter))();
   }

   //Finally I print the statistics regarding simulation; note that I do this
   //using signals towards the python environment
   if(!this->isKilled ) { //&& this->interactive){
      #ifndef NDEBUG
      std::cerr << __PRETTY_FUNCTION__ << " Signaling the simulation end to Python" << std::endl;
      #endif
      kill(this->pid, 10);
   }
}

/// Simulation engine constructor
simulation_engine::simulation_engine(sc_module_name name, int pid,  bool interactive, bool &controlPaused) :
                                            sc_module(name), pid(pid), interactive(interactive), controlPaused(controlPaused),
                                                                                                                            isKilled(false), goingToPause(false){
    end_of_sim_callbacks.clear();
    SC_METHOD(pause);
    sensitive << sc_controller::stop;
    dont_initialize();
    end_module();
}

/// Blocks the SystemC execution
void simulation_engine::pause(){
    this->controlPaused = true;
    if(!this->goingToPause){
        std::vector<TimeoutCallback *>::iterator cbIter, cbIterEnd;
        for(cbIter = timeout_callbacks.begin(), cbIterEnd = timeout_callbacks.end();
                                                        cbIter != cbIterEnd; cbIter++){
            #ifndef NDEBUG
            std::cerr << __PRETTY_FUNCTION__ << " Calling Timeout callback" << std::endl;
            #endif
            (*(*cbIter))();
        }
    }

    boost::mutex::scoped_lock lk(sc_controller::global_mutex);
    sc_controller::stopped_condition.wait(lk);
    this->goingToPause = false;
    this->controlPaused = false;
}
