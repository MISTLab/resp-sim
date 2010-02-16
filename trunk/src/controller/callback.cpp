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
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 *
 *   (c) Giovanni Beltrame, Luca Fossati
 *       Giovanni.Beltrame@esa.int fossati@elet.polimi.it
 *
\***************************************************************************/

#include "callback.hpp"

#include <vector>
#include <iostream>

std::vector<resp::EOScallback *> resp::end_of_sim_callbacks;

void resp::registerEosCallback(resp::EOScallback &cb){
    end_of_sim_callbacks.push_back(&cb);
}

void resp::notifyEosCallback(){
    std::vector<EOScallback *>::iterator cbIter, cbIterEnd;
    for(cbIter = end_of_sim_callbacks.begin(), cbIterEnd = end_of_sim_callbacks.end();
                                                    cbIter != cbIterEnd; cbIter++){
        (*(*cbIter))();
    }
}

std::vector<resp::PauseCallback *> resp::pause_callbacks;

void resp::registerPauseCallback(resp::PauseCallback &cb){
    pause_callbacks.push_back(&cb);
}

void resp::notifyPauseCallback(){
    std::vector<PauseCallback *>::iterator cbIter, cbIterEnd;
    for(cbIter = pause_callbacks.begin(), cbIterEnd = pause_callbacks.end();
                                                    cbIter != cbIterEnd; cbIter++){
        (*(*cbIter))();
    }
}

std::vector<resp::ErrorCallback *> resp::error_callbacks;

void resp::registerErrorCallback(resp::ErrorCallback &cb){
    error_callbacks.push_back(&cb);
}

void resp::notifyErrorCallback(){
    std::vector<ErrorCallback *>::iterator cbIter, cbIterEnd;
    for(cbIter = error_callbacks.begin(), cbIterEnd = error_callbacks.end();
                                                    cbIter != cbIterEnd; cbIter++){
        (*(*cbIter))();
    }
}

std::vector<resp::DeltaCallback *> resp::delta_callbacks;

void resp::registerDeltaCallback(resp::DeltaCallback &cb){
    delta_callbacks.push_back(&cb);
}

void resp::notifyDeltaCallback(){
    std::vector<DeltaCallback *>::iterator cbIter, cbIterEnd;
    for(cbIter = delta_callbacks.begin(), cbIterEnd = delta_callbacks.end();
                                                    cbIter != cbIterEnd; cbIter++){
        (*(*cbIter))();
    }
}
