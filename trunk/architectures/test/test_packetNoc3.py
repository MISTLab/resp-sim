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
#   Architecture written by: Fabio Arlati arlati.fabio@gmail.com
#
##############################################################################

###### GENERAL PARAMETERS #####
PROCESSOR_FREQUENCY = 333         # MHz
PROCESSOR_NUMBER  = 1             #
try:
    PROCESSOR_NAMESPACE
except:
    PROCESSOR_NAMESPACE = arm9tdmi_funcLT_wrapper.Processor_arm9tdmi_funclt

# Memory/bus
MEM1_SIZE          = 256             # KBytes
MEM1_FREQUENCY     = 166             # MHz
MEM2_SIZE          = 1 #32              # MBytes
MEM2_FREQUENCY     = 50              # MHz
NOC_ACTIVE         = True
NOC_FREQUENCY      = 166             # MHz
NOC_DESCRIPTION    = 'architectures/test/test_packetNoc3.xml'
DATA_CACHE_ACTIVE  = False #True
INSTR_CACHE_ACTIVE = False #True
CACHE_SIZE         = 16              # KBytes
CACHE_BLOCK_SIZE   = 8               # words
CACHE_WAYS         = 8
CACHE_REM_POLICY   = CacheLT32.LRU
CACHE_WR_POLICY    = CacheLT32.THROUGH #BACK
CACHE_READ_LAT     = 3.0            # ns
CACHE_WRITE_LAT    = 3.0            # ns
CACHE_LOAD_LAT     = 3.0            # ns
CACHE_STORE_LAT    = 3.0            # ns
CACHE_REMOVE_LAT   = 3.0            # ns

# Software
try:
    SOFTWARE
except:
    SOFTWARE = 'test_noc_gamma'

if SOFTWARE:
    try:
        ARGS
    except:
        ARGS = []

OS_EMULATION = True     # True or False

# Modified stats auto-printer
def statsPrinter():
    print '\x1b[34m\x1b[1mReal Elapsed Time (seconds):\x1b[0m'
    print '\x1b[31m' + str(controller.print_real_time()) + '\x1b[0m'
    print '\x1b[34m\x1b[1mSimulated Elapsed Time (nano-seconds):\x1b[0m'
    print '\x1b[31m' + str(controller.get_simulated_time()) + '\x1b[0m'
    if NOC_ACTIVE:
        print '\x1b[34m\x1b[1mNOC Accesses:\x1b[0m'
        noc.printStats()

################################################
##### AUTO VARIABLE SETUP ######################
################################################

# Find the specified software in the _build_ directory if not an absolute path
if not SOFTWARE or not os.path.isfile(SOFTWARE):
    SOFTWARE = findInFolder(SOFTWARE, 'software/build/arm/')
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
    processors.append(PROCESSOR_NAMESPACE('processor_' + str(i), latency))

##### MEMORY INSTANTIATION #####
memorySize1 = 1024*MEM1_SIZE
latencyMem1 = scwrapper.sc_time(float(1000)/float(MEM1_FREQUENCY), scwrapper.SC_NS)
mem1 = MemoryLT32.MemoryLT32('mem1', memorySize1, latencyMem1)
memorySize2 = 1024*1024*MEM2_SIZE/4
latencyMem2 = scwrapper.sc_time(float(1000)/float(MEM2_FREQUENCY), scwrapper.SC_NS)
mem2 = MemoryLT32.MemoryLT32('mem2', memorySize2, latencyMem2)
latencyMem3 = scwrapper.sc_time(float(1000)/float(MEM2_FREQUENCY), scwrapper.SC_NS)
mem3 = MemoryLT32.MemoryLT32('mem3', memorySize2, latencyMem3)
latencyMem4 = scwrapper.sc_time(float(1000)/float(MEM2_FREQUENCY), scwrapper.SC_NS)
mem4 = MemoryLT32.MemoryLT32('mem4', memorySize2, latencyMem4)
latencyMem5 = scwrapper.sc_time(float(1000)/float(MEM2_FREQUENCY), scwrapper.SC_NS)
mem5 = MemoryLT32.MemoryLT32('mem5', memorySize2, latencyMem5)

##### RECONFIGURABLE COMPONENTS INSTANTIATION #####
cE = cE_wrapper.configEngine('cE',0x0,cE_wrapper.LRU)
cE.setRequestDelay(scwrapper.sc_time(3, scwrapper.SC_NS))
cE.setExecDelay(scwrapper.sc_time(3, scwrapper.SC_NS))
cE.setConfigDelay(scwrapper.sc_time(3, scwrapper.SC_NS))
cE.setRemoveDelay(scwrapper.sc_time(3, scwrapper.SC_NS))
#eFPGAs
eF = []
eF.append(eFPGA_wrapper.eFPGA('eF1',100,100,scwrapper.sc_time(58, scwrapper.SC_NS),11,1))
EFPGA_NUMBER = eF.__len__()

#required memory
memorySize6 = 1024*MEM1_SIZE
latencyMem6 = scwrapper.sc_time(float(1000)/float(MEM1_FREQUENCY), scwrapper.SC_NS)
mem6 = MemoryLT32.MemoryLT32('mem6', memorySize6, latencyMem6)


################################################
##### INTERCONNECTIONS #########################
################################################

if NOC_ACTIVE:
    clockNoc = scwrapper.sc_time(float(1000)/float(NOC_FREQUENCY), scwrapper.SC_NS)
    noc = packetNoc32.packetNoc('noc',NOC_DESCRIPTION,clockNoc)
    connectPorts(noc, noc.initiatorSocket, mem1, mem1.targetSocket)
    connectPorts(noc, noc.initiatorSocket, mem2, mem2.targetSocket)
    connectPorts(noc, noc.initiatorSocket, mem3, mem3.targetSocket)
    connectPorts(noc, noc.initiatorSocket, mem4, mem4.targetSocket)
    connectPorts(noc, noc.initiatorSocket, mem5, mem5.targetSocket)
    # Add memory mapping
    noc.addBinding(4,0x0,memorySize1-1)
    noc.addBinding(5,memorySize1,memorySize1+memorySize2-1)
    noc.addBinding(6,memorySize1+memorySize2,memorySize1+memorySize2*2-1)
    noc.addBinding(7,memorySize1+memorySize2*2,memorySize1+memorySize2*3-1)
    noc.addBinding(8,memorySize1+memorySize2*3,memorySize1+memorySize2*4-1)
else:
    raise Exception('At least an interconnection layer should be introduced in this architecture')

if DATA_CACHE_ACTIVE or INSTR_CACHE_ACTIVE:
    directory = DirectoryLT32.DirectoryLT32('dir',2*PROCESSOR_NUMBER)

##### CACHE, BUS, AND MEMORY CONNECTIONS #####
dataCaches = []
instrCaches = []
for i in range(0, PROCESSOR_NUMBER):
    if DATA_CACHE_ACTIVE:
        dataCaches.append(CoherentCacheLT32.CoherentCacheLT32('dataCache_' + str(i), CACHE_SIZE*1024, memorySize1+memorySize2*4, CACHE_WAYS, CACHE_BLOCK_SIZE, CACHE_REM_POLICY, CACHE_WR_POLICY))
        dataCaches[i].setReadLatency(scwrapper.sc_time(CACHE_READ_LAT,scwrapper.SC_NS))
        dataCaches[i].setWriteLatency(scwrapper.sc_time(CACHE_WRITE_LAT,scwrapper.SC_NS))
        dataCaches[i].setLoadLatency(scwrapper.sc_time(CACHE_LOAD_LAT,scwrapper.SC_NS))
        dataCaches[i].setStoreLatency(scwrapper.sc_time(CACHE_STORE_LAT,scwrapper.SC_NS))
        dataCaches[i].setRemoveLatency(scwrapper.sc_time(CACHE_REMOVE_LAT,scwrapper.SC_NS))
        #dataCaches[i].setScratchpad(4194304,1048576,scwrapper.sc_time(0.001,scwrapper.SC_NS))
        connectPorts(processors[i], processors[i].dataMem.initSocket, dataCaches[i], dataCaches[i].targetSocket)
        connectPorts(dataCaches[i], dataCaches[i].dirInitSocket, directory, directory.targetSocket)
        connectPorts(directory, directory.initSocket, dataCaches[i], dataCaches[i].dirTargetSocket)
        connectPorts(dataCaches[i], dataCaches[i].initSocket, noc, noc.targetSocket)
    else:
        connectPorts(processors[i], processors[i].dataMem.initSocket, noc, noc.targetSocket)

    if INSTR_CACHE_ACTIVE:
        instrCaches.append(CoherentCacheLT32.CoherentCacheLT32('instrCache_' + str(i), CACHE_SIZE*1024, memorySize1+memorySize2*4, CACHE_WAYS, CACHE_BLOCK_SIZE, CACHE_REM_POLICY, CACHE_WR_POLICY))
        instrCaches[i].setReadLatency(scwrapper.sc_time(CACHE_READ_LAT,scwrapper.SC_NS))
        instrCaches[i].setWriteLatency(scwrapper.sc_time(CACHE_WRITE_LAT,scwrapper.SC_NS))
        instrCaches[i].setLoadLatency(scwrapper.sc_time(CACHE_LOAD_LAT,scwrapper.SC_NS))
        instrCaches[i].setStoreLatency(scwrapper.sc_time(CACHE_STORE_LAT,scwrapper.SC_NS))
        instrCaches[i].setRemoveLatency(scwrapper.sc_time(CACHE_REMOVE_LAT,scwrapper.SC_NS))
        #instrCaches[i].setScratchpad(4194304,1048576,scwrapper.sc_time(0.001,scwrapper.SC_NS))
        connectPorts(processors[i], processors[i].instrMem.initSocket, instrCaches[i], instrCaches[i].targetSocket)
        connectPorts(instrCaches[i], instrCaches[i].dirInitSocket, directory, directory.targetSocket)
        connectPorts(directory, directory.initSocket, instrCaches[i], instrCaches[i].dirTargetSocket)
        connectPorts(instrCaches[i], instrCaches[i].initSocket, noc, noc.targetSocket)
    else:
        connectPorts(processors[i], processors[i].instrMem.initSocket, noc, noc.targetSocket)

##### RECONFIGURABLE COMPONENTS CONNECTIONS #####
connectPorts(cE, cE.ramSocket, mem6, mem6.targetSocket)
bSPosition = 1
for i in range(0, EFPGA_NUMBER):
    connectPorts(cE, cE.initiatorSocket, eF[i], eF[i].targetSocket)
    connectPorts(cE, cE.destSocket, eF[i], eF[i].bS.targetSocket)
    cE.bindFPGA(memorySize6+bSPosition)
    bSPosition = bSPosition+1

# TRAFFIC GENERATOR INSTANTIATION, CONFIGURATION AND CONNECTION


SM_FREQUENCY       = 100
NUM_OF_TRANSMISSIONS = 50000
masterLatency1 = scwrapper.sc_time(float(1000)/float(SM_FREQUENCY), scwrapper.SC_NS)
m1 = trafficGenerator.trafficGenerator('master1', masterLatency1)
m2 = trafficGenerator.trafficGenerator('master2', masterLatency1)
m3 = trafficGenerator.trafficGenerator('master3', masterLatency1)
m4 = trafficGenerator.trafficGenerator('master4', masterLatency1)
#m5 = trafficGenerator.trafficGenerator('master5', masterLatency1)
#m6 = trafficGenerator.trafficGenerator('master6', masterLatency1)
addressSpace = memorySize1+memorySize2*4
m1.setParameters(NUM_OF_TRANSMISSIONS, 0x0, addressSpace-1, 128, 32, memorySize1+memorySize2*4)
m2.setParameters(NUM_OF_TRANSMISSIONS, 0x0, addressSpace-1, 1024, 32, memorySize1+memorySize2*4)
m3.setParameters(NUM_OF_TRANSMISSIONS, 0x0, addressSpace-1, 512, 64, memorySize1+memorySize2*4)
m4.setParameters(NUM_OF_TRANSMISSIONS, 0x0, addressSpace-1, 4096, 64, memorySize1+memorySize2*4)
#m5.setParameters(NUM_OF_TRANSMISSIONS, 0x0, memorySize1+memorySize2*4-1, memorySize1/8, 16, memorySize1+memorySize2*4)
#m6.setParameters(NUM_OF_TRANSMISSIONS, 0x0, memorySize1+memorySize2*4-1, memorySize1/64, 4, memorySize1+memorySize2*4)
connectPorts(m1, m1.initSocket, noc, noc.targetSocket)
connectPorts(m2, m2.initSocket, noc, noc.targetSocket)
connectPorts(m3, m3.initSocket, noc, noc.targetSocket)
connectPorts(m4, m4.initSocket, noc, noc.targetSocket)
#connectPorts(m5, m5.initSocket, noc, noc.targetSocket)
#connectPorts(m6, m6.initSocket, noc, noc.targetSocket)

#dummy end instantiation
NUM_OF_SP = 5
de1 = dummyEnd_wrapper.DummyEnd("dummyEnd1",NUM_OF_SP, latencyMem1)
connectPorts(noc, noc.initiatorSocket, de1, de1.targetSocket)
noc.addBinding(9,memorySize1+memorySize2*4,memorySize1+memorySize2*4+4)


################################################
##### SYSTEM INIT ##############################
################################################

##### LOAD SOFTWARE #####
import os
if not os.path.exists(SOFTWARE):
    raise Exception('Error, ' + str(SOFTWARE) + ' does not exists')

loader = loader_wrapper.Loader(SOFTWARE)
print "Writing memory"
loader.loadProgInMemory(mem1)

print "Setting up CPU registers"
for i in range(0, PROCESSOR_NUMBER):
    processors[i].ENTRY_POINT = loader.getProgStart()
    processors[i].PROGRAM_LIMIT = loader.getProgDim() + loader.getDataStart()
    processors[i].PROGRAM_START = loader.getDataStart()
    # Set the processor ID
    processors[i].resetOp();
    processors[i].MP_ID.immediateWrite(i)


# Now I initialize the OS emulator
print "Setting up OS Emulation"
tools = list()
if OS_EMULATION:
    for i in range(0, PROCESSOR_NUMBER):
        curEmu = trapwrapper.OSEmulator32(processors[i].getInterface())
        curEmu.initSysCalls(SOFTWARE)
        curEmu.set_program_args(ARGS)
        # OpenMP Support
        curEmu.set_environ('OMP_NUM_THREADS', str(PROCESSOR_NUMBER)) 

        processors[i].toolManager.addTool(curEmu)
        ##### CONCURRENCY MANAGEMENT #####
        concurrentEmu = cm_wrapper.ConcurrencyEmulator32(processors[i].getInterface(),memorySize1+memorySize2*4)
        concurrentEmu.initSysCalls(SOFTWARE,True)
        processors[i].toolManager.addTool(concurrentEmu)
        tools.append(curEmu)
        tools.append(concurrentEmu)

# Now I initialize the reconfiguration emulator
for i in range(0, PROCESSOR_NUMBER):
    recEmu = recEmu_wrapper.reconfEmulator32(processors[i].getInterface(),cE,SOFTWARE)
    processors[i].toolManager.addTool(recEmu)
    tools.append(recEmu)

    # Registration of the callbacks
    recEmu.registerCppCall('read_bitmap', scwrapper.sc_time(1000000, scwrapper.SC_NS),1,1)
    recEmu.registerCppCall('write_bitmap', scwrapper.sc_time(1000000, scwrapper.SC_NS),1,1)
    print 'Registered routine calls for processor ' + str(i) + ': '
    recEmu.printRegisteredFunctions()


# Printing some useful statistics before the simulation begins
noc.printBindings()
noc.printGRT()
noc.changeAllTimeouts(scwrapper.sc_time(1000, scwrapper.SC_NS))

BUFFER_SIZE = 2
INIT_THRESHOLD = 50
MIN_THRESHOLD = 40
MAX_THRESHOLD = 1000
MULT_FACTOR = 5
RED_FACTOR = 1
ENABLE = False
clockGNoc = scwrapper.sc_time(float(1000)/float(NOC_FREQUENCY), scwrapper.SC_NS)
governor = packetNocGovernor.packetNocGovernor('governor',noc,clockGNoc)
governor.enableGovernor(ENABLE)
governor.initialize(BUFFER_SIZE, INIT_THRESHOLD, MAX_THRESHOLD, MIN_THRESHOLD, MULT_FACTOR, RED_FACTOR)

# We can finally run the simulation
# run_simulation()
