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
PROCESSOR_FREQUENCY = 100         # MHz
PROCESSOR_NUMBER  = 1             #
try:
    PROCESSOR_NAMESPACE
except:
    PROCESSOR_NAMESPACE = arm9tdmi_funcLT_wrapper.Processor_arm9tdmi_funclt

# Memory/bus
MEMORY_SIZE        = 32              # MBytes
MEM_LATENCY        = 10.0            # ns
BUS_ACTIVE         = True
BUS_LATENCY        = 10.0            # ns
DATA_CACHE_ACTIVE  = True
INSTR_CACHE_ACTIVE = True
CACHE_SIZE         = 8               # MBytes
CACHE_BLOCK_SIZE   = 32              # words
CACHE_WAYS         = 8
CACHE_REM_POLICY   = CacheLT32.LRU
CACHE_WR_POLICY    = CacheLT32.THROUGH
CACHE_READ_LAT     = 0.0             # ns
CACHE_WRITE_LAT    = 0.0             # ns
CACHE_LOAD_LAT     = 0.0             # ns
CACHE_STORE_LAT    = 0.0             # ns
CACHE_REMOVE_LAT   = 0.0             # ns

# Software
try:
    SOFTWARE
except:
    SOFTWARE = 'arm.out'

if SOFTWARE:
    try:
        ARGS
    except:
        ARGS = None

OS_EMULATION = True     # True or False

# Modified stats auto-printer
def statsPrinter():
    print '\x1b[34m\x1b[1mSimulated Elapsed Time (nano-seconds):\x1b[0m'
    print '\x1b[31m' + str(controller.get_simulated_time()) + '\x1b[0m'
    if BUS_ACTIVE:
        print '\x1b[34m\x1b[1mBus Accesses:\x1b[0m'
        print '\x1b[31m' + str(bus.numAccesses) + '\x1b[0m'
        print '\x1b[34m\x1b[1mBus Words:\x1b[0m'
        print '\x1b[31m' + str(bus.numWords) + '\x1b[0m'

################################################
##### AUTO VARIABLE SETUP ######################
################################################

# Find the specified software in the _build_ directory if not an absolute path
if not SOFTWARE or not os.path.isfile(SOFTWARE):
    SOFTWARE = findInFolder(SOFTWARE, 'software/build/arm')
    if not SOFTWARE:
        raise Exception('Unable to find program')

# This kind of caches works only with a single processor
if PROCESSOR_NUMBER > 1:
    raise Exception('Simple caches will work only in single-core systems')

################################################
##### COMPONENT CREATION #######################
################################################

###### PROCESSOR INSTANTIATION #####
#Processor instantiation; decomment the following line to add a second processor and eventually add similar lines
#for the third, fourth .... processor
processors = []
latency = scwrapper.sc_time(float(1000)/float(PROCESSOR_FREQUENCY),  scwrapper.SC_NS)
for i in range(0, PROCESSOR_NUMBER):
    processors.append(PROCESSOR_NAMESPACE('processor_' + str(i), latency))

##### MEMORY INSTANTIATION #####
memorySize = 1024*1024*MEMORY_SIZE
latencyMem = scwrapper.sc_time(MEM_LATENCY, scwrapper.SC_NS)
mem = MemoryLT32.MemoryLT32( 'mem', memorySize, latencyMem)

################################################
##### INTERCONNECTIONS #########################
################################################

if BUS_ACTIVE:
    latencyBus = scwrapper.sc_time(BUS_LATENCY, scwrapper.SC_NS)
    bus  = BusLT32.BusLT32('bus',latencyBus,2)
    connectPortsForce(bus, bus.initiatorSocket, mem, mem.targetSocket)
    # Add memory mapping
    bus.addBinding("mem",0x0,memorySize,False)

##### CACHE, BUS, AND MEMORY CONNECTIONS #####
if DATA_CACHE_ACTIVE:
    dataCache = CacheLT32.CacheLT32('dataCache', CACHE_SIZE*1024*1024, memorySize, CACHE_WAYS, CACHE_BLOCK_SIZE, CACHE_REM_POLICY, CACHE_WR_POLICY)
    dataCache.setReadLatency(scwrapper.sc_time(CACHE_READ_LAT,scwrapper.SC_NS))
    dataCache.setWriteLatency(scwrapper.sc_time(CACHE_WRITE_LAT,scwrapper.SC_NS))
    dataCache.setLoadLatency(scwrapper.sc_time(CACHE_LOAD_LAT,scwrapper.SC_NS))
    dataCache.setStoreLatency(scwrapper.sc_time(CACHE_STORE_LAT,scwrapper.SC_NS))
    dataCache.setRemoveLatency(scwrapper.sc_time(CACHE_REMOVE_LAT,scwrapper.SC_NS))
    #dataCache.setScratchpad(4194304,1048576,scwrapper.sc_time(0.001,scwrapper.SC_NS))
    connectPortsForce(processors[0], processors[0].dataMem.initSocket, dataCache, dataCache.targetSocket)
    if BUS_ACTIVE:
        connectPortsForce(dataCache, dataCache.initSocket, bus, bus.targetSocket)
    else:
        connectPortsForce(dataCache, dataCache.initSocket, mem, mem.targetSocket)
else:
    if BUS_ACTIVE:
        connectPortsForce(processors[0], processors[0].dataMem.initSocket, bus, bus.targetSocket)
    else:
        connectPortsForce(processors[0], processors[0].dataMem.initSocket, mem, mem.targetSocket)

if INSTR_CACHE_ACTIVE:
    instrCache = CacheLT32.CacheLT32('instrCache', CACHE_SIZE*1024*1024, memorySize, CACHE_WAYS, CACHE_BLOCK_SIZE, CACHE_REM_POLICY, CACHE_WR_POLICY)
    instrCache.setReadLatency(scwrapper.sc_time(CACHE_READ_LAT,scwrapper.SC_NS))
    instrCache.setWriteLatency(scwrapper.sc_time(CACHE_WRITE_LAT,scwrapper.SC_NS))
    instrCache.setLoadLatency(scwrapper.sc_time(CACHE_LOAD_LAT,scwrapper.SC_NS))
    instrCache.setStoreLatency(scwrapper.sc_time(CACHE_STORE_LAT,scwrapper.SC_NS))
    instrCache.setRemoveLatency(scwrapper.sc_time(CACHE_REMOVE_LAT,scwrapper.SC_NS))
    #instrCache.setScratchpad(4194304,1048576,scwrapper.sc_time(0.001,scwrapper.SC_NS))
    connectPortsForce(processors[0], processors[0].instrMem.initSocket, instrCache, instrCache.targetSocket)
    if BUS_ACTIVE:
        connectPortsForce(instrCache, instrCache.initSocket, bus, bus.targetSocket)
    else:
        connectPortsForce(instrCache, instrCache.initSocket, mem, mem.targetSocket)
else:
    if BUS_ACTIVE:
        connectPortsForce(processors[0], processors[0].instrMem.initSocket, bus, bus.targetSocket)
    else:
        connectPortsForce(processors[0], processors[0].instrMem.initSocket, mem, mem.targetSocket)

################################################
##### SYSTEM INIT ##############################
################################################

##### LOAD SOFTWARE #####

import os
if not os.path.exists(SOFTWARE):
    raise Exception('Error, ' + str(SOFTWARE) + ' does not exists')

loader = loader_wrapper.Loader(SOFTWARE)
#Initialization of the processors and loading in memory of the application program
for i in range(0, loader.getProgDim()):
    mem.write_byte_dbg(i + loader.getDataStart(), loader.getProgDataValue(i))

for i in range(0, PROCESSOR_NUMBER):
    processors[i].ENTRY_POINT = loader.getProgStart()
    processors[i].PROGRAM_LIMIT = loader.getProgDim() + loader.getDataStart()
    processors[i].PROGRAM_START = loader.getDataStart()

# Now I initialize the OS emulator
if OS_EMULATION:
    for i in range(0, PROCESSOR_NUMBER):
        curEmu = trapwrapper.OSEmulator32(processors[i].getInterface())
        curEmu.initSysCalls(SOFTWARE)
        processors[i].toolManager.addTool(curEmu)

# We can finally run the simulation
run_simulation()
