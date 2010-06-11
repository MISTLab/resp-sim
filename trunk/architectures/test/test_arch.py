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

#This file contains a simple architecture composed of a master and a slave; they exchange characters. Trough this file it
#is possible to test if the setup was succesfull

#Creation of the master and slave components; note how what we do is actually calling the constructor of the classes. The
#call here is performed in python, and this in turn calls the C++ constructor. These python constructors indeed have
#the same signature of the c++ ones


instMaster = testMaster_wrapper.testMaster('master')
instProbe = ProbeLT32.ProbeLT32('probe')
instSlave = testSlave_wrapper.testSlave('slave')
#Connection of the ports: first we specify the source component and its ports; then the target component and its port.
connectPorts(instMaster, instMaster.initSocket, instProbe, instProbe.targetSocket)
connectPorts(instProbe, instProbe.initiatorSocket, instSlave, instSlave.targetSocket)

instMaster1 = testMaster_wrapper.testMaster('master1')
instProbe1 = ProbeLT32.ProbeLT32('probe1')
instSlave1 = testSlave_wrapper.testSlave('slave1')
#Connection of the ports: first we specify the source component and its ports; then the target component and its port.
connectPorts(instMaster1, instMaster1.initSocket, instProbe1, instProbe1.targetSocket)
connectPorts(instProbe1, instProbe1.initiatorSocket, instSlave1, instSlave1.targetSocket)


# Modified stats auto-printer
def statsPrinter():
    print '\x1b[34m\x1b[1mReal Elapsed Time (seconds):\x1b[0m'
    print '\x1b[31m' + str(controller.print_real_time()) + '\x1b[0m'
    print '\x1b[34m\x1b[1mSimulated Elapsed Time (nano-seconds):\x1b[0m'
    print '\x1b[31m' + str(controller.get_simulated_time()) + '\x1b[0m'

#finally we start the simulation for 10 NS
#run_simulation(10)
