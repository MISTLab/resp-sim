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


#ifndef REGISTERS_HPP
#define REGISTERS_HPP

#include <ostream>

#define FUNC_MODEL
#define LT_IF
#define key_C 0
#define key_F 1
#define key_I 2
#define key_N 3
#define key_mode 4
#define key_V 5
#define key_Z 6
namespace arm7tdmi_funclt_trap{

    class InnerField{

        public:
        InnerField & operator =( const InnerField & other ) throw();
        virtual InnerField & operator =( const unsigned int & other ) throw() = 0;
        virtual operator unsigned int() const throw() = 0;
        virtual ~InnerField();
    };

};

namespace arm7tdmi_funclt_trap{

    class Register{

        public:
        Register( const Register & other );
        Register();
        virtual void immediateWrite( const unsigned int & value ) throw() = 0;
        virtual unsigned int readNewValue() throw() = 0;
        virtual void clockCycle() throw();
        virtual InnerField & operator []( int bitField ) throw() = 0;
        virtual unsigned int operator ~() throw() = 0;
        virtual unsigned int operator +( const Register & other ) const throw() = 0;
        virtual unsigned int operator -( const Register & other ) const throw() = 0;
        virtual unsigned int operator *( const Register & other ) const throw() = 0;
        virtual unsigned int operator /( const Register & other ) const throw() = 0;
        virtual unsigned int operator |( const Register & other ) const throw() = 0;
        virtual unsigned int operator &( const Register & other ) const throw() = 0;
        virtual unsigned int operator ^( const Register & other ) const throw() = 0;
        virtual unsigned int operator <<( const Register & other ) const throw() = 0;
        virtual unsigned int operator >>( const Register & other ) const throw() = 0;
        virtual bool operator <( const Register & other ) const throw() = 0;
        virtual bool operator >( const Register & other ) const throw() = 0;
        virtual bool operator <=( const Register & other ) const throw() = 0;
        virtual bool operator >=( const Register & other ) const throw() = 0;
        virtual bool operator ==( const Register & other ) const throw() = 0;
        virtual bool operator !=( const Register & other ) const throw() = 0;
        virtual Register & operator =( const unsigned int & other ) throw() = 0;
        virtual Register & operator +=( const unsigned int & other ) throw() = 0;
        virtual Register & operator -=( const unsigned int & other ) throw() = 0;
        virtual Register & operator *=( const unsigned int & other ) throw() = 0;
        virtual Register & operator /=( const unsigned int & other ) throw() = 0;
        virtual Register & operator |=( const unsigned int & other ) throw() = 0;
        virtual Register & operator &=( const unsigned int & other ) throw() = 0;
        virtual Register & operator ^=( const unsigned int & other ) throw() = 0;
        virtual Register & operator <<=( const unsigned int & other ) throw() = 0;
        virtual Register & operator >>=( const unsigned int & other ) throw() = 0;
        virtual Register & operator =( const Register & other ) throw() = 0;
        virtual Register & operator +=( const Register & other ) throw() = 0;
        virtual Register & operator -=( const Register & other ) throw() = 0;
        virtual Register & operator *=( const Register & other ) throw() = 0;
        virtual Register & operator /=( const Register & other ) throw() = 0;
        virtual Register & operator |=( const Register & other ) throw() = 0;
        virtual Register & operator &=( const Register & other ) throw() = 0;
        virtual Register & operator ^=( const Register & other ) throw() = 0;
        virtual Register & operator <<=( const Register & other ) throw() = 0;
        virtual Register & operator >>=( const Register & other ) throw() = 0;
        virtual std::ostream & operator <<( std::ostream & other ) const throw() = 0;
        virtual operator unsigned int() const throw() = 0;
    };

};

namespace arm7tdmi_funclt_trap{

    class Reg32_0 : public Register{
        public:
        class InnerField_C : public InnerField{
            private:
            unsigned int & value;

            public:
            InnerField_C( unsigned int & value );
            InnerField & operator =( const unsigned int & other ) throw();
            inline operator unsigned int() const throw(){
                return (this->value & 0x20000000) >> 29;
            }
            virtual ~InnerField_C();
        };

        class InnerField_F : public InnerField{
            private:
            unsigned int & value;

            public:
            InnerField_F( unsigned int & value );
            InnerField & operator =( const unsigned int & other ) throw();
            inline operator unsigned int() const throw(){
                return (this->value & 0x40) >> 6;
            }
            virtual ~InnerField_F();
        };

        class InnerField_I : public InnerField{
            private:
            unsigned int & value;

            public:
            InnerField_I( unsigned int & value );
            InnerField & operator =( const unsigned int & other ) throw();
            inline operator unsigned int() const throw(){
                return (this->value & 0x80) >> 7;
            }
            virtual ~InnerField_I();
        };

        class InnerField_N : public InnerField{
            private:
            unsigned int & value;

            public:
            InnerField_N( unsigned int & value );
            InnerField & operator =( const unsigned int & other ) throw();
            inline operator unsigned int() const throw(){
                return (this->value & 0x80000000L) >> 31;
            }
            virtual ~InnerField_N();
        };

        class InnerField_mode : public InnerField{
            private:
            unsigned int & value;

            public:
            InnerField_mode( unsigned int & value );
            InnerField & operator =( const unsigned int & other ) throw();
            inline operator unsigned int() const throw(){
                return (this->value & 0xf);
            }
            virtual ~InnerField_mode();
        };

        class InnerField_V : public InnerField{
            private:
            unsigned int & value;

            public:
            InnerField_V( unsigned int & value );
            InnerField & operator =( const unsigned int & other ) throw();
            inline operator unsigned int() const throw(){
                return (this->value & 0x10000000) >> 28;
            }
            virtual ~InnerField_V();
        };

        class InnerField_Z : public InnerField{
            private:
            unsigned int & value;

            public:
            InnerField_Z( unsigned int & value );
            InnerField & operator =( const unsigned int & other ) throw();
            inline operator unsigned int() const throw(){
                return (this->value & 0x40000000) >> 30;
            }
            virtual ~InnerField_Z();
        };

        class InnerField_Empty : public InnerField{

            public:
            InnerField_Empty();
            InnerField & operator =( const unsigned int & other ) throw();
            inline operator unsigned int() const throw(){
                return 0;
            }
            virtual ~InnerField_Empty();
        };

        private:
        InnerField_C field_C;
        InnerField_F field_F;
        InnerField_I field_I;
        InnerField_N field_N;
        InnerField_mode field_mode;
        InnerField_V field_V;
        InnerField_Z field_Z;
        InnerField_Empty field_empty;
        unsigned int value;

        public:
        Reg32_0();
        inline InnerField & operator []( int bitField ) throw(){
            switch(bitField){
                case key_C:{
                    return this->field_C;
                    break;
                }
                case key_F:{
                    return this->field_F;
                    break;
                }
                case key_I:{
                    return this->field_I;
                    break;
                }
                case key_N:{
                    return this->field_N;
                    break;
                }
                case key_mode:{
                    return this->field_mode;
                    break;
                }
                case key_V:{
                    return this->field_V;
                    break;
                }
                case key_Z:{
                    return this->field_Z;
                    break;
                }
                default:{
                    return this->field_empty;
                    break;
                }
            }
        }
        void immediateWrite( const unsigned int & value ) throw();
        unsigned int readNewValue() throw();
        unsigned int operator ~() throw();
        Reg32_0 & operator =( const unsigned int & other ) throw();
        Reg32_0 & operator +=( const unsigned int & other ) throw();
        Reg32_0 & operator -=( const unsigned int & other ) throw();
        Reg32_0 & operator *=( const unsigned int & other ) throw();
        Reg32_0 & operator /=( const unsigned int & other ) throw();
        Reg32_0 & operator |=( const unsigned int & other ) throw();
        Reg32_0 & operator &=( const unsigned int & other ) throw();
        Reg32_0 & operator ^=( const unsigned int & other ) throw();
        Reg32_0 & operator <<=( const unsigned int & other ) throw();
        Reg32_0 & operator >>=( const unsigned int & other ) throw();
        unsigned int operator +( const Reg32_0 & other ) const throw();
        unsigned int operator -( const Reg32_0 & other ) const throw();
        unsigned int operator *( const Reg32_0 & other ) const throw();
        unsigned int operator /( const Reg32_0 & other ) const throw();
        unsigned int operator |( const Reg32_0 & other ) const throw();
        unsigned int operator &( const Reg32_0 & other ) const throw();
        unsigned int operator ^( const Reg32_0 & other ) const throw();
        unsigned int operator <<( const Reg32_0 & other ) const throw();
        unsigned int operator >>( const Reg32_0 & other ) const throw();
        bool operator <( const Reg32_0 & other ) const throw();
        bool operator >( const Reg32_0 & other ) const throw();
        bool operator <=( const Reg32_0 & other ) const throw();
        bool operator >=( const Reg32_0 & other ) const throw();
        bool operator ==( const Reg32_0 & other ) const throw();
        bool operator !=( const Reg32_0 & other ) const throw();
        Reg32_0 & operator =( const Reg32_0 & other ) throw();
        Reg32_0 & operator +=( const Reg32_0 & other ) throw();
        Reg32_0 & operator -=( const Reg32_0 & other ) throw();
        Reg32_0 & operator *=( const Reg32_0 & other ) throw();
        Reg32_0 & operator /=( const Reg32_0 & other ) throw();
        Reg32_0 & operator |=( const Reg32_0 & other ) throw();
        Reg32_0 & operator &=( const Reg32_0 & other ) throw();
        Reg32_0 & operator ^=( const Reg32_0 & other ) throw();
        Reg32_0 & operator <<=( const Reg32_0 & other ) throw();
        Reg32_0 & operator >>=( const Reg32_0 & other ) throw();
        unsigned int operator +( const Register & other ) const throw();
        unsigned int operator -( const Register & other ) const throw();
        unsigned int operator *( const Register & other ) const throw();
        unsigned int operator /( const Register & other ) const throw();
        unsigned int operator |( const Register & other ) const throw();
        unsigned int operator &( const Register & other ) const throw();
        unsigned int operator ^( const Register & other ) const throw();
        unsigned int operator <<( const Register & other ) const throw();
        unsigned int operator >>( const Register & other ) const throw();
        bool operator <( const Register & other ) const throw();
        bool operator >( const Register & other ) const throw();
        bool operator <=( const Register & other ) const throw();
        bool operator >=( const Register & other ) const throw();
        bool operator ==( const Register & other ) const throw();
        bool operator !=( const Register & other ) const throw();
        Reg32_0 & operator =( const Register & other ) throw();
        Reg32_0 & operator +=( const Register & other ) throw();
        Reg32_0 & operator -=( const Register & other ) throw();
        Reg32_0 & operator *=( const Register & other ) throw();
        Reg32_0 & operator /=( const Register & other ) throw();
        Reg32_0 & operator |=( const Register & other ) throw();
        Reg32_0 & operator &=( const Register & other ) throw();
        Reg32_0 & operator ^=( const Register & other ) throw();
        Reg32_0 & operator <<=( const Register & other ) throw();
        Reg32_0 & operator >>=( const Register & other ) throw();
        inline operator unsigned int() const throw(){
            return this->value;
        }
        std::ostream & operator <<( std::ostream & stream ) const throw();
    };

};

namespace arm7tdmi_funclt_trap{

    class Reg32_1 : public Register{
        public:
        class InnerField_Empty : public InnerField{

            public:
            InnerField_Empty();
            InnerField & operator =( const unsigned int & other ) throw();
            inline operator unsigned int() const throw(){
                return 0;
            }
            virtual ~InnerField_Empty();
        };

        private:
        InnerField_Empty field_empty;
        unsigned int value;

        public:
        Reg32_1();
        inline InnerField & operator []( int bitField ) throw(){
            return this->field_empty;
        }
        void immediateWrite( const unsigned int & value ) throw();
        unsigned int readNewValue() throw();
        unsigned int operator ~() throw();
        Reg32_1 & operator =( const unsigned int & other ) throw();
        Reg32_1 & operator +=( const unsigned int & other ) throw();
        Reg32_1 & operator -=( const unsigned int & other ) throw();
        Reg32_1 & operator *=( const unsigned int & other ) throw();
        Reg32_1 & operator /=( const unsigned int & other ) throw();
        Reg32_1 & operator |=( const unsigned int & other ) throw();
        Reg32_1 & operator &=( const unsigned int & other ) throw();
        Reg32_1 & operator ^=( const unsigned int & other ) throw();
        Reg32_1 & operator <<=( const unsigned int & other ) throw();
        Reg32_1 & operator >>=( const unsigned int & other ) throw();
        unsigned int operator +( const Reg32_1 & other ) const throw();
        unsigned int operator -( const Reg32_1 & other ) const throw();
        unsigned int operator *( const Reg32_1 & other ) const throw();
        unsigned int operator /( const Reg32_1 & other ) const throw();
        unsigned int operator |( const Reg32_1 & other ) const throw();
        unsigned int operator &( const Reg32_1 & other ) const throw();
        unsigned int operator ^( const Reg32_1 & other ) const throw();
        unsigned int operator <<( const Reg32_1 & other ) const throw();
        unsigned int operator >>( const Reg32_1 & other ) const throw();
        bool operator <( const Reg32_1 & other ) const throw();
        bool operator >( const Reg32_1 & other ) const throw();
        bool operator <=( const Reg32_1 & other ) const throw();
        bool operator >=( const Reg32_1 & other ) const throw();
        bool operator ==( const Reg32_1 & other ) const throw();
        bool operator !=( const Reg32_1 & other ) const throw();
        Reg32_1 & operator =( const Reg32_1 & other ) throw();
        Reg32_1 & operator +=( const Reg32_1 & other ) throw();
        Reg32_1 & operator -=( const Reg32_1 & other ) throw();
        Reg32_1 & operator *=( const Reg32_1 & other ) throw();
        Reg32_1 & operator /=( const Reg32_1 & other ) throw();
        Reg32_1 & operator |=( const Reg32_1 & other ) throw();
        Reg32_1 & operator &=( const Reg32_1 & other ) throw();
        Reg32_1 & operator ^=( const Reg32_1 & other ) throw();
        Reg32_1 & operator <<=( const Reg32_1 & other ) throw();
        Reg32_1 & operator >>=( const Reg32_1 & other ) throw();
        unsigned int operator +( const Register & other ) const throw();
        unsigned int operator -( const Register & other ) const throw();
        unsigned int operator *( const Register & other ) const throw();
        unsigned int operator /( const Register & other ) const throw();
        unsigned int operator |( const Register & other ) const throw();
        unsigned int operator &( const Register & other ) const throw();
        unsigned int operator ^( const Register & other ) const throw();
        unsigned int operator <<( const Register & other ) const throw();
        unsigned int operator >>( const Register & other ) const throw();
        bool operator <( const Register & other ) const throw();
        bool operator >( const Register & other ) const throw();
        bool operator <=( const Register & other ) const throw();
        bool operator >=( const Register & other ) const throw();
        bool operator ==( const Register & other ) const throw();
        bool operator !=( const Register & other ) const throw();
        Reg32_1 & operator =( const Register & other ) throw();
        Reg32_1 & operator +=( const Register & other ) throw();
        Reg32_1 & operator -=( const Register & other ) throw();
        Reg32_1 & operator *=( const Register & other ) throw();
        Reg32_1 & operator /=( const Register & other ) throw();
        Reg32_1 & operator |=( const Register & other ) throw();
        Reg32_1 & operator &=( const Register & other ) throw();
        Reg32_1 & operator ^=( const Register & other ) throw();
        Reg32_1 & operator <<=( const Register & other ) throw();
        Reg32_1 & operator >>=( const Register & other ) throw();
        inline operator unsigned int() const throw(){
            return this->value;
        }
        std::ostream & operator <<( std::ostream & stream ) const throw();
    };

};



#endif
