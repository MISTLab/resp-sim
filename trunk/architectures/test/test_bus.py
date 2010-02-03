a9 = arm9tdmi_funcLT_wrapper.ARM9Processor('a9',scwrapper.sc_time(10.0,scwrapper.SC_NS))
#bus  = BusLT32.BusLT32('bus',scwrapper.SC_ZERO_TIME)
bus  = BusLT32.BusLT32('bus',scwrapper.sc_time(100,scwrapper.SC_NS),2)
memorySize = 1024*1024*256
latencyMem = scwrapper.sc_time(10.0, scwrapper.SC_NS)
mem1 = MemoryLT32.MemoryLT32('mem1', memorySize, latencyMem)
mem2 = MemoryLT32.MemoryLT32('mem2', memorySize, latencyMem)

#connectPortsForce(a9, a9.dataMem.initSocket, mem, mem.targetSocket)
#connectPortsForce(a9, a9.instrMem.initSocket, mem, mem.targetSocket)
connectPortsForce(a9, a9.instrMem.initSocket, bus, bus.targetSocket)
connectPortsForce(a9, a9.dataMem.initSocket, bus, bus.targetSocket)
connectPortsForce(bus, bus.initiatorSocket, mem1, mem1.targetSocket)
connectPortsForce(bus, bus.initiatorSocket, mem2, mem2.targetSocket)
bus.addBinding("mem 1",0x0,1024*1024,False)
bus.addBinding("mem 2",1024*1024+1,1024*1024*256)
bus.printBindings()

loader = loader_wrapper.Loader('test_reconfig')
#Initialization of the processors and loading in memory of the application program
for i in range(0, loader.getProgDim()):
    mem1.write_byte_dbg(i + loader.getDataStart(), loader.getProgDataValue(i))
a9.ENTRY_POINT = loader.getProgStart()
a9.PROGRAM_LIMIT = loader.getProgDim() + loader.getDataStart()
a9.PROGRAM_START = loader.getDataStart()

curEmu = trapwrapper.OSEmulator32(a9.getInterface())
curEmu.initSysCalls('test_reconfig')
a9.toolManager.addTool(curEmu)

print bus.initiatorSocket.size()

run_simulation()

