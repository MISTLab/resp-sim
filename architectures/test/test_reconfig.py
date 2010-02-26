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
MEMORY_SIZE       = 32              # MBytes
MEM_LATENCY       = 10.0            # ns
BUS_LATENCY       = 0.001            # ns

# Software
try:
    SOFTWARE
except:
    SOFTWARE = 'test_reconfig'

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

################################################
##### AUTO VARIABLE SETUP ######################
################################################

# Find the specified software in the _build_ directory if not an absolute path
if not SOFTWARE or not os.path.isfile(SOFTWARE):
    SOFTWARE = findInFolder(SOFTWARE, '_build_/arm/software/')
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
mem = MemoryLT32.MemoryLT32( 'mem', memorySize, latencyMem)

##### RECONFIGURABLE COMPONENTS INSTANTIATION #####
bS = bS_wrapper.bitstreamSink32('bS')
cE = cE_wrapper.configEngine('cE',processors[0].getInterface(),0x0,memorySize+1,cE_wrapper.RANDOM)
#cE.setRequestDelay(scwrapper.sc_time(100000, scwrapper.SC_NS))
#cE.setExecDelay(scwrapper.sc_time(100000, scwrapper.SC_NS))
#cE.setConfigDelay(scwrapper.sc_time(100000, scwrapper.SC_NS))
#cE.setRemoveDelay(scwrapper.sc_time(100000, scwrapper.SC_NS))
eF1 = eFPGA_wrapper.eFPGA('eF1',50,100,scwrapper.sc_time(0.025, scwrapper.SC_NS),2,5)
eF2 = eFPGA_wrapper.eFPGA('eF2',60,200,scwrapper.sc_time(0.005, scwrapper.SC_NS),4,10)

################################################
##### INTERCONNECTIONS #########################
################################################

latencyBus = scwrapper.sc_time(BUS_LATENCY, scwrapper.SC_NS)
bus  = BusLT32.BusLT32('bus',latencyBus,2)

##### BUS CONNECTIONS #####
# Connecting the master components to the bus
for i in range(0, PROCESSOR_NUMBER):
    connectPortsForce(processors[i], processors[i].instrMem.initSocket, bus, bus.targetSocket)
    connectPortsForce(processors[i], processors[i].dataMem.initSocket, bus, bus.targetSocket)

connectPortsForce(bus, bus.initiatorSocket, mem, mem.targetSocket)

# Add memory mapping
bus.addBinding("mem",0x0,memorySize,False)

##### RECONFIGURABLE COMPONENTS CONNECTIONS #####
connectPortsForce(cE, cE.busSocket, bus, bus.targetSocket)
connectPortsForce(bus, bus.initiatorSocket, bS, bS.targetSocket)
# Add bitstream sink binding
bus.addBinding("bS", memorySize+1, memorySize+1)

connectPortsForce(cE, cE.initiatorSocket, eF1, eF1.targetSocket)
connectPortsForce(cE, cE.initiatorSocket, eF2, eF2.targetSocket)

################################################
##### SYSTEM INIT ##############################
################################################

# Declare Python callbacks
class printValueCall(cE_wrapper.reconfCB32):
     def __init__(self, latency, w, h):
	cE_wrapper.reconfCB32.__init__(self, latency, w , h)
     def __call__(self, processorInstance):
        processorInstance.preCall();
        callArgs = processorInstance.readArgs()
        param1 = callArgs.__getitem__(0)

#	addr = 0
#	addr = cE.configure('printValue', self.latency, self.width, self.height, False)
#	cE.execute(addr)
#	print '(Python) printValue allocated at address ' + str(addr);

	cE.confexec('printValue', self.latency, self.width, self.height, False)

	cE.printSystemStatus()
#	cE.manualRemove('printValue')

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

# Now I initialize the reconfiguration emulator
cE.recEmu.initExec(SOFTWARE)
for i in range(0, PROCESSOR_NUMBER):
    processors[i].toolManager.addTool(cE.recEmu)

# Registration of the callbacks
printValueInstance = printValueCall(scwrapper.sc_time(0.100, scwrapper.SC_MS),50,100)			# REMEMBER: PYTHON USES ONLY NS!!
cE.registerPythonCall('printValue', printValueInstance);
#cE.registerCppCall('printValue', scwrapper.sc_time(0.050, scwrapper.SC_NS),25,50)
#cE.registerCppCall('printValue')

cE.registerCppCall('generate', scwrapper.sc_time(1, scwrapper.SC_NS),1,1)

cE.registerCppCall('sum');

# Printing some useful statistics before the simulation begins
bus.printBindings()
cE.recEmu.printRegisteredFunctions()
cE.printSystemStatus()

# We can finally run the simulation
run_simulation()
