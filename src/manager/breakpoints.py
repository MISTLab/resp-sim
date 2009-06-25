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

import respkernel
import sc_controller

""" Commodity module to handle callbacks and breakpoints via Python """

resp_ns = respkernel.get_namespace()
breaks = []

class GenericBreakpoint( sc_controller.DeltaCallback ):
    """ A generic breakpoint class: it stops simulation when a certain condition occurs """

    def __init__(self, object, attribute, checker):
        sc_controller.DeltaCallback.__init__(self)
        self.object = object
        self.attribute = attribute
        self.checker = checker

    def __call__(self):
        value = getattr(self.object, self.attribute)
        if self.checker(value):
            respkernel.controller.pause_simulation()

#
# Commodity checker classes
#
class equals():
    def __init__(self,y):
        self.y = y

    def __call__(self, x):
        if callable(x):
            return x() == self.y
        else:
            return x == self.y

class lesser():
    def __init__(self,y):
        self.y = y

    def __call__(self, x):
        if callable(x):
            return x() < self.y
        else:
            return x < self.y

class greater():
    def __init__(self,y):
        self.y = y

    def __call__(self, x):
        if callable(x):
            return x() > self.y
        else:
            return x > self.y

#
# Breakpoint registration and deregistration
#
def register_breakpoint(object, attribute, checker):
    gb = GenericBreakpoint( object, attribute, checker )
    breaks.append(gb)
    respkernel.controller.register_delta_callback( gb )
    return gb

def unregister_breakpoint(bp):
    respkernel.controller.remove_delta_callback( bp )
    breaks.remove(bp)
