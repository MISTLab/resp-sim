def statsPrinter():
    print '\x1b[34m\x1b[1mSimulated Elapsed Time (nano-seconds):\x1b[0m'
    print '\x1b[31m' + str(controller.get_simulated_time()) + '\x1b[0m'

withBus = True
withDataCache = True
withInstrCache = True
zeroTime = False
memorySize = 1024*1024*256
memoryLatency = scwrapper.sc_time(5,scwrapper.SC_NS)
busLatency = scwrapper.sc_time(5,scwrapper.SC_NS)
progName = 'arm.out'

a9 = arm9tdmi_funcLT_wrapper.Processor_arm9tdmi_funclt('a9',scwrapper.sc_time(10.0,scwrapper.SC_NS))
if zeroTime and withBus:
    bus  = BusLT32.BusLT32('bus',scwrapper.SC_ZERO_TIME,2)
    mem = MemoryLT32.MemoryLT32('mem', memorySize, scwrapper.SC_ZERO_TIME)
elif withBus:
    bus  = BusLT32.BusLT32('bus',busLatency,2)
    mem = MemoryLT32.MemoryLT32('mem', memorySize, memoryLatency)
elif zeroTime:
    mem = MemoryLT32.MemoryLT32('mem', memorySize, scwrapper.SC_ZERO_TIME)
else:
    mem = MemoryLT32.MemoryLT32('mem', memorySize, memoryLatency)
    
if withDataCache:
    dataCache = CacheLT32.CacheLT32('dataCache', 8192, memorySize, 32, 2, CacheLT32.LRU, CacheLT32.BACK)
    #dataCache.setReadLatency(scwrapper.sc_time(0.001,scwrapper.SC_PS))
    #dataCache.setScratchpad(4194304,1048576,scwrapper.sc_time(0.001,scwrapper.SC_PS))
    connectPortsForce(a9, a9.dataMem.initSocket, dataCache, dataCache.targetSocket)
    if withBus:
        connectPortsForce(dataCache, dataCache.initSocket, bus, bus.targetSocket)
    else:
        connectPortsForce(dataCache, dataCache.initSocket, mem, mem.targetSocket)
else:
    if withBus:
        connectPortsForce(a9, a9.dataMem.initSocket, bus, bus.targetSocket)
    else:
        connectPortsForce(a9, a9.dataMem.initSocket, mem, mem.targetSocket)
    
if withInstrCache:
    instrCache = CacheLT32.CacheLT32('instrCache', 8192, memorySize, 32, 2, CacheLT32.LRU, CacheLT32.BACK)
    connectPortsForce(a9, a9.instrMem.initSocket, instrCache, instrCache.targetSocket)
    if withBus:
        connectPortsForce(instrCache, instrCache.initSocket, bus, bus.targetSocket)
    else:
        connectPortsForce(instrCache, instrCache.initSocket, mem, mem.targetSocket)
else:
    if withBus:
        connectPortsForce(a9, a9.instrMem.initSocket, bus, bus.targetSocket)
    else:
        connectPortsForce(a9, a9.instrMem.initSocket, mem, mem.targetSocket)

if withBus:
    connectPortsForce(bus, bus.initiatorSocket, mem, mem.targetSocket)
    bus.addBinding("mem",0x0,memorySize,False)

loader = loader_wrapper.Loader(progName)
#Initialization of the processors and loading in memory of the application program
for i in range(0, loader.getProgDim()):
    mem.write_byte_dbg(i + loader.getDataStart(), loader.getProgDataValue(i))
a9.ENTRY_POINT = loader.getProgStart()
a9.PROGRAM_LIMIT = loader.getProgDim() + loader.getDataStart()
a9.PROGRAM_START = loader.getDataStart()

curEmu = trapwrapper.OSEmulator32(a9.getInterface())
curEmu.initSysCalls(progName)
a9.toolManager.addTool(curEmu)

run_simulation()
