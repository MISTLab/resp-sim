# -*- coding: iso-8859-1 -*-
##############################################################################
#
#
#         ___           ___           ___           ___
#        /  /\         /  /\         /  /\         /  /\
#       /  /::\       /  /:/_       /  /:/_       /  /::\
#      /  /:/\:\     /  /:/ /\     /  /:/ /\     /  /:/\:\
#     /  /:/~/:/    /  /:/ /:/_   /  /:/ /::\   /  /:/~/:/
#    /__/:/ /:/___ /__/:/ /:/ /\ /__/:/ /:/\:\ /__/:/ /:/
#    \  \:\/:::::/ \  \:\/:/ /:/ \  \:\/:/~/:/ \  \:\/:/
#     \  \::/~~~~   \  \::/ /:/   \  \::/ /:/   \  \::/
#      \  \:\        \  \:\/:/     \__\/ /:/     \  \:\
#       \  \:\        \  \::/        /__/:/       \  \:\
#        \__\/         \__\/         \__\/         \__\/
#
#
#
#
#   This file is part of ReSP.
#
#   TRAP is free software; you can redistribute it and/or modify
#   it under the terms of the GNU Lesser General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public License
#   along with this program; if not, write to the
#   Free Software Foundation, Inc.,
#   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#   or see <http://www.gnu.org/licenses/>.
#
#
#
#   (c) Giovanni Beltrame, Luca Fossati
#       Giovanni.Beltrame@esa.int fossati@elet.polimi.it
#
##############################################################################

class ClassAttr:
    def __init__(self):
        self.name = ''
        self.instance = None
        self.typeName = ''
        self.pyAttrType = None
        self.path = []

    def __repr__(self):
        representation = 'Name:@' + self.name + '@'
        representation += 'Type:@' + str(self.typeName) + '@'
        representation += 'Path:@' + str(self.path) + '@'
        return representation

    def __str__(self):
        from hci import console
        from hci import colors
        lineWidth = 90
        representation = colors.colorMap['bright_red'] + 'Name: ' + colors.colorMap['none'] + colors.colorMap['cyan'] + self.name + colors.colorMap['none']
        representation += colors.colorMap['bright_red'] + ' type: ' + colors.colorMap['none'] + colors.colorMap['cyan']  + str(self.typeName) + colors.colorMap['none']
        if len(self.path) > 0:
            representation += colors.colorMap['bright_red'] + ' Path: ' + colors.colorMap['none'] + colors.colorMap['cyan']  + str(self.path) + colors.colorMap['none'] + ' '
        else:
            representation += ' '
        return console.go_new_line(representation, lineWidth, 3)


class ClassMethod:
    def __init__(self):
        self.name = ''
        self.instance = None
        self.retTypeName = ''
        self.pyRetType = None
        self.path = []

    def __repr__(self):
        representation = 'Name:@' + self.name + '@'
        representation += 'RetVal:@' + str(self.retTypeName) + '@'
        representation += 'Path:@' + str(self.path) + '@'
        return representation

    def __str__(self):
        from hci import console
        from hci import colors
        lineWidth = 90
        representation = colors.colorMap['bright_red'] + 'Name: ' + colors.colorMap['none'] + colors.colorMap['cyan'] + self.name + colors.colorMap['none']
        representation += colors.colorMap['bright_red'] + ' Return value type: ' + colors.colorMap['none'] + colors.colorMap['cyan'] + str(self.retTypeName) + colors.colorMap['none']
        if len(self.path) > 0:
            representation += colors.colorMap['bright_red'] + ' Path: ' + colors.colorMap['none'] + colors.colorMap['cyan'] + str(self.path) + colors.colorMap['none'] + ' '
        else:
            representation += ' '
        return console.go_new_line(representation, lineWidth, 3)

class Component:
    def __init__(self):
        self.module = None
        self.classs = None

        #Lists the types of the parameters of the constructor; in case one parameters has a default value,
        #then a tuple is used and the second element is the default value
        self.constructor = []

        self.in_tlm_ports = []
        self.out_tlm_ports = []
        self.out_sysc_ports = []
        self.in_sysc_ports = []
        self.inout_sysc_ports = []
        self.sysc_signals = []

        self.in_tlm_port_gens = []
        self.out_tlm_port_gens = []
        self.out_sysc_port_gens = []
        self.in_sysc_port_gens = []
        self.inout_sysc_port_gens = []
        self.sysc_signal_gens = []

    def getName(self):
        return self.module.__name__.lower()

    def __repr__(self):
        """Returns a formal representation of this class"""
        if self.module and self.classs:
            representation = 'Component:@' + self.module.__name__ + '@Class:@' + self.classs.__name__ + '@\n'

            representation += 'Constructor:' + repr(self.constructor).replace('[', '\\[').replace(']', '\\]') + '\n'
            representation += 'In_TLM_ports:@' + str(len(self.in_tlm_ports)) + '@' + repr(self.in_tlm_ports).replace('[', '\\[').replace(']', '\\]') + '\n'
            representation += 'Out_TLM_ports:@' + str(len(self.out_tlm_ports)) + '@' + repr(self.out_tlm_ports).replace('[', '\\[').replace(']', '\\]') + '\n'
            representation += 'In_SystemC_ports:@' + str(len(self.in_sysc_ports)) + '@' + repr(self.in_sysc_ports).replace('[', '\\[').replace(']', '\\]') + '\n'
            representation += 'Out_SystemC_ports:@' + str(len(self.out_sysc_ports)) + '@' + repr(self.out_sysc_ports).replace('[', '\\[').replace(']', '\\]') + '\n'
            representation += 'InOut_SystemC_ports:@' + str(len(self.inout_sysc_ports)) + '@' + repr(self.inout_sysc_ports).replace('[', '\\[').replace(']', '\\]') + '\n'
            representation += 'SystemC_signals:@' + str(len(self.sysc_signals)) + '@' + repr(self.sysc_signals).replace('[', '\\[').replace(']', '\\]') + '\n'

            representation += 'In_TLM_port_gen:@' + str(len(self.in_tlm_port_gens)) + '@' + repr(self.in_tlm_port_gens).replace('[', '\\[').replace(']', '\\]') + '\n'
            representation += 'Out_TLM_port_gen:@' + str(len(self.out_tlm_port_gens)) + '@' + repr(self.out_tlm_port_gens).replace('[', '\\[').replace(']', '\\]') + '\n'
            representation += 'In_SystemC_port_gen:@' + str(len(self.in_sysc_port_gens)) + '@' + repr(self.in_sysc_port_gens).replace('[', '\\[').replace(']', '\\]') + '\n'
            representation += 'Out_SystemC_port_gen:@' + str(len(self.out_sysc_port_gens)) + '@' + repr(self.out_sysc_port_gens).replace('[', '\\[').replace(']', '\\]') + '\n'
            representation += 'InOut_SystemC_port_gen:@' + str(len(self.inout_sysc_port_gens)) + '@' + repr(self.inout_sysc_port_gens).replace('[', '\\[').replace(']', '\\]') + '\n'
            representation += 'System_ signal_gen:@' + str(len(self.sysc_signal_gens)) + '@' + repr(self.sysc_signal_gens).replace('[', '\\[').replace(']', '\\]') + '\n'

            return representation
        else:
            return ''

    def __str__(self):
        """Returns an informal string representation of this class"""
        from hci import console
        from hci import colors
        lineWidth = 90
        if self.module and self.classs:
            representation = console.go_new_line('\t' + colors.colorMap['bright_green'] + '** '+ self.module.__name__ + '.' + self.classs.__name__ + colors.colorMap['none']+':', lineWidth) + '\n\t\t'
            representation += console.go_new_line(console.clean_docstr(self.classs.__doc__ or 'no doc found...'), lineWidth) + '\n\t\t'
            representation += 'Constructor: ' + str(self.constructor) + '\n\t\t'
            in_tlm_portsStr = ''
            for i in self.in_tlm_ports:
                in_tlm_portsStr += '\n\t\t\t' + str(i)
            representation += 'Input TLM ports: ' + in_tlm_portsStr + '\n\t\t'
            out_tlm_portsStr = ''
            for i in self.out_tlm_ports:
                out_tlm_portsStr += '\n\t\t\t' + str(i)
            representation += 'Output TLM ports: ' + out_tlm_portsStr + '\n\t\t'
            in_sysc_portsStr = ''
            for i in self.in_sysc_ports:
                in_sysc_portsStr += '\n\t\t\t' + str(i)
            representation += 'Input SystemC ports: ' + in_sysc_portsStr + '\n\t\t'
            out_sysc_portsStr = ''
            for i in self.out_sysc_ports:
                out_sysc_portsStr += '\n\t\t\t' + str(i)
            representation += 'Output SystemC ports: ' + out_sysc_portsStr + '\n\t\t'
            inout_sysc_portsStr = ''
            for i in self.inout_sysc_ports:
                inout_sysc_portsStr += '\n\t\t\t' + str(i)
            representation += 'InOut SystemC ports: ' + inout_sysc_portsStr + '\n\t\t'
            sysc_signalsStr = ''
            for i in self.sysc_signals:
                sysc_signalsStr += '\n\t\t\t' + str(i)
            representation += 'SystemC signals: ' + sysc_signalsStr + '\n\t\t'

            in_tlm_port_gensStr = ''
            for i in self.in_tlm_port_gens:
                in_tlm_port_gensStr += '\n\t\t\t' + str(i)
            representation += 'Input TLM ports generators: ' + in_tlm_port_gensStr + '\n\t\t'
            out_tlm_ports_gensStr = ''
            for i in self.out_tlm_port_gens:
                out_tlm_ports_genStr += '\n\t\t\t' + str(i)
            representation += 'Output TLM ports generators: ' + out_tlm_ports_gensStr + '\n\t\t'
            in_sysc_ports_gensStr = ''
            for i in self.in_sysc_port_gens:
                in_sysc_ports_genStr += '\n\t\t\t' + str(i)
            representation += 'Input SystemC ports generators: ' + in_sysc_ports_gensStr + '\n\t\t'
            out_sysc_ports_gensStr = ''
            for i in self.out_sysc_port_gens:
                out_sysc_ports_genStr += '\n\t\t\t' + str(i)
            representation += 'Output SystemC ports generators: ' + out_sysc_ports_gensStr + '\n\t\t'
            inout_sysc_port_gensStr = ''
            for i in self.inout_sysc_port_gens:
                inout_sysc_port_gensStr += '\n\t\t\t' + str(i)
            representation += 'InOut SystemC ports generators: ' + inout_sysc_port_gensStr + '\n\t\t'
            sysc_signal_gensStr = ''
            for i in self.sysc_signal_gens:
                sysc_signal_gensStr += '\n\t\t\t' + str(i)
            representation += 'SystemC signals generators: ' + sysc_signal_gensStr

            return representation
        else:
            return ''
