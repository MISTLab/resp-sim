/***************************************************************************\
 *
 *   
 *            ___        ___           ___           ___
 *           /  /\      /  /\         /  /\         /  /\
 *          /  /:/     /  /::\       /  /::\       /  /::\
 *         /  /:/     /  /:/\:\     /  /:/\:\     /  /:/\:\
 *        /  /:/     /  /:/~/:/    /  /:/~/::\   /  /:/~/:/
 *       /  /::\    /__/:/ /:/___ /__/:/ /:/\:\ /__/:/ /:/
 *      /__/:/\:\   \  \:\/:::::/ \  \:\/:/__\/ \  \:\/:/
 *      \__\/  \:\   \  \::/~~~~   \  \::/       \  \::/
 *           \  \:\   \  \:\        \  \:\        \  \:\
 *            \  \ \   \  \:\        \  \:\        \  \:\
 *             \__\/    \__\/         \__\/         \__\/
 *   
 *
 *
 *   
 *   This file is part of TRAP.
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
 *   (c) Luca Fossati, fossati@elet.polimi.it
 *
\***************************************************************************/



#include <alias.hpp>
#include <registers.hpp>
#include <ostream>
#include <list>

using namespace leon3_acclt_trap;
void leon3_acclt_trap::Alias::setPipeId( unsigned int pipeId ){
    if(this->pipeId < 0){
        this->pipeId = pipeId;
    }
    else{
        THROW_ERROR("Error, pipeline Id of alias can only be set during alias initialization; \
            it already has value " << this->pipeId);
    }
}

void leon3_acclt_trap::Alias::writeAll( const unsigned int & value ) throw(){
    this->pipelineReg->writeAll(value);
}

unsigned int leon3_acclt_trap::Alias::operator ~() throw(){
    return ~*this->pipelineReg->getRegister(this->pipeId);
}

Alias & leon3_acclt_trap::Alias::operator =( const Alias & other ) throw(){
    *this->pipelineReg->getRegister(this->pipeId) = *other.pipelineReg->getRegister(other.pipeId);
    return *this;
}

Alias & leon3_acclt_trap::Alias::operator +=( const Alias & other ) throw(){
    *this->pipelineReg->getRegister(this->pipeId) += *other.pipelineReg->getRegister(other.pipeId);
    return *this;
}

Alias & leon3_acclt_trap::Alias::operator -=( const Alias & other ) throw(){
    *this->pipelineReg->getRegister(this->pipeId) -= *other.pipelineReg->getRegister(other.pipeId);
    return *this;
}

Alias & leon3_acclt_trap::Alias::operator *=( const Alias & other ) throw(){
    *this->pipelineReg->getRegister(this->pipeId) *= *other.pipelineReg->getRegister(other.pipeId);
    return *this;
}

Alias & leon3_acclt_trap::Alias::operator /=( const Alias & other ) throw(){
    *this->pipelineReg->getRegister(this->pipeId) /= *other.pipelineReg->getRegister(other.pipeId);
    return *this;
}

Alias & leon3_acclt_trap::Alias::operator |=( const Alias & other ) throw(){
    *this->pipelineReg->getRegister(this->pipeId) |= *other.pipelineReg->getRegister(other.pipeId);
    return *this;
}

Alias & leon3_acclt_trap::Alias::operator &=( const Alias & other ) throw(){
    *this->pipelineReg->getRegister(this->pipeId) &= *other.pipelineReg->getRegister(other.pipeId);
    return *this;
}

Alias & leon3_acclt_trap::Alias::operator ^=( const Alias & other ) throw(){
    *this->pipelineReg->getRegister(this->pipeId) ^= *other.pipelineReg->getRegister(other.pipeId);
    return *this;
}

Alias & leon3_acclt_trap::Alias::operator <<=( const Alias & other ) throw(){
    *this->pipelineReg->getRegister(this->pipeId) <<= *other.pipelineReg->getRegister(other.pipeId);
    return *this;
}

Alias & leon3_acclt_trap::Alias::operator >>=( const Alias & other ) throw(){
    *this->pipelineReg->getRegister(this->pipeId) >>= *other.pipelineReg->getRegister(other.pipeId);
    return *this;
}

bool leon3_acclt_trap::Alias::operator <( const Register & other ) const throw(){
    return (*this->pipelineReg->getRegister(this->pipeId) < other);
}

bool leon3_acclt_trap::Alias::operator >( const Register & other ) const throw(){
    return (*this->pipelineReg->getRegister(this->pipeId) > other);
}

bool leon3_acclt_trap::Alias::operator <=( const Register & other ) const throw(){
    return (*this->pipelineReg->getRegister(this->pipeId) <= other);
}

bool leon3_acclt_trap::Alias::operator >=( const Register & other ) const throw(){
    return (*this->pipelineReg->getRegister(this->pipeId) >= other);
}

bool leon3_acclt_trap::Alias::operator ==( const Register & other ) const throw(){
    return (*this->pipelineReg->getRegister(this->pipeId) == other);
}

bool leon3_acclt_trap::Alias::operator !=( const Register & other ) const throw(){
    return (*this->pipelineReg->getRegister(this->pipeId) != other);
}

Alias & leon3_acclt_trap::Alias::operator =( const Register & other ) throw(){
    *this->pipelineReg->getRegister(this->pipeId) = other;
    return *this;
}

Alias & leon3_acclt_trap::Alias::operator +=( const Register & other ) throw(){
    *this->pipelineReg->getRegister(this->pipeId) += other;
    return *this;
}

Alias & leon3_acclt_trap::Alias::operator -=( const Register & other ) throw(){
    *this->pipelineReg->getRegister(this->pipeId) -= other;
    return *this;
}

Alias & leon3_acclt_trap::Alias::operator *=( const Register & other ) throw(){
    *this->pipelineReg->getRegister(this->pipeId) *= other;
    return *this;
}

Alias & leon3_acclt_trap::Alias::operator /=( const Register & other ) throw(){
    *this->pipelineReg->getRegister(this->pipeId) /= other;
    return *this;
}

Alias & leon3_acclt_trap::Alias::operator |=( const Register & other ) throw(){
    *this->pipelineReg->getRegister(this->pipeId) |= other;
    return *this;
}

Alias & leon3_acclt_trap::Alias::operator &=( const Register & other ) throw(){
    *this->pipelineReg->getRegister(this->pipeId) &= other;
    return *this;
}

Alias & leon3_acclt_trap::Alias::operator ^=( const Register & other ) throw(){
    *this->pipelineReg->getRegister(this->pipeId) ^= other;
    return *this;
}

Alias & leon3_acclt_trap::Alias::operator <<=( const Register & other ) throw(){
    *this->pipelineReg->getRegister(this->pipeId) <<= other;
    return *this;
}

Alias & leon3_acclt_trap::Alias::operator >>=( const Register & other ) throw(){
    *this->pipelineReg->getRegister(this->pipeId) >>= other;
    return *this;
}

std::ostream & leon3_acclt_trap::Alias::operator <<( std::ostream & stream ) const \
    throw(){
    stream << *this->pipelineReg->getRegister(this->pipeId);
    return stream;
}

void leon3_acclt_trap::Alias::updateAlias( Alias & newAlias ) throw(){
    this->pipelineReg = newAlias.pipelineReg;
    std::list<Alias *>::iterator referredIter, referredEnd;
    for(referredIter = this->referredAliases.begin(), referredEnd = this->referredAliases.end(); \
        referredIter != referredEnd; referredIter++){
        (*referredIter)->newReferredAlias(newAlias.pipelineReg);
    }
    if(this->referringAliases != NULL){
        this->referringAliases->referredAliases.remove(this);
    }
    this->referringAliases = &newAlias;
    newAlias.referredAliases.push_back(this);
}

void leon3_acclt_trap::Alias::updateAlias( PipelineRegister & newAlias ) throw(){
    this->pipelineReg = &newAlias;
    std::list<Alias *>::iterator referredIter, referredEnd;
    for(referredIter = this->referredAliases.begin(), referredEnd = this->referredAliases.end(); \
        referredIter != referredEnd; referredIter++){
        (*referredIter)->newReferredAlias(&newAlias);
    }
    if(this->referringAliases != NULL){
        this->referringAliases->referredAliases.remove(this);
    }
    this->referringAliases = NULL;
}

void leon3_acclt_trap::Alias::directSetAlias( Alias & newAlias ) throw(){
    this->pipelineReg = newAlias.pipelineReg;
    this->pipeId = newAlias.pipeId;
    if(this->referringAliases != NULL){
        this->referringAliases->referredAliases.remove(this);
    }
    this->referringAliases = &newAlias;
    newAlias.referredAliases.push_back(this);
}

void leon3_acclt_trap::Alias::directSetAlias( PipelineRegister & newAlias ) throw(){
    this->pipelineReg = &newAlias;
    if(this->referringAliases != NULL){
        this->referringAliases->referredAliases.remove(this);
    }
    this->referringAliases = NULL;
}

leon3_acclt_trap::Alias::Alias( PipelineRegister * pipelineReg, int pipeId ) : pipelineReg(pipelineReg), \
    pipeId(pipeId){
    this->referringAliases = NULL;
}

leon3_acclt_trap::Alias::Alias( int pipeId ) : pipelineReg(NULL), pipeId(pipeId){
    this->referringAliases = NULL;
}

leon3_acclt_trap::Alias::Alias() : pipelineReg(NULL), pipeId(-1){
    this->referringAliases = NULL;
}

leon3_acclt_trap::Alias::Alias( Alias * initAlias, int pipeId ) : pipelineReg(initAlias->pipelineReg), \
    pipeId(pipeId){
    initAlias->referredAliases.push_back(this);
    this->referringAliases = initAlias;
}

leon3_acclt_trap::Alias::~Alias(){
    std::list<Alias *>::iterator referredIter, referredEnd;
    for(referredIter = this->referredAliases.begin(), referredEnd = this->referredAliases.end(); \
        referredIter != referredEnd; referredIter++){
        if((*referredIter)->referringAliases == this){
            (*referredIter)->referringAliases = NULL;
        }
    }
    if(this->referringAliases != NULL){
        this->referringAliases->referredAliases.remove(this);
    }
    this->referringAliases = NULL;
}


