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


#ifndef LOADER_HPP
#define LOADER_HPP

#include <string>
#include <vector>

extern "C" {
#include <bfd.h>
}

class Loader{
  private:
    bfd * execImage;
    unsigned char * programData;
    unsigned int progDim;
    unsigned int dataStart;
    std::vector<unsigned char> tlsData;

    ///examines the bfd in order to find the sections containing data
    ///to be loaded; at the same time it fills the programData
    ///array
    void loadProgramData();
  public:
    ///Initializes the loader of executable files by creating
    ///the corresponding bfd image of the executable file
    ///specified as parameter
    Loader(std::string fileName);
    ~Loader();
    ///Returns the entry point of the loaded program
    unsigned int getProgStart();
    ///Returns the start address of the data to load
    unsigned int getDataStart();
    ///Returns the dimensione of the loaded program
    unsigned int getProgDim();
    ///Returns a pointer to the array contianing the program data
    unsigned char * getProgData();
    ///Returns the byte of the program data at index idx
    unsigned char getProgDataValue(unsigned long idx);
    ///Specifies whether the current executable has a
    ///Thread-Local-Storage (TLS) section or not
    bool hasTLS();
    ///Returns the thread local storage bytes, including both
    ///static data and the bss
    std::vector<unsigned char> getTLSData();
};

#endif
