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

""" Stats

    This package contains all the python code for the power analyzer,
    i.e. all the classes and routines which keep track of power modeling
"""

####################  MODEL DATABASE  ####################

connected_models = {}

def get_global_values():
    energy = 0
    power = 0
    for m in connected_models.values():
        energy += m.energy
        power += m.power
    return ( energy , power )

def get_values(name):
    return ( connected_models[name].energy , connected_models[name].power)

def print_energy_breakdown():
    total_energy = 0
    total_power = 0
    for m in connected_models.values():
        print "%s\t:\t%f mJ\t\t%f mW" % ( m.name, m.energy, m.power)
        total_energy += m.energy
        total_power += m.power
    print "-----------------------------------------------------"
    print "Total:\t\t%f mJ\t\t%f mW" % (total_energy, total_power)