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
MEM1_SIZE          = 128             # KBytes
MEM1_FREQUENCY     = 166             # MHz
MEM2_SIZE          = 32              # MBytes
MEM2_FREQUENCY     = 50              # MHz
BSRAM_SIZE         = 8               # KBytes
BSRAM_FREQUENCY    = 333             # MHz
BUS_ACTIVE         = True
BUS_FREQUENCY      = 83              # MHz
NOC_ACTIVE         = True
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
CACHE_READ_LAT     = 10.0            # ns
CACHE_WRITE_LAT    = 10.0            # ns
CACHE_LOAD_LAT     = 10.0            # ns
CACHE_STORE_LAT    = 10.0            # ns
CACHE_REMOVE_LAT   = 10.0            # ns

# Software
try:
    SOFTWARE
except:
    SOFTWARE = 'c_pi'

if SOFTWARE:
    try:
        ARGS
    except:
        ARGS = []
        ARGS.append('ffmpeg')
        ARGS.append('-i')
        ARGS.append('sheep.mpg')
#        ARGS.append('software/apps/ffmpeg/minimal.mpg')
        ARGS.append('-b')
        ARGS.append('64000')
        ARGS.append('-threads')
        ARGS.append(str(PROCESSOR_NUMBER))
        ARGS.append('sheep2.mpg')
#        ARGS.append('software/apps/ffmpeg/minimal2.mpg')

OS_EMULATION = True     # True or False

# Modified stats auto-printer
FILE_OUTPUT = False
def statsPrinter():
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
cE = cE_wrapper.configEngine('cE',0x0,cE_wrapper.LRU)
cE.setRequestDelay(scwrapper.sc_time(10, scwrapper.SC_NS))
cE.setExecDelay(scwrapper.sc_time(10, scwrapper.SC_NS))
cE.setConfigDelay(scwrapper.sc_time(10, scwrapper.SC_NS))
cE.setRemoveDelay(scwrapper.sc_time(10, scwrapper.SC_NS))
#eFPGAs
eF = []
eF.append(eFPGA_wrapper.eFPGA('eF1',100,100,scwrapper.sc_time(58, scwrapper.SC_NS),11,1))
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
    noc.addBinding("mem1",0x0,memorySize1)
    noc.addBinding("mem2",memorySize1+1,memorySize1+memorySize2)
#    noc.addBinding("bus",memorySize1+memorySize2+1,memorySize1+memorySize2+1)
    bSmem = MemoryLT32.MemoryLT32('bSRAM', memorySize1, latencyMem1)
    connectPorts(bus, bus.initiatorSocket, bSmem, bSmem.targetSocket)
    bus.addBinding("bSRAM",0x0,memorySize1)
elif BUS_ACTIVE:
    latencyBus = scwrapper.sc_time(float(1000)/float(BUS_FREQUENCY), scwrapper.SC_NS)
    bus = BusLT32.BusLT32('bus',2*PROCESSOR_NUMBER+additionalMasters,latencyBus)
    connectPorts(bus, bus.initiatorSocket, mem1, mem1.targetSocket)
    connectPorts(bus, bus.initiatorSocket, mem2, mem2.targetSocket)
   # Add memory mapping
    bus.addBinding("mem1",0x0,memorySize1)
    bus.addBinding("mem2",memorySize1+1,memorySize1+memorySize2)
elif NOC_ACTIVE:
    latencyNoc = scwrapper.sc_time(float(1000)/float(NOC_FREQUENCY), scwrapper.SC_NS)
    if CONFIGURE_THROUGH_ITC:
        noc = NocLT32.NocLT32('noc',2*PROCESSOR_NUMBER+additionalMasters,2+EFPGA_NUMBER,NOC_TOPOLOGY,latencyNoc)
    else:
        noc = NocLT32.NocLT32('noc',2*PROCESSOR_NUMBER+additionalMasters,2,NOC_TOPOLOGY,latencyNoc)
    connectPorts(noc, noc.initiatorSocket, mem1, mem1.targetSocket)
    connectPorts(noc, noc.initiatorSocket, mem2, mem2.targetSocket)
    # Add memory mapping
    noc.addBinding("mem1",0x0,memorySize1)
    noc.addBinding("mem2",memorySize1+1,memorySize1+memorySize2)
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
            connectPorts(bus, bus.initiatorSocket, eF[i].bS, eF[i].bS.targetSocket)
            bus.addBinding("bS"+str(bSPosition), memorySize1+memorySize2+bSPosition, memorySize1+memorySize2+bSPosition)
        else:
            connectPorts(noc, noc.initiatorSocket, eF[i].bS, eF[i].bS.targetSocket)
            noc.addBinding("bS"+str(bSPosition), memorySize1+memorySize2+bSPosition, memorySize1+memorySize2+bSPosition)
    else:
        connectPorts(cE, cE.destSocket, eF[i].bS, eF[i].bS.targetSocket)
    cE.bindFPGA(memorySize1+memorySize2+bSPosition)
    bSPosition = bSPosition+1

################################################
##### SYSTEM INIT ##############################
################################################

# Declare Python callbacks
class printValueCall(recEmu_wrapper.reconfCB32):
     def __init__(self, latency, w, h):
        recEmu_wrapper.reconfCB32.__init__(self, latency, w , h)
     def __call__(self, processorInstance):

        processorInstance.preCall();
        callArgs = processorInstance.readArgs()
        param1 = callArgs.__getitem__(0)

#        addr = 0
#        addr = cE.configure('printValue', self.latency, self.width, self.height, False)
#        cE.execute(addr)
#        cE.execute('printValue')
#        print '(Python) printValue allocated at address ' + str(addr);

        cE.executeForce('printValue', self.latency, self.width, self.height, False)

        cE.printSystemStatus()
#        cE.manualRemove('printValue')

        print '(Python) Value: ' + str(param1)
#        print 'Width: ' + str(self.width) + '\tHeight: ' + str(self.height) + '\tLatency: ' + str(self.latency.to_string())

        processorInstance.setRetVal(param1 + 1);
        processorInstance.returnFromCall();
        processorInstance.postCall();
        return True

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
    trapwrapper.OSEmulatorBase.set_program_args(ARGS)
    for i in range(0, PROCESSOR_NUMBER):
        curEmu = trapwrapper.OSEmulator32(processors[i].getInterface())
        curEmu.initSysCalls(SOFTWARE)
        processors[i].toolManager.addTool(curEmu)
        ##### CONCURRENCY MANAGEMENT #####
        concurrentEmu = cm_wrapper.ConcurrencyEmulator32(processors[i].getInterface(),memorySize1+memorySize2)
        concurrentEmu.initSysCalls(SOFTWARE,True)
        processors[i].toolManager.addTool(concurrentEmu)
        tools.append(curEmu)
        tools.append(concurrentEmu)
    # OpenMP Support
    trapwrapper.OSEmulatorBase.set_environ('OMP_NUM_THREADS', str(PROCESSOR_NUMBER)) 

pytCalls = list()
# Now I initialize the reconfiguration emulator
for i in range(0, PROCESSOR_NUMBER):
    recEmu = recEmu_wrapper.reconfEmulator32(processors[i].getInterface(),cE,SOFTWARE)
    processors[i].toolManager.addTool(recEmu)
    tools.append(recEmu)

    # Registration of the callbacks
    printValueInstance = printValueCall(scwrapper.sc_time(0.100, scwrapper.SC_MS),10,10)			# REMEMBER: PYTHON USES ONLY NS!!
#    recEmu.registerPythonCall('printValue', printValueInstance);
    pytCalls.append(printValueInstance)

#    recEmu.registerCppCall('printValue', scwrapper.sc_time(0.100, scwrapper.SC_NS),10,10)
#    recEmu.registerCppCall('printValue')
#    recEmu.registerCppCall('generate', scwrapper.sc_time(0.010, scwrapper.SC_NS),1,1)
#    recEmu.registerCppCall('sum');
    print 'Registered routine calls for processor ' + str(i) + ': '
    recEmu.printRegisteredFunctions()

# Printing some useful statistics before the simulation begins
if NOC_ACTIVE:
    noc.printBindings()
if BUS_ACTIVE:
    bus.printBindings()
cE.printSystemStatus()

# We can finally run the simulation
#run_simulation()
