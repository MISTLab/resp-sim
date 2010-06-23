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


#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <iostream>
#include <sstream>
#include <cstdlib>

#ifndef STATIC_PLATFORM
#include <exception>
#include <stdexcept>
#include <execinfo.h>
#include <signal.h>
#endif

#ifdef STATIC_PLATFORM
namespace resp{
void killAll(std::string errorMsg);
}
#endif

#ifdef MAKE_STRING
#undef MAKE_STRING
#endif
#define MAKE_STRING( msg )  ( ((std::ostringstream&)((std::ostringstream() << '\x0') << msg)).str().substr(1) )

#ifdef THROW_EXCEPTION
#undef THROW_EXCEPTION
#endif
#ifdef STATIC_PLATFORM
#define THROW_EXCEPTION( msg ) ( resp::killAll(MAKE_STRING( "At: function " << __PRETTY_FUNCTION__ << " file: " << __FILE__ << ":" << __LINE__ << " --> " << msg )) )
#else
namespace resp{

bool stackTraceEnabled = false;

void RaiseTraceException(std::string message);
}
#define THROW_EXCEPTION( msg ) ( resp::RaiseTraceException(MAKE_STRING( "At: function " << __PRETTY_FUNCTION__ << " file: " << __FILE__ << ":" << __LINE__ << " --> " << msg )) )
#endif

#endif
