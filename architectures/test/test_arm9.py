a9 = arm9tdmi_funcLT_wrapper.ARM9Processor('a9',scwrapper.sc_time(10.0,scwrapper.SC_NS))
memorySize = 1024*1024*256
latencyMem = scwrapper.sc_time(10.0, scwrapper.SC_NS)
mem = MemoryLT32.MemoryLT32('mem', memorySize, latencyMem)
connectPortsForce(a9, a9.dataMem.initSocket, mem, mem.socket)
connectPortsForce(a9, a9.instrMem.initSocket, mem, mem.socket)

loader = loader_wrapper.Loader('arm.out')
#Initialization of the processors and loading in memory of the application program
for i in range(0, loader.getProgDim()):
    mem.write_byte_dbg(i + loader.getDataStart(), loader.getProgDataValue(i))
a9.ENTRY_POINT = loader.getProgStart()
a9.PROGRAM_LIMIT = loader.getProgDim() + loader.getDataStart()
a9.PROGRAM_START = loader.getDataStart()

curEmu = trapwrapper.OSEmulator32(a9.getInterface())
curEmu.initSysCalls('arm.out')
pro = trapwrapper.Profiler32(a9.getInterface(),'arm.out')
a9.toolManager.addTool(curEmu)
#a9.toolManager.addTool(pro)
run_simulation()
pro.printCsvStats('arm9.csv')

