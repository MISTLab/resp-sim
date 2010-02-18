a9 = arm9tdmi_funcLT_wrapper.Processor_arm9tdmi_funclt('a9',scwrapper.sc_time(10.0,scwrapper.SC_NS))
#bus  = BusLT32.BusLT32('bus',scwrapper.sc_time(5,scwrapper.SC_NS),2)
#bus  = BusLT32.BusLT32('bus',scwrapper.SC_ZERO_TIME,2)
memorySize = 1024*1024*256
latencyMem = scwrapper.sc_time(10, scwrapper.SC_NS)
mem = MemoryLT32.MemoryLT32('mem', memorySize, latencyMem)
#mem = MemoryLT32.MemoryLT32('mem', memorySize, scwrapper.SC_ZERO_TIME)
dataCache = CacheLT32.CacheLT32('dataCache', 8192, 1024*1024*256, 32, 2, CacheLT32.LRU, CacheLT32.BACK)
#dataCache.setReadLatency(scwrapper.sc_time(0.001,scwrapper.SC_PS))
#dataCache.setScratchpad(4194304,1048576,scwrapper.sc_time(0.001,scwrapper.SC_PS))
instrCache = CacheLT32.CacheLT32('instrCache', 8192, 1024*1024*256, 32, 2)

connectPortsForce(a9, a9.dataMem.initSocket, dataCache, dataCache.targetSocket)
connectPortsForce(a9, a9.instrMem.initSocket, instrCache, instrCache.targetSocket)
connectPortsForce(dataCache, dataCache.initSocket, mem, mem.targetSocket)
connectPortsForce(instrCache, instrCache.initSocket, mem, mem.targetSocket)
#connectPortsForce(a9, a9.dataMem.initSocket, mem, mem.targetSocket)
#connectPortsForce(a9, a9.instrMem.initSocket, mem, mem.targetSocket)
#connectPortsForce(bus, bus.initiatorSocket, mem, mem.targetSocket)

#connectPortsForce(dataCache, dataCache.initSocket, bus, bus.targetSocket)
#connectPortsForce(instrCache, instrCache.initSocket, bus, bus.targetSocket)
#connectPortsForce(a9, a9.dataMem.initSocket, bus, bus.targetSocket)
#connectPortsForce(a9, a9.instrMem.initSocket, bus, bus.targetSocket)
#bus.addBinding("mem",0x0,1024*1024*256,False)

loader = loader_wrapper.Loader('test_reconfig2')
#Initialization of the processors and loading in memory of the application program
for i in range(0, loader.getProgDim()):
    mem.write_byte_dbg(i + loader.getDataStart(), loader.getProgDataValue(i))
a9.ENTRY_POINT = loader.getProgStart()
a9.PROGRAM_LIMIT = loader.getProgDim() + loader.getDataStart()
a9.PROGRAM_START = loader.getDataStart()

curEmu = trapwrapper.OSEmulator32(a9.getInterface())
curEmu.initSysCalls('test_reconfig2')
a9.toolManager.addTool(curEmu)

run_simulation()
