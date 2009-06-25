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


#ifndef SIMULATION_ENGINE_H
#define SIMULATION_ENGINE_H

/*
 *   The simulation engine
 *
 *   It implements the simulation start and stop mechanism
 *
 */

#include <systemc.h>
#include "callback.hpp"

namespace resp{

void register_EOS_callback(EOScallback &callBack);

class simulation_engine : public sc_module{
    friend class sc_simcontext;
  private:
    int pid;
    bool interactive;
    bool &controlPaused;
  public:
    bool isKilled;
    bool goingToPause;
    // Blocks the SystemC execution
    void pause();

    /// Executes the simulation cycle for a specified amount of time
//     void simulate( sc_time& duration );

    SC_HAS_PROCESS(simulation_engine);

    /// Simulation engine constructor
    simulation_engine(sc_module_name name, int pid,  bool interactive, bool &controlPaused);
    ///Overloading of the end_of_simulation method; it can be used to execute methods
    ///at the end of the simulation
    void end_of_simulation();
};

}

#endif /* SIMULATION_ENGINE_H */
