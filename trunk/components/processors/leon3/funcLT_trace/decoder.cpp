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
#include <trap_utils.hpp>

using namespace leon3_trace_trap;
int leon3_trace_trap::Decoder::decode( unsigned int instrCode ) const throw(){
    switch(instrCode & 0x1c00000){
        case 0x0:{
            switch(instrCode & 0xc0000000L){
                case 0xc0000000L:{
                    switch(instrCode & 0x380000){
                        case 0x0:{
                            if((instrCode & 0x2000) != 0x0){
                                // Instruction LD_imm
                                return 8;
                            }
                            else{
                                // Instruction LD_reg
                                return 9;
                            }
                        break;}
                        case 0x200000:{
                            if((instrCode & 0x2000) == 0x0){
                                // Instruction ST_reg
                                return 23;
                            }
                            else{
                                // Instruction ST_imm
                                return 22;
                            }
                        break;}
                        case 0x80000:{
                            if((instrCode & 0x2000) != 0x0){
                                // Instruction LDUB_imm
                                return 4;
                            }
                            else{
                                // Instruction LDUB_reg
                                return 5;
                            }
                        break;}
                        case 0x280000:{
                            if((instrCode & 0x2000) != 0x0){
                                // Instruction STB_imm
                                return 18;
                            }
                            else{
                                // Instruction STB_reg
                                return 19;
                            }
                        break;}
                        case 0x380000:{
                            if((instrCode & 0x2000) == 0x0){
                                // Instruction STD_reg
                                return 25;
                            }
                            else{
                                // Instruction STD_imm
                                return 24;
                            }
                        break;}
                        case 0x100000:{
                            if((instrCode & 0x2000) != 0x0){
                                // Instruction LDUH_imm
                                return 6;
                            }
                            else{
                                // Instruction LDUH_reg
                                return 7;
                            }
                        break;}
                        case 0x300000:{
                            if((instrCode & 0x2000) != 0x0){
                                // Instruction STH_imm
                                return 20;
                            }
                            else{
                                // Instruction STH_reg
                                return 21;
                            }
                        break;}
                        case 0x180000:{
                            if((instrCode & 0x2000) == 0x0){
                                // Instruction LDD_reg
                                return 11;
                            }
                            else{
                                // Instruction LDD_imm
                                return 10;
                            }
                        break;}
                        default:{
                            // Non-valid pattern
                            return 144;
                        }
                    }
                break;}
                case 0x80000000L:{
                    switch(instrCode & 0x380000){
                        case 0x0:{
                            if((instrCode & 0x2000) == 0x0){
                                // Instruction ADD_reg
                                return 68;
                            }
                            else{
                                // Instruction ADD_imm
                                return 67;
                            }
                        break;}
                        case 0x100000:{
                            if((instrCode & 0x2000) != 0x0){
                                // Instruction OR_imm
                                return 45;
                            }
                            else{
                                // Instruction OR_reg
                                return 46;
                            }
                        break;}
                        case 0x180000:{
                            if((instrCode & 0x2000) != 0x0){
                                // Instruction XOR_imm
                                return 53;
                            }
                            else{
                                // Instruction XOR_reg
                                return 54;
                            }
                        break;}
                        case 0x200000:{
                            if((instrCode & 0x2000) == 0x0){
                                // Instruction SUB_reg
                                return 80;
                            }
                            else{
                                // Instruction SUB_imm
                                return 79;
                            }
                        break;}
                        case 0x80000:{
                            if((instrCode & 0x2000) != 0x0){
                                // Instruction AND_imm
                                return 37;
                            }
                            else{
                                // Instruction AND_reg
                                return 38;
                            }
                        break;}
                        case 0x280000:{
                            if((instrCode & 0x2000) == 0x0){
                                // Instruction ANDN_reg
                                return 42;
                            }
                            else{
                                // Instruction ANDN_imm
                                return 41;
                            }
                        break;}
                        case 0x300000:{
                            if((instrCode & 0x2000) != 0x0){
                                // Instruction ORN_imm
                                return 49;
                            }
                            else{
                                // Instruction ORN_reg
                                return 50;
                            }
                        break;}
                        case 0x380000:{
                            if((instrCode & 0x2000) == 0x0){
                                // Instruction XNOR_reg
                                return 58;
                            }
                            else{
                                // Instruction XNOR_imm
                                return 57;
                            }
                        break;}
                        default:{
                            // Non-valid pattern
                            return 144;
                        }
                    }
                break;}
                case 0x40000000:{
                    // Instruction CALL
                    return 118;
                break;}
                case 0x0:{
                    // Instruction UNIMP
                    return 141;
                break;}
                default:{
                    // Non-valid pattern
                    return 144;
                }
            }
        break;}
        case 0x800000:{
            switch(instrCode & 0xc0000000L){
                case 0x80000000L:{
                    switch(instrCode & 0x380000){
                        case 0x200000:{
                            if((instrCode & 0x2000) == 0x0){
                                // Instruction SUBcc_reg
                                return 82;
                            }
                            else{
                                // Instruction SUBcc_imm
                                return 81;
                            }
                        break;}
                        case 0x80000:{
                            if((instrCode & 0x2000) != 0x0){
                                // Instruction ANDcc_imm
                                return 39;
                            }
                            else{
                                // Instruction ANDcc_reg
                                return 40;
                            }
                        break;}
                        case 0x0:{
                            if((instrCode & 0x2000) != 0x0){
                                // Instruction ADDcc_imm
                                return 69;
                            }
                            else{
                                // Instruction ADDcc_reg
                                return 70;
                            }
                        break;}
                        case 0x100000:{
                            if((instrCode & 0x2000) != 0x0){
                                // Instruction ORcc_imm
                                return 47;
                            }
                            else{
                                // Instruction ORcc_reg
                                return 48;
                            }
                        break;}
                        case 0x180000:{
                            if((instrCode & 0x2000) == 0x0){
                                // Instruction XORcc_reg
                                return 56;
                            }
                            else{
                                // Instruction XORcc_imm
                                return 55;
                            }
                        break;}
                        case 0x280000:{
                            if((instrCode & 0x2000) == 0x0){
                                // Instruction ANDNcc_reg
                                return 44;
                            }
                            else{
                                // Instruction ANDNcc_imm
                                return 43;
                            }
                        break;}
                        case 0x300000:{
                            if((instrCode & 0x2000) == 0x0){
                                // Instruction ORNcc_reg
                                return 52;
                            }
                            else{
                                // Instruction ORNcc_imm
                                return 51;
                            }
                        break;}
                        case 0x380000:{
                            if((instrCode & 0x2000) == 0x0){
                                // Instruction XNORcc_reg
                                return 60;
                            }
                            else{
                                // Instruction XNORcc_imm
                                return 59;
                            }
                        break;}
                        default:{
                            // Non-valid pattern
                            return 144;
                        }
                    }
                break;}
                case 0xc0000000L:{
                    switch(instrCode & 0x380000){
                        case 0x200000:{
                            // Instruction STA_reg
                            return 28;
                        break;}
                        case 0x380000:{
                            // Instruction STDA_reg
                            return 29;
                        break;}
                        case 0x180000:{
                            // Instruction LDDA_reg
                            return 17;
                        break;}
                        case 0x80000:{
                            // Instruction LDUBA_reg
                            return 14;
                        break;}
                        case 0x0:{
                            // Instruction LDA_reg
                            return 16;
                        break;}
                        case 0x300000:{
                            // Instruction STHA_reg
                            return 27;
                        break;}
                        case 0x280000:{
                            // Instruction STBA_reg
                            return 26;
                        break;}
                        case 0x100000:{
                            // Instruction LDUHA_reg
                            return 15;
                        break;}
                        default:{
                            // Non-valid pattern
                            return 144;
                        }
                    }
                break;}
                case 0x0:{
                    // Instruction BRANCH
                    return 117;
                break;}
                case 0x40000000:{
                    // Instruction CALL
                    return 118;
                break;}
                default:{
                    // Non-valid pattern
                    return 144;
                }
            }
        break;}
        case 0x400000:{
            switch(instrCode & 0xc0000000L){
                case 0x80000000L:{
                    switch(instrCode & 0x300000){
                        case 0x100000:{
                            if((instrCode & 0x2000) != 0x0){
                                if((instrCode & 0x80000) != 0x0){
                                    // Instruction SMUL_imm
                                    return 95;
                                }
                                else{
                                    // Instruction UMUL_imm
                                    return 93;
                                }
                            }
                            else{
                                if((instrCode & 0x80000) != 0x0){
                                    // Instruction SMUL_reg
                                    return 96;
                                }
                                else{
                                    // Instruction UMUL_reg
                                    return 94;
                                }
                            }
                        break;}
                        case 0x300000:{
                            if((instrCode & 0x2000) != 0x0){
                                if((instrCode & 0x80000) == 0x0){
                                    // Instruction UDIV_imm
                                    return 105;
                                }
                                else{
                                    // Instruction SDIV_imm
                                    return 107;
                                }
                            }
                            else{
                                if((instrCode & 0x80000) != 0x0){
                                    // Instruction SDIV_reg
                                    return 108;
                                }
                                else{
                                    // Instruction UDIV_reg
                                    return 106;
                                }
                            }
                        break;}
                        case 0x200000:{
                            if((instrCode & 0x2000) != 0x0){
                                // Instruction SUBX_imm
                                return 83;
                            }
                            else{
                                // Instruction SUBX_reg
                                return 84;
                            }
                        break;}
                        case 0x0:{
                            if((instrCode & 0x2000) == 0x0){
                                // Instruction ADDX_reg
                                return 72;
                            }
                            else{
                                // Instruction ADDX_imm
                                return 71;
                            }
                        break;}
                        default:{
                            // Non-valid pattern
                            return 144;
                        }
                    }
                break;}
                case 0xc0000000L:{
                    switch(instrCode & 0x300000){
                        case 0x0:{
                            if((instrCode & 0x2000) == 0x0){
                                // Instruction LDSB_reg
                                return 1;
                            }
                            else{
                                // Instruction LDSB_imm
                                return 0;
                            }
                        break;}
                        case 0x100000:{
                            if((instrCode & 0x2000) != 0x0){
                                // Instruction LDSH_imm
                                return 2;
                            }
                            else{
                                // Instruction LDSH_reg
                                return 3;
                            }
                        break;}
                        case 0x200000:{
                            if((instrCode & 0x2000) == 0x0){
                                // Instruction LDSTUB_reg
                                return 31;
                            }
                            else{
                                // Instruction LDSTUB_imm
                                return 30;
                            }
                        break;}
                        case 0x300000:{
                            if((instrCode & 0x2000) == 0x0){
                                // Instruction SWAP_reg
                                return 34;
                            }
                            else{
                                // Instruction SWAP_imm
                                return 33;
                            }
                        break;}
                        default:{
                            // Non-valid pattern
                            return 144;
                        }
                    }
                break;}
                case 0x40000000:{
                    // Instruction CALL
                    return 118;
                break;}
                default:{
                    // Non-valid pattern
                    return 144;
                }
            }
        break;}
        case 0x1000000:{
            switch(instrCode & 0xc0000000L){
                case 0x80000000L:{
                    switch(instrCode & 0x380000){
                        case 0x280000:{
                            if((instrCode & 0x2000) != 0x0){
                                // Instruction SLL_imm
                                return 61;
                            }
                            else{
                                // Instruction SLL_reg
                                return 62;
                            }
                        break;}
                        case 0x380000:{
                            if((instrCode & 0x2000) != 0x0){
                                // Instruction SRA_imm
                                return 65;
                            }
                            else{
                                // Instruction SRA_reg
                                return 66;
                            }
                        break;}
                        case 0x300000:{
                            if((instrCode & 0x2000) != 0x0){
                                // Instruction SRL_imm
                                return 63;
                            }
                            else{
                                // Instruction SRL_reg
                                return 64;
                            }
                        break;}
                        case 0x200000:{
                            if((instrCode & 0x2000) == 0x0){
                                // Instruction MULScc_reg
                                return 92;
                            }
                            else{
                                // Instruction MULScc_imm
                                return 91;
                            }
                        break;}
                        case 0x0:{
                            if((instrCode & 0x2000) == 0x0){
                                // Instruction TADDcc_reg
                                return 76;
                            }
                            else{
                                // Instruction TADDcc_imm
                                return 75;
                            }
                        break;}
                        case 0x180000:{
                            if((instrCode & 0x2000) == 0x0){
                                // Instruction TSUBccTV_reg
                                return 90;
                            }
                            else{
                                // Instruction TSUBccTV_imm
                                return 89;
                            }
                        break;}
                        case 0x100000:{
                            if((instrCode & 0x2000) != 0x0){
                                // Instruction TADDccTV_imm
                                return 77;
                            }
                            else{
                                // Instruction TADDccTV_reg
                                return 78;
                            }
                        break;}
                        case 0x80000:{
                            if((instrCode & 0x2000) == 0x0){
                                // Instruction TSUBcc_reg
                                return 88;
                            }
                            else{
                                // Instruction TSUBcc_imm
                                return 87;
                            }
                        break;}
                        default:{
                            // Non-valid pattern
                            return 144;
                        }
                    }
                break;}
                case 0x0:{
                    // Instruction SETHI
                    return 36;
                break;}
                case 0x40000000:{
                    // Instruction CALL
                    return 118;
                break;}
                default:{
                    // Non-valid pattern
                    return 144;
                }
            }
        break;}
        case 0x1c00000:{
            switch(instrCode & 0xc0000000L){
                case 0x80000000L:{
                    switch(instrCode & 0x380000){
                        case 0x200000:{
                            if((instrCode & 0x2000) != 0x0){
                                // Instruction SAVE_imm
                                return 113;
                            }
                            else{
                                // Instruction SAVE_reg
                                return 114;
                            }
                        break;}
                        case 0x280000:{
                            if((instrCode & 0x2000) == 0x0){
                                // Instruction RESTORE_reg
                                return 116;
                            }
                            else{
                                // Instruction RESTORE_imm
                                return 115;
                            }
                        break;}
                        case 0x0:{
                            if((instrCode & 0x2000) != 0x0){
                                // Instruction JUMP_imm
                                return 119;
                            }
                            else{
                                // Instruction JUMP_reg
                                return 120;
                            }
                        break;}
                        case 0x100000:{
                            if((instrCode & 0x2000) == 0x0){
                                // Instruction TRAP_reg
                                return 124;
                            }
                            else{
                                // Instruction TRAP_imm
                                return 123;
                            }
                        break;}
                        case 0x80000:{
                            if((instrCode & 0x2000) != 0x0){
                                // Instruction RETT_imm
                                return 121;
                            }
                            else{
                                // Instruction RETT_reg
                                return 122;
                            }
                        break;}
                        case 0x300000:{
                            if((instrCode & 0x2000) == 0x0){
                                // Instruction UMAC_reg
                                return 102;
                            }
                            else{
                                // Instruction UMAC_imm
                                return 101;
                            }
                        break;}
                        case 0x180000:{
                            if((instrCode & 0x2000) == 0x0){
                                // Instruction FLUSH_reg
                                return 142;
                            }
                            else{
                                // Instruction FLUSH_imm
                                return 143;
                            }
                        break;}
                        case 0x380000:{
                            if((instrCode & 0x2000) != 0x0){
                                // Instruction SMAC_imm
                                return 103;
                            }
                            else{
                                // Instruction SMAC_reg
                                return 104;
                            }
                        break;}
                        default:{
                            // Non-valid pattern
                            return 144;
                        }
                    }
                break;}
                case 0x40000000:{
                    // Instruction CALL
                    return 118;
                break;}
                default:{
                    // Non-valid pattern
                    return 144;
                }
            }
        break;}
        case 0xc00000:{
            switch(instrCode & 0xc0000000L){
                case 0x80000000L:{
                    switch(instrCode & 0x300000){
                        case 0x300000:{
                            if((instrCode & 0x2000) != 0x0){
                                if((instrCode & 0x80000) == 0x0){
                                    // Instruction UDIVcc_imm
                                    return 109;
                                }
                                else{
                                    // Instruction SDIVcc_imm
                                    return 111;
                                }
                            }
                            else{
                                if((instrCode & 0x80000) == 0x0){
                                    // Instruction UDIVcc_reg
                                    return 110;
                                }
                                else{
                                    // Instruction SDIVcc_reg
                                    return 112;
                                }
                            }
                        break;}
                        case 0x100000:{
                            if((instrCode & 0x2000) == 0x0){
                                if((instrCode & 0x80000) == 0x0){
                                    // Instruction UMULcc_reg
                                    return 98;
                                }
                                else{
                                    // Instruction SMULcc_reg
                                    return 100;
                                }
                            }
                            else{
                                if((instrCode & 0x80000) != 0x0){
                                    // Instruction SMULcc_imm
                                    return 99;
                                }
                                else{
                                    // Instruction UMULcc_imm
                                    return 97;
                                }
                            }
                        break;}
                        case 0x200000:{
                            if((instrCode & 0x2000) == 0x0){
                                // Instruction SUBXcc_reg
                                return 86;
                            }
                            else{
                                // Instruction SUBXcc_imm
                                return 85;
                            }
                        break;}
                        case 0x0:{
                            if((instrCode & 0x2000) != 0x0){
                                // Instruction ADDXcc_imm
                                return 73;
                            }
                            else{
                                // Instruction ADDXcc_reg
                                return 74;
                            }
                        break;}
                        default:{
                            // Non-valid pattern
                            return 144;
                        }
                    }
                break;}
                case 0x40000000:{
                    // Instruction CALL
                    return 118;
                break;}
                case 0xc0000000L:{
                    if((instrCode & 0x100000) == 0x0){
                        if((instrCode & 0x200000) != 0x0){
                            // Instruction LDSTUBA_reg
                            return 32;
                        }
                        else{
                            // Instruction LDSBA_reg
                            return 12;
                        }
                    }
                    else{
                        if((instrCode & 0x80000) == 0x0){
                            // Instruction LDSHA_reg
                            return 13;
                        }
                        else{
                            // Instruction SWAPA_reg
                            return 35;
                        }
                    }
                break;}
                default:{
                    // Non-valid pattern
                    return 144;
                }
            }
        break;}
        case 0x1800000:{
            switch(instrCode & 0xc0000000L){
                case 0x80000000L:{
                    switch(instrCode & 0x180000){
                        case 0x80000:{
                            if((instrCode & 0x2000) == 0x0){
                                // Instruction WRITEpsr_reg
                                return 134;
                            }
                            else{
                                // Instruction WRITEpsr_imm
                                return 135;
                            }
                        break;}
                        case 0x180000:{
                            if((instrCode & 0x2000) != 0x0){
                                // Instruction WRITEtbr_imm
                                return 139;
                            }
                            else{
                                // Instruction WRITEtbr_reg
                                return 138;
                            }
                        break;}
                        case 0x0:{
                            if((instrCode & 0x2000) == 0x0){
                                if((instrCode & 0xfff83fe0L) == 0x81800000L ){
                                    // Instruction WRITEY_reg
                                    return 130;
                                }
                                // Instruction WRITEasr_reg
                                return 132;
                            }
                            else{
                                if((instrCode & 0xfff82000L) == 0x81802000L ){
                                    // Instruction WRITEY_imm
                                    return 131;
                                }
                                // Instruction WRITEasr_imm
                                return 133;
                            }
                        break;}
                        case 0x100000:{
                            if((instrCode & 0x2000) != 0x0){
                                // Instruction WRITEwim_imm
                                return 137;
                            }
                            else{
                                // Instruction WRITEwim_reg
                                return 136;
                            }
                        break;}
                        default:{
                            // Non-valid pattern
                            return 144;
                        }
                    }
                break;}
                case 0x40000000:{
                    // Instruction CALL
                    return 118;
                break;}
                default:{
                    // Non-valid pattern
                    return 144;
                }
            }
        break;}
        case 0x1400000:{
            if((instrCode & 0x40000000) == 0x0){
                if((instrCode & 0x80000) != 0x0){
                    if((instrCode & 0x100000) == 0x0){
                        // Instruction READpsr
                        return 127;
                    }
                    else{
                        // Instruction READtbr
                        return 129;
                    }
                }
                else{
                    if((instrCode & 0x100000) == 0x0){
                        if((instrCode & 0xc1ffffffL) == 0x81400000L ){
                            // Instruction READy
                            return 125;
                        }
                        if((instrCode & 0xffffffffL) == 0x8143c000L ){
                            // Instruction STBAR
                            return 140;
                        }
                        // Instruction READasr
                        return 126;
                    }
                    else{
                        // Instruction READwim
                        return 128;
                    }
                }
            }
            else{
                // Instruction CALL
                return 118;
            }
        break;}
        default:{
            // Non-valid pattern
            return 144;
        }
    }
}


