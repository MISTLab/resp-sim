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

""" Module containing the elements to easily represent the node of a connection
among two components: note it is not sufficient to specify the components
which are being connected, but also the ports which connect them are necessary"""

# Here goes the GPL header...

from exc import *

class ConnectionNode:
    """Represents a node in the connection graph: it is an abstract
    representation of a component; the connections among this component
    and the other components of the arfchitecture are kept in a map:
    the key of the map represents the port of the current component
    to which I am connected; then there is a list of pairs (acutally
    a list of tuples): the first element of the pair represents the name
    of the target component to which I'm connected and the second element the
    port of the target component used for the connection"""
    def __init__(self, component, componentName):
        self.component = component
        self.componentName = componentName
        self.TLMsourceConn = {}
        self.TLMtargetConn = {}
        self.SysCsourceConn = {}
        self.SysCtargetConn = {}


    def addTarget(self, targetName, targetPort, thisPort, SCsignal = None):
        """Adds a connection among thisPort and a target component; in case
        SCsignal is specified, this is a connection among two systemc ports,
        not TLM, and SCsignal is the instance of the signal used to connect the
        ports"""
        if not SCsignal:
            if self.TLMtargetConn.has_key(thisPort):
                self.TLMtargetConn[thisPort].append((targetName, targetPort))
            else:
                self.TLMtargetConn[thisPort] = [(targetName, targetPort)]
        else:
            if self.SysCtargetConn.has_key(thisPort):
                self.SysCtargetConn[thisPort].append((targetName, targetPort, SCsignal))
            else:
                self.SysCtargetConn[thisPort] = [(targetName, targetPort, SCsignal)]


    def addSource(self, sourceName, sourcePort, thisPort, SCsignal = None):
        """Adds a connection among thisPort and a source component; in case
        SCsignal is specified, this is a connection among two systemc ports,
        not TLM, and SCsignal is the instance of the signal used to connect the
        ports"""
        if not SCsignal:
            if self.TLMsourceConn.has_key(thisPort):
                self.TLMsourceConn[thisPort].append((sourceName, sourcePort))
            else:
                self.TLMsourceConn[thisPort] = [(sourceName, sourcePort)]
        else:
            if self.SysCsourceConn.has_key(thisPort):
                self.SysCsourceConn[thisPort].append((sourceName, sourcePort, SCsignal))
            else:
                self.SysCsourceConn[thisPort] = [(sourceName, sourcePort, SCsignal)]

    def getSources(self):
        """Returns a tuple listing the names of all the components currently
        connected as masters to the component represented by this node"""
        components = []

        [[components.append(pair[0]) for pair in allList] for allList in self.TLMsourceConn.values()]
        [[components.append(pair[0]) for pair in allList] for allList in self.SysCsourceConn.values()]

        return tuple(components)

    def getTargets(self):
        """Returns a tuple listing the names of all the components currently
        connected as slaves to the component represented by this node"""
        components = []

        [[components.append(pair[0]) for pair in allList] for allList in self.TLMtargetConn.values()]
        [[components.append(pair[0]) for pair in allList] for allList in self.SysCtargetConn.values()]

        return tuple(components)

    def __repr__(self):
        """Returns a formal representation of this class"""
        representation = 'Component: ' + self.componentName + '\n'
        representation += '\tTLM Connections to SourcePorts: ' + str(self.TLMsourceConn)
        representation += '\tTLM Connections to TargetPorts: ' + str(self.TLMtargetConn)
        representation += '\tSystemC Connections to SourcePorts: ' + str(self.SysCsourceConn)
        representation += '\tSystemC Connections to TargetPorts: ' + str(self.SysCtargetConn)
        return representation

    def __str__(self):
        """Returns an informal string representation of this class"""
        return repr(self)

