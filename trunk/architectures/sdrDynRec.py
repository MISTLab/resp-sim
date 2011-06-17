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
PROCESSOR_NUMBER  = 2             #
try:
    PROCESSOR_NAMESPACE
except:
    PROCESSOR_NAMESPACE = arm9tdmi_funcLT_wrapper.Processor_arm9tdmi_funclt

# Memory/bus
MEM1_SIZE          = 256             # KBytes
MEM1_FREQUENCY     = 166             # MHz
MEM2_SIZE          = 32              # MBytes
MEM2_FREQUENCY     = 50              # MHz
BSRAM_SIZE         = 8               # KBytes
BSRAM_FREQUENCY    = 333             # MHz
BUS_ACTIVE         = True
BUS_FREQUENCY      = 83              # MHz
NOC_ACTIVE         = False
NOC_FREQUENCY      = 166             # MHz
NOC_TOPOLOGY       = NocLT32.FULL
CONFIGURE_THROUGH_ITC = False
DATA_CACHE_ACTIVE  = True
INSTR_CACHE_ACTIVE = True
CACHE_SIZE         = 16              # KBytes
CACHE_BLOCK_SIZE   = 8               # words
CACHE_WAYS         = 8
CACHE_REM_POLICY   = CacheLT32.LRU
CACHE_WR_POLICY    = CacheLT32.THROUGH
CACHE_READ_LAT     = 3.0            # ns
CACHE_WRITE_LAT    = 3.0            # ns
CACHE_LOAD_LAT     = 3.0            # ns
CACHE_STORE_LAT    = 3.0            # ns
CACHE_REMOVE_LAT   = 3.0            # ns

# Software
try:
    SOFTWARE
except:
    SOFTWARE = 'sdr'

if SOFTWARE:
    try:
        ARGS
    except:
        ARGS = []

OS_EMULATION = True     # True or False

# Modified stats auto-printer
FILE_OUTPUT = False
def statsPrinter():
    print '\x1b[34m\x1b[1mReal Elapsed Time (seconds):\x1b[0m'
    print '\x1b[31m' + str(controller.print_real_time()) + '\x1b[0m'
    print '\x1b[34m\x1b[1mSimulated Elapsed Time (nano-seconds):\x1b[0m'
    print '\x1b[31m' + str(controller.get_simulated_time()) + '\x1b[0m'
    if NOC_ACTIVE:
        print '\x1b[34m\x1b[1mNOC Accesses:\x1b[0m'
        noc.printAccesses()
    if BUS_ACTIVE:
        print '\x1b[34m\x1b[1mBus Accesses:\x1b[0m'
        bus.printAccesses()
    if FILE_OUTPUT:
        writeHeader = not os.path.exists('architectures/results.txt')
        output = open('architectures/results.txt','a')
        if writeHeader:
            print >> output, "Benchmark;Processors;Cache;Real Time (s);Simulated Time (ns);BusAcc#0;BusAcc#1;BusAcc#2;BusAcc#3;BusAcc#4;BusAcc#5;BusAcc#6;BusAcc#7;BusAcc#8;BusAcc#9;"
        output.write(os.path.basename(SOFTWARE)+';')
        output.write(str(PROCESSOR_NUMBER)+';')
        if DATA_CACHE_ACTIVE or INSTR_CACHE_ACTIVE:
            output.write(str(CACHE_WR_POLICY)+';')
        else:
            output.write('NONE;')
        output.write(str(controller.print_real_time())+';')
        output.write(str(controller.get_simulated_time())+';')
        for i in range(0,10):
            output.write(str(bus.getAccesses(i))+';')
        output.write('\n')
        output.close()

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
memorySize2 = 1024*1024*MEM2_SIZE
latencyMem2 = scwrapper.sc_time(float(1000)/float(MEM2_FREQUENCY), scwrapper.SC_NS)
mem2 = MemoryLT32.MemoryLT32('mem2', memorySize2, latencyMem2)

##### RECONFIGURABLE COMPONENTS INSTANTIATION #####
cE = cE_wrapper.configEngine('cE',memorySize1,cE_wrapper.LRU)
cE.setRequestDelay(scwrapper.sc_time(3, scwrapper.SC_NS))
cE.setExecDelay(scwrapper.sc_time(3, scwrapper.SC_NS))
cE.setConfigDelay(scwrapper.sc_time(3, scwrapper.SC_NS))
cE.setRemoveDelay(scwrapper.sc_time(3, scwrapper.SC_NS))
#eFPGAs
eF = []
eF.append(eFPGA_wrapper.eFPGA('Altera',250,200,scwrapper.sc_time(400, scwrapper.SC_NS),7.5,1))
eF.append(eFPGA_wrapper.eFPGA('Virtex1',150,140,scwrapper.sc_time(58, scwrapper.SC_NS),11,1))
eF.append(eFPGA_wrapper.eFPGA('Virtex2',150,140,scwrapper.sc_time(58, scwrapper.SC_NS),11,1))
eF.append(eFPGA_wrapper.eFPGA('Atmel',24,96,scwrapper.sc_time(4000, scwrapper.SC_NS),5,1))
EFPGA_NUMBER = eF.__len__()

################################################
##### INTERCONNECTIONS #########################
################################################

additionalMasters = 1
if CONFIGURE_THROUGH_ITC:
    additionalMasters = additionalMasters + 1
if NOC_ACTIVE and BUS_ACTIVE:
    latencyNoc = scwrapper.sc_time(float(1000)/float(NOC_FREQUENCY), scwrapper.SC_NS)
    latencyBus = scwrapper.sc_time(float(1000)/float(BUS_FREQUENCY), scwrapper.SC_NS)
    noc = NocLT32.NocLT32('noc',2*PROCESSOR_NUMBER,2,NOC_TOPOLOGY,latencyNoc)
    bus = BusLT32.BusLT32('bus',1+additionalMasters,latencyBus)
    connectPorts(noc, noc.initiatorSocket, mem1, mem1.targetSocket)
    connectPorts(noc, noc.initiatorSocket, mem2, mem2.targetSocket)
    connectPorts(noc, noc.initiatorSocket, bus, bus.targetSocket)
    # Add memory mapping
    noc.addBinding("mem1",0x0,memorySize1-1)
    noc.addBinding("mem2",memorySize1,memorySize1+memorySize2-1)
#    noc.addBinding("bus",memorySize1+memorySize2+1,memorySize1+memorySize2+1)
    bSmem = MemoryLT32.MemoryLT32('bSRAM', memorySize1, latencyMem1)
    connectPorts(bus, bus.initiatorSocket, bSmem, bSmem.targetSocket)
    bus.addBinding("bSRAM",0x0,memorySize1-1)
elif BUS_ACTIVE:
    latencyBus = scwrapper.sc_time(float(1000)/float(BUS_FREQUENCY), scwrapper.SC_NS)
    bus = BusLT32.BusLT32('bus',2*PROCESSOR_NUMBER+additionalMasters,latencyBus)
    connectPorts(bus, bus.initiatorSocket, mem1, mem1.targetSocket)
    connectPorts(bus, bus.initiatorSocket, mem2, mem2.targetSocket)
   # Add memory mapping
    bus.addBinding("mem1",0x0,memorySize1-1)
    bus.addBinding("mem2",memorySize1,memorySize1+memorySize2-1)
elif NOC_ACTIVE:
    latencyNoc = scwrapper.sc_time(float(1000)/float(NOC_FREQUENCY), scwrapper.SC_NS)
    if CONFIGURE_THROUGH_ITC:
        noc = NocLT32.NocLT32('noc',2*PROCESSOR_NUMBER+additionalMasters,2+EFPGA_NUMBER,NOC_TOPOLOGY,latencyNoc)
    else:
        noc = NocLT32.NocLT32('noc',2*PROCESSOR_NUMBER+additionalMasters,2,NOC_TOPOLOGY,latencyNoc)
    connectPorts(noc, noc.initiatorSocket, mem1, mem1.targetSocket)
    connectPorts(noc, noc.initiatorSocket, mem2, mem2.targetSocket)
    # Add memory mapping
    noc.addBinding("mem1",0x0,memorySize1-1)
    noc.addBinding("mem2",memorySize1,memorySize1+memorySize2-1)
else:
    raise Exception('At least an interconnection layer should be introduced in this architecture')

if DATA_CACHE_ACTIVE or INSTR_CACHE_ACTIVE:
    directory = DirectoryLT32.DirectoryLT32('dir',2*PROCESSOR_NUMBER)

##### CACHE, BUS, AND MEMORY CONNECTIONS #####
dataCaches = []
instrCaches = []
for i in range(0, PROCESSOR_NUMBER):
    if DATA_CACHE_ACTIVE:
        dataCaches.append(CoherentCacheLT32.CoherentCacheLT32('dataCache_' + str(i), CACHE_SIZE*1024, memorySize1+memorySize2, CACHE_WAYS, CACHE_BLOCK_SIZE, CACHE_REM_POLICY, CACHE_WR_POLICY))
        dataCaches[i].setReadLatency(scwrapper.sc_time(CACHE_READ_LAT,scwrapper.SC_NS))
        dataCaches[i].setWriteLatency(scwrapper.sc_time(CACHE_WRITE_LAT,scwrapper.SC_NS))
        dataCaches[i].setLoadLatency(scwrapper.sc_time(CACHE_LOAD_LAT,scwrapper.SC_NS))
        dataCaches[i].setStoreLatency(scwrapper.sc_time(CACHE_STORE_LAT,scwrapper.SC_NS))
        dataCaches[i].setRemoveLatency(scwrapper.sc_time(CACHE_REMOVE_LAT,scwrapper.SC_NS))
        #dataCaches[i].setScratchpad(4194304,1048576,scwrapper.sc_time(0.001,scwrapper.SC_NS))
        connectPorts(processors[i], processors[i].dataMem.initSocket, dataCaches[i], dataCaches[i].targetSocket)
        connectPorts(dataCaches[i], dataCaches[i].dirInitSocket, directory, directory.targetSocket)
        connectPorts(directory, directory.initSocket, dataCaches[i], dataCaches[i].dirTargetSocket)
        if NOC_ACTIVE:
            connectPorts(dataCaches[i], dataCaches[i].initSocket, noc, noc.targetSocket)
        else:
            connectPorts(dataCaches[i], dataCaches[i].initSocket, bus, bus.targetSocket)
    else:
        if NOC_ACTIVE:
            connectPorts(processors[i], processors[i].dataMem.initSocket, noc, noc.targetSocket)
        else:
            connectPorts(processors[i], processors[i].dataMem.initSocket, bus, bus.targetSocket)

    if INSTR_CACHE_ACTIVE:
        instrCaches.append(CoherentCacheLT32.CoherentCacheLT32('instrCache_' + str(i), CACHE_SIZE*1024, memorySize1+memorySize2, CACHE_WAYS, CACHE_BLOCK_SIZE, CACHE_REM_POLICY, CACHE_WR_POLICY))
        instrCaches[i].setReadLatency(scwrapper.sc_time(CACHE_READ_LAT,scwrapper.SC_NS))
        instrCaches[i].setWriteLatency(scwrapper.sc_time(CACHE_WRITE_LAT,scwrapper.SC_NS))
        instrCaches[i].setLoadLatency(scwrapper.sc_time(CACHE_LOAD_LAT,scwrapper.SC_NS))
        instrCaches[i].setStoreLatency(scwrapper.sc_time(CACHE_STORE_LAT,scwrapper.SC_NS))
        instrCaches[i].setRemoveLatency(scwrapper.sc_time(CACHE_REMOVE_LAT,scwrapper.SC_NS))
        #instrCaches[i].setScratchpad(4194304,1048576,scwrapper.sc_time(0.001,scwrapper.SC_NS))
        connectPorts(processors[i], processors[i].instrMem.initSocket, instrCaches[i], instrCaches[i].targetSocket)
        connectPorts(instrCaches[i], instrCaches[i].dirInitSocket, directory, directory.targetSocket)
        connectPorts(directory, directory.initSocket, instrCaches[i], instrCaches[i].dirTargetSocket)
        if NOC_ACTIVE:
            connectPorts(instrCaches[i], instrCaches[i].initSocket, noc, noc.targetSocket)
        else:
            connectPorts(instrCaches[i], instrCaches[i].initSocket, bus, bus.targetSocket)
    else:
        if NOC_ACTIVE:
            connectPorts(processors[i], processors[i].instrMem.initSocket, noc, noc.targetSocket)
        else:
            connectPorts(processors[i], processors[i].instrMem.initSocket, bus, bus.targetSocket)

##### RECONFIGURABLE COMPONENTS CONNECTIONS #####
if BUS_ACTIVE:
    connectPorts(cE, cE.ramSocket, bus, bus.targetSocket)
    if CONFIGURE_THROUGH_ITC:
        connectPorts(cE, cE.destSocket, bus, bus.targetSocket)
else:
    connectPorts(cE, cE.ramSocket, noc, noc.targetSocket)
    if CONFIGURE_THROUGH_ITC:
        connectPorts(cE, cE.destSocket, noc, noc.targetSocket)

bSPosition = 1
for i in range(0, EFPGA_NUMBER):
    connectPorts(cE, cE.initiatorSocket, eF[i], eF[i].targetSocket)
    if CONFIGURE_THROUGH_ITC:
        if BUS_ACTIVE:
            connectPorts(bus, bus.initiatorSocket, eF[i], eF[i].bS.targetSocket)
            bus.addBinding("bS"+str(bSPosition), memorySize1+memorySize2+bSPosition, memorySize1+memorySize2+bSPosition)
        else:
            connectPorts(noc, noc.initiatorSocket, eF[i], eF[i].bS.targetSocket)
            noc.addBinding("bS"+str(bSPosition), memorySize1+memorySize2+bSPosition, memorySize1+memorySize2+bSPosition)
    else:
        connectPorts(cE, cE.destSocket, eF[i], eF[i].bS.targetSocket)
    cE.bindFPGA(memorySize1+memorySize2+bSPosition)
    bSPosition = bSPosition+1

################################################
##### SYSTEM INIT ##############################
################################################

# Declare Python callbacks

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
#    if DATA_CACHE_ACTIVE or INSTR_CACHE_ACTIVE:
#        processors[i].enableHistory('outProc'+str(i))
#    else:
#        processors[i].enableHistory('outProcNC'+str(i))


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
        concurrentEmu = cm_wrapper.ConcurrencyEmulator32(processors[i].getInterface(),memorySize1+memorySize2)
        concurrentEmu.initSysCalls(SOFTWARE,True)
        processors[i].toolManager.addTool(concurrentEmu)
        tools.append(curEmu)
        tools.append(concurrentEmu)

pytCalls = list()
# Now I initialize the reconfiguration emulator
for i in range(0, PROCESSOR_NUMBER):
    recEmu = recEmu_wrapper.reconfEmulator32(processors[i].getInterface(),cE,SOFTWARE)
    processors[i].toolManager.addTool(recEmu)
    tools.append(recEmu)

    # Registration of the callbacks
    recEmu.registerCppCall('openWavToRead16')
    recEmu.registerCppCall('openWavToWrite16')
    recEmu.registerCppCall('readSample16')
    recEmu.registerCppCall('writeSample16')
    recEmu.registerCppCall('seekFirstSample16')
    recEmu.registerCppCall('closeWav16')
    recEmu.registerCppCall('printout')
    recEmu.registerCppCall('printresult')
    recEmu.registerCppCall('printStatus')

    # 50%
#    recEmu.registerCppCall('hcMixer',scwrapper.sc_time(3181, scwrapper.SC_NS),20,8)
#    recEmu.registerCppCall('decimatorFilter',scwrapper.sc_time(7882, scwrapper.SC_NS),20,20)
#    recEmu.registerCppCall('CW300Filter',scwrapper.sc_time(42497, scwrapper.SC_NS),20,50)
#    recEmu.registerCppCall('CW1KFilter',scwrapper.sc_time(21068, scwrapper.SC_NS),20,40)
#   recEmu.registerCppCall('SSB2K4Filter',scwrapper.sc_time(14860, scwrapper.SC_NS),20,30)
#    recEmu.registerCppCall('interpolatorFilter',scwrapper.sc_time(3477, scwrapper.SC_NS),20,20)
#    recEmu.registerCppCall('normalize',scwrapper.sc_time(4535771, scwrapper.SC_NS),50,96)
#    recEmu.registerCppCall('decode')
#    recEmu.registerCppCall('rectify',scwrapper.sc_time(10130, scwrapper.SC_NS),20,96)
#    recEmu.registerCppCall('average',scwrapper.sc_time(16646, scwrapper.SC_NS),40,96)
#    recEmu.registerCppCall('filter',scwrapper.sc_time(2908096, scwrapper.SC_NS),100,96)
#    recEmu.registerCppCall('posavg',scwrapper.sc_time(858, scwrapper.SC_NS),40,20)
#    recEmu.registerCppCall('negavg',scwrapper.sc_time(810, scwrapper.SC_NS),40,20)
#    recEmu.registerCppCall('decodeSymbol',scwrapper.sc_time(2004, scwrapper.SC_NS),10,20)

    # 25%
    recEmu.registerCppCall('hcMixer',scwrapper.sc_time(1590, scwrapper.SC_NS),20,8)
    recEmu.registerCppCall('decimatorFilter',scwrapper.sc_time(3941, scwrapper.SC_NS),20,20)
    recEmu.registerCppCall('CW300Filter',scwrapper.sc_time(21249, scwrapper.SC_NS),20,50)
    recEmu.registerCppCall('CW1KFilter',scwrapper.sc_time(10534, scwrapper.SC_NS),20,40)
    recEmu.registerCppCall('SSB2K4Filter',scwrapper.sc_time(7430, scwrapper.SC_NS),20,30)
    recEmu.registerCppCall('interpolatorFilter',scwrapper.sc_time(1739, scwrapper.SC_NS),20,20)
    recEmu.registerCppCall('normalize',scwrapper.sc_time(2267886, scwrapper.SC_NS),50,96)
#    recEmu.registerCppCall('decode')
    recEmu.registerCppCall('rectify',scwrapper.sc_time(5065, scwrapper.SC_NS),20,96)
    recEmu.registerCppCall('average',scwrapper.sc_time(8323, scwrapper.SC_NS),40,96)
    recEmu.registerCppCall('filter',scwrapper.sc_time(1454048, scwrapper.SC_NS),100,96)
#    recEmu.registerCppCall('posavg',scwrapper.sc_time(429, scwrapper.SC_NS),40,20)
#    recEmu.registerCppCall('negavg',scwrapper.sc_time(405, scwrapper.SC_NS),40,20)
#    recEmu.registerCppCall('decodeSymbol',scwrapper.sc_time(1002, scwrapper.SC_NS),10,20)

    # 10%
#    recEmu.registerCppCall('hcMixer',scwrapper.sc_time(636, scwrapper.SC_NS),20,8)
#    recEmu.registerCppCall('decimatorFilter',scwrapper.sc_time(1576, scwrapper.SC_NS),20,20)
#    recEmu.registerCppCall('CW300Filter',scwrapper.sc_time(8500, scwrapper.SC_NS),20,50)
#    recEmu.registerCppCall('CW1KFilter',scwrapper.sc_time(4214, scwrapper.SC_NS),20,40)
#    recEmu.registerCppCall('SSB2K4Filter',scwrapper.sc_time(2972, scwrapper.SC_NS),20,30)
#    recEmu.registerCppCall('interpolatorFilter',scwrapper.sc_time(695, scwrapper.SC_NS),20,20)
#    recEmu.registerCppCall('normalize',scwrapper.sc_time(907154, scwrapper.SC_NS),50,96)
#    recEmu.registerCppCall('decode')
#    recEmu.registerCppCall('rectify',scwrapper.sc_time(2026, scwrapper.SC_NS),20,96)
#    recEmu.registerCppCall('average',scwrapper.sc_time(3329, scwrapper.SC_NS),40,96)
#    recEmu.registerCppCall('filter',scwrapper.sc_time(581619, scwrapper.SC_NS),100,96)
#    recEmu.registerCppCall('posavg',scwrapper.sc_time(172, scwrapper.SC_NS),40,20)
#    recEmu.registerCppCall('negavg',scwrapper.sc_time(162, scwrapper.SC_NS),40,20)
#    recEmu.registerCppCall('decodeSymbol',scwrapper.sc_time(401, scwrapper.SC_NS),10,20)

    print 'Registered routine calls for processor ' + str(i) + ': '
    recEmu.printRegisteredFunctions()

# Printing some useful statistics before the simulation begins
if NOC_ACTIVE:
    noc.printBindings()
if BUS_ACTIVE:
    bus.printBindings()
cE.printSystemStatus()

# We can finally run the simulation
run_simulation()
