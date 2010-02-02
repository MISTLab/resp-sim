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


#ifndef ALIAS_HPP
#define ALIAS_HPP

#include <registers.hpp>
#include <ostream>
#include <list>

#define ACC_MODEL
#define AT_IF
namespace leon3_accat_trap{

    class Alias{
        private:
        PipelineRegister * pipelineReg;
        int pipeId;
        std::list< Alias * > referredAliases;
        Alias * referringAliases;

        public:
        ~Alias();
        Alias( Alias * initAlias, int pipeId = -1 );
        Alias();
        Alias( int pipeId );
        Alias( PipelineRegister * pipelineReg, int pipeId );
        inline InnerField & operator []( int bitField ) throw(){
            return (*this->pipelineReg->getRegister(this->pipeId))[bitField];
        }
        void setPipeId( unsigned int pipeId );
        inline PipelineRegister * getPipeReg() const throw(){
            return this->pipelineReg;
        }
        inline Register * getReg() const throw(){
            return this->pipelineReg->getRegister(this->pipeId);
        }
        inline void lock() throw(){
            this->pipelineReg->lock();
        }
        inline void unlock() throw(){
            this->pipelineReg->unlock();
        }
        inline void unlock( int wbLatency ) throw(){
            this->pipelineReg->unlock(wbLatency);
        }
        inline bool isLocked() throw(){
            return this->pipelineReg->isLocked();
        }
        void writeAll( const unsigned int & value ) throw();
        unsigned int operator ~() throw();
        inline Alias & operator =( const unsigned int & other ) throw(){
            *this->pipelineReg->getRegister(this->pipeId) = other;
            return *this;
        }
        inline Alias & operator +=( const unsigned int & other ) throw(){
            *this->pipelineReg->getRegister(this->pipeId) += other;
            return *this;
        }
        inline Alias & operator -=( const unsigned int & other ) throw(){
            *this->pipelineReg->getRegister(this->pipeId) -= other;
            return *this;
        }
        inline Alias & operator *=( const unsigned int & other ) throw(){
            *this->pipelineReg->getRegister(this->pipeId) *= other;
            return *this;
        }
        inline Alias & operator /=( const unsigned int & other ) throw(){
            *this->pipelineReg->getRegister(this->pipeId) /= other;
            return *this;
        }
        inline Alias & operator |=( const unsigned int & other ) throw(){
            *this->pipelineReg->getRegister(this->pipeId) |= other;
            return *this;
        }
        inline Alias & operator &=( const unsigned int & other ) throw(){
            *this->pipelineReg->getRegister(this->pipeId) &= other;
            return *this;
        }
        inline Alias & operator ^=( const unsigned int & other ) throw(){
            *this->pipelineReg->getRegister(this->pipeId) ^= other;
            return *this;
        }
        inline Alias & operator <<=( const unsigned int & other ) throw(){
            *this->pipelineReg->getRegister(this->pipeId) <<= other;
            return *this;
        }
        inline Alias & operator >>=( const unsigned int & other ) throw(){
            *this->pipelineReg->getRegister(this->pipeId) >>= other;
            return *this;
        }
        Alias & operator =( const Alias & other ) throw();
        Alias & operator +=( const Alias & other ) throw();
        Alias & operator -=( const Alias & other ) throw();
        Alias & operator *=( const Alias & other ) throw();
        Alias & operator /=( const Alias & other ) throw();
        Alias & operator |=( const Alias & other ) throw();
        Alias & operator &=( const Alias & other ) throw();
        Alias & operator ^=( const Alias & other ) throw();
        Alias & operator <<=( const Alias & other ) throw();
        Alias & operator >>=( const Alias & other ) throw();
        inline unsigned int operator +( const Register & other ) const throw(){
            return  (*this->pipelineReg->getRegister(this->pipeId) + other);
        }
        inline unsigned int operator -( const Register & other ) const throw(){
            return  (*this->pipelineReg->getRegister(this->pipeId) - other);
        }
        inline unsigned int operator *( const Register & other ) const throw(){
            return  (*this->pipelineReg->getRegister(this->pipeId) * other);
        }
        inline unsigned int operator /( const Register & other ) const throw(){
            return  (*this->pipelineReg->getRegister(this->pipeId) / other);
        }
        inline unsigned int operator |( const Register & other ) const throw(){
            return  (*this->pipelineReg->getRegister(this->pipeId) | other);
        }
        inline unsigned int operator &( const Register & other ) const throw(){
            return  (*this->pipelineReg->getRegister(this->pipeId) & other);
        }
        inline unsigned int operator ^( const Register & other ) const throw(){
            return  (*this->pipelineReg->getRegister(this->pipeId) ^ other);
        }
        inline unsigned int operator <<( const Register & other ) const throw(){
            return  (*this->pipelineReg->getRegister(this->pipeId) << other);
        }
        inline unsigned int operator >>( const Register & other ) const throw(){
            return  (*this->pipelineReg->getRegister(this->pipeId) >> other);
        }
        bool operator <( const Register & other ) const throw();
        bool operator >( const Register & other ) const throw();
        bool operator <=( const Register & other ) const throw();
        bool operator >=( const Register & other ) const throw();
        bool operator ==( const Register & other ) const throw();
        bool operator !=( const Register & other ) const throw();
        Alias & operator =( const Register & other ) throw();
        Alias & operator +=( const Register & other ) throw();
        Alias & operator -=( const Register & other ) throw();
        Alias & operator *=( const Register & other ) throw();
        Alias & operator /=( const Register & other ) throw();
        Alias & operator |=( const Register & other ) throw();
        Alias & operator &=( const Register & other ) throw();
        Alias & operator ^=( const Register & other ) throw();
        Alias & operator <<=( const Register & other ) throw();
        Alias & operator >>=( const Register & other ) throw();
        inline operator unsigned int() const throw(){
            return *this->pipelineReg->getRegister(this->pipeId);
        }
        std::ostream & operator <<( std::ostream & stream ) const throw();
        void updateAlias( Alias & newAlias ) throw();
        void updateAlias( PipelineRegister & newAlias ) throw();
        inline void propagateAlias( PipelineRegister & newAlias ) throw(){
            if(this->referringAliases != NULL){
                return;
            }
            this->pipelineReg = &newAlias;
            std::list<Alias *>::iterator referredIter, referredEnd;
            for(referredIter = this->referredAliases.begin(), referredEnd = this->referredAliases.end(); \
                referredIter != referredEnd; referredIter++){
                (*referredIter)->newReferredAlias(&newAlias);
            }
        }
        void directSetAlias( Alias & newAlias ) throw();
        void directSetAlias( PipelineRegister & newAlias ) throw();
        inline void newReferredAlias( PipelineRegister * newAlias ) throw(){
            this->pipelineReg = newAlias;
            std::list<Alias *>::iterator referredIter, referredEnd;
            for(referredIter = this->referredAliases.begin(), referredEnd = this->referredAliases.end(); \
                referredIter != referredEnd; referredIter++){
                (*referredIter)->newReferredAlias(newAlias);
            }
        }
    };

};



#endif
