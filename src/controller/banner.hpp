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


#ifndef BANNER_HPP
#define BANNER_HPP

#include <iostream>
#include <string>

std::string banner = std::string("\n") +
"      ___           ___           ___           ___ \n" +
"     /  /\\         /  /\\         /  /\\         /  /\\ \n" +
"    /  /::\\       /  /:/_       /  /:/_       /  /::\\ \n" +
"   /  /:/\\:\\     /  /:/ /\\     /  /:/ /\\     /  /:/\\:\\ \n" +
"  /  /:/~/:/    /  /:/ /:/_   /  /:/ /::\\   /  /:/~/:/ \n" +
" /__/:/ /:/___ /__/:/ /:/ /\\ /__/:/ /:/\\:\\ /__/:/ /:/ \n" +
" \\  \\:\\/:::::/ \\  \\:\\/:/ /:/ \\  \\:\\/:/~/:/ \\  \\:\\/:/ \n" +
"  \\  \\::/~~~~   \\  \\::/ /:/   \\  \\::/ /:/   \\  \\::/ \n" +
"   \\  \\:\\        \\  \\:\\/:/     \\__\\/ /:/     \\  \\:\\ \n" +
"    \\  \\:\\        \\  \\::/        /__/:/       \\  \\:\\ \n" +
"     \\__\\/         \\__\\/         \\__\\/         \\__\\/ \n\n" +
"  v0.3.1 - Politecnico di Milano, European Space Agency \n" +
"     This tool is distributed under the GPL License \n\n";

#define PRINT_BANNER() std::cerr << banner << std::endl;

#endif
