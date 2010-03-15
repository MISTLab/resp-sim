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

###### GENERAL PARAMETERS #####
PROCESSOR_FREQUENCY = 500         # MHz
PROCESSOR_NUMBER    = 4           #
try:
    PROCESSOR_NAMESPACE
except:
    PROCESSOR_NAMESPACE = arm7tdmi_funcLT_wrapper

# Memory/bus
MEMORY_SIZE       = 32             # MBytes
MEM_LATENCY       = 10.0           # ns


# Software
try:
    SOFTWARE
except:
    SOFTWARE = 'ffmpeg'

if SOFTWARE:
    try:
        ARGS
    except:
        ARGS = None

OS_EMULATION = True     # True or False

################################################
##### AUTO VARIABLE SETUP ######################
################################################

# Find the specified software in the _build_ directory if not an absolute path
if not SOFTWARE or not os.path.isfile(SOFTWARE):
    SOFTWARE = findInFolder(SOFTWARE, 'software/build/arm')
    if not SOFTWARE:
        raise Exception('Unable to find program')

################################################
##### COMPONENT CREATION #######################
################################################

###### PROCESSOR INSTANTIATION #####
#Processor instantiation; decomment the following line to add a second processor and eventually add similar lines
#for the third, fourth .... processor
processors = []
latency = scwrapper.sc_time(float(1000)/float(PROCESSOR_FREQUENCY),  scwrapper.SC_NS)
for i in range(0, PROCESSOR_NUMBER):
    processors.append(PROCESSOR_NAMESPACE.Processor_arm7tdmi_funclt('processor_' + str(i), latency))

##### MEMORY INSTANTIATION #####
memorySize = 1024*1024*MEMORY_SIZE
latencyMem = scwrapper.sc_time(MEM_LATENCY, scwrapper.SC_NS)
mem = MemoryLT32.MemoryLT32( 'mem', memorySize, latencyMem)

################################################
##### INTERCONNECTIONS #########################
################################################

bus  = BusLT32.BusLT32('bus',scwrapper.SC_ZERO_TIME)

##### BUS CONNECTIONS #####
# Connecting the master components to the bus
for i in range(0, PROCESSOR_NUMBER):
    connectPortsForce(processors[i], processors[i].instrMem.initSocket, bus, bus.targetSocket)
    connectPortsForce(processors[i], processors[i].dataMem.initSocket, bus, bus.targetSocket)

connectPortsForce(bus, bus.initiatorSocket, mem, mem.targetSocket)

# Add memory mapping
bus.addBinding("mem",0x0,memorySize,False)

################################################
##### SYSTEM INIT ##############################
################################################

##### LOAD SOFTWARE #####

import os
if not os.path.exists(SOFTWARE):
    raise Exception('Error, ' + str(SOFTWARE) + ' does not exists')

loader = loader_wrapper.Loader(SOFTWARE)
#Initialization of the processors and loading in memory of the application program
print "Writing memory"
loader.loadProgInMemory(mem)

print "Setting up CPU registers"

for i in range(0, PROCESSOR_NUMBER):
    #if i == 0:
    processors[i].ENTRY_POINT = loader.getProgStart()
    #else:
    #    processors[i].ENTRY_POINT = 1044
    
    processors[i].PROGRAM_LIMIT = loader.getProgDim() + loader.getDataStart()
    processors[i].PROGRAM_START = loader.getDataStart()
    # Set the processor ID
    processors[i].MP_ID.immediateWrite(i)
    #processors[i].MPROC_ID = (i << 28)
    processors[i].resetOp()

print "Setting up OS Emulation"

# Now I initialize the OS emulator
tools = list()
if OS_EMULATION:
    for i in range(0, PROCESSOR_NUMBER):
        curEmu = trapwrapper.OSEmulator32(processors[i].getInterface())
        curEmu.initSysCalls(SOFTWARE)
        
        ##### CONCURRENCY MANAGEMENT #####
        concurrentEmu = cm_wrapper.ConcurrencyEmulator32(processors[i].getInterface(),memorySize)
        concurrentEmu.initSysCalls(SOFTWARE,True)

        processors[i].toolManager.addTool(curEmu)
        processors[i].toolManager.addTool(concurrentEmu)
        tools.append(curEmu)
        tools.append(concurrentEmu)
    
    # OpenMP Support
    trapwrapper.OSEmulatorBase.set_environ('OMP_NUM_THREADS', str(PROCESSOR_NUMBER)) 

    # Command line
    #ARGUMENTS = ("ffmpeg.exe", "-idct", "simplearm", "-y", "-threads", "1", "-i", "/home/beltrame/Projects/resp-old/software/application/sheep.mpg", "/home/beltrame/minimal.avi")
    #trapwrapper.OSEmulatorBase.set_program_args(ARGUMENTS)
