PROCESSOR_FREQUENCY = 1000        # MHz
PROCESSOR_NAMESPACE = arm7tdmi_funcLT_wrapper.Processor_arm7tdmi_funclt

# Memory/bus
MEMORY_SIZE       = 32              # MBytes
MEM_LATENCY       = 10.0            # ns
BUS_ACTIVE        = True
BUS_LATENCY       = 10.0            # ns

SOFTWARE = 'test_fi_fir'
ARGS = [SOFTWARE]

OS_EMULATION = True     # True or False

# Modified stats auto-printer
results = [28, 36, 44, 52, 60, 68, 76, 84]
output = 'results.txt'

def statsPrinter():
    sw = buf.getStatus()
    risOk = True
    res = ''
    for i in range(len(results)):
      if results[i] != long(buf.getDatum(i)):
        risOk = False
        
    if controller.error == True:
      res = 'HW detected\n'
    elif sw != 0:
      res = 'SW detected\n'
    elif risOk == True and sw == 0:
      res = 'No effect\n'
    else:
      res = 'Not detected\n'
      t='Expected Result: '
      for i in range(len(results)):
        t=t+' ' + str(results[i])
      t=t+'\nObtained Result: '
      for i in range(len(results)):
        t=t+' ' + str(buf.getDatum(i))      
      print t
      print "Error: " +str(buf.getStatus())
    fp=open(output,'a')
    fp.write(res)
    fp.close()
    print res
    return


def campaignReportPrinter():
  no = 0
  sw = 0
  hw = 0
  ok = 0
  fp=open(output,'r')
  for l in fp:
    if l == 'HW detected\n':
      hw = hw +1
    elif l == 'SW detected\n':
      sw = sw +1
    elif l == 'Not detected\n':
      no = no +1
    else:
      ok = ok +1 
  fp.close()
  import os
  #os.system('rm results.txt')
  print "\n\n-------------------------------------------------------------------------------------------------------" 
  print "Statistics\nNo effect: " + str(ok) + "\nSW detected: " + str(sw) + "\nHW detected: " + str(hw) + "\nNot detected: " + str(no)   
  print "-------------------------------------------------------------------------------------------------------" 

enable_fault_injection()

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
latency = scwrapper.sc_time(float(1000)/float(PROCESSOR_FREQUENCY),  scwrapper.SC_NS)
processor = PROCESSOR_NAMESPACE('processor_0', latency)

##### MEMORY INSTANTIATION #####
memorySize = 1024*1024*MEMORY_SIZE
latencyMem = scwrapper.sc_time(MEM_LATENCY, scwrapper.SC_NS)
mem = MemoryLT32.MemoryLT32('mem', memorySize, latencyMem)

buf = DummyBufferLT32.DummyBufferLT32("buffer", 32, latencyMem)

################################################
##### INTERCONNECTIONS #########################
################################################

latencyBus = scwrapper.sc_time(BUS_LATENCY, scwrapper.SC_NS)
bus = BusLT32.BusLT32('bus',2,latencyBus)

##### BUS CONNECTIONS #####
# Connecting the master components to the bus
connectPorts(processor, processor.instrMem.initSocket, bus, bus.targetSocket)
connectPorts(processor, processor.dataMem.initSocket, bus, bus.targetSocket)

connectPorts(bus, bus.initiatorSocket, mem, mem.targetSocket)
connectPorts(bus, bus.initiatorSocket, buf, buf.targetSocket)

# Add memory mapping
bus.addBinding("mem",0x0,memorySize)
bus.addBinding("buffer", 0x0D800040, 0x0D8000C0)

################################################
##### SYSTEM INIT ##############################
################################################

##### LOAD SOFTWARE #####

import os
if not os.path.exists(SOFTWARE):
    raise Exception('Error, ' + str(SOFTWARE) + ' does not exists')

loader = loader_wrapper.Loader(SOFTWARE)
#Initialization of the processors and loading in memory of the application program
loader.loadProgInMemory(mem)

processor.ENTRY_POINT = loader.getProgStart()
processor.PROGRAM_LIMIT = loader.getProgDim() + loader.getDataStart()
processor.PROGRAM_START = loader.getDataStart()
processor.resetOp();

# Now I initialize the OS emulator
if OS_EMULATION:
    curEmu = trapwrapper.OSEmulator32(processor.getInterface())
    curEmu.initSysCalls(SOFTWARE)
    curEmu.set_program_args(ARGS)
    processor.toolManager.addTool(curEmu)

# We can finally run the simulation
#run_simulation()

manager.registerComponent(processor)

fi = manager.getFaultInjector()
#fi.loadFaultList('tmpList.txt')
fi.generateFaultList(simulationDuration=350000,numberOfSims=10)
