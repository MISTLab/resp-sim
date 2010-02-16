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


#ifndef EOSCALLBACK_HPP
#define EOSCALLBACK_HPP

#include <vector>

namespace resp {

///superclass of all the callbacks which are called by SystemC when the simulation
///has ended
class EOScallback{
  public:
   EOScallback(){}
   virtual void operator()() = 0;
   virtual ~EOScallback(){}
};

class PauseCallback{
  public:
   PauseCallback(){}
   virtual void operator()() = 0;
   virtual ~PauseCallback(){}
};

class ErrorCallback{
  public:
   ErrorCallback(){}
   virtual void operator()() = 0;
   virtual ~ErrorCallback(){}
};

class DeltaCallback{
  public:
   DeltaCallback(){}
   virtual void operator()() = 0;
   virtual ~DeltaCallback(){}
};
void registerEosCallback(EOScallback &cb);
void notifyEosCallback();
extern std::vector<EOScallback *> end_of_sim_callbacks;
void registerPauseCallback(PauseCallback &cb);
void notifyPauseCallback();
extern std::vector<PauseCallback *> pause_callbacks;
void registerErrorCallback(ErrorCallback &cb);
void notifyErrorCallback();
extern std::vector<ErrorCallback *> error_callbacks;
void registerDeltaCallback(DeltaCallback &cb);
void notifyDeltaCallback();
extern std::vector<DeltaCallback *> delta_callbacks;

}

#endif
