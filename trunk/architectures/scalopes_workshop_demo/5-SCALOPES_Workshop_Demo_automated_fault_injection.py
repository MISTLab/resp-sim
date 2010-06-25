###### GENERAL PARAMETERS #####
PROCESSOR_FREQUENCY = 333         # MHz
PROCESSOR_NUMBER  = 1             #
try:
    PROCESSOR_NAMESPACE
except:
    PROCESSOR_NAMESPACE = arm9tdmi_funcLT_wrapper.Processor_arm9tdmi_funclt

# Memory/bus
MEMORY_SIZE       = 32              # MBytes
MEM_FREQUENCY     = 50              # MHz
BUS_ACTIVE        = True
BUS_FREQUENCY     = 50              # MHz

# Software
try:
    SOFTWARE
except:
    SOFTWARE = 'edgeDetectorARM'

if SOFTWARE:
    try:
        ARGS
    except:
        ARGS = []

OS_EMULATION = True     # True or False


# Modified stats auto-printer

SHORTLOG = "architectures/scalopes_workshop_demo/summary_classification_results.txt"
LOG = "architectures/scalopes_workshop_demo/classification_results.txt"

def statsPrinter():
    print '\n\x1b[34m\x1b[1mRESULTS:\x1b[0m'
    for tool in tools:
        if isinstance(tool,checkerTool.checkerTool32):
            log = ""
            shortlog = ''
            if controller.error == True:
              log = "Exception thrown\n"
              shortlog = 'exception\n'
            elif manager.getFaultInjector().timeout:
              log = "Simulation timeout\n"
              shortlog = 'timeout\n'
            else:
              log = tool.getLog()
              shortlog = tool.getShortLog() + '\n'
            print log
            fp=open(LOG,'a')
            fp.write("------------------------------------------------------------------------\n")
            fp.write('Experiment ' + str(fi._faultInjector__currExpNum) + "\n")
            fp.write("------------------------------------------------------------------------\n")
            fp.write(log)
            fp.write("\n")
            fp.close()

            fp=open(SHORTLOG,'a')
            fp.write(shortlog)
            fp.close()
            break

def campaignReportPrinter():
  no = 0
  to = 0
  exc = 0
  contr = 0
  crit = 0
  notcrit = 0
  fp=open(SHORTLOG,'r')
  for l in fp:
    if l == 'exception\n':
      exc = exc +1
    elif l == 'timeout\n':
      to = to +1
    elif l == 'controlflow\n':
      contr = contr +1
    elif l == 'corrupteddata\n':
      crit = crit +1
    elif l == 'non-corrupteddata\n':
      notcrit = notcrit +1
    else:
      no = no +1 
  fp.close()
  import os
  os.system('rm ' + SHORTLOG)
  print "\n\n-------------------------------------------------------------------------------------------------------" 
  print "Statistics\nNo effect: " + str(no) + "\nControlflow error: " + str(contr) + "\nCorrupted Image: " + str(crit) \
     + "\nNon-corrupted Image: " + str(notcrit) + "\nTimeout: " + str(to) + "\nException: " + str(exc)   
  print "-------------------------------------------------------------------------------------------------------" 

            
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

enable_fault_injection()

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
cE.setRequestDelay(scwrapper.sc_time(3, scwrapper.SC_NS))
cE.setExecDelay(scwrapper.sc_time(3, scwrapper.SC_NS))
cE.setConfigDelay(scwrapper.sc_time(3, scwrapper.SC_NS))
cE.setRemoveDelay(scwrapper.sc_time(3, scwrapper.SC_NS))
#eFPGAs
eF = []
eF.append(eFPGA_wrapper.eFPGA('eF1',100,100,scwrapper.sc_time(58, scwrapper.SC_NS),11,1))
EFPGA_NUMBER = eF.__len__()

################################################
##### INTERCONNECTIONS #########################
################################################

if BUS_ACTIVE:
    latencyBus = scwrapper.sc_time(float(1000)/float(BUS_FREQUENCY), scwrapper.SC_NS)
    bus = BusLT32.BusLT32('bus',2*PROCESSOR_NUMBER,latencyBus)

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

    bSPosition = 1
    for i in range(0, EFPGA_NUMBER):
        connectPorts(cE, cE.initiatorSocket, eF[i], eF[i].targetSocket)
        connectPorts(cE, cE.destSocket, eF[i], eF[i].bS.targetSocket)
        cE.bindFPGA(memorySize+bSPosition)
        bSPosition = bSPosition+1

else:
    if PROCESSOR_NUMBER > 1:
        raise Exception('Multi-core systems need to have an interconnection layer between processors and memory')
    else:
        ##### MEMORY CONNECTIONS #####
        # Connecting the master component to the memory
        raise Exception('Sorry, memory currently supports only a single connection (data or instruction). Please activate an interconnection layer between processors and memory')
        #connectPorts(processors[0], processors[0].instrMem.initSocket, mem, mem.targetSocket)
        #connectPorts(processors[0], processors[0].dataMem.initSocket, mem, mem.targetSocket)

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
    processors[i].ENTRY_POINT = loader.getProgStart()
    processors[i].PROGRAM_LIMIT = loader.getProgDim() + loader.getDataStart()
    processors[i].PROGRAM_START = loader.getDataStart()
    processors[i].resetOp();

# Now I initialize the OS emulator
tools = list()
print "Setting up OS Emulation"
if OS_EMULATION:
    trapwrapper.OSEmulatorBase.set_program_args(ARGS)
    for i in range(0, PROCESSOR_NUMBER):
        ## CHECKER ##
        FUNC_DESC = "architectures/scalopes_workshop_demo/functionsDescriptor.txt"
        PROFILER_OUTPUT_DIR = "architectures/scalopes_workshop_demo"
        CHECKPOINTS_LIST = "architectures/scalopes_workshop_demo/checkpointsList.txt"
        GOLDEN_TRACE = "architectures/scalopes_workshop_demo/short_trace.txt"
        checker = checkerTool.checkerTool32(processors[i].getInterface(), SOFTWARE, FUNC_DESC, CHECKPOINTS_LIST, GOLDEN_TRACE, PROFILER_OUTPUT_DIR)
        processors[i].toolManager.addTool(checker)
        tools.append(checker)

        curEmu = trapwrapper.OSEmulator32(processors[i].getInterface())
        curEmu.initSysCalls(SOFTWARE)
        processors[i].toolManager.addTool(curEmu)

# Now I initialize the reconfiguration emulator
for i in range(0, PROCESSOR_NUMBER):
    recEmu = recEmu_wrapper.reconfEmulator32(processors[i].getInterface(),cE,SOFTWARE)
    processors[i].toolManager.addTool(recEmu)
    tools.append(recEmu)

    # Registration of the callbacks
    recEmu.registerCppCall('puts', scwrapper.sc_time(1000, scwrapper.SC_NS),1,1)
    recEmu.registerCppCall('read_bitmap', scwrapper.sc_time(1000, scwrapper.SC_NS),1,1)
    recEmu.registerCppCall('write_bitmap', scwrapper.sc_time(1000, scwrapper.SC_NS),1,1)


manager.registerComponent(processors[0])

fi = manager.getFaultInjector()
#fi.generateFaultList(simulationDuration=330000000,numberOfSims=5,numberOfTimeIntervals=1,injectionTimeWindow=[12224206,303376586])


