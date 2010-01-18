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
#include <ostream>

using namespace arm7tdmi_funclt_trap;
InnerField & arm7tdmi_funclt_trap::InnerField::operator =( const InnerField & other \
    ) throw(){
    *this = (unsigned int)other;
    return *this;
}

arm7tdmi_funclt_trap::InnerField::~InnerField(){

}
void arm7tdmi_funclt_trap::Register::clockCycle() throw(){

}

arm7tdmi_funclt_trap::Register::Register(){

}

arm7tdmi_funclt_trap::Register::Register( const Register & other ){

}

InnerField & arm7tdmi_funclt_trap::Reg32_0::InnerField_C::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xdfffffffL;
    this->value |= ((other & 0x1) << 29);
    return *this;
}

arm7tdmi_funclt_trap::Reg32_0::InnerField_C::InnerField_C( unsigned int & value ) \
    : value(value){

}

arm7tdmi_funclt_trap::Reg32_0::InnerField_C::~InnerField_C(){

}
InnerField & arm7tdmi_funclt_trap::Reg32_0::InnerField_F::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xffffffbfL;
    this->value |= ((other & 0x1) << 6);
    return *this;
}

arm7tdmi_funclt_trap::Reg32_0::InnerField_F::InnerField_F( unsigned int & value ) \
    : value(value){

}

arm7tdmi_funclt_trap::Reg32_0::InnerField_F::~InnerField_F(){

}
InnerField & arm7tdmi_funclt_trap::Reg32_0::InnerField_I::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xffffff7fL;
    this->value |= ((other & 0x1) << 7);
    return *this;
}

arm7tdmi_funclt_trap::Reg32_0::InnerField_I::InnerField_I( unsigned int & value ) \
    : value(value){

}

arm7tdmi_funclt_trap::Reg32_0::InnerField_I::~InnerField_I(){

}
InnerField & arm7tdmi_funclt_trap::Reg32_0::InnerField_N::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0x7fffffff;
    this->value |= ((other & 0x1) << 31);
    return *this;
}

arm7tdmi_funclt_trap::Reg32_0::InnerField_N::InnerField_N( unsigned int & value ) \
    : value(value){

}

arm7tdmi_funclt_trap::Reg32_0::InnerField_N::~InnerField_N(){

}
InnerField & arm7tdmi_funclt_trap::Reg32_0::InnerField_mode::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xfffffff0L;
    this->value |= other;
    return *this;
}

arm7tdmi_funclt_trap::Reg32_0::InnerField_mode::InnerField_mode( unsigned int & value \
    ) : value(value){

}

arm7tdmi_funclt_trap::Reg32_0::InnerField_mode::~InnerField_mode(){

}
InnerField & arm7tdmi_funclt_trap::Reg32_0::InnerField_V::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xefffffffL;
    this->value |= ((other & 0x1) << 28);
    return *this;
}

arm7tdmi_funclt_trap::Reg32_0::InnerField_V::InnerField_V( unsigned int & value ) \
    : value(value){

}

arm7tdmi_funclt_trap::Reg32_0::InnerField_V::~InnerField_V(){

}
InnerField & arm7tdmi_funclt_trap::Reg32_0::InnerField_Z::operator =( const unsigned \
    int & other ) throw(){
    this->value &= 0xbfffffffL;
    this->value |= ((other & 0x1) << 30);
    return *this;
}

arm7tdmi_funclt_trap::Reg32_0::InnerField_Z::InnerField_Z( unsigned int & value ) \
    : value(value){

}

arm7tdmi_funclt_trap::Reg32_0::InnerField_Z::~InnerField_Z(){

}
InnerField & arm7tdmi_funclt_trap::Reg32_0::InnerField_Empty::operator =( const unsigned \
    int & other ) throw(){
    return *this;
}

arm7tdmi_funclt_trap::Reg32_0::InnerField_Empty::InnerField_Empty(){

}

arm7tdmi_funclt_trap::Reg32_0::InnerField_Empty::~InnerField_Empty(){

}
void arm7tdmi_funclt_trap::Reg32_0::immediateWrite( const unsigned int & value ) throw(){
    this->value = value;
}

unsigned int arm7tdmi_funclt_trap::Reg32_0::readNewValue() throw(){
    return this->value;
}

unsigned int arm7tdmi_funclt_trap::Reg32_0::operator ~() throw(){
    return ~(this->value);
}

Reg32_0 & arm7tdmi_funclt_trap::Reg32_0::operator =( const unsigned int & other ) \
    throw(){
    this->value = other;
    return *this;
}

Reg32_0 & arm7tdmi_funclt_trap::Reg32_0::operator +=( const unsigned int & other \
    ) throw(){
    this->value += other;
    return *this;
}

Reg32_0 & arm7tdmi_funclt_trap::Reg32_0::operator -=( const unsigned int & other \
    ) throw(){
    this->value -= other;
    return *this;
}

Reg32_0 & arm7tdmi_funclt_trap::Reg32_0::operator *=( const unsigned int & other \
    ) throw(){
    this->value *= other;
    return *this;
}

Reg32_0 & arm7tdmi_funclt_trap::Reg32_0::operator /=( const unsigned int & other \
    ) throw(){
    this->value /= other;
    return *this;
}

Reg32_0 & arm7tdmi_funclt_trap::Reg32_0::operator |=( const unsigned int & other \
    ) throw(){
    this->value |= other;
    return *this;
}

Reg32_0 & arm7tdmi_funclt_trap::Reg32_0::operator &=( const unsigned int & other \
    ) throw(){
    this->value &= other;
    return *this;
}

Reg32_0 & arm7tdmi_funclt_trap::Reg32_0::operator ^=( const unsigned int & other \
    ) throw(){
    this->value ^= other;
    return *this;
}

Reg32_0 & arm7tdmi_funclt_trap::Reg32_0::operator <<=( const unsigned int & other \
    ) throw(){
    this->value <<= other;
    return *this;
}

Reg32_0 & arm7tdmi_funclt_trap::Reg32_0::operator >>=( const unsigned int & other \
    ) throw(){
    this->value >>= other;
    return *this;
}

unsigned int arm7tdmi_funclt_trap::Reg32_0::operator +( const Reg32_0 & other ) const \
    throw(){
    return (this->value + other.value);
}

unsigned int arm7tdmi_funclt_trap::Reg32_0::operator -( const Reg32_0 & other ) const \
    throw(){
    return (this->value - other.value);
}

unsigned int arm7tdmi_funclt_trap::Reg32_0::operator *( const Reg32_0 & other ) const \
    throw(){
    return (this->value * other.value);
}

unsigned int arm7tdmi_funclt_trap::Reg32_0::operator /( const Reg32_0 & other ) const \
    throw(){
    return (this->value / other.value);
}

unsigned int arm7tdmi_funclt_trap::Reg32_0::operator |( const Reg32_0 & other ) const \
    throw(){
    return (this->value | other.value);
}

unsigned int arm7tdmi_funclt_trap::Reg32_0::operator &( const Reg32_0 & other ) const \
    throw(){
    return (this->value & other.value);
}

unsigned int arm7tdmi_funclt_trap::Reg32_0::operator ^( const Reg32_0 & other ) const \
    throw(){
    return (this->value ^ other.value);
}

unsigned int arm7tdmi_funclt_trap::Reg32_0::operator <<( const Reg32_0 & other ) \
    const throw(){
    return (this->value << other.value);
}

unsigned int arm7tdmi_funclt_trap::Reg32_0::operator >>( const Reg32_0 & other ) \
    const throw(){
    return (this->value >> other.value);
}

bool arm7tdmi_funclt_trap::Reg32_0::operator <( const Reg32_0 & other ) const throw(){
    return (this->value < other.value);
}

bool arm7tdmi_funclt_trap::Reg32_0::operator >( const Reg32_0 & other ) const throw(){
    return (this->value > other.value);
}

bool arm7tdmi_funclt_trap::Reg32_0::operator <=( const Reg32_0 & other ) const throw(){
    return (this->value <= other.value);
}

bool arm7tdmi_funclt_trap::Reg32_0::operator >=( const Reg32_0 & other ) const throw(){
    return (this->value >= other.value);
}

bool arm7tdmi_funclt_trap::Reg32_0::operator ==( const Reg32_0 & other ) const throw(){
    return (this->value == other.value);
}

bool arm7tdmi_funclt_trap::Reg32_0::operator !=( const Reg32_0 & other ) const throw(){
    return (this->value != other.value);
}

Reg32_0 & arm7tdmi_funclt_trap::Reg32_0::operator =( const Reg32_0 & other ) throw(){
    this->value = other;
    return *this;
}

Reg32_0 & arm7tdmi_funclt_trap::Reg32_0::operator +=( const Reg32_0 & other ) throw(){
    this->value += other;
    return *this;
}

Reg32_0 & arm7tdmi_funclt_trap::Reg32_0::operator -=( const Reg32_0 & other ) throw(){
    this->value -= other;
    return *this;
}

Reg32_0 & arm7tdmi_funclt_trap::Reg32_0::operator *=( const Reg32_0 & other ) throw(){
    this->value *= other;
    return *this;
}

Reg32_0 & arm7tdmi_funclt_trap::Reg32_0::operator /=( const Reg32_0 & other ) throw(){
    this->value /= other;
    return *this;
}

Reg32_0 & arm7tdmi_funclt_trap::Reg32_0::operator |=( const Reg32_0 & other ) throw(){
    this->value |= other;
    return *this;
}

Reg32_0 & arm7tdmi_funclt_trap::Reg32_0::operator &=( const Reg32_0 & other ) throw(){
    this->value &= other;
    return *this;
}

Reg32_0 & arm7tdmi_funclt_trap::Reg32_0::operator ^=( const Reg32_0 & other ) throw(){
    this->value ^= other;
    return *this;
}

Reg32_0 & arm7tdmi_funclt_trap::Reg32_0::operator <<=( const Reg32_0 & other ) throw(){
    this->value <<= other;
    return *this;
}

Reg32_0 & arm7tdmi_funclt_trap::Reg32_0::operator >>=( const Reg32_0 & other ) throw(){
    this->value >>= other;
    return *this;
}

unsigned int arm7tdmi_funclt_trap::Reg32_0::operator +( const Register & other ) \
    const throw(){
    return (this->value + other);
}

unsigned int arm7tdmi_funclt_trap::Reg32_0::operator -( const Register & other ) \
    const throw(){
    return (this->value - other);
}

unsigned int arm7tdmi_funclt_trap::Reg32_0::operator *( const Register & other ) \
    const throw(){
    return (this->value * other);
}

unsigned int arm7tdmi_funclt_trap::Reg32_0::operator /( const Register & other ) \
    const throw(){
    return (this->value / other);
}

unsigned int arm7tdmi_funclt_trap::Reg32_0::operator |( const Register & other ) \
    const throw(){
    return (this->value | other);
}

unsigned int arm7tdmi_funclt_trap::Reg32_0::operator &( const Register & other ) \
    const throw(){
    return (this->value & other);
}

unsigned int arm7tdmi_funclt_trap::Reg32_0::operator ^( const Register & other ) \
    const throw(){
    return (this->value ^ other);
}

unsigned int arm7tdmi_funclt_trap::Reg32_0::operator <<( const Register & other ) \
    const throw(){
    return (this->value << other);
}

unsigned int arm7tdmi_funclt_trap::Reg32_0::operator >>( const Register & other ) \
    const throw(){
    return (this->value >> other);
}

bool arm7tdmi_funclt_trap::Reg32_0::operator <( const Register & other ) const throw(){
    return (this->value < other);
}

bool arm7tdmi_funclt_trap::Reg32_0::operator >( const Register & other ) const throw(){
    return (this->value > other);
}

bool arm7tdmi_funclt_trap::Reg32_0::operator <=( const Register & other ) const throw(){
    return (this->value <= other);
}

bool arm7tdmi_funclt_trap::Reg32_0::operator >=( const Register & other ) const throw(){
    return (this->value >= other);
}

bool arm7tdmi_funclt_trap::Reg32_0::operator ==( const Register & other ) const throw(){
    return (this->value == other);
}

bool arm7tdmi_funclt_trap::Reg32_0::operator !=( const Register & other ) const throw(){
    return (this->value != other);
}

Reg32_0 & arm7tdmi_funclt_trap::Reg32_0::operator =( const Register & other ) throw(){
    this->value = other;
    return *this;
}

Reg32_0 & arm7tdmi_funclt_trap::Reg32_0::operator +=( const Register & other ) throw(){
    this->value += other;
    return *this;
}

Reg32_0 & arm7tdmi_funclt_trap::Reg32_0::operator -=( const Register & other ) throw(){
    this->value -= other;
    return *this;
}

Reg32_0 & arm7tdmi_funclt_trap::Reg32_0::operator *=( const Register & other ) throw(){
    this->value *= other;
    return *this;
}

Reg32_0 & arm7tdmi_funclt_trap::Reg32_0::operator /=( const Register & other ) throw(){
    this->value /= other;
    return *this;
}

Reg32_0 & arm7tdmi_funclt_trap::Reg32_0::operator |=( const Register & other ) throw(){
    this->value |= other;
    return *this;
}

Reg32_0 & arm7tdmi_funclt_trap::Reg32_0::operator &=( const Register & other ) throw(){
    this->value &= other;
    return *this;
}

Reg32_0 & arm7tdmi_funclt_trap::Reg32_0::operator ^=( const Register & other ) throw(){
    this->value ^= other;
    return *this;
}

Reg32_0 & arm7tdmi_funclt_trap::Reg32_0::operator <<=( const Register & other ) throw(){
    this->value <<= other;
    return *this;
}

Reg32_0 & arm7tdmi_funclt_trap::Reg32_0::operator >>=( const Register & other ) throw(){
    this->value >>= other;
    return *this;
}

std::ostream & arm7tdmi_funclt_trap::Reg32_0::operator <<( std::ostream & stream \
    ) const throw(){
    stream << std::hex << std::showbase << this->value << std::dec;
    return stream;
}

arm7tdmi_funclt_trap::Reg32_0::Reg32_0() : field_C(this->value), field_F(this->value), \
    field_I(this->value), field_N(this->value), field_mode(this->value), field_V(this->value), \
    field_Z(this->value){
    this->value = 0;
}

InnerField & arm7tdmi_funclt_trap::Reg32_1::InnerField_Empty::operator =( const unsigned \
    int & other ) throw(){
    return *this;
}

arm7tdmi_funclt_trap::Reg32_1::InnerField_Empty::InnerField_Empty(){

}

arm7tdmi_funclt_trap::Reg32_1::InnerField_Empty::~InnerField_Empty(){

}
void arm7tdmi_funclt_trap::Reg32_1::immediateWrite( const unsigned int & value ) throw(){
    this->value = value;
}

unsigned int arm7tdmi_funclt_trap::Reg32_1::readNewValue() throw(){
    return this->value;
}

unsigned int arm7tdmi_funclt_trap::Reg32_1::operator ~() throw(){
    return ~(this->value);
}

Reg32_1 & arm7tdmi_funclt_trap::Reg32_1::operator =( const unsigned int & other ) \
    throw(){
    this->value = other;
    return *this;
}

Reg32_1 & arm7tdmi_funclt_trap::Reg32_1::operator +=( const unsigned int & other \
    ) throw(){
    this->value += other;
    return *this;
}

Reg32_1 & arm7tdmi_funclt_trap::Reg32_1::operator -=( const unsigned int & other \
    ) throw(){
    this->value -= other;
    return *this;
}

Reg32_1 & arm7tdmi_funclt_trap::Reg32_1::operator *=( const unsigned int & other \
    ) throw(){
    this->value *= other;
    return *this;
}

Reg32_1 & arm7tdmi_funclt_trap::Reg32_1::operator /=( const unsigned int & other \
    ) throw(){
    this->value /= other;
    return *this;
}

Reg32_1 & arm7tdmi_funclt_trap::Reg32_1::operator |=( const unsigned int & other \
    ) throw(){
    this->value |= other;
    return *this;
}

Reg32_1 & arm7tdmi_funclt_trap::Reg32_1::operator &=( const unsigned int & other \
    ) throw(){
    this->value &= other;
    return *this;
}

Reg32_1 & arm7tdmi_funclt_trap::Reg32_1::operator ^=( const unsigned int & other \
    ) throw(){
    this->value ^= other;
    return *this;
}

Reg32_1 & arm7tdmi_funclt_trap::Reg32_1::operator <<=( const unsigned int & other \
    ) throw(){
    this->value <<= other;
    return *this;
}

Reg32_1 & arm7tdmi_funclt_trap::Reg32_1::operator >>=( const unsigned int & other \
    ) throw(){
    this->value >>= other;
    return *this;
}

unsigned int arm7tdmi_funclt_trap::Reg32_1::operator +( const Reg32_1 & other ) const \
    throw(){
    return (this->value + other.value);
}

unsigned int arm7tdmi_funclt_trap::Reg32_1::operator -( const Reg32_1 & other ) const \
    throw(){
    return (this->value - other.value);
}

unsigned int arm7tdmi_funclt_trap::Reg32_1::operator *( const Reg32_1 & other ) const \
    throw(){
    return (this->value * other.value);
}

unsigned int arm7tdmi_funclt_trap::Reg32_1::operator /( const Reg32_1 & other ) const \
    throw(){
    return (this->value / other.value);
}

unsigned int arm7tdmi_funclt_trap::Reg32_1::operator |( const Reg32_1 & other ) const \
    throw(){
    return (this->value | other.value);
}

unsigned int arm7tdmi_funclt_trap::Reg32_1::operator &( const Reg32_1 & other ) const \
    throw(){
    return (this->value & other.value);
}

unsigned int arm7tdmi_funclt_trap::Reg32_1::operator ^( const Reg32_1 & other ) const \
    throw(){
    return (this->value ^ other.value);
}

unsigned int arm7tdmi_funclt_trap::Reg32_1::operator <<( const Reg32_1 & other ) \
    const throw(){
    return (this->value << other.value);
}

unsigned int arm7tdmi_funclt_trap::Reg32_1::operator >>( const Reg32_1 & other ) \
    const throw(){
    return (this->value >> other.value);
}

bool arm7tdmi_funclt_trap::Reg32_1::operator <( const Reg32_1 & other ) const throw(){
    return (this->value < other.value);
}

bool arm7tdmi_funclt_trap::Reg32_1::operator >( const Reg32_1 & other ) const throw(){
    return (this->value > other.value);
}

bool arm7tdmi_funclt_trap::Reg32_1::operator <=( const Reg32_1 & other ) const throw(){
    return (this->value <= other.value);
}

bool arm7tdmi_funclt_trap::Reg32_1::operator >=( const Reg32_1 & other ) const throw(){
    return (this->value >= other.value);
}

bool arm7tdmi_funclt_trap::Reg32_1::operator ==( const Reg32_1 & other ) const throw(){
    return (this->value == other.value);
}

bool arm7tdmi_funclt_trap::Reg32_1::operator !=( const Reg32_1 & other ) const throw(){
    return (this->value != other.value);
}

Reg32_1 & arm7tdmi_funclt_trap::Reg32_1::operator =( const Reg32_1 & other ) throw(){
    this->value = other;
    return *this;
}

Reg32_1 & arm7tdmi_funclt_trap::Reg32_1::operator +=( const Reg32_1 & other ) throw(){
    this->value += other;
    return *this;
}

Reg32_1 & arm7tdmi_funclt_trap::Reg32_1::operator -=( const Reg32_1 & other ) throw(){
    this->value -= other;
    return *this;
}

Reg32_1 & arm7tdmi_funclt_trap::Reg32_1::operator *=( const Reg32_1 & other ) throw(){
    this->value *= other;
    return *this;
}

Reg32_1 & arm7tdmi_funclt_trap::Reg32_1::operator /=( const Reg32_1 & other ) throw(){
    this->value /= other;
    return *this;
}

Reg32_1 & arm7tdmi_funclt_trap::Reg32_1::operator |=( const Reg32_1 & other ) throw(){
    this->value |= other;
    return *this;
}

Reg32_1 & arm7tdmi_funclt_trap::Reg32_1::operator &=( const Reg32_1 & other ) throw(){
    this->value &= other;
    return *this;
}

Reg32_1 & arm7tdmi_funclt_trap::Reg32_1::operator ^=( const Reg32_1 & other ) throw(){
    this->value ^= other;
    return *this;
}

Reg32_1 & arm7tdmi_funclt_trap::Reg32_1::operator <<=( const Reg32_1 & other ) throw(){
    this->value <<= other;
    return *this;
}

Reg32_1 & arm7tdmi_funclt_trap::Reg32_1::operator >>=( const Reg32_1 & other ) throw(){
    this->value >>= other;
    return *this;
}

unsigned int arm7tdmi_funclt_trap::Reg32_1::operator +( const Register & other ) \
    const throw(){
    return (this->value + other);
}

unsigned int arm7tdmi_funclt_trap::Reg32_1::operator -( const Register & other ) \
    const throw(){
    return (this->value - other);
}

unsigned int arm7tdmi_funclt_trap::Reg32_1::operator *( const Register & other ) \
    const throw(){
    return (this->value * other);
}

unsigned int arm7tdmi_funclt_trap::Reg32_1::operator /( const Register & other ) \
    const throw(){
    return (this->value / other);
}

unsigned int arm7tdmi_funclt_trap::Reg32_1::operator |( const Register & other ) \
    const throw(){
    return (this->value | other);
}

unsigned int arm7tdmi_funclt_trap::Reg32_1::operator &( const Register & other ) \
    const throw(){
    return (this->value & other);
}

unsigned int arm7tdmi_funclt_trap::Reg32_1::operator ^( const Register & other ) \
    const throw(){
    return (this->value ^ other);
}

unsigned int arm7tdmi_funclt_trap::Reg32_1::operator <<( const Register & other ) \
    const throw(){
    return (this->value << other);
}

unsigned int arm7tdmi_funclt_trap::Reg32_1::operator >>( const Register & other ) \
    const throw(){
    return (this->value >> other);
}

bool arm7tdmi_funclt_trap::Reg32_1::operator <( const Register & other ) const throw(){
    return (this->value < other);
}

bool arm7tdmi_funclt_trap::Reg32_1::operator >( const Register & other ) const throw(){
    return (this->value > other);
}

bool arm7tdmi_funclt_trap::Reg32_1::operator <=( const Register & other ) const throw(){
    return (this->value <= other);
}

bool arm7tdmi_funclt_trap::Reg32_1::operator >=( const Register & other ) const throw(){
    return (this->value >= other);
}

bool arm7tdmi_funclt_trap::Reg32_1::operator ==( const Register & other ) const throw(){
    return (this->value == other);
}

bool arm7tdmi_funclt_trap::Reg32_1::operator !=( const Register & other ) const throw(){
    return (this->value != other);
}

Reg32_1 & arm7tdmi_funclt_trap::Reg32_1::operator =( const Register & other ) throw(){
    this->value = other;
    return *this;
}

Reg32_1 & arm7tdmi_funclt_trap::Reg32_1::operator +=( const Register & other ) throw(){
    this->value += other;
    return *this;
}

Reg32_1 & arm7tdmi_funclt_trap::Reg32_1::operator -=( const Register & other ) throw(){
    this->value -= other;
    return *this;
}

Reg32_1 & arm7tdmi_funclt_trap::Reg32_1::operator *=( const Register & other ) throw(){
    this->value *= other;
    return *this;
}

Reg32_1 & arm7tdmi_funclt_trap::Reg32_1::operator /=( const Register & other ) throw(){
    this->value /= other;
    return *this;
}

Reg32_1 & arm7tdmi_funclt_trap::Reg32_1::operator |=( const Register & other ) throw(){
    this->value |= other;
    return *this;
}

Reg32_1 & arm7tdmi_funclt_trap::Reg32_1::operator &=( const Register & other ) throw(){
    this->value &= other;
    return *this;
}

Reg32_1 & arm7tdmi_funclt_trap::Reg32_1::operator ^=( const Register & other ) throw(){
    this->value ^= other;
    return *this;
}

Reg32_1 & arm7tdmi_funclt_trap::Reg32_1::operator <<=( const Register & other ) throw(){
    this->value <<= other;
    return *this;
}

Reg32_1 & arm7tdmi_funclt_trap::Reg32_1::operator >>=( const Register & other ) throw(){
    this->value >>= other;
    return *this;
}

std::ostream & arm7tdmi_funclt_trap::Reg32_1::operator <<( std::ostream & stream \
    ) const throw(){
    stream << std::hex << std::showbase << this->value << std::dec;
    return stream;
}

arm7tdmi_funclt_trap::Reg32_1::Reg32_1(){
    this->value = 0;
}


