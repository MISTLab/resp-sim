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
PROCESSOR_FREQUENCY = 1000        # MHz
PROCESSOR_NUMBER  = 1             #
try:
    PROCESSOR_NAMESPACE
except:
    PROCESSOR_NAMESPACE = arm7tdmi_funcLT_wrapper.Processor_arm7tdmi_funclt

# Memory/bus
MEMORY_SIZE       = 32              # MBytes
MEM_LATENCY       = 10.0            # ns
BUS_LATENCY       = 10.0            # ns
CONFIGURE_THROUGH_ITC = False

# Software
try:
    SOFTWARE
except:
    SOFTWARE = 'edgeDetector'

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
    print '\x1b[34m\x1b[1mBus Accesses:\x1b[0m'
    print '\x1b[31m' + str(bus.numAccesses) + '\x1b[0m'
    print '\x1b[34m\x1b[1mBus Words:\x1b[0m'
    print '\x1b[31m' + str(bus.numWords) + '\x1b[0m'

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
memorySize = 1024*1024*MEMORY_SIZE
latencyMem = scwrapper.sc_time(MEM_LATENCY, scwrapper.SC_NS)
mem = MemoryLT32.MemoryLT32('mem', memorySize, latencyMem)

##### RECONFIGURABLE COMPONENTS INSTANTIATION #####
cE = cE_wrapper.configEngine('cE',0x0,cE_wrapper.LRU)
cE.setRequestDelay(scwrapper.sc_time(1, scwrapper.SC_NS))
cE.setExecDelay(scwrapper.sc_time(1, scwrapper.SC_NS))
cE.setConfigDelay(scwrapper.sc_time(1, scwrapper.SC_NS))
cE.setRemoveDelay(scwrapper.sc_time(1, scwrapper.SC_NS))
eF = []
eF.append(eFPGA_wrapper.eFPGA('eF1',50,100,scwrapper.sc_time(0.025, scwrapper.SC_NS),2,5) )
eF.append(eFPGA_wrapper.eFPGA('eF2',60,200,scwrapper.sc_time(0.005, scwrapper.SC_NS),4,10) )
EFPGA_NUMBER = eF.__len__()

################################################
##### INTERCONNECTIONS #########################
################################################

additionalMasters = 1
if CONFIGURE_THROUGH_ITC:
    additionalMasters = additionalMasters + 1
latencyBus = scwrapper.sc_time(BUS_LATENCY, scwrapper.SC_NS)
bus  = BusLT32.BusLT32('bus',2*PROCESSOR_NUMBER+additionalMasters,latencyBus)

##### BUS CONNECTIONS #####
# Connecting the master components to the bus
for i in range(0, PROCESSOR_NUMBER):
    connectPorts(processors[i], processors[i].instrMem.initSocket, bus, bus.targetSocket)
    connectPorts(processors[i], processors[i].dataMem.initSocket, bus, bus.targetSocket)

connectPorts(bus, bus.initiatorSocket, mem, mem.targetSocket)

# Add memory mapping
bus.addBinding("mem",0x0,memorySize)

##### RECONFIGURABLE COMPONENTS CONNECTIONS #####
connectPorts(cE, cE.ramSocket, bus, bus.targetSocket)
if CONFIGURE_THROUGH_ITC:
   connectPorts(cE, cE.destSocket, bus, bus.targetSocket)

bSPosition = 1
for i in range(0, EFPGA_NUMBER):
    connectPorts(cE, cE.initiatorSocket, eF[i], eF[i].targetSocket)
    if CONFIGURE_THROUGH_ITC:
        connectPorts(bus, bus.initiatorSocket, eF[i], eF[i].bS.targetSocket)
        bus.addBinding("bS"+str(bSPosition), memorySize+bSPosition, memorySize+bSPosition)
    else:
        connectPorts(cE, cE.destSocket, eF[i], eF[i].bS.targetSocket)
    cE.bindFPGA(memorySize+bSPosition)
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
#Initialization of the processors and loading in memory of the application program
print "Writing memory"
loader.loadProgInMemory(mem)

print "Setting up CPU registers"
for i in range(0, PROCESSOR_NUMBER):
    processors[i].ENTRY_POINT = loader.getProgStart()
    processors[i].PROGRAM_LIMIT = loader.getProgDim() + loader.getDataStart()
    processors[i].PROGRAM_START = loader.getDataStart()
    processors[i].resetOp()

# Now I initialize the OS emulator
print "Setting up OS Emulation"
tools = list()
if OS_EMULATION:
    for i in range(0, PROCESSOR_NUMBER):
        curEmu = trapwrapper.OSEmulator32(processors[i].getInterface())
        curEmu.initSysCalls(SOFTWARE)
        curEmu.set_program_args(ARGS)
        processors[i].toolManager.addTool(curEmu)
        tools.append(curEmu)

pytCalls = list()
# Now I initialize the reconfiguration emulator
for i in range(0, PROCESSOR_NUMBER):
    recEmu = recEmu_wrapper.reconfEmulator32(processors[i].getInterface(),cE,SOFTWARE)
    processors[i].toolManager.addTool(recEmu)
    tools.append(recEmu)

    # Registration of the callbacks
    printValueInstance = printValueCall(scwrapper.sc_time(0.100, scwrapper.SC_MS),50,100)			# REMEMBER: PYTHON USES ONLY NS!!
    recEmu.registerPythonCall('printValue', printValueInstance);
    pytCalls.append(printValueInstance)

#    recEmu.registerCppCall('printValue', scwrapper.sc_time(0.100, scwrapper.SC_NS),50,100)
#    recEmu.registerCppCall('printValue')
    recEmu.registerCppCall('generate', scwrapper.sc_time(1, scwrapper.SC_NS),1,1)
    recEmu.registerCppCall('sum');
    recEmu.registerCppCall('read_bitmap');
    recEmu.registerCppCall('write_bitmap');
    print 'Registered routine calls for processor ' + str(i) + ': '
    recEmu.printRegisteredFunctions()

# Printing some useful statistics before the simulation begins
bus.printBindings()
cE.printSystemStatus()

# We can finally run the simulation
#run_simulation()
