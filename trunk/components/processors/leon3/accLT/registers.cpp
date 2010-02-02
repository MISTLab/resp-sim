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



#include <registers.hpp>
#include <systemc.h>
#include <ostream>
#include <trap_utils.hpp>

using namespace leon3_acclt_trap;
InnerField & leon3_acclt_trap::InnerField::operator =( const InnerField & other ) \
    throw(){
    *this = (unsigned int)other;
    return *this;
}

leon3_acclt_trap::InnerField::~InnerField(){

}
void leon3_acclt_trap::Register::lock() throw(){
    this->lockedLatency = -1;
    this->numLocked++;
}

void leon3_acclt_trap::Register::unlock() throw(){
    this->lockedLatency = -1;
    if(this->numLocked > 0){
        this->numLocked--;
    }
}

void leon3_acclt_trap::Register::unlock( int wbLatency ) throw(){
    if(wbLatency > 0){
        this->lockedLatency = wbLatency;
    }
    else{
        this->lockedLatency = -1;
        if(this->numLocked > 0){
            this->numLocked--;
        }
    }
}

bool leon3_acclt_trap::Register::isLocked() throw(){
    if(this->lockedLatency > 0){
        this->lockedLatency--;
        if(this->lockedLatency == 0){
            this->numLocked--;
        }
    }
    if(this->numLocked > 0){
        return true;
    }
    else{
        this->numLocked = 0;
        return false;
    }
}

leon3_acclt_trap::Register::Register(){
    this->numLocked = 0;
    this->lockedLatency = -1;
    this->hasToPropagate = NULL;
    this->timeStamp = SC_ZERO_TIME;
}

leon3_acclt_trap::Register::Register( const Register & other ){
    this->numLocked = other.numLocked;
    this->lockedLatency = other.lockedLatency;
    this->hasToPropagate = other.hasToPropagate;
    this->timeStamp = other.timeStamp;
}

InnerField & leon3_acclt_trap::Reg32_0::InnerField_VER::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xf0ffffffL;
    this->value |= ((other & 0xf) << 24);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_0::InnerField_VER::InnerField_VER( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_0::InnerField_VER::~InnerField_VER(){

}
InnerField & leon3_acclt_trap::Reg32_0::InnerField_ICC_z::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xffbfffffL;
    this->value |= ((other & 0x1) << 22);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_0::InnerField_ICC_z::InnerField_ICC_z( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_0::InnerField_ICC_z::~InnerField_ICC_z(){

}
InnerField & leon3_acclt_trap::Reg32_0::InnerField_ICC_v::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xffdfffffL;
    this->value |= ((other & 0x1) << 21);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_0::InnerField_ICC_v::InnerField_ICC_v( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_0::InnerField_ICC_v::~InnerField_ICC_v(){

}
InnerField & leon3_acclt_trap::Reg32_0::InnerField_EF::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xffffefffL;
    this->value |= ((other & 0x1) << 12);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_0::InnerField_EF::InnerField_EF( unsigned int & value, sc_time \
    & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_0::InnerField_EF::~InnerField_EF(){

}
InnerField & leon3_acclt_trap::Reg32_0::InnerField_EC::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xffffdfffL;
    this->value |= ((other & 0x1) << 13);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_0::InnerField_EC::InnerField_EC( unsigned int & value, sc_time \
    & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_0::InnerField_EC::~InnerField_EC(){

}
InnerField & leon3_acclt_trap::Reg32_0::InnerField_ICC_n::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xff7fffffL;
    this->value |= ((other & 0x1) << 23);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_0::InnerField_ICC_n::InnerField_ICC_n( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_0::InnerField_ICC_n::~InnerField_ICC_n(){

}
InnerField & leon3_acclt_trap::Reg32_0::InnerField_S::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xffffff7fL;
    this->value |= ((other & 0x1) << 7);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_0::InnerField_S::InnerField_S( unsigned int & value, sc_time \
    & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_0::InnerField_S::~InnerField_S(){

}
InnerField & leon3_acclt_trap::Reg32_0::InnerField_ET::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xffffffdfL;
    this->value |= ((other & 0x1) << 5);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_0::InnerField_ET::InnerField_ET( unsigned int & value, sc_time \
    & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_0::InnerField_ET::~InnerField_ET(){

}
InnerField & leon3_acclt_trap::Reg32_0::InnerField_ICC_c::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xffefffffL;
    this->value |= ((other & 0x1) << 20);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_0::InnerField_ICC_c::InnerField_ICC_c( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_0::InnerField_ICC_c::~InnerField_ICC_c(){

}
InnerField & leon3_acclt_trap::Reg32_0::InnerField_PS::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xffffffbfL;
    this->value |= ((other & 0x1) << 6);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_0::InnerField_PS::InnerField_PS( unsigned int & value, sc_time \
    & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_0::InnerField_PS::~InnerField_PS(){

}
InnerField & leon3_acclt_trap::Reg32_0::InnerField_PIL::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xfffff0ffL;
    this->value |= ((other & 0xf) << 8);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_0::InnerField_PIL::InnerField_PIL( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_0::InnerField_PIL::~InnerField_PIL(){

}
InnerField & leon3_acclt_trap::Reg32_0::InnerField_CWP::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xffffffe0L;
    this->value |= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_0::InnerField_CWP::InnerField_CWP( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_0::InnerField_CWP::~InnerField_CWP(){

}
InnerField & leon3_acclt_trap::Reg32_0::InnerField_IMPL::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xfffffff;
    this->value |= ((other & 0xf) << 28);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_0::InnerField_IMPL::InnerField_IMPL( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_0::InnerField_IMPL::~InnerField_IMPL(){

}
InnerField & leon3_acclt_trap::Reg32_0::InnerField_Empty::operator =( const unsigned \
    int & other ) throw(){
    return *this;
}

leon3_acclt_trap::Reg32_0::InnerField_Empty::InnerField_Empty(){

}

leon3_acclt_trap::Reg32_0::InnerField_Empty::~InnerField_Empty(){

}
void leon3_acclt_trap::Reg32_0::forceValue( const unsigned int & value ) throw(){
    this->value = value;
}

void leon3_acclt_trap::Reg32_0::immediateWrite( const unsigned int & value ) throw(){
    this->value = value;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
}

unsigned int leon3_acclt_trap::Reg32_0::readNewValue() throw(){
    return this->value;
}

unsigned int leon3_acclt_trap::Reg32_0::operator ~() throw(){
    return ~(this->value);
}

Reg32_0 & leon3_acclt_trap::Reg32_0::operator =( const unsigned int & other ) throw(){
    this->value = other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_0 & leon3_acclt_trap::Reg32_0::operator +=( const unsigned int & other ) throw(){
    this->value += other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_0 & leon3_acclt_trap::Reg32_0::operator -=( const unsigned int & other ) throw(){
    this->value -= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_0 & leon3_acclt_trap::Reg32_0::operator *=( const unsigned int & other ) throw(){
    this->value *= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_0 & leon3_acclt_trap::Reg32_0::operator /=( const unsigned int & other ) throw(){
    this->value /= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_0 & leon3_acclt_trap::Reg32_0::operator |=( const unsigned int & other ) throw(){
    this->value |= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_0 & leon3_acclt_trap::Reg32_0::operator &=( const unsigned int & other ) throw(){
    this->value &= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_0 & leon3_acclt_trap::Reg32_0::operator ^=( const unsigned int & other ) throw(){
    this->value ^= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_0 & leon3_acclt_trap::Reg32_0::operator <<=( const unsigned int & other ) throw(){
    this->value <<= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_0 & leon3_acclt_trap::Reg32_0::operator >>=( const unsigned int & other ) throw(){
    this->value >>= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

unsigned int leon3_acclt_trap::Reg32_0::operator +( const Reg32_0 & other ) const \
    throw(){
    return (this->value + other.value);
}

unsigned int leon3_acclt_trap::Reg32_0::operator -( const Reg32_0 & other ) const \
    throw(){
    return (this->value - other.value);
}

unsigned int leon3_acclt_trap::Reg32_0::operator *( const Reg32_0 & other ) const \
    throw(){
    return (this->value * other.value);
}

unsigned int leon3_acclt_trap::Reg32_0::operator /( const Reg32_0 & other ) const \
    throw(){
    return (this->value / other.value);
}

unsigned int leon3_acclt_trap::Reg32_0::operator |( const Reg32_0 & other ) const \
    throw(){
    return (this->value | other.value);
}

unsigned int leon3_acclt_trap::Reg32_0::operator &( const Reg32_0 & other ) const \
    throw(){
    return (this->value & other.value);
}

unsigned int leon3_acclt_trap::Reg32_0::operator ^( const Reg32_0 & other ) const \
    throw(){
    return (this->value ^ other.value);
}

unsigned int leon3_acclt_trap::Reg32_0::operator <<( const Reg32_0 & other ) const \
    throw(){
    return (this->value << other.value);
}

unsigned int leon3_acclt_trap::Reg32_0::operator >>( const Reg32_0 & other ) const \
    throw(){
    return (this->value >> other.value);
}

bool leon3_acclt_trap::Reg32_0::operator <( const Reg32_0 & other ) const throw(){
    return (this->value < other.value);
}

bool leon3_acclt_trap::Reg32_0::operator >( const Reg32_0 & other ) const throw(){
    return (this->value > other.value);
}

bool leon3_acclt_trap::Reg32_0::operator <=( const Reg32_0 & other ) const throw(){
    return (this->value <= other.value);
}

bool leon3_acclt_trap::Reg32_0::operator >=( const Reg32_0 & other ) const throw(){
    return (this->value >= other.value);
}

bool leon3_acclt_trap::Reg32_0::operator ==( const Reg32_0 & other ) const throw(){
    return (this->value == other.value);
}

bool leon3_acclt_trap::Reg32_0::operator !=( const Reg32_0 & other ) const throw(){
    return (this->value != other.value);
}

Reg32_0 & leon3_acclt_trap::Reg32_0::operator =( const Reg32_0 & other ) throw(){
    this->value = other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_0 & leon3_acclt_trap::Reg32_0::operator +=( const Reg32_0 & other ) throw(){
    this->value += other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_0 & leon3_acclt_trap::Reg32_0::operator -=( const Reg32_0 & other ) throw(){
    this->value -= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_0 & leon3_acclt_trap::Reg32_0::operator *=( const Reg32_0 & other ) throw(){
    this->value *= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_0 & leon3_acclt_trap::Reg32_0::operator /=( const Reg32_0 & other ) throw(){
    this->value /= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_0 & leon3_acclt_trap::Reg32_0::operator |=( const Reg32_0 & other ) throw(){
    this->value |= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_0 & leon3_acclt_trap::Reg32_0::operator &=( const Reg32_0 & other ) throw(){
    this->value &= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_0 & leon3_acclt_trap::Reg32_0::operator ^=( const Reg32_0 & other ) throw(){
    this->value ^= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_0 & leon3_acclt_trap::Reg32_0::operator <<=( const Reg32_0 & other ) throw(){
    this->value <<= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_0 & leon3_acclt_trap::Reg32_0::operator >>=( const Reg32_0 & other ) throw(){
    this->value >>= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

unsigned int leon3_acclt_trap::Reg32_0::operator +( const Register & other ) const \
    throw(){
    return (this->value + other);
}

unsigned int leon3_acclt_trap::Reg32_0::operator -( const Register & other ) const \
    throw(){
    return (this->value - other);
}

unsigned int leon3_acclt_trap::Reg32_0::operator *( const Register & other ) const \
    throw(){
    return (this->value * other);
}

unsigned int leon3_acclt_trap::Reg32_0::operator /( const Register & other ) const \
    throw(){
    return (this->value / other);
}

unsigned int leon3_acclt_trap::Reg32_0::operator |( const Register & other ) const \
    throw(){
    return (this->value | other);
}

unsigned int leon3_acclt_trap::Reg32_0::operator &( const Register & other ) const \
    throw(){
    return (this->value & other);
}

unsigned int leon3_acclt_trap::Reg32_0::operator ^( const Register & other ) const \
    throw(){
    return (this->value ^ other);
}

unsigned int leon3_acclt_trap::Reg32_0::operator <<( const Register & other ) const \
    throw(){
    return (this->value << other);
}

unsigned int leon3_acclt_trap::Reg32_0::operator >>( const Register & other ) const \
    throw(){
    return (this->value >> other);
}

bool leon3_acclt_trap::Reg32_0::operator <( const Register & other ) const throw(){
    return (this->value < other);
}

bool leon3_acclt_trap::Reg32_0::operator >( const Register & other ) const throw(){
    return (this->value > other);
}

bool leon3_acclt_trap::Reg32_0::operator <=( const Register & other ) const throw(){
    return (this->value <= other);
}

bool leon3_acclt_trap::Reg32_0::operator >=( const Register & other ) const throw(){
    return (this->value >= other);
}

bool leon3_acclt_trap::Reg32_0::operator ==( const Register & other ) const throw(){
    return (this->value == other);
}

bool leon3_acclt_trap::Reg32_0::operator !=( const Register & other ) const throw(){
    return (this->value != other);
}

Reg32_0 & leon3_acclt_trap::Reg32_0::operator =( const Register & other ) throw(){
    this->value = other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_0 & leon3_acclt_trap::Reg32_0::operator +=( const Register & other ) throw(){
    this->value += other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_0 & leon3_acclt_trap::Reg32_0::operator -=( const Register & other ) throw(){
    this->value -= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_0 & leon3_acclt_trap::Reg32_0::operator *=( const Register & other ) throw(){
    this->value *= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_0 & leon3_acclt_trap::Reg32_0::operator /=( const Register & other ) throw(){
    this->value /= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_0 & leon3_acclt_trap::Reg32_0::operator |=( const Register & other ) throw(){
    this->value |= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_0 & leon3_acclt_trap::Reg32_0::operator &=( const Register & other ) throw(){
    this->value &= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_0 & leon3_acclt_trap::Reg32_0::operator ^=( const Register & other ) throw(){
    this->value ^= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_0 & leon3_acclt_trap::Reg32_0::operator <<=( const Register & other ) throw(){
    this->value <<= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_0 & leon3_acclt_trap::Reg32_0::operator >>=( const Register & other ) throw(){
    this->value >>= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

std::ostream & leon3_acclt_trap::Reg32_0::operator <<( std::ostream & stream ) const \
    throw(){
    stream << std::hex << std::showbase << this->value << std::dec;
    return stream;
}

leon3_acclt_trap::Reg32_0::Reg32_0() : field_VER(this->value, this->timeStamp, this->hasToPropagate), \
    field_ICC_z(this->value, this->timeStamp, this->hasToPropagate), field_ICC_v(this->value, \
    this->timeStamp, this->hasToPropagate), field_EF(this->value, this->timeStamp, this->hasToPropagate), \
    field_EC(this->value, this->timeStamp, this->hasToPropagate), field_ICC_n(this->value, \
    this->timeStamp, this->hasToPropagate), field_S(this->value, this->timeStamp, this->hasToPropagate), \
    field_ET(this->value, this->timeStamp, this->hasToPropagate), field_ICC_c(this->value, \
    this->timeStamp, this->hasToPropagate), field_PS(this->value, this->timeStamp, this->hasToPropagate), \
    field_PIL(this->value, this->timeStamp, this->hasToPropagate), field_CWP(this->value, \
    this->timeStamp, this->hasToPropagate), field_IMPL(this->value, this->timeStamp, \
    this->hasToPropagate){
    this->value = 0;
}

InnerField & leon3_acclt_trap::Reg32_1::InnerField_WIM_28::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xefffffffL;
    this->value |= ((other & 0x1) << 28);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_1::InnerField_WIM_28::InnerField_WIM_28( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_1::InnerField_WIM_28::~InnerField_WIM_28(){

}
InnerField & leon3_acclt_trap::Reg32_1::InnerField_WIM_29::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xdfffffffL;
    this->value |= ((other & 0x1) << 29);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_1::InnerField_WIM_29::InnerField_WIM_29( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_1::InnerField_WIM_29::~InnerField_WIM_29(){

}
InnerField & leon3_acclt_trap::Reg32_1::InnerField_WIM_24::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xfeffffffL;
    this->value |= ((other & 0x1) << 24);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_1::InnerField_WIM_24::InnerField_WIM_24( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_1::InnerField_WIM_24::~InnerField_WIM_24(){

}
InnerField & leon3_acclt_trap::Reg32_1::InnerField_WIM_25::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xfdffffffL;
    this->value |= ((other & 0x1) << 25);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_1::InnerField_WIM_25::InnerField_WIM_25( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_1::InnerField_WIM_25::~InnerField_WIM_25(){

}
InnerField & leon3_acclt_trap::Reg32_1::InnerField_WIM_26::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xfbffffffL;
    this->value |= ((other & 0x1) << 26);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_1::InnerField_WIM_26::InnerField_WIM_26( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_1::InnerField_WIM_26::~InnerField_WIM_26(){

}
InnerField & leon3_acclt_trap::Reg32_1::InnerField_WIM_27::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xf7ffffffL;
    this->value |= ((other & 0x1) << 27);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_1::InnerField_WIM_27::InnerField_WIM_27( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_1::InnerField_WIM_27::~InnerField_WIM_27(){

}
InnerField & leon3_acclt_trap::Reg32_1::InnerField_WIM_20::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xffefffffL;
    this->value |= ((other & 0x1) << 20);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_1::InnerField_WIM_20::InnerField_WIM_20( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_1::InnerField_WIM_20::~InnerField_WIM_20(){

}
InnerField & leon3_acclt_trap::Reg32_1::InnerField_WIM_21::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xffdfffffL;
    this->value |= ((other & 0x1) << 21);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_1::InnerField_WIM_21::InnerField_WIM_21( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_1::InnerField_WIM_21::~InnerField_WIM_21(){

}
InnerField & leon3_acclt_trap::Reg32_1::InnerField_WIM_22::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xffbfffffL;
    this->value |= ((other & 0x1) << 22);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_1::InnerField_WIM_22::InnerField_WIM_22( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_1::InnerField_WIM_22::~InnerField_WIM_22(){

}
InnerField & leon3_acclt_trap::Reg32_1::InnerField_WIM_23::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xff7fffffL;
    this->value |= ((other & 0x1) << 23);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_1::InnerField_WIM_23::InnerField_WIM_23( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_1::InnerField_WIM_23::~InnerField_WIM_23(){

}
InnerField & leon3_acclt_trap::Reg32_1::InnerField_WIM_9::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xfffffdffL;
    this->value |= ((other & 0x1) << 9);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_1::InnerField_WIM_9::InnerField_WIM_9( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_1::InnerField_WIM_9::~InnerField_WIM_9(){

}
InnerField & leon3_acclt_trap::Reg32_1::InnerField_WIM_8::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xfffffeffL;
    this->value |= ((other & 0x1) << 8);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_1::InnerField_WIM_8::InnerField_WIM_8( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_1::InnerField_WIM_8::~InnerField_WIM_8(){

}
InnerField & leon3_acclt_trap::Reg32_1::InnerField_WIM_1::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xfffffffdL;
    this->value |= ((other & 0x1) << 1);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_1::InnerField_WIM_1::InnerField_WIM_1( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_1::InnerField_WIM_1::~InnerField_WIM_1(){

}
InnerField & leon3_acclt_trap::Reg32_1::InnerField_WIM_0::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xfffffffeL;
    this->value |= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_1::InnerField_WIM_0::InnerField_WIM_0( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_1::InnerField_WIM_0::~InnerField_WIM_0(){

}
InnerField & leon3_acclt_trap::Reg32_1::InnerField_WIM_3::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xfffffff7L;
    this->value |= ((other & 0x1) << 3);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_1::InnerField_WIM_3::InnerField_WIM_3( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_1::InnerField_WIM_3::~InnerField_WIM_3(){

}
InnerField & leon3_acclt_trap::Reg32_1::InnerField_WIM_2::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xfffffffbL;
    this->value |= ((other & 0x1) << 2);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_1::InnerField_WIM_2::InnerField_WIM_2( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_1::InnerField_WIM_2::~InnerField_WIM_2(){

}
InnerField & leon3_acclt_trap::Reg32_1::InnerField_WIM_5::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xffffffdfL;
    this->value |= ((other & 0x1) << 5);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_1::InnerField_WIM_5::InnerField_WIM_5( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_1::InnerField_WIM_5::~InnerField_WIM_5(){

}
InnerField & leon3_acclt_trap::Reg32_1::InnerField_WIM_4::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xffffffefL;
    this->value |= ((other & 0x1) << 4);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_1::InnerField_WIM_4::InnerField_WIM_4( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_1::InnerField_WIM_4::~InnerField_WIM_4(){

}
InnerField & leon3_acclt_trap::Reg32_1::InnerField_WIM_7::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xffffff7fL;
    this->value |= ((other & 0x1) << 7);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_1::InnerField_WIM_7::InnerField_WIM_7( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_1::InnerField_WIM_7::~InnerField_WIM_7(){

}
InnerField & leon3_acclt_trap::Reg32_1::InnerField_WIM_6::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xffffffbfL;
    this->value |= ((other & 0x1) << 6);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_1::InnerField_WIM_6::InnerField_WIM_6( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_1::InnerField_WIM_6::~InnerField_WIM_6(){

}
InnerField & leon3_acclt_trap::Reg32_1::InnerField_WIM_11::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xfffff7ffL;
    this->value |= ((other & 0x1) << 11);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_1::InnerField_WIM_11::InnerField_WIM_11( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_1::InnerField_WIM_11::~InnerField_WIM_11(){

}
InnerField & leon3_acclt_trap::Reg32_1::InnerField_WIM_10::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xfffffbffL;
    this->value |= ((other & 0x1) << 10);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_1::InnerField_WIM_10::InnerField_WIM_10( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_1::InnerField_WIM_10::~InnerField_WIM_10(){

}
InnerField & leon3_acclt_trap::Reg32_1::InnerField_WIM_13::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xffffdfffL;
    this->value |= ((other & 0x1) << 13);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_1::InnerField_WIM_13::InnerField_WIM_13( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_1::InnerField_WIM_13::~InnerField_WIM_13(){

}
InnerField & leon3_acclt_trap::Reg32_1::InnerField_WIM_12::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xffffefffL;
    this->value |= ((other & 0x1) << 12);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_1::InnerField_WIM_12::InnerField_WIM_12( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_1::InnerField_WIM_12::~InnerField_WIM_12(){

}
InnerField & leon3_acclt_trap::Reg32_1::InnerField_WIM_15::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xffff7fffL;
    this->value |= ((other & 0x1) << 15);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_1::InnerField_WIM_15::InnerField_WIM_15( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_1::InnerField_WIM_15::~InnerField_WIM_15(){

}
InnerField & leon3_acclt_trap::Reg32_1::InnerField_WIM_14::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xffffbfffL;
    this->value |= ((other & 0x1) << 14);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_1::InnerField_WIM_14::InnerField_WIM_14( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_1::InnerField_WIM_14::~InnerField_WIM_14(){

}
InnerField & leon3_acclt_trap::Reg32_1::InnerField_WIM_17::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xfffdffffL;
    this->value |= ((other & 0x1) << 17);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_1::InnerField_WIM_17::InnerField_WIM_17( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_1::InnerField_WIM_17::~InnerField_WIM_17(){

}
InnerField & leon3_acclt_trap::Reg32_1::InnerField_WIM_16::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xfffeffffL;
    this->value |= ((other & 0x1) << 16);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_1::InnerField_WIM_16::InnerField_WIM_16( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_1::InnerField_WIM_16::~InnerField_WIM_16(){

}
InnerField & leon3_acclt_trap::Reg32_1::InnerField_WIM_19::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xfff7ffffL;
    this->value |= ((other & 0x1) << 19);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_1::InnerField_WIM_19::InnerField_WIM_19( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_1::InnerField_WIM_19::~InnerField_WIM_19(){

}
InnerField & leon3_acclt_trap::Reg32_1::InnerField_WIM_18::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xfffbffffL;
    this->value |= ((other & 0x1) << 18);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_1::InnerField_WIM_18::InnerField_WIM_18( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_1::InnerField_WIM_18::~InnerField_WIM_18(){

}
InnerField & leon3_acclt_trap::Reg32_1::InnerField_WIM_31::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0x7fffffff;
    this->value |= ((other & 0x1) << 31);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_1::InnerField_WIM_31::InnerField_WIM_31( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_1::InnerField_WIM_31::~InnerField_WIM_31(){

}
InnerField & leon3_acclt_trap::Reg32_1::InnerField_WIM_30::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xbfffffffL;
    this->value |= ((other & 0x1) << 30);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_1::InnerField_WIM_30::InnerField_WIM_30( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_1::InnerField_WIM_30::~InnerField_WIM_30(){

}
InnerField & leon3_acclt_trap::Reg32_1::InnerField_Empty::operator =( const unsigned \
    int & other ) throw(){
    return *this;
}

leon3_acclt_trap::Reg32_1::InnerField_Empty::InnerField_Empty(){

}

leon3_acclt_trap::Reg32_1::InnerField_Empty::~InnerField_Empty(){

}
void leon3_acclt_trap::Reg32_1::forceValue( const unsigned int & value ) throw(){
    this->value = value;
}

void leon3_acclt_trap::Reg32_1::immediateWrite( const unsigned int & value ) throw(){
    this->value = value;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
}

unsigned int leon3_acclt_trap::Reg32_1::readNewValue() throw(){
    return this->value;
}

unsigned int leon3_acclt_trap::Reg32_1::operator ~() throw(){
    return ~(this->value);
}

Reg32_1 & leon3_acclt_trap::Reg32_1::operator =( const unsigned int & other ) throw(){
    this->value = other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_1 & leon3_acclt_trap::Reg32_1::operator +=( const unsigned int & other ) throw(){
    this->value += other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_1 & leon3_acclt_trap::Reg32_1::operator -=( const unsigned int & other ) throw(){
    this->value -= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_1 & leon3_acclt_trap::Reg32_1::operator *=( const unsigned int & other ) throw(){
    this->value *= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_1 & leon3_acclt_trap::Reg32_1::operator /=( const unsigned int & other ) throw(){
    this->value /= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_1 & leon3_acclt_trap::Reg32_1::operator |=( const unsigned int & other ) throw(){
    this->value |= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_1 & leon3_acclt_trap::Reg32_1::operator &=( const unsigned int & other ) throw(){
    this->value &= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_1 & leon3_acclt_trap::Reg32_1::operator ^=( const unsigned int & other ) throw(){
    this->value ^= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_1 & leon3_acclt_trap::Reg32_1::operator <<=( const unsigned int & other ) throw(){
    this->value <<= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_1 & leon3_acclt_trap::Reg32_1::operator >>=( const unsigned int & other ) throw(){
    this->value >>= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

unsigned int leon3_acclt_trap::Reg32_1::operator +( const Reg32_1 & other ) const \
    throw(){
    return (this->value + other.value);
}

unsigned int leon3_acclt_trap::Reg32_1::operator -( const Reg32_1 & other ) const \
    throw(){
    return (this->value - other.value);
}

unsigned int leon3_acclt_trap::Reg32_1::operator *( const Reg32_1 & other ) const \
    throw(){
    return (this->value * other.value);
}

unsigned int leon3_acclt_trap::Reg32_1::operator /( const Reg32_1 & other ) const \
    throw(){
    return (this->value / other.value);
}

unsigned int leon3_acclt_trap::Reg32_1::operator |( const Reg32_1 & other ) const \
    throw(){
    return (this->value | other.value);
}

unsigned int leon3_acclt_trap::Reg32_1::operator &( const Reg32_1 & other ) const \
    throw(){
    return (this->value & other.value);
}

unsigned int leon3_acclt_trap::Reg32_1::operator ^( const Reg32_1 & other ) const \
    throw(){
    return (this->value ^ other.value);
}

unsigned int leon3_acclt_trap::Reg32_1::operator <<( const Reg32_1 & other ) const \
    throw(){
    return (this->value << other.value);
}

unsigned int leon3_acclt_trap::Reg32_1::operator >>( const Reg32_1 & other ) const \
    throw(){
    return (this->value >> other.value);
}

bool leon3_acclt_trap::Reg32_1::operator <( const Reg32_1 & other ) const throw(){
    return (this->value < other.value);
}

bool leon3_acclt_trap::Reg32_1::operator >( const Reg32_1 & other ) const throw(){
    return (this->value > other.value);
}

bool leon3_acclt_trap::Reg32_1::operator <=( const Reg32_1 & other ) const throw(){
    return (this->value <= other.value);
}

bool leon3_acclt_trap::Reg32_1::operator >=( const Reg32_1 & other ) const throw(){
    return (this->value >= other.value);
}

bool leon3_acclt_trap::Reg32_1::operator ==( const Reg32_1 & other ) const throw(){
    return (this->value == other.value);
}

bool leon3_acclt_trap::Reg32_1::operator !=( const Reg32_1 & other ) const throw(){
    return (this->value != other.value);
}

Reg32_1 & leon3_acclt_trap::Reg32_1::operator =( const Reg32_1 & other ) throw(){
    this->value = other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_1 & leon3_acclt_trap::Reg32_1::operator +=( const Reg32_1 & other ) throw(){
    this->value += other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_1 & leon3_acclt_trap::Reg32_1::operator -=( const Reg32_1 & other ) throw(){
    this->value -= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_1 & leon3_acclt_trap::Reg32_1::operator *=( const Reg32_1 & other ) throw(){
    this->value *= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_1 & leon3_acclt_trap::Reg32_1::operator /=( const Reg32_1 & other ) throw(){
    this->value /= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_1 & leon3_acclt_trap::Reg32_1::operator |=( const Reg32_1 & other ) throw(){
    this->value |= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_1 & leon3_acclt_trap::Reg32_1::operator &=( const Reg32_1 & other ) throw(){
    this->value &= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_1 & leon3_acclt_trap::Reg32_1::operator ^=( const Reg32_1 & other ) throw(){
    this->value ^= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_1 & leon3_acclt_trap::Reg32_1::operator <<=( const Reg32_1 & other ) throw(){
    this->value <<= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_1 & leon3_acclt_trap::Reg32_1::operator >>=( const Reg32_1 & other ) throw(){
    this->value >>= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

unsigned int leon3_acclt_trap::Reg32_1::operator +( const Register & other ) const \
    throw(){
    return (this->value + other);
}

unsigned int leon3_acclt_trap::Reg32_1::operator -( const Register & other ) const \
    throw(){
    return (this->value - other);
}

unsigned int leon3_acclt_trap::Reg32_1::operator *( const Register & other ) const \
    throw(){
    return (this->value * other);
}

unsigned int leon3_acclt_trap::Reg32_1::operator /( const Register & other ) const \
    throw(){
    return (this->value / other);
}

unsigned int leon3_acclt_trap::Reg32_1::operator |( const Register & other ) const \
    throw(){
    return (this->value | other);
}

unsigned int leon3_acclt_trap::Reg32_1::operator &( const Register & other ) const \
    throw(){
    return (this->value & other);
}

unsigned int leon3_acclt_trap::Reg32_1::operator ^( const Register & other ) const \
    throw(){
    return (this->value ^ other);
}

unsigned int leon3_acclt_trap::Reg32_1::operator <<( const Register & other ) const \
    throw(){
    return (this->value << other);
}

unsigned int leon3_acclt_trap::Reg32_1::operator >>( const Register & other ) const \
    throw(){
    return (this->value >> other);
}

bool leon3_acclt_trap::Reg32_1::operator <( const Register & other ) const throw(){
    return (this->value < other);
}

bool leon3_acclt_trap::Reg32_1::operator >( const Register & other ) const throw(){
    return (this->value > other);
}

bool leon3_acclt_trap::Reg32_1::operator <=( const Register & other ) const throw(){
    return (this->value <= other);
}

bool leon3_acclt_trap::Reg32_1::operator >=( const Register & other ) const throw(){
    return (this->value >= other);
}

bool leon3_acclt_trap::Reg32_1::operator ==( const Register & other ) const throw(){
    return (this->value == other);
}

bool leon3_acclt_trap::Reg32_1::operator !=( const Register & other ) const throw(){
    return (this->value != other);
}

Reg32_1 & leon3_acclt_trap::Reg32_1::operator =( const Register & other ) throw(){
    this->value = other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_1 & leon3_acclt_trap::Reg32_1::operator +=( const Register & other ) throw(){
    this->value += other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_1 & leon3_acclt_trap::Reg32_1::operator -=( const Register & other ) throw(){
    this->value -= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_1 & leon3_acclt_trap::Reg32_1::operator *=( const Register & other ) throw(){
    this->value *= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_1 & leon3_acclt_trap::Reg32_1::operator /=( const Register & other ) throw(){
    this->value /= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_1 & leon3_acclt_trap::Reg32_1::operator |=( const Register & other ) throw(){
    this->value |= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_1 & leon3_acclt_trap::Reg32_1::operator &=( const Register & other ) throw(){
    this->value &= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_1 & leon3_acclt_trap::Reg32_1::operator ^=( const Register & other ) throw(){
    this->value ^= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_1 & leon3_acclt_trap::Reg32_1::operator <<=( const Register & other ) throw(){
    this->value <<= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_1 & leon3_acclt_trap::Reg32_1::operator >>=( const Register & other ) throw(){
    this->value >>= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

std::ostream & leon3_acclt_trap::Reg32_1::operator <<( std::ostream & stream ) const \
    throw(){
    stream << std::hex << std::showbase << this->value << std::dec;
    return stream;
}

leon3_acclt_trap::Reg32_1::Reg32_1() : field_WIM_28(this->value, this->timeStamp, \
    this->hasToPropagate), field_WIM_29(this->value, this->timeStamp, this->hasToPropagate), \
    field_WIM_24(this->value, this->timeStamp, this->hasToPropagate), field_WIM_25(this->value, \
    this->timeStamp, this->hasToPropagate), field_WIM_26(this->value, this->timeStamp, \
    this->hasToPropagate), field_WIM_27(this->value, this->timeStamp, this->hasToPropagate), \
    field_WIM_20(this->value, this->timeStamp, this->hasToPropagate), field_WIM_21(this->value, \
    this->timeStamp, this->hasToPropagate), field_WIM_22(this->value, this->timeStamp, \
    this->hasToPropagate), field_WIM_23(this->value, this->timeStamp, this->hasToPropagate), \
    field_WIM_9(this->value, this->timeStamp, this->hasToPropagate), field_WIM_8(this->value, \
    this->timeStamp, this->hasToPropagate), field_WIM_1(this->value, this->timeStamp, \
    this->hasToPropagate), field_WIM_0(this->value, this->timeStamp, this->hasToPropagate), \
    field_WIM_3(this->value, this->timeStamp, this->hasToPropagate), field_WIM_2(this->value, \
    this->timeStamp, this->hasToPropagate), field_WIM_5(this->value, this->timeStamp, \
    this->hasToPropagate), field_WIM_4(this->value, this->timeStamp, this->hasToPropagate), \
    field_WIM_7(this->value, this->timeStamp, this->hasToPropagate), field_WIM_6(this->value, \
    this->timeStamp, this->hasToPropagate), field_WIM_11(this->value, this->timeStamp, \
    this->hasToPropagate), field_WIM_10(this->value, this->timeStamp, this->hasToPropagate), \
    field_WIM_13(this->value, this->timeStamp, this->hasToPropagate), field_WIM_12(this->value, \
    this->timeStamp, this->hasToPropagate), field_WIM_15(this->value, this->timeStamp, \
    this->hasToPropagate), field_WIM_14(this->value, this->timeStamp, this->hasToPropagate), \
    field_WIM_17(this->value, this->timeStamp, this->hasToPropagate), field_WIM_16(this->value, \
    this->timeStamp, this->hasToPropagate), field_WIM_19(this->value, this->timeStamp, \
    this->hasToPropagate), field_WIM_18(this->value, this->timeStamp, this->hasToPropagate), \
    field_WIM_31(this->value, this->timeStamp, this->hasToPropagate), field_WIM_30(this->value, \
    this->timeStamp, this->hasToPropagate){
    this->value = 0;
}

InnerField & leon3_acclt_trap::Reg32_2::InnerField_TBA::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xfff;
    this->value |= ((other & 0xfffff) << 12);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_2::InnerField_TBA::InnerField_TBA( unsigned int & value, \
    sc_time & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), \
    hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_2::InnerField_TBA::~InnerField_TBA(){

}
InnerField & leon3_acclt_trap::Reg32_2::InnerField_TT::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xfffff00fL;
    this->value |= ((other & 0xff) << 4);
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

leon3_acclt_trap::Reg32_2::InnerField_TT::InnerField_TT( unsigned int & value, sc_time \
    & timeStamp, bool * & hasToPropagate ) : value(value), timeStamp(timeStamp), hasToPropagate(hasToPropagate){

}

leon3_acclt_trap::Reg32_2::InnerField_TT::~InnerField_TT(){

}
InnerField & leon3_acclt_trap::Reg32_2::InnerField_Empty::operator =( const unsigned \
    int & other ) throw(){
    return *this;
}

leon3_acclt_trap::Reg32_2::InnerField_Empty::InnerField_Empty(){

}

leon3_acclt_trap::Reg32_2::InnerField_Empty::~InnerField_Empty(){

}
void leon3_acclt_trap::Reg32_2::forceValue( const unsigned int & value ) throw(){
    this->value = value;
}

void leon3_acclt_trap::Reg32_2::immediateWrite( const unsigned int & value ) throw(){
    this->value = value;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
}

unsigned int leon3_acclt_trap::Reg32_2::readNewValue() throw(){
    return this->value;
}

unsigned int leon3_acclt_trap::Reg32_2::operator ~() throw(){
    return ~(this->value);
}

Reg32_2 & leon3_acclt_trap::Reg32_2::operator =( const unsigned int & other ) throw(){
    this->value = other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_2 & leon3_acclt_trap::Reg32_2::operator +=( const unsigned int & other ) throw(){
    this->value += other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_2 & leon3_acclt_trap::Reg32_2::operator -=( const unsigned int & other ) throw(){
    this->value -= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_2 & leon3_acclt_trap::Reg32_2::operator *=( const unsigned int & other ) throw(){
    this->value *= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_2 & leon3_acclt_trap::Reg32_2::operator /=( const unsigned int & other ) throw(){
    this->value /= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_2 & leon3_acclt_trap::Reg32_2::operator |=( const unsigned int & other ) throw(){
    this->value |= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_2 & leon3_acclt_trap::Reg32_2::operator &=( const unsigned int & other ) throw(){
    this->value &= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_2 & leon3_acclt_trap::Reg32_2::operator ^=( const unsigned int & other ) throw(){
    this->value ^= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_2 & leon3_acclt_trap::Reg32_2::operator <<=( const unsigned int & other ) throw(){
    this->value <<= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_2 & leon3_acclt_trap::Reg32_2::operator >>=( const unsigned int & other ) throw(){
    this->value >>= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

unsigned int leon3_acclt_trap::Reg32_2::operator +( const Reg32_2 & other ) const \
    throw(){
    return (this->value + other.value);
}

unsigned int leon3_acclt_trap::Reg32_2::operator -( const Reg32_2 & other ) const \
    throw(){
    return (this->value - other.value);
}

unsigned int leon3_acclt_trap::Reg32_2::operator *( const Reg32_2 & other ) const \
    throw(){
    return (this->value * other.value);
}

unsigned int leon3_acclt_trap::Reg32_2::operator /( const Reg32_2 & other ) const \
    throw(){
    return (this->value / other.value);
}

unsigned int leon3_acclt_trap::Reg32_2::operator |( const Reg32_2 & other ) const \
    throw(){
    return (this->value | other.value);
}

unsigned int leon3_acclt_trap::Reg32_2::operator &( const Reg32_2 & other ) const \
    throw(){
    return (this->value & other.value);
}

unsigned int leon3_acclt_trap::Reg32_2::operator ^( const Reg32_2 & other ) const \
    throw(){
    return (this->value ^ other.value);
}

unsigned int leon3_acclt_trap::Reg32_2::operator <<( const Reg32_2 & other ) const \
    throw(){
    return (this->value << other.value);
}

unsigned int leon3_acclt_trap::Reg32_2::operator >>( const Reg32_2 & other ) const \
    throw(){
    return (this->value >> other.value);
}

bool leon3_acclt_trap::Reg32_2::operator <( const Reg32_2 & other ) const throw(){
    return (this->value < other.value);
}

bool leon3_acclt_trap::Reg32_2::operator >( const Reg32_2 & other ) const throw(){
    return (this->value > other.value);
}

bool leon3_acclt_trap::Reg32_2::operator <=( const Reg32_2 & other ) const throw(){
    return (this->value <= other.value);
}

bool leon3_acclt_trap::Reg32_2::operator >=( const Reg32_2 & other ) const throw(){
    return (this->value >= other.value);
}

bool leon3_acclt_trap::Reg32_2::operator ==( const Reg32_2 & other ) const throw(){
    return (this->value == other.value);
}

bool leon3_acclt_trap::Reg32_2::operator !=( const Reg32_2 & other ) const throw(){
    return (this->value != other.value);
}

Reg32_2 & leon3_acclt_trap::Reg32_2::operator =( const Reg32_2 & other ) throw(){
    this->value = other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_2 & leon3_acclt_trap::Reg32_2::operator +=( const Reg32_2 & other ) throw(){
    this->value += other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_2 & leon3_acclt_trap::Reg32_2::operator -=( const Reg32_2 & other ) throw(){
    this->value -= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_2 & leon3_acclt_trap::Reg32_2::operator *=( const Reg32_2 & other ) throw(){
    this->value *= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_2 & leon3_acclt_trap::Reg32_2::operator /=( const Reg32_2 & other ) throw(){
    this->value /= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_2 & leon3_acclt_trap::Reg32_2::operator |=( const Reg32_2 & other ) throw(){
    this->value |= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_2 & leon3_acclt_trap::Reg32_2::operator &=( const Reg32_2 & other ) throw(){
    this->value &= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_2 & leon3_acclt_trap::Reg32_2::operator ^=( const Reg32_2 & other ) throw(){
    this->value ^= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_2 & leon3_acclt_trap::Reg32_2::operator <<=( const Reg32_2 & other ) throw(){
    this->value <<= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_2 & leon3_acclt_trap::Reg32_2::operator >>=( const Reg32_2 & other ) throw(){
    this->value >>= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

unsigned int leon3_acclt_trap::Reg32_2::operator +( const Register & other ) const \
    throw(){
    return (this->value + other);
}

unsigned int leon3_acclt_trap::Reg32_2::operator -( const Register & other ) const \
    throw(){
    return (this->value - other);
}

unsigned int leon3_acclt_trap::Reg32_2::operator *( const Register & other ) const \
    throw(){
    return (this->value * other);
}

unsigned int leon3_acclt_trap::Reg32_2::operator /( const Register & other ) const \
    throw(){
    return (this->value / other);
}

unsigned int leon3_acclt_trap::Reg32_2::operator |( const Register & other ) const \
    throw(){
    return (this->value | other);
}

unsigned int leon3_acclt_trap::Reg32_2::operator &( const Register & other ) const \
    throw(){
    return (this->value & other);
}

unsigned int leon3_acclt_trap::Reg32_2::operator ^( const Register & other ) const \
    throw(){
    return (this->value ^ other);
}

unsigned int leon3_acclt_trap::Reg32_2::operator <<( const Register & other ) const \
    throw(){
    return (this->value << other);
}

unsigned int leon3_acclt_trap::Reg32_2::operator >>( const Register & other ) const \
    throw(){
    return (this->value >> other);
}

bool leon3_acclt_trap::Reg32_2::operator <( const Register & other ) const throw(){
    return (this->value < other);
}

bool leon3_acclt_trap::Reg32_2::operator >( const Register & other ) const throw(){
    return (this->value > other);
}

bool leon3_acclt_trap::Reg32_2::operator <=( const Register & other ) const throw(){
    return (this->value <= other);
}

bool leon3_acclt_trap::Reg32_2::operator >=( const Register & other ) const throw(){
    return (this->value >= other);
}

bool leon3_acclt_trap::Reg32_2::operator ==( const Register & other ) const throw(){
    return (this->value == other);
}

bool leon3_acclt_trap::Reg32_2::operator !=( const Register & other ) const throw(){
    return (this->value != other);
}

Reg32_2 & leon3_acclt_trap::Reg32_2::operator =( const Register & other ) throw(){
    this->value = other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_2 & leon3_acclt_trap::Reg32_2::operator +=( const Register & other ) throw(){
    this->value += other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_2 & leon3_acclt_trap::Reg32_2::operator -=( const Register & other ) throw(){
    this->value -= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_2 & leon3_acclt_trap::Reg32_2::operator *=( const Register & other ) throw(){
    this->value *= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_2 & leon3_acclt_trap::Reg32_2::operator /=( const Register & other ) throw(){
    this->value /= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_2 & leon3_acclt_trap::Reg32_2::operator |=( const Register & other ) throw(){
    this->value |= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_2 & leon3_acclt_trap::Reg32_2::operator &=( const Register & other ) throw(){
    this->value &= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_2 & leon3_acclt_trap::Reg32_2::operator ^=( const Register & other ) throw(){
    this->value ^= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_2 & leon3_acclt_trap::Reg32_2::operator <<=( const Register & other ) throw(){
    this->value <<= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_2 & leon3_acclt_trap::Reg32_2::operator >>=( const Register & other ) throw(){
    this->value >>= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

std::ostream & leon3_acclt_trap::Reg32_2::operator <<( std::ostream & stream ) const \
    throw(){
    stream << std::hex << std::showbase << this->value << std::dec;
    return stream;
}

leon3_acclt_trap::Reg32_2::Reg32_2() : field_TBA(this->value, this->timeStamp, this->hasToPropagate), \
    field_TT(this->value, this->timeStamp, this->hasToPropagate){
    this->value = 0;
}

InnerField & leon3_acclt_trap::Reg32_3::InnerField_Empty::operator =( const unsigned \
    int & other ) throw(){
    return *this;
}

leon3_acclt_trap::Reg32_3::InnerField_Empty::InnerField_Empty(){

}

leon3_acclt_trap::Reg32_3::InnerField_Empty::~InnerField_Empty(){

}
void leon3_acclt_trap::Reg32_3::forceValue( const unsigned int & value ) throw(){
    this->value = value;
}

void leon3_acclt_trap::Reg32_3::immediateWrite( const unsigned int & value ) throw(){
    this->value = value;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
}

unsigned int leon3_acclt_trap::Reg32_3::readNewValue() throw(){
    return this->value;
}

unsigned int leon3_acclt_trap::Reg32_3::operator ~() throw(){
    return ~(this->value);
}

Reg32_3 & leon3_acclt_trap::Reg32_3::operator =( const unsigned int & other ) throw(){
    this->value = other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_3 & leon3_acclt_trap::Reg32_3::operator +=( const unsigned int & other ) throw(){
    this->value += other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_3 & leon3_acclt_trap::Reg32_3::operator -=( const unsigned int & other ) throw(){
    this->value -= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_3 & leon3_acclt_trap::Reg32_3::operator *=( const unsigned int & other ) throw(){
    this->value *= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_3 & leon3_acclt_trap::Reg32_3::operator /=( const unsigned int & other ) throw(){
    this->value /= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_3 & leon3_acclt_trap::Reg32_3::operator |=( const unsigned int & other ) throw(){
    this->value |= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_3 & leon3_acclt_trap::Reg32_3::operator &=( const unsigned int & other ) throw(){
    this->value &= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_3 & leon3_acclt_trap::Reg32_3::operator ^=( const unsigned int & other ) throw(){
    this->value ^= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_3 & leon3_acclt_trap::Reg32_3::operator <<=( const unsigned int & other ) throw(){
    this->value <<= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_3 & leon3_acclt_trap::Reg32_3::operator >>=( const unsigned int & other ) throw(){
    this->value >>= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

unsigned int leon3_acclt_trap::Reg32_3::operator +( const Reg32_3 & other ) const \
    throw(){
    return (this->value + other.value);
}

unsigned int leon3_acclt_trap::Reg32_3::operator -( const Reg32_3 & other ) const \
    throw(){
    return (this->value - other.value);
}

unsigned int leon3_acclt_trap::Reg32_3::operator *( const Reg32_3 & other ) const \
    throw(){
    return (this->value * other.value);
}

unsigned int leon3_acclt_trap::Reg32_3::operator /( const Reg32_3 & other ) const \
    throw(){
    return (this->value / other.value);
}

unsigned int leon3_acclt_trap::Reg32_3::operator |( const Reg32_3 & other ) const \
    throw(){
    return (this->value | other.value);
}

unsigned int leon3_acclt_trap::Reg32_3::operator &( const Reg32_3 & other ) const \
    throw(){
    return (this->value & other.value);
}

unsigned int leon3_acclt_trap::Reg32_3::operator ^( const Reg32_3 & other ) const \
    throw(){
    return (this->value ^ other.value);
}

unsigned int leon3_acclt_trap::Reg32_3::operator <<( const Reg32_3 & other ) const \
    throw(){
    return (this->value << other.value);
}

unsigned int leon3_acclt_trap::Reg32_3::operator >>( const Reg32_3 & other ) const \
    throw(){
    return (this->value >> other.value);
}

bool leon3_acclt_trap::Reg32_3::operator <( const Reg32_3 & other ) const throw(){
    return (this->value < other.value);
}

bool leon3_acclt_trap::Reg32_3::operator >( const Reg32_3 & other ) const throw(){
    return (this->value > other.value);
}

bool leon3_acclt_trap::Reg32_3::operator <=( const Reg32_3 & other ) const throw(){
    return (this->value <= other.value);
}

bool leon3_acclt_trap::Reg32_3::operator >=( const Reg32_3 & other ) const throw(){
    return (this->value >= other.value);
}

bool leon3_acclt_trap::Reg32_3::operator ==( const Reg32_3 & other ) const throw(){
    return (this->value == other.value);
}

bool leon3_acclt_trap::Reg32_3::operator !=( const Reg32_3 & other ) const throw(){
    return (this->value != other.value);
}

Reg32_3 & leon3_acclt_trap::Reg32_3::operator =( const Reg32_3 & other ) throw(){
    this->value = other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_3 & leon3_acclt_trap::Reg32_3::operator +=( const Reg32_3 & other ) throw(){
    this->value += other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_3 & leon3_acclt_trap::Reg32_3::operator -=( const Reg32_3 & other ) throw(){
    this->value -= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_3 & leon3_acclt_trap::Reg32_3::operator *=( const Reg32_3 & other ) throw(){
    this->value *= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_3 & leon3_acclt_trap::Reg32_3::operator /=( const Reg32_3 & other ) throw(){
    this->value /= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_3 & leon3_acclt_trap::Reg32_3::operator |=( const Reg32_3 & other ) throw(){
    this->value |= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_3 & leon3_acclt_trap::Reg32_3::operator &=( const Reg32_3 & other ) throw(){
    this->value &= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_3 & leon3_acclt_trap::Reg32_3::operator ^=( const Reg32_3 & other ) throw(){
    this->value ^= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_3 & leon3_acclt_trap::Reg32_3::operator <<=( const Reg32_3 & other ) throw(){
    this->value <<= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_3 & leon3_acclt_trap::Reg32_3::operator >>=( const Reg32_3 & other ) throw(){
    this->value >>= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

unsigned int leon3_acclt_trap::Reg32_3::operator +( const Register & other ) const \
    throw(){
    return (this->value + other);
}

unsigned int leon3_acclt_trap::Reg32_3::operator -( const Register & other ) const \
    throw(){
    return (this->value - other);
}

unsigned int leon3_acclt_trap::Reg32_3::operator *( const Register & other ) const \
    throw(){
    return (this->value * other);
}

unsigned int leon3_acclt_trap::Reg32_3::operator /( const Register & other ) const \
    throw(){
    return (this->value / other);
}

unsigned int leon3_acclt_trap::Reg32_3::operator |( const Register & other ) const \
    throw(){
    return (this->value | other);
}

unsigned int leon3_acclt_trap::Reg32_3::operator &( const Register & other ) const \
    throw(){
    return (this->value & other);
}

unsigned int leon3_acclt_trap::Reg32_3::operator ^( const Register & other ) const \
    throw(){
    return (this->value ^ other);
}

unsigned int leon3_acclt_trap::Reg32_3::operator <<( const Register & other ) const \
    throw(){
    return (this->value << other);
}

unsigned int leon3_acclt_trap::Reg32_3::operator >>( const Register & other ) const \
    throw(){
    return (this->value >> other);
}

bool leon3_acclt_trap::Reg32_3::operator <( const Register & other ) const throw(){
    return (this->value < other);
}

bool leon3_acclt_trap::Reg32_3::operator >( const Register & other ) const throw(){
    return (this->value > other);
}

bool leon3_acclt_trap::Reg32_3::operator <=( const Register & other ) const throw(){
    return (this->value <= other);
}

bool leon3_acclt_trap::Reg32_3::operator >=( const Register & other ) const throw(){
    return (this->value >= other);
}

bool leon3_acclt_trap::Reg32_3::operator ==( const Register & other ) const throw(){
    return (this->value == other);
}

bool leon3_acclt_trap::Reg32_3::operator !=( const Register & other ) const throw(){
    return (this->value != other);
}

Reg32_3 & leon3_acclt_trap::Reg32_3::operator =( const Register & other ) throw(){
    this->value = other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_3 & leon3_acclt_trap::Reg32_3::operator +=( const Register & other ) throw(){
    this->value += other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_3 & leon3_acclt_trap::Reg32_3::operator -=( const Register & other ) throw(){
    this->value -= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_3 & leon3_acclt_trap::Reg32_3::operator *=( const Register & other ) throw(){
    this->value *= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_3 & leon3_acclt_trap::Reg32_3::operator /=( const Register & other ) throw(){
    this->value /= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_3 & leon3_acclt_trap::Reg32_3::operator |=( const Register & other ) throw(){
    this->value |= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_3 & leon3_acclt_trap::Reg32_3::operator &=( const Register & other ) throw(){
    this->value &= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_3 & leon3_acclt_trap::Reg32_3::operator ^=( const Register & other ) throw(){
    this->value ^= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_3 & leon3_acclt_trap::Reg32_3::operator <<=( const Register & other ) throw(){
    this->value <<= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

Reg32_3 & leon3_acclt_trap::Reg32_3::operator >>=( const Register & other ) throw(){
    this->value >>= other;
    *(this->hasToPropagate) = true;
    this->timeStamp = sc_time_stamp();
    return *this;
}

std::ostream & leon3_acclt_trap::Reg32_3::operator <<( std::ostream & stream ) const \
    throw(){
    stream << std::hex << std::showbase << this->value << std::dec;
    return stream;
}

leon3_acclt_trap::Reg32_3::Reg32_3(){
    this->value = 0;
}

InnerField & leon3_acclt_trap::Reg32_3_const_0::InnerField_Empty::operator =( const \
    unsigned int & other ) throw(){
    return *this;
}

leon3_acclt_trap::Reg32_3_const_0::InnerField_Empty::InnerField_Empty(){

}

leon3_acclt_trap::Reg32_3_const_0::InnerField_Empty::~InnerField_Empty(){

}
bool leon3_acclt_trap::Reg32_3_const_0::isLocked() throw(){
    return false;
}

void leon3_acclt_trap::Reg32_3_const_0::forceValue( const unsigned int & value ) throw(){

}

void leon3_acclt_trap::Reg32_3_const_0::immediateWrite( const unsigned int & value \
    ) throw(){

}

unsigned int leon3_acclt_trap::Reg32_3_const_0::readNewValue() throw(){
    return this->value;
}

unsigned int leon3_acclt_trap::Reg32_3_const_0::operator ~() throw(){
    return ~(0);
}

Reg32_3_const_0 & leon3_acclt_trap::Reg32_3_const_0::operator =( const unsigned int \
    & other ) throw(){
    return *this;
}

Reg32_3_const_0 & leon3_acclt_trap::Reg32_3_const_0::operator +=( const unsigned \
    int & other ) throw(){
    return *this;
}

Reg32_3_const_0 & leon3_acclt_trap::Reg32_3_const_0::operator -=( const unsigned \
    int & other ) throw(){
    return *this;
}

Reg32_3_const_0 & leon3_acclt_trap::Reg32_3_const_0::operator *=( const unsigned \
    int & other ) throw(){
    return *this;
}

Reg32_3_const_0 & leon3_acclt_trap::Reg32_3_const_0::operator /=( const unsigned \
    int & other ) throw(){
    return *this;
}

Reg32_3_const_0 & leon3_acclt_trap::Reg32_3_const_0::operator |=( const unsigned \
    int & other ) throw(){
    return *this;
}

Reg32_3_const_0 & leon3_acclt_trap::Reg32_3_const_0::operator &=( const unsigned \
    int & other ) throw(){
    return *this;
}

Reg32_3_const_0 & leon3_acclt_trap::Reg32_3_const_0::operator ^=( const unsigned \
    int & other ) throw(){
    return *this;
}

Reg32_3_const_0 & leon3_acclt_trap::Reg32_3_const_0::operator <<=( const unsigned \
    int & other ) throw(){
    return *this;
}

Reg32_3_const_0 & leon3_acclt_trap::Reg32_3_const_0::operator >>=( const unsigned \
    int & other ) throw(){
    return *this;
}

unsigned int leon3_acclt_trap::Reg32_3_const_0::operator +( const Reg32_3_const_0 \
    & other ) const throw(){
    return (0 + other.value);
}

unsigned int leon3_acclt_trap::Reg32_3_const_0::operator -( const Reg32_3_const_0 \
    & other ) const throw(){
    return (0 - other.value);
}

unsigned int leon3_acclt_trap::Reg32_3_const_0::operator *( const Reg32_3_const_0 \
    & other ) const throw(){
    return (0 * other.value);
}

unsigned int leon3_acclt_trap::Reg32_3_const_0::operator /( const Reg32_3_const_0 \
    & other ) const throw(){
    return (0 / other.value);
}

unsigned int leon3_acclt_trap::Reg32_3_const_0::operator |( const Reg32_3_const_0 \
    & other ) const throw(){
    return (0 | other.value);
}

unsigned int leon3_acclt_trap::Reg32_3_const_0::operator &( const Reg32_3_const_0 \
    & other ) const throw(){
    return (0 & other.value);
}

unsigned int leon3_acclt_trap::Reg32_3_const_0::operator ^( const Reg32_3_const_0 \
    & other ) const throw(){
    return (0 ^ other.value);
}

unsigned int leon3_acclt_trap::Reg32_3_const_0::operator <<( const Reg32_3_const_0 \
    & other ) const throw(){
    return (0 << other.value);
}

unsigned int leon3_acclt_trap::Reg32_3_const_0::operator >>( const Reg32_3_const_0 \
    & other ) const throw(){
    return (0 >> other.value);
}

bool leon3_acclt_trap::Reg32_3_const_0::operator <( const Reg32_3_const_0 & other \
    ) const throw(){
    return (0 < other.value);
}

bool leon3_acclt_trap::Reg32_3_const_0::operator >( const Reg32_3_const_0 & other \
    ) const throw(){
    return (0 > other.value);
}

bool leon3_acclt_trap::Reg32_3_const_0::operator <=( const Reg32_3_const_0 & other \
    ) const throw(){
    return (0 <= other.value);
}

bool leon3_acclt_trap::Reg32_3_const_0::operator >=( const Reg32_3_const_0 & other \
    ) const throw(){
    return (0 >= other.value);
}

bool leon3_acclt_trap::Reg32_3_const_0::operator ==( const Reg32_3_const_0 & other \
    ) const throw(){
    return (0 == other.value);
}

bool leon3_acclt_trap::Reg32_3_const_0::operator !=( const Reg32_3_const_0 & other \
    ) const throw(){
    return (0 != other.value);
}

Reg32_3_const_0 & leon3_acclt_trap::Reg32_3_const_0::operator =( const Reg32_3_const_0 \
    & other ) throw(){
    return *this;
}

Reg32_3_const_0 & leon3_acclt_trap::Reg32_3_const_0::operator +=( const Reg32_3_const_0 \
    & other ) throw(){
    return *this;
}

Reg32_3_const_0 & leon3_acclt_trap::Reg32_3_const_0::operator -=( const Reg32_3_const_0 \
    & other ) throw(){
    return *this;
}

Reg32_3_const_0 & leon3_acclt_trap::Reg32_3_const_0::operator *=( const Reg32_3_const_0 \
    & other ) throw(){
    return *this;
}

Reg32_3_const_0 & leon3_acclt_trap::Reg32_3_const_0::operator /=( const Reg32_3_const_0 \
    & other ) throw(){
    return *this;
}

Reg32_3_const_0 & leon3_acclt_trap::Reg32_3_const_0::operator |=( const Reg32_3_const_0 \
    & other ) throw(){
    return *this;
}

Reg32_3_const_0 & leon3_acclt_trap::Reg32_3_const_0::operator &=( const Reg32_3_const_0 \
    & other ) throw(){
    return *this;
}

Reg32_3_const_0 & leon3_acclt_trap::Reg32_3_const_0::operator ^=( const Reg32_3_const_0 \
    & other ) throw(){
    return *this;
}

Reg32_3_const_0 & leon3_acclt_trap::Reg32_3_const_0::operator <<=( const Reg32_3_const_0 \
    & other ) throw(){
    return *this;
}

Reg32_3_const_0 & leon3_acclt_trap::Reg32_3_const_0::operator >>=( const Reg32_3_const_0 \
    & other ) throw(){
    return *this;
}

unsigned int leon3_acclt_trap::Reg32_3_const_0::operator +( const Register & other \
    ) const throw(){
    return (0 + other);
}

unsigned int leon3_acclt_trap::Reg32_3_const_0::operator -( const Register & other \
    ) const throw(){
    return (0 - other);
}

unsigned int leon3_acclt_trap::Reg32_3_const_0::operator *( const Register & other \
    ) const throw(){
    return (0 * other);
}

unsigned int leon3_acclt_trap::Reg32_3_const_0::operator /( const Register & other \
    ) const throw(){
    return (0 / other);
}

unsigned int leon3_acclt_trap::Reg32_3_const_0::operator |( const Register & other \
    ) const throw(){
    return (0 | other);
}

unsigned int leon3_acclt_trap::Reg32_3_const_0::operator &( const Register & other \
    ) const throw(){
    return (0 & other);
}

unsigned int leon3_acclt_trap::Reg32_3_const_0::operator ^( const Register & other \
    ) const throw(){
    return (0 ^ other);
}

unsigned int leon3_acclt_trap::Reg32_3_const_0::operator <<( const Register & other \
    ) const throw(){
    return (0 << other);
}

unsigned int leon3_acclt_trap::Reg32_3_const_0::operator >>( const Register & other \
    ) const throw(){
    return (0 >> other);
}

bool leon3_acclt_trap::Reg32_3_const_0::operator <( const Register & other ) const \
    throw(){
    return (0 < other);
}

bool leon3_acclt_trap::Reg32_3_const_0::operator >( const Register & other ) const \
    throw(){
    return (0 > other);
}

bool leon3_acclt_trap::Reg32_3_const_0::operator <=( const Register & other ) const \
    throw(){
    return (0 <= other);
}

bool leon3_acclt_trap::Reg32_3_const_0::operator >=( const Register & other ) const \
    throw(){
    return (0 >= other);
}

bool leon3_acclt_trap::Reg32_3_const_0::operator ==( const Register & other ) const \
    throw(){
    return (0 == other);
}

bool leon3_acclt_trap::Reg32_3_const_0::operator !=( const Register & other ) const \
    throw(){
    return (0 != other);
}

Reg32_3_const_0 & leon3_acclt_trap::Reg32_3_const_0::operator =( const Register & \
    other ) throw(){
    return *this;
}

Reg32_3_const_0 & leon3_acclt_trap::Reg32_3_const_0::operator +=( const Register \
    & other ) throw(){
    return *this;
}

Reg32_3_const_0 & leon3_acclt_trap::Reg32_3_const_0::operator -=( const Register \
    & other ) throw(){
    return *this;
}

Reg32_3_const_0 & leon3_acclt_trap::Reg32_3_const_0::operator *=( const Register \
    & other ) throw(){
    return *this;
}

Reg32_3_const_0 & leon3_acclt_trap::Reg32_3_const_0::operator /=( const Register \
    & other ) throw(){
    return *this;
}

Reg32_3_const_0 & leon3_acclt_trap::Reg32_3_const_0::operator |=( const Register \
    & other ) throw(){
    return *this;
}

Reg32_3_const_0 & leon3_acclt_trap::Reg32_3_const_0::operator &=( const Register \
    & other ) throw(){
    return *this;
}

Reg32_3_const_0 & leon3_acclt_trap::Reg32_3_const_0::operator ^=( const Register \
    & other ) throw(){
    return *this;
}

Reg32_3_const_0 & leon3_acclt_trap::Reg32_3_const_0::operator <<=( const Register \
    & other ) throw(){
    return *this;
}

Reg32_3_const_0 & leon3_acclt_trap::Reg32_3_const_0::operator >>=( const Register \
    & other ) throw(){
    return *this;
}

std::ostream & leon3_acclt_trap::Reg32_3_const_0::operator <<( std::ostream & stream \
    ) const throw(){
    stream << std::hex << std::showbase << 0 << std::dec;
    return stream;
}

leon3_acclt_trap::Reg32_3_const_0::Reg32_3_const_0(){
    this->value = 0;
}

void leon3_acclt_trap::RegisterBankClass::setNewRegister( unsigned int numReg, Register \
    * newReg ){
    if(numReg > this->size - 1){
        THROW_EXCEPTION("Register number " << numReg << " is out of register bank boundaries");
    }
    else{
        this->registers[numReg] = newReg;
    }
}

void leon3_acclt_trap::RegisterBankClass::setSize( unsigned int size ) throw(){

    for(unsigned int i = 0; i < this->size; i++){
        if(this->registers[i] != NULL){
            delete this->registers[i];
        }
    }
    if(this->registers != NULL){
        delete [] this->registers;
    }
    this->size = size;
    this->registers = new Register *[this->size];
    for(unsigned int i = 0; i < this->size; i++){
        this->registers[i] = NULL;
    }
}

leon3_acclt_trap::RegisterBankClass::RegisterBankClass( unsigned int size ){
    this->size = size;
    this->registers = new Register *[this->size];
    for(unsigned int i = 0; i < this->size; i++){
        this->registers[i] = NULL;
    }
}

leon3_acclt_trap::RegisterBankClass::RegisterBankClass(){
    this->size = 0;
    this->registers = NULL;
}

leon3_acclt_trap::RegisterBankClass::~RegisterBankClass(){

    for(unsigned int i = 0; i < this->size; i++){
        if(this->registers[i] != NULL){
            delete this->registers[i];
        }
    }
    if(this->registers != NULL){
        delete [] this->registers;
    }
}
void leon3_acclt_trap::PipelineRegister::lock() throw(){
    for(int i = 0; i < 7; i++){
        this->reg_stage[i]->lock();
    }
    this->reg_all->lock();
}

void leon3_acclt_trap::PipelineRegister::unlock() throw(){
    for(int i = 0; i < 7; i++){
        this->reg_stage[i]->unlock();
    }
    this->reg_all->unlock();
}

void leon3_acclt_trap::PipelineRegister::unlock( int wbLatency ) throw(){
    for(int i = 0; i < 7; i++){
        this->reg_stage[i]->unlock();
    }
    this->reg_all->unlock();
}

bool leon3_acclt_trap::PipelineRegister::isLocked() throw(){
    return this->reg_all->isLocked();
}

bool leon3_acclt_trap::PipelineRegister::isLocked( int stageId ) throw(){
    return this->reg_stage[stageId]->isLocked();
}

void leon3_acclt_trap::PipelineRegister::forceValue( const unsigned int & value ) \
    throw(){
    this->reg_all->forceValue(value);
}

void leon3_acclt_trap::PipelineRegister::writeAll( const unsigned int & value ) throw(){
    *(this->reg_all) = value;
    for(int i = 0; i < 7; i++){
        *(this->reg_stage[i]) = value;
    }
}

void leon3_acclt_trap::PipelineRegister::immediateWrite( const unsigned int & value \
    ) throw(){
    this->reg_all->immediateWrite(value);
}

unsigned int leon3_acclt_trap::PipelineRegister::readNewValue() throw(){
    return this->reg_all->readNewValue();
}

void leon3_acclt_trap::PipelineRegister::propagate() throw(){
    bool hasChanges = false;
    if(this->reg_stage[6]->timeStamp > this->reg_all->timeStamp){
        hasChanges = true;
        this->reg_all->timeStamp = this->reg_stage[6]->timeStamp;
        this->reg_all->forceValue(*(this->reg_stage[6]));
    }
    for(int i = 5; i >= 1; i--){
        if(this->reg_stage[i]->timeStamp > this->reg_stage[i + 1]->timeStamp){
            hasChanges = true;
            this->reg_stage[i + 1]->timeStamp = this->reg_stage[i]->timeStamp;
            this->reg_stage[i + 1]->forceValue(*(this->reg_stage[i]));
        }
    }
    if(this->reg_all->timeStamp > this->reg_stage[0]->timeStamp){
        hasChanges = true;
        this->reg_stage[0]->timeStamp = this->reg_all->timeStamp;
        this->reg_stage[0]->forceValue(*(this->reg_all));
    }
    if(this->reg_all->timeStamp > this->reg_stage[1]->timeStamp){
        hasChanges = true;
        this->reg_stage[1]->timeStamp = this->reg_all->timeStamp;
        this->reg_stage[1]->forceValue(*(this->reg_all));
    }
    this->hasToPropagate = hasChanges;
}

void leon3_acclt_trap::PipelineRegister::setRegister( Register * regPtr, int regIndex ){
    if(regIndex < 0){
        if(this->reg_all != NULL){
            THROW_EXCEPTION("Error, trying to initialize main register after the pipeline register \
                initialization has already completed");
        }
        this->reg_all = regPtr;
        this->reg_all->hasToPropagate = &(this->hasToPropagate);
    }
    else{
        if(this->reg_stage[regIndex] != NULL){
            THROW_EXCEPTION("Error, trying to initialize register " << regIndex << " after the \
                pipeline register initialization has already completed");
        }
        this->reg_stage[regIndex] = regPtr;
        this->reg_stage[regIndex]->hasToPropagate = &(this->hasToPropagate);
    }
}

InnerField & leon3_acclt_trap::PipelineRegister::operator []( int bitField ) throw(){
    return (*(this->reg_all))[bitField];
}

unsigned int leon3_acclt_trap::PipelineRegister::operator ~() throw(){
    return ~(*(this->reg_all));
}

unsigned int leon3_acclt_trap::PipelineRegister::operator +( const Register & other \
    ) const throw(){
    return (*(this->reg_all)) + other;
}

unsigned int leon3_acclt_trap::PipelineRegister::operator -( const Register & other \
    ) const throw(){
    return (*(this->reg_all)) - other;
}

unsigned int leon3_acclt_trap::PipelineRegister::operator *( const Register & other \
    ) const throw(){
    return (*(this->reg_all)) * other;
}

unsigned int leon3_acclt_trap::PipelineRegister::operator /( const Register & other \
    ) const throw(){
    return (*(this->reg_all)) / other;
}

unsigned int leon3_acclt_trap::PipelineRegister::operator |( const Register & other \
    ) const throw(){
    return (*(this->reg_all)) | other;
}

unsigned int leon3_acclt_trap::PipelineRegister::operator &( const Register & other \
    ) const throw(){
    return (*(this->reg_all)) & other;
}

unsigned int leon3_acclt_trap::PipelineRegister::operator ^( const Register & other \
    ) const throw(){
    return (*(this->reg_all)) ^ other;
}

unsigned int leon3_acclt_trap::PipelineRegister::operator <<( const Register & other \
    ) const throw(){
    return (*(this->reg_all)) << other;
}

unsigned int leon3_acclt_trap::PipelineRegister::operator >>( const Register & other \
    ) const throw(){
    return (*(this->reg_all)) >> other;
}

bool leon3_acclt_trap::PipelineRegister::operator <( const Register & other ) const \
    throw(){
    return (*(this->reg_all)) < other;
}

bool leon3_acclt_trap::PipelineRegister::operator >( const Register & other ) const \
    throw(){
    return (*(this->reg_all)) > other;
}

bool leon3_acclt_trap::PipelineRegister::operator <=( const Register & other ) const \
    throw(){
    return (*(this->reg_all)) <= other;
}

bool leon3_acclt_trap::PipelineRegister::operator >=( const Register & other ) const \
    throw(){
    return (*(this->reg_all)) >= other;
}

bool leon3_acclt_trap::PipelineRegister::operator ==( const Register & other ) const \
    throw(){
    return (*(this->reg_all)) == other;
}

bool leon3_acclt_trap::PipelineRegister::operator !=( const Register & other ) const \
    throw(){
    return (*(this->reg_all)) != other;
}

Register & leon3_acclt_trap::PipelineRegister::operator =( const unsigned int & other \
    ) throw(){
    return (*(this->reg_all)) = other;
}

Register & leon3_acclt_trap::PipelineRegister::operator +=( const unsigned int & \
    other ) throw(){
    return (*(this->reg_all)) += other;
}

Register & leon3_acclt_trap::PipelineRegister::operator -=( const unsigned int & \
    other ) throw(){
    return (*(this->reg_all)) -= other;
}

Register & leon3_acclt_trap::PipelineRegister::operator *=( const unsigned int & \
    other ) throw(){
    return (*(this->reg_all)) *= other;
}

Register & leon3_acclt_trap::PipelineRegister::operator /=( const unsigned int & \
    other ) throw(){
    return (*(this->reg_all)) /= other;
}

Register & leon3_acclt_trap::PipelineRegister::operator |=( const unsigned int & \
    other ) throw(){
    return (*(this->reg_all)) |= other;
}

Register & leon3_acclt_trap::PipelineRegister::operator &=( const unsigned int & \
    other ) throw(){
    return (*(this->reg_all)) &= other;
}

Register & leon3_acclt_trap::PipelineRegister::operator ^=( const unsigned int & \
    other ) throw(){
    return (*(this->reg_all)) ^= other;
}

Register & leon3_acclt_trap::PipelineRegister::operator <<=( const unsigned int & \
    other ) throw(){
    return (*(this->reg_all)) <<= other;
}

Register & leon3_acclt_trap::PipelineRegister::operator >>=( const unsigned int & \
    other ) throw(){
    return (*(this->reg_all)) >>= other;
}

Register & leon3_acclt_trap::PipelineRegister::operator =( const Register & other \
    ) throw(){
    return (*(this->reg_all)) = other;
}

Register & leon3_acclt_trap::PipelineRegister::operator +=( const Register & other \
    ) throw(){
    return (*(this->reg_all)) += other;
}

Register & leon3_acclt_trap::PipelineRegister::operator -=( const Register & other \
    ) throw(){
    return (*(this->reg_all)) -= other;
}

Register & leon3_acclt_trap::PipelineRegister::operator *=( const Register & other \
    ) throw(){
    return (*(this->reg_all)) *= other;
}

Register & leon3_acclt_trap::PipelineRegister::operator /=( const Register & other \
    ) throw(){
    return (*(this->reg_all)) /= other;
}

Register & leon3_acclt_trap::PipelineRegister::operator |=( const Register & other \
    ) throw(){
    return (*(this->reg_all)) |= other;
}

Register & leon3_acclt_trap::PipelineRegister::operator &=( const Register & other \
    ) throw(){
    return (*(this->reg_all)) &= other;
}

Register & leon3_acclt_trap::PipelineRegister::operator ^=( const Register & other \
    ) throw(){
    return (*(this->reg_all)) ^= other;
}

Register & leon3_acclt_trap::PipelineRegister::operator <<=( const Register & other \
    ) throw(){
    return (*(this->reg_all)) <<= other;
}

Register & leon3_acclt_trap::PipelineRegister::operator >>=( const Register & other \
    ) throw(){
    return (*(this->reg_all)) >>= other;
}

std::ostream & leon3_acclt_trap::PipelineRegister::operator <<( std::ostream & stream \
    ) const throw(){
    return stream << (*(this->reg_all));
}

leon3_acclt_trap::PipelineRegister::operator unsigned int() const throw(){
    return *(this->reg_all);
}

leon3_acclt_trap::PipelineRegister::PipelineRegister( Register * reg_fetch, Register \
    * reg_decode, Register * reg_regs, Register * reg_execute, Register * reg_memory, \
    Register * reg_exception, Register * reg_wb, Register * reg_all ){
    this->reg_stage[0] = reg_fetch;
    this->reg_stage[0]->hasToPropagate = &(this->hasToPropagate);
    this->reg_stage[1] = reg_decode;
    this->reg_stage[1]->hasToPropagate = &(this->hasToPropagate);
    this->reg_stage[2] = reg_regs;
    this->reg_stage[2]->hasToPropagate = &(this->hasToPropagate);
    this->reg_stage[3] = reg_execute;
    this->reg_stage[3]->hasToPropagate = &(this->hasToPropagate);
    this->reg_stage[4] = reg_memory;
    this->reg_stage[4]->hasToPropagate = &(this->hasToPropagate);
    this->reg_stage[5] = reg_exception;
    this->reg_stage[5]->hasToPropagate = &(this->hasToPropagate);
    this->reg_stage[6] = reg_wb;
    this->reg_stage[6]->hasToPropagate = &(this->hasToPropagate);
    this->reg_all = reg_all;
    this->reg_all->hasToPropagate = &(this->hasToPropagate);
    this->hasToPropagate = false;
}

leon3_acclt_trap::PipelineRegister::PipelineRegister(){
    this->reg_stage[0] = NULL;
    this->reg_stage[1] = NULL;
    this->reg_stage[2] = NULL;
    this->reg_stage[3] = NULL;
    this->reg_stage[4] = NULL;
    this->reg_stage[5] = NULL;
    this->reg_stage[6] = NULL;
    this->reg_all = NULL;
    this->hasToPropagate = false;
}

void leon3_acclt_trap::PipelineRegister_exception_decode_fetch::propagate() throw(){
    bool hasChanges = false;
    if(this->reg_stage[5]->timeStamp > this->reg_all->timeStamp){
        hasChanges = true;
        this->reg_all->forceValue(*(this->reg_stage[5]));
        this->reg_all->timeStamp = this->reg_stage[5]->timeStamp;
        for(int i = 0; i < 7; i++){
            this->reg_stage[i]->forceValue(*(this->reg_stage[5]));
            this->reg_stage[i]->timeStamp = this->reg_stage[5]->timeStamp;
        }
    }
    else if(this->reg_stage[1]->timeStamp > this->reg_all->timeStamp){
        hasChanges = true;
        this->reg_all->forceValue(*(this->reg_stage[1]));
        this->reg_all->timeStamp = this->reg_stage[1]->timeStamp;
        for(int i = 0; i < 7; i++){
            this->reg_stage[i]->forceValue(*(this->reg_stage[1]));
            this->reg_stage[i]->timeStamp = this->reg_stage[1]->timeStamp;
        }
    }
    else if(this->reg_stage[0]->timeStamp > this->reg_all->timeStamp){
        hasChanges = true;
        this->reg_all->forceValue(*(this->reg_stage[0]));
        this->reg_all->timeStamp = this->reg_stage[0]->timeStamp;
        for(int i = 0; i < 7; i++){
            this->reg_stage[i]->forceValue(*(this->reg_stage[0]));
            this->reg_stage[i]->timeStamp = this->reg_stage[0]->timeStamp;
        }
    }
    this->hasToPropagate = hasChanges;
}

leon3_acclt_trap::PipelineRegister_exception_decode_fetch::PipelineRegister_exception_decode_fetch( \
    Register * reg_fetch, Register * reg_decode, Register * reg_regs, Register * reg_execute, \
    Register * reg_memory, Register * reg_exception, Register * reg_wb, Register * reg_all \
    ) : PipelineRegister(reg_fetch, reg_decode, reg_regs, reg_execute, reg_memory, reg_exception, \
    reg_wb, reg_all){

}

leon3_acclt_trap::PipelineRegister_exception_decode_fetch::PipelineRegister_exception_decode_fetch() \
    : PipelineRegister(){

}


