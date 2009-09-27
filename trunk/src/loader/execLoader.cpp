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



#include <string>
#include <map>
#include <iostream>

#include "utils.hpp"

extern "C" {
#include <bfd.h>
}

#include "execLoader.hpp"
#include "bfdWrapper.hpp"

Loader::Loader(std::string fileName){
    this->execImage = &(BFDWrapper::getInstance(fileName).getBFDDescriptor());
    this->programData = NULL;
    this->progDim = 0;
    this->dataStart = 0;

    this->loadProgramData();
}

Loader::~Loader(){
    if(this->programData != NULL){
        delete [] this->programData;
    }
}

unsigned int Loader::getProgStart(){
    if(this->execImage == NULL){
        THROW_EXCEPTION("Binary parser not yet correcly created");
    }
    return bfd_get_start_address(this->execImage);
}

unsigned int Loader::getProgDim(){
    if(this->execImage == NULL){
        THROW_EXCEPTION("Binary parser not yet correcly created");
    }
    return this->progDim;
}

unsigned char * Loader::getProgData(){
    if(this->execImage == NULL){
        THROW_EXCEPTION("Binary parser not yet correcly created");
    }
    if(this->programData == NULL){
        THROW_EXCEPTION("The program data was not correcly computed");
    }
    return this->programData;
}

unsigned char Loader::getProgDataValue(unsigned long idx) {
    if(this->execImage == NULL){
        THROW_EXCEPTION("Binary parser not yet correcly created");
    }
    if(this->programData == NULL){
        THROW_EXCEPTION("The program data was not correcly computed");
    }
    return this->programData[idx];
}

unsigned int Loader::getDataStart(){
    if(this->execImage == NULL){
        THROW_EXCEPTION("Binary parser not yet correcly created");
    }
    return this->dataStart;
}

void Loader::loadProgramData(){
    bfd_section *p = NULL;
    std::map<unsigned long, unsigned char> memMap;
    std::map<unsigned long, unsigned char> tlsMap;
    for (p = this->execImage->sections; p != NULL; p = p->next){
        flagword flags = bfd_get_section_flags(this->execImage, p);
        if((flags & SEC_ALLOC) != 0 && (flags & SEC_DEBUGGING) == 0){
            //Ok,  this is a section which must be in the final executable;
            //Lets see if it has content: if not I have to pad the section with zeros,
            //otherwise I load it
            bfd_size_type datasize = bfd_section_size(this->execImage, p);
            bfd_vma vma = bfd_get_section_vma(this->execImage, p);
            std::map<unsigned long, unsigned char> * curMap = NULL;
            if((flags & SEC_THREAD_LOCAL) == 0){
                curMap = &memMap;
            }
            else{
                curMap = &tlsMap;
            }
            //Normal section containing executable data
            std::map<unsigned long, unsigned char>::iterator curMapPos = curMap->begin();
            if((flags & SEC_HAS_CONTENTS) != 0){
                bfd_byte *data = new bfd_byte[datasize];
                bfd_get_section_contents (this->execImage, p, data, 0, datasize);
                for(unsigned int i = 0; i < datasize; i++){
                    curMapPos = curMap->insert(curMapPos, std::pair<unsigned long, unsigned char>(vma + i, data[i]));
                }
                delete [] data;
            }
            else{
                for(unsigned int i = 0; i < datasize; i++)
                    curMapPos = curMap->insert(curMapPos, std::pair<unsigned long, unsigned char>(vma + i, 0));
            }
        }
    }
    //ok,  I now have all the map of the memory; I simply have to fill in the
    //this->programData  array
    this->dataStart = memMap.begin()->first;
    this->progDim = memMap.rbegin()->first - this->dataStart + 1;
    this->programData = new unsigned char[this->progDim];
    std::map<unsigned long, unsigned char>::iterator memBeg,  memEnd;
    for(memBeg = memMap.begin(),  memEnd = memMap.end(); memBeg != memEnd; memBeg++){
        this->programData[memBeg->first - this->dataStart] = memBeg->second;
    }
    //Now I process thread local storage
    if(!tlsMap.empty()){
        unsigned int tlsStart = tlsMap.begin()->first;
        unsigned int tlsEnd = tlsMap.rbegin()->first;
        for(unsigned int i = 0; i < tlsEnd - tlsStart; i++){
            this->tlsData.push_back(0);
        }
        for(memBeg = tlsMap.begin(),  memEnd = tlsMap.end(); memBeg != memEnd; memBeg++){
            this->tlsData[memBeg->first - tlsStart](memBeg->second);
        }
    }
}

///Specifies whether the current executable has a
///Thread-Local-Storage (TLS) section or not
bool Loader::hasTLS(){
    return !this->tlsData.empty();
}

///Returns the thread local storage bytes, including both
///static data and the bss
std::vector<unsigned char> Loader::getTLSData(){
    return this->tlsData;
}
