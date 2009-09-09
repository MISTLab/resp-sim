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
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#
#
#   (c) Giovanni Beltrame, Luca Fossati
#       Giovanni.Beltrame@esa.int fossati@elet.polimi.it
#
##############################################################################

from generic import model

class simple_processor(model):
    """ A simple processor class using two probes and two parameters """

    def __init__(self, parameters=None):
        """ Set the defaults for the arm7tdmi """

        # Set default parameters (ARM7TDMI 0.13um)
        if not parameters:
            parameters = dict()
            parameters["idlePower"]   = 0.01     # mW/MHz
            parameters["activePower"] = 0.03     # mW/MHz
            parameters["frequency"]   = 250      # MHz

        # Set probe names
        probes = [ 'active_time', 'idle_time' ]

        # Init model
        model.__init__(self, model.PULL, probes, parameters)

        self.kind = "processor"


    def update_values(self):
        """ To be reimplemented by each model """
        energy = (self.idle_time*self["idlePower"]*self.frequency + self.active_time*self["activePower"]*self.frequency)*1e-9 # ns * mW = pJ = 1e9 mJ
        self.power = energy/(self.get_current_time()*1e-12)
        self.energy = energy