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


#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include <systemc.h>
#include <customExceptions.hpp>
#include <instructions.hpp>
#include <decoder.hpp>
#include <interface.hpp>
#include <ToolsIf.hpp>
#include <tlm_utils/tlm_quantumkeeper.h>
#include <registers.hpp>
#include <alias.hpp>
#include <externalPorts.hpp>
#ifdef __GNUC__
#ifdef __GNUC_MINOR__
#if (__GNUC__ >= 4 && __GNUC_MINOR__ >= 3)
#include <tr1/unordered_map>
#define template_map std::tr1::unordered_map
#else
#include <ext/hash_map>
#define  template_map __gnu_cxx::hash_map
#endif
#else
#include <ext/hash_map>
#define  template_map __gnu_cxx::hash_map
#endif
#else
#ifdef _WIN32
#include <hash_map>
#define  template_map stdext::hash_map
#else
#include <map>
#define  template_map std::map
#endif
#endif


#define FUNC_MODEL
#define LT_IF
using namespace trap;
namespace arm7tdmi_funclt_trap{

    class ARM7Processor : public sc_module{
        private:
        Decoder decoder;
        tlm_utils::tlm_quantumkeeper quantKeeper;
        bool instrExecuting;
        sc_event instrEndEvent;
        static Instruction * * INSTRUCTIONS;
        template_map< unsigned int, CacheElem > instrCache;
        static int numInstances;

        public:
        SC_HAS_PROCESS( ARM7Processor );
        ARM7Processor( sc_module_name name, sc_time latency );
        void mainLoop();
        void resetOp();
        void end_of_elaboration();
        Instruction * decode( unsigned int bitString );
        ARM7TDMI_ABIIf * abiIf;
        ARM7TDMI_ABIIf & getInterface();
        ToolsManager< unsigned int > toolManager;
        Reg32_0 CPSR;
        Reg32_1 MP_ID;
        Reg32_1 RB[30];
        Reg32_0 SPSR[5];
        Alias FP;
        Alias SPTR;
        Alias LINKR;
        Alias SP_IRQ;
        Alias LR_IRQ;
        Alias SP_FIQ;
        Alias LR_FIQ;
        Alias PC;
        Alias REGS[16];
        TLMMemory instrMem;
        TLMMemory dataMem;
        sc_time latency;
        unsigned int numInstructions;
        unsigned int ENTRY_POINT;
        unsigned int PROGRAM_LIMIT;
        unsigned int PROGRAM_START;
        ~ARM7Processor();
    };

};



#endif
