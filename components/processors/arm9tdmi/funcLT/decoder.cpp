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



#include <decoder.hpp>
#include <instructions.hpp>

using namespace arm9tdmi_funclt_trap;
arm9tdmi_funclt_trap::CacheElem::CacheElem( Instruction * instr, unsigned int count \
    ) : instr(instr), count(count){

}

arm9tdmi_funclt_trap::CacheElem::CacheElem() : instr(NULL), count(1){

}

int arm9tdmi_funclt_trap::Decoder::decode( unsigned int instrCode ) const throw(){
    switch(instrCode & 0xe000000){
        case 0x0:{
            switch(instrCode & 0x3000000){
                case 0x1000000:{
                    switch(instrCode & 0x1800000){
                        case 0x1000000:{
                            switch(instrCode & 0x300000){
                                case 0x100000:{
                                    if((instrCode & 0x10) != 0x0){
                                        if((instrCode & 0x80) != 0x80){
                                            if((instrCode & 0x400000) != 0x0){
                                                // Instruction CMP_sr
                                                return 18;
                                            }
                                            else{
                                                // Instruction TST_sr
                                                return 65;
                                            }
                                        }
                                        else{
                                            if((instrCode & 0x40) != 0x0){
                                                if((instrCode & 0x20) != 0x0){
                                                    // Instruction LDRSH_off
                                                    return 29;
                                                }
                                                else{
                                                    // Instruction LDRSB_off
                                                    return 30;
                                                }
                                            }
                                            else{
                                                // Instruction LDRH_off
                                                return 28;
                                            }
                                        }
                                    }
                                    else{
                                        if((instrCode & 0x400000) != 0x0){
                                            // Instruction CMP_si
                                            return 17;
                                        }
                                        else{
                                            // Instruction TST_si
                                            return 64;
                                        }
                                    }
                                break;}
                                case 0x300000:{
                                    if((instrCode & 0xd0) != 0x90){
                                        if((instrCode & 0x400010) != 0x0){
                                            if((instrCode & 0x10) != 0x0){
                                                if((instrCode & 0x80) == 0x0){
                                                    if((instrCode & 0x400000) == 0x0){
                                                        // Instruction TEQ_sr
                                                        return 62;
                                                    }
                                                    else{
                                                        // Instruction CMN_sr
                                                        return 15;
                                                    }
                                                }
                                                else{
                                                    if((instrCode & 0x20) != 0x0){
                                                        // Instruction LDRSH_off
                                                        return 29;
                                                    }
                                                    else{
                                                        // Instruction LDRSB_off
                                                        return 30;
                                                    }
                                                }
                                            }
                                            else{
                                                // Instruction CMN_si
                                                return 14;
                                            }
                                        }
                                        else{
                                            // Instruction TEQ_si
                                            return 61;
                                        }
                                    }
                                    else{
                                        // Instruction LDRH_off
                                        return 28;
                                    }
                                break;}
                                case 0x200000:{
                                    if((instrCode & 0x10) != 0x0){
                                        if((instrCode & 0x60) != 0x0){
                                            if((instrCode & 0x20) != 0x0){
                                                if((instrCode & 0x40) != 0x0){
                                                    if((instrCode & 0x80) != 0x0){
                                                        // Instruction LDRSH_off
                                                        return 29;
                                                    }
                                                    else{
                                                        // Instruction BKPT
                                                        return 89;
                                                    }
                                                }
                                                else{
                                                    // Instruction STRH_off
                                                    return 72;
                                                }
                                            }
                                            else{
                                                // Instruction LDRSB_off
                                                return 30;
                                            }
                                        }
                                        else{
                                            // Instruction BRANCHX
                                            return 10;
                                        }
                                    }
                                    else{
                                        // Instruction msr_reg_Instr
                                        return 42;
                                    }
                                break;}
                                case 0x0:{
                                    if((instrCode & 0x20) == 0x0){
                                        if((instrCode & 0x10) != 0x0){
                                            if((instrCode & 0x40) == 0x0){
                                                if((instrCode & 0x400000) == 0x0){
                                                    // Instruction SWAP
                                                    return 73;
                                                }
                                                else{
                                                    // Instruction SWAPB
                                                    return 74;
                                                }
                                            }
                                            else{
                                                // Instruction LDRSB_off
                                                return 30;
                                            }
                                        }
                                        else{
                                            // Instruction mrs_Instr
                                            return 40;
                                        }
                                    }
                                    else{
                                        if((instrCode & 0x40) != 0x0){
                                            // Instruction LDRSH_off
                                            return 29;
                                        }
                                        else{
                                            // Instruction STRH_off
                                            return 72;
                                        }
                                    }
                                break;}
                                default:{
                                    // Non-valid pattern
                                    return 90;
                                }
                            }
                        break;}
                        case 0x1800000:{
                            if((instrCode & 0x600010) != 0x200000){
                                if((instrCode & 0x400010) != 0x0){
                                    if((instrCode & 0x200010) != 0x200000){
                                        if((instrCode & 0x10) != 0x0){
                                            if((instrCode & 0x600080) != 0x200000){
                                                if((instrCode & 0x400080) != 0x0){
                                                    if((instrCode & 0x80) == 0x80){
                                                        if((instrCode & 0x40) == 0x0){
                                                            if((instrCode & 0x100000) != 0x0){
                                                                // Instruction LDRH_off
                                                                return 28;
                                                            }
                                                            else{
                                                                // Instruction STRH_off
                                                                return 72;
                                                            }
                                                        }
                                                        else{
                                                            if((instrCode & 0x20) != 0x0){
                                                                // Instruction LDRSH_off
                                                                return 29;
                                                            }
                                                            else{
                                                                // Instruction LDRSB_off
                                                                return 30;
                                                            }
                                                        }
                                                    }
                                                    else{
                                                        if((instrCode & 0x200000) != 0x0){
                                                            // Instruction MVN_sr
                                                            return 44;
                                                        }
                                                        else{
                                                            // Instruction BIC_sr
                                                            return 12;
                                                        }
                                                    }
                                                }
                                                else{
                                                    // Instruction ORR_sr
                                                    return 47;
                                                }
                                            }
                                            else{
                                                // Instruction MOV_sr
                                                return 38;
                                            }
                                        }
                                        else{
                                            // Instruction BIC_si
                                            return 11;
                                        }
                                    }
                                    else{
                                        // Instruction MVN_si
                                        return 43;
                                    }
                                }
                                else{
                                    // Instruction ORR_si
                                    return 46;
                                }
                            }
                            else{
                                // Instruction MOV_si
                                return 37;
                            }
                        break;}
                        case 0x800000:{
                            if((instrCode & 0x40) == 0x0){
                                if((instrCode & 0x100000) != 0x0){
                                    // Instruction LDRH_off
                                    return 28;
                                }
                                else{
                                    // Instruction STRH_off
                                    return 72;
                                }
                            }
                            else{
                                if((instrCode & 0x20) != 0x0){
                                    // Instruction LDRSH_off
                                    return 29;
                                }
                                else{
                                    // Instruction LDRSB_off
                                    return 30;
                                }
                            }
                        break;}
                        case 0x0:{
                            if((instrCode & 0x40) == 0x0){
                                if((instrCode & 0x100000) != 0x0){
                                    // Instruction LDRH_off
                                    return 28;
                                }
                                else{
                                    // Instruction STRH_off
                                    return 72;
                                }
                            }
                            else{
                                if((instrCode & 0x20) != 0x0){
                                    // Instruction LDRSH_off
                                    return 29;
                                }
                                else{
                                    // Instruction LDRSB_off
                                    return 30;
                                }
                            }
                        break;}
                        default:{
                            // Non-valid pattern
                            return 90;
                        }
                    }
                break;}
                case 0x0:{
                    if((instrCode & 0xe00010) != 0x0){
                        if((instrCode & 0xe00010) != 0x800000){
                            if((instrCode & 0xa00010) != 0x0){
                                if((instrCode & 0x200010) != 0x0){
                                    if((instrCode & 0xc00010) != 0x800000){
                                        if((instrCode & 0x400010) != 0x0){
                                            if((instrCode & 0x800010) != 0x0){
                                                if((instrCode & 0x10) != 0x0){
                                                    switch(instrCode & 0xc0){
                                                        case 0x0:{
                                                            switch(instrCode & 0xe00000){
                                                                case 0x0:{
                                                                    // Instruction AND_sr
                                                                    return 7;
                                                                break;}
                                                                case 0x800000:{
                                                                    // Instruction ADD_sr
                                                                    return 4;
                                                                break;}
                                                                case 0xa00000:{
                                                                    // Instruction ADC_sr
                                                                    return 1;
                                                                break;}
                                                                case 0x400000:{
                                                                    // Instruction SUB_sr
                                                                    return 59;
                                                                break;}
                                                                case 0x200000:{
                                                                    // Instruction EOR_sr
                                                                    return 21;
                                                                break;}
                                                                case 0xe00000:{
                                                                    // Instruction RSC_sr
                                                                    return 53;
                                                                break;}
                                                                case 0x600000:{
                                                                    // Instruction RSB_sr
                                                                    return 50;
                                                                break;}
                                                                case 0xc00000:{
                                                                    // Instruction SBC_sr
                                                                    return 56;
                                                                break;}
                                                                default:{
                                                                    // Non-valid pattern
                                                                    return 90;
                                                                }
                                                            }
                                                        break;}
                                                        case 0x40:{
                                                            switch(instrCode & 0xe00000){
                                                                case 0x0:{
                                                                    // Instruction AND_sr
                                                                    return 7;
                                                                break;}
                                                                case 0x800000:{
                                                                    // Instruction ADD_sr
                                                                    return 4;
                                                                break;}
                                                                case 0xa00000:{
                                                                    // Instruction ADC_sr
                                                                    return 1;
                                                                break;}
                                                                case 0x400000:{
                                                                    // Instruction SUB_sr
                                                                    return 59;
                                                                break;}
                                                                case 0x200000:{
                                                                    // Instruction EOR_sr
                                                                    return 21;
                                                                break;}
                                                                case 0xe00000:{
                                                                    // Instruction RSC_sr
                                                                    return 53;
                                                                break;}
                                                                case 0x600000:{
                                                                    // Instruction RSB_sr
                                                                    return 50;
                                                                break;}
                                                                case 0xc00000:{
                                                                    // Instruction SBC_sr
                                                                    return 56;
                                                                break;}
                                                                default:{
                                                                    // Non-valid pattern
                                                                    return 90;
                                                                }
                                                            }
                                                        break;}
                                                        case 0x80:{
                                                            if((instrCode & 0x400020) == 0x0){
                                                                if((instrCode & 0x200000) == 0x0){
                                                                    if((instrCode & 0x800000) == 0x0){
                                                                        // Instruction mul_Instr
                                                                        return 32;
                                                                    }
                                                                    else{
                                                                        // Instruction umull_Instr
                                                                        return 36;
                                                                    }
                                                                }
                                                                else{
                                                                    if((instrCode & 0x800000) == 0x0){
                                                                        // Instruction mla_Instr
                                                                        return 31;
                                                                    }
                                                                    else{
                                                                        // Instruction umlal_Instr
                                                                        return 35;
                                                                    }
                                                                }
                                                            }
                                                            else{
                                                                if((instrCode & 0x20) == 0x0){
                                                                    if((instrCode & 0x200000) == 0x0){
                                                                        // Instruction smull_Instr
                                                                        return 34;
                                                                    }
                                                                    else{
                                                                        // Instruction smlal_Instr
                                                                        return 33;
                                                                    }
                                                                }
                                                                else{
                                                                    if((instrCode & 0x100000) != 0x0){
                                                                        // Instruction LDRH_off
                                                                        return 28;
                                                                    }
                                                                    else{
                                                                        // Instruction STRH_off
                                                                        return 72;
                                                                    }
                                                                }
                                                            }
                                                        break;}
                                                        case 0xc0:{
                                                            if((instrCode & 0x20) != 0x0){
                                                                // Instruction LDRSH_off
                                                                return 29;
                                                            }
                                                            else{
                                                                // Instruction LDRSB_off
                                                                return 30;
                                                            }
                                                        break;}
                                                        default:{
                                                            // Non-valid pattern
                                                            return 90;
                                                        }
                                                    }
                                                }
                                                else{
                                                    // Instruction RSC_si
                                                    return 52;
                                                }
                                            }
                                            else{
                                                // Instruction RSB_si
                                                return 49;
                                            }
                                        }
                                        else{
                                            // Instruction EOR_si
                                            return 20;
                                        }
                                    }
                                    else{
                                        // Instruction ADC_si
                                        return 0;
                                    }
                                }
                                else{
                                    // Instruction SBC_si
                                    return 55;
                                }
                            }
                            else{
                                // Instruction SUB_si
                                return 58;
                            }
                        }
                        else{
                            // Instruction ADD_si
                            return 3;
                        }
                    }
                    else{
                        // Instruction AND_si
                        return 6;
                    }
                break;}
                default:{
                    // Non-valid pattern
                    return 90;
                }
            }
        break;}
        case 0x2000000:{
            switch(instrCode & 0x1800000){
                case 0x1000000:{
                    if((instrCode & 0x100000) != 0x0){
                        if((instrCode & 0x200000) == 0x0){
                            if((instrCode & 0x400000) != 0x0){
                                // Instruction CMP_i
                                return 19;
                            }
                            else{
                                // Instruction TST_i
                                return 66;
                            }
                        }
                        else{
                            if((instrCode & 0x400000) == 0x0){
                                // Instruction TEQ_i
                                return 63;
                            }
                            else{
                                // Instruction CMN_i
                                return 16;
                            }
                        }
                    }
                    else{
                        // Instruction msr_imm_Instr
                        return 41;
                    }
                break;}
                case 0x1800000:{
                    if((instrCode & 0x200000) != 0x0){
                        if((instrCode & 0x400000) == 0x0){
                            // Instruction MOV_i
                            return 39;
                        }
                        else{
                            // Instruction MVN_i
                            return 45;
                        }
                    }
                    else{
                        if((instrCode & 0x400000) == 0x0){
                            // Instruction ORR_i
                            return 48;
                        }
                        else{
                            // Instruction BIC_i
                            return 13;
                        }
                    }
                break;}
                case 0x800000:{
                    if((instrCode & 0x200000) == 0x0){
                        if((instrCode & 0x400000) == 0x0){
                            // Instruction ADD_i
                            return 5;
                        }
                        else{
                            // Instruction SBC_i
                            return 57;
                        }
                    }
                    else{
                        if((instrCode & 0x400000) == 0x0){
                            // Instruction ADC_i
                            return 2;
                        }
                        else{
                            // Instruction RSC_i
                            return 54;
                        }
                    }
                break;}
                case 0x0:{
                    if((instrCode & 0x200000) != 0x200000){
                        if((instrCode & 0x400000) == 0x0){
                            // Instruction AND_i
                            return 8;
                        }
                        else{
                            // Instruction SUB_i
                            return 60;
                        }
                    }
                    else{
                        if((instrCode & 0x400000) == 0x0){
                            // Instruction EOR_i
                            return 22;
                        }
                        else{
                            // Instruction RSB_i
                            return 51;
                        }
                    }
                break;}
                default:{
                    // Non-valid pattern
                    return 90;
                }
            }
        break;}
        case 0x4000000:{
            if((instrCode & 0x100000) == 0x0){
                if((instrCode & 0x400000) == 0x0){
                    if((instrCode & 0xf700000) == 0x4200000 ){
                        // Instruction STRT_imm
                        return 81;
                    }
                    // Instruction STR_imm
                    return 68;
                }
                else{
                    if((instrCode & 0xf700000) == 0x4600000 ){
                        // Instruction STRBT_imm
                        return 79;
                    }
                    // Instruction STRB_imm
                    return 70;
                }
            }
            else{
                if((instrCode & 0x400000) == 0x0){
                    if((instrCode & 0xf700000) == 0x4300000 ){
                        // Instruction LDRT_imm
                        return 77;
                    }
                    // Instruction LDR_imm
                    return 24;
                }
                else{
                    if((instrCode & 0xf700000) == 0x4700000 ){
                        // Instruction LDRBT_imm
                        return 75;
                    }
                    // Instruction LDRB_imm
                    return 26;
                }
            }
        break;}
        case 0x6000000:{
            if((instrCode & 0x100000) == 0x0){
                if((instrCode & 0x400000) == 0x0){
                    if((instrCode & 0xf700010) == 0x6200000 ){
                        // Instruction STRT_off
                        return 82;
                    }
                    // Instruction STR_off
                    return 69;
                }
                else{
                    if((instrCode & 0xf700010) == 0x6600000 ){
                        // Instruction STRBT_off
                        return 80;
                    }
                    // Instruction STRB_off
                    return 71;
                }
            }
            else{
                if((instrCode & 0x400000) == 0x0){
                    if((instrCode & 0xf700010) == 0x6300000 ){
                        // Instruction LDRT_off
                        return 78;
                    }
                    // Instruction LDR_off
                    return 25;
                }
                else{
                    if((instrCode & 0xf700010) == 0x6700000 ){
                        // Instruction LDRBT_off
                        return 76;
                    }
                    // Instruction LDRB_off
                    return 27;
                }
            }
        break;}
        case 0x8000000:{
            if((instrCode & 0x100000) == 0x0){
                // Instruction STM
                return 67;
            }
            else{
                // Instruction LDM
                return 23;
            }
        break;}
        case 0xa000000:{
            // Instruction BRANCH
            return 9;
        break;}
        case 0xe000000:{
            if((instrCode & 0x1000000) != 0x1000000){
                if((instrCode & 0x10) != 0x0){
                    if((instrCode & 0x100000) != 0x0){
                        // Instruction MRC
                        return 86;
                    }
                    else{
                        // Instruction MCR
                        return 85;
                    }
                }
                else{
                    // Instruction CDP
                    return 87;
                }
            }
            else{
                // Instruction SWI
                return 88;
            }
        break;}
        case 0xc000000:{
            if((instrCode & 0x100000) != 0x0){
                // Instruction LDC
                return 83;
            }
            else{
                // Instruction STC
                return 84;
            }
        break;}
        default:{
            // Non-valid pattern
            return 90;
        }
    }
}


