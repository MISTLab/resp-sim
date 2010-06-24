###### GENERAL PARAMETERS #####
PROCESSOR_FREQUENCY = 333         # MHz
PROCESSOR_NUMBER  = 4             #
try:
    PROCESSOR_NAMESPACE
except:
    PROCESSOR_NAMESPACE = arm9tdmi_funcLT_wrapper.Processor_arm9tdmi_funclt

# Memory/bus
MEMORY_SIZE       = 32              # MBytes
MEM_FREQUENCY     = 50              # MHz
BUS_ACTIVE        = True
BUS_FREQUENCY     = 50              # MHz
CONFIGURE_THROUGH_ITC = False
DATA_CACHE_ACTIVE  = True
INSTR_CACHE_ACTIVE = True
CACHE_SIZE         = 16             # KBytes
CACHE_BLOCK_SIZE   = 8              # words
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
    SOFTWARE = 'edgeDetectorARMParallel'

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
    if BUS_ACTIVE:
        print '\x1b[34m\x1b[1mBus Accesses:\x1b[0m'
        bus.printAccesses()
    if DATA_CACHE_ACTIVE:
        print '\x1b[34m\x1b[1mData Cache 1 Stats:\x1b[0m'
        print '\x1b[32m\x1b[1mRead Hits   \x1b[0m\x1b[31m' + str(dataCaches[0].numReadHit) + '\t\t \x1b[0m\x1b[32m\x1b[1mRead Miss   \x1b[0m\x1b[31m' + str(dataCaches[0].numReadMiss) + '\x1b[0m'
        print '\x1b[32m\x1b[1mWrite Hits  \x1b[0m\x1b[31m' + str(dataCaches[0].numWriteHit) + '\t\t \x1b[0m\x1b[32m\x1b[1mWrite Miss  \x1b[0m\x1b[31m' + str(dataCaches[0].numWriteMiss) + '\x1b[0m'
        print '\x1b[34m\x1b[1mData Cache 2 Stats:\x1b[0m'
        print '\x1b[32m\x1b[1mRead Hits   \x1b[0m\x1b[31m' + str(dataCaches[1].numReadHit) + '\t\t \x1b[0m\x1b[32m\x1b[1mRead Miss   \x1b[0m\x1b[31m' + str(dataCaches[1].numReadMiss) + '\x1b[0m'
        print '\x1b[32m\x1b[1mWrite Hits  \x1b[0m\x1b[31m' + str(dataCaches[1].numWriteHit) + '\t\t \x1b[0m\x1b[32m\x1b[1mWrite Miss  \x1b[0m\x1b[31m' + str(dataCaches[1].numWriteMiss) + '\x1b[0m'
        print '\x1b[34m\x1b[1mData Cache 3 Stats:\x1b[0m'
        print '\x1b[32m\x1b[1mRead Hits   \x1b[0m\x1b[31m' + str(dataCaches[2].numReadHit) + '\t\t \x1b[0m\x1b[32m\x1b[1mRead Miss   \x1b[0m\x1b[31m' + str(dataCaches[2].numReadMiss) + '\x1b[0m'
        print '\x1b[32m\x1b[1mWrite Hits  \x1b[0m\x1b[31m' + str(dataCaches[2].numWriteHit) + '\t\t \x1b[0m\x1b[32m\x1b[1mWrite Miss  \x1b[0m\x1b[31m' + str(dataCaches[2].numWriteMiss) + '\x1b[0m'
        print '\x1b[34m\x1b[1mData Cache 4 Stats:\x1b[0m'
        print '\x1b[32m\x1b[1mRead Hits   \x1b[0m\x1b[31m' + str(dataCaches[3].numReadHit) + '\t\t \x1b[0m\x1b[32m\x1b[1mRead Miss   \x1b[0m\x1b[31m' + str(dataCaches[3].numReadMiss) + '\x1b[0m'
        print '\x1b[32m\x1b[1mWrite Hits  \x1b[0m\x1b[31m' + str(dataCaches[3].numWriteHit) + '\t\t \x1b[0m\x1b[32m\x1b[1mWrite Miss  \x1b[0m\x1b[31m' + str(dataCaches[3].numWriteMiss) + '\x1b[0m'
    if INSTR_CACHE_ACTIVE:
        print '\x1b[34m\x1b[1mInstruction Cache 1 Stats:\x1b[0m'
        print '\x1b[32m\x1b[1mRead Hits   \x1b[0m\x1b[31m' + str(instrCaches[0].numReadHit) + '\t\t \x1b[0m\x1b[32m\x1b[1mRead Miss   \x1b[0m\x1b[31m' + str(instrCaches[0].numReadMiss) + '\x1b[0m'
        print '\x1b[32m\x1b[1mWrite Hits  \x1b[0m\x1b[31m' + str(instrCaches[0].numWriteHit) + '\t\t\t \x1b[0m\x1b[32m\x1b[1mWrite Miss  \x1b[0m\x1b[31m' + str(instrCaches[0].numWriteMiss) + '\x1b[0m'
        print '\x1b[34m\x1b[1mInstruction Cache 2 Stats:\x1b[0m'
        print '\x1b[32m\x1b[1mRead Hits   \x1b[0m\x1b[31m' + str(instrCaches[1].numReadHit) + '\t\t \x1b[0m\x1b[32m\x1b[1mRead Miss   \x1b[0m\x1b[31m' + str(instrCaches[1].numReadMiss) + '\x1b[0m'
        print '\x1b[32m\x1b[1mWrite Hits  \x1b[0m\x1b[31m' + str(instrCaches[1].numWriteHit) + '\t\t\t \x1b[0m\x1b[32m\x1b[1mWrite Miss  \x1b[0m\x1b[31m' + str(instrCaches[1].numWriteMiss) + '\x1b[0m'
        print '\x1b[34m\x1b[1mInstruction Cache 3 Stats:\x1b[0m'
        print '\x1b[32m\x1b[1mRead Hits   \x1b[0m\x1b[31m' + str(instrCaches[2].numReadHit) + '\t\t \x1b[0m\x1b[32m\x1b[1mRead Miss   \x1b[0m\x1b[31m' + str(instrCaches[2].numReadMiss) + '\x1b[0m'
        print '\x1b[32m\x1b[1mWrite Hits  \x1b[0m\x1b[31m' + str(instrCaches[2].numWriteHit) + '\t\t\t \x1b[0m\x1b[32m\x1b[1mWrite Miss  \x1b[0m\x1b[31m' + str(instrCaches[2].numWriteMiss) + '\x1b[0m'
        print '\x1b[34m\x1b[1mInstruction Cache 4 Stats:\x1b[0m'
        print '\x1b[32m\x1b[1mRead Hits   \x1b[0m\x1b[31m' + str(instrCaches[3].numReadHit) + '\t\t \x1b[0m\x1b[32m\x1b[1mRead Miss   \x1b[0m\x1b[31m' + str(instrCaches[3].numReadMiss) + '\x1b[0m'
        print '\x1b[32m\x1b[1mWrite Hits  \x1b[0m\x1b[31m' + str(instrCaches[3].numWriteHit) + '\t\t\t \x1b[0m\x1b[32m\x1b[1mWrite Miss  \x1b[0m\x1b[31m' + str(instrCaches[3].numWriteMiss) + '\x1b[0m'

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
latencyMem = scwrapper.sc_time(float(1000)/float(MEM_FREQUENCY), scwrapper.SC_NS)
mem = MemoryLT32.MemoryLT32('mem', memorySize, latencyMem)

##### RECONFIGURABLE COMPONENTS INSTANTIATION #####
cE = cE_wrapper.configEngine('cE',0x0,cE_wrapper.LRU)
cE.setRequestDelay(scwrapper.sc_time(1, scwrapper.SC_NS))
cE.setExecDelay(scwrapper.sc_time(1, scwrapper.SC_NS))
cE.setConfigDelay(scwrapper.sc_time(1, scwrapper.SC_NS))
cE.setRemoveDelay(scwrapper.sc_time(1, scwrapper.SC_NS))
#eFPGAs
eF = []
eF.append(eFPGA_wrapper.eFPGA('eF1',50,100,scwrapper.sc_time(0.025, scwrapper.SC_NS),2,5))
eF.append(eFPGA_wrapper.eFPGA('eF2',60,200,scwrapper.sc_time(0.005, scwrapper.SC_NS),4,10))
EFPGA_NUMBER = eF.__len__()

################################################
##### INTERCONNECTIONS #########################
################################################

additionalMasters = 1
if CONFIGURE_THROUGH_ITC:
    additionalMasters = additionalMasters + 1
if BUS_ACTIVE:
    latencyBus = scwrapper.sc_time(float(1000)/float(BUS_FREQUENCY), scwrapper.SC_NS)
    bus = BusLT32.BusLT32('bus',2*PROCESSOR_NUMBER+additionalMasters,latencyBus)
    connectPorts(bus, bus.initiatorSocket, mem, mem.targetSocket)
   # Add memory mapping
    bus.addBinding("mem",0x0,memorySize)
else:
    if PROCESSOR_NUMBER > 1:
        raise Exception('Multi-core systems need to have an interconnection layer between processors and memory')
    else:
        ##### MEMORY CONNECTIONS #####
        # Connecting the master component to the memory
        raise Exception('Sorry, memory currently supports only a single connection (data or instruction). Please activate an interconnection layer between processors and memory')
        #connectPorts(processors[0], processors[0].instrMem.initSocket, mem, mem.targetSocket)
        #connectPorts(processors[0], processors[0].dataMem.initSocket, mem, mem.targetSocket)

if DATA_CACHE_ACTIVE or INSTR_CACHE_ACTIVE:
    directory = DirectoryLT32.DirectoryLT32('dir',2*PROCESSOR_NUMBER)

##### CACHE, BUS, AND MEMORY CONNECTIONS #####
dataCaches = []
instrCaches = []
for i in range(0, PROCESSOR_NUMBER):
    if DATA_CACHE_ACTIVE:
        dataCaches.append(CoherentCacheLT32.CoherentCacheLT32('dataCache_' + str(i), CACHE_SIZE*1024*1024, memorySize, CACHE_WAYS, CACHE_BLOCK_SIZE, CACHE_REM_POLICY, CACHE_WR_POLICY))
        dataCaches[i].setReadLatency(scwrapper.sc_time(CACHE_READ_LAT,scwrapper.SC_NS))
        dataCaches[i].setWriteLatency(scwrapper.sc_time(CACHE_WRITE_LAT,scwrapper.SC_NS))
        dataCaches[i].setLoadLatency(scwrapper.sc_time(CACHE_LOAD_LAT,scwrapper.SC_NS))
        dataCaches[i].setStoreLatency(scwrapper.sc_time(CACHE_STORE_LAT,scwrapper.SC_NS))
        dataCaches[i].setRemoveLatency(scwrapper.sc_time(CACHE_REMOVE_LAT,scwrapper.SC_NS))
        #dataCaches[i].setScratchpad(4194304,1048576,scwrapper.sc_time(0.001,scwrapper.SC_NS))
        connectPorts(processors[i], processors[i].dataMem.initSocket, dataCaches[i], dataCaches[i].targetSocket)
        connectPorts(dataCaches[i], dataCaches[i].dirInitSocket, directory, directory.targetSocket)
        connectPorts(directory, directory.initSocket, dataCaches[i], dataCaches[i].dirTargetSocket)
        connectPorts(dataCaches[i], dataCaches[i].initSocket, bus, bus.targetSocket)
    else:
        connectPorts(processors[i], processors[i].dataMem.initSocket, bus, bus.targetSocket)

    if INSTR_CACHE_ACTIVE:
        instrCaches.append(CoherentCacheLT32.CoherentCacheLT32('instrCache_' + str(i), CACHE_SIZE*1024*1024, memorySize, CACHE_WAYS, CACHE_BLOCK_SIZE, CACHE_REM_POLICY, CACHE_WR_POLICY))
        instrCaches[i].setReadLatency(scwrapper.sc_time(CACHE_READ_LAT,scwrapper.SC_NS))
        instrCaches[i].setWriteLatency(scwrapper.sc_time(CACHE_WRITE_LAT,scwrapper.SC_NS))
        instrCaches[i].setLoadLatency(scwrapper.sc_time(CACHE_LOAD_LAT,scwrapper.SC_NS))
        instrCaches[i].setStoreLatency(scwrapper.sc_time(CACHE_STORE_LAT,scwrapper.SC_NS))
        instrCaches[i].setRemoveLatency(scwrapper.sc_time(CACHE_REMOVE_LAT,scwrapper.SC_NS))
        #instrCaches[i].setScratchpad(4194304,1048576,scwrapper.sc_time(0.001,scwrapper.SC_NS))
        connectPorts(processors[i], processors[i].instrMem.initSocket, instrCaches[i], instrCaches[i].targetSocket)
        connectPorts(instrCaches[i], instrCaches[i].dirInitSocket, directory, directory.targetSocket)
        connectPorts(directory, directory.initSocket, instrCaches[i], instrCaches[i].dirTargetSocket)
        connectPorts(instrCaches[i], instrCaches[i].initSocket, bus, bus.targetSocket)
    else:
        connectPorts(processors[i], processors[i].instrMem.initSocket, bus, bus.targetSocket)

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

##### LOAD SOFTWARE #####

import os
if not os.path.exists(SOFTWARE):
    raise Exception('Error, ' + str(SOFTWARE) + ' does not exists')

loader = loader_wrapper.Loader(SOFTWARE)
print "Writing memory"
loader.loadProgInMemory(mem)

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
cm_wrapper.ConcurrencyManager.threadStackSize = 1024*40
tools = list()
if OS_EMULATION:
    trapwrapper.OSEmulatorBase.set_program_args(ARGS)
    for i in range(0, PROCESSOR_NUMBER):
        curEmu = trapwrapper.OSEmulator32(processors[i].getInterface())
        curEmu.initSysCalls(SOFTWARE)
        processors[i].toolManager.addTool(curEmu)
        ##### CONCURRENCY MANAGEMENT #####
        concurrentEmu = cm_wrapper.ConcurrencyEmulator32(processors[i].getInterface(),memorySize)
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
    recEmu.registerCppCall('rgb2grey', scwrapper.sc_time(1000, scwrapper.SC_NS),50,100)
    recEmu.registerCppCall('edgeOverlapping', scwrapper.sc_time(1000, scwrapper.SC_NS),60,150)
    print 'Registered routine calls for processor ' + str(i) + ': '
    recEmu.printRegisteredFunctions()
