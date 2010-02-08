l3 = leon3_funcLT_wrapper.Processor_leon3_funclt('l3',scwrapper.sc_time(10.0,scwrapper.SC_NS))
memorySize = 1024*1024*256
latencyMem = scwrapper.sc_time(10.0, scwrapper.SC_NS)
mem = MemoryLT32.MemoryLT32('mem', memorySize, latencyMem)
connectPortsForce(l3, l3.dataMem.initSocket, mem, mem.targetSocket)
connectPortsForce(l3, l3.instrMem.initSocket, mem, mem.targetSocket)

loader = loader_wrapper.Loader('sparc.out')
#Initialization of the processors and loading in memory of the application program
for i in range(0, loader.getProgDim()):
    mem.write_byte_dbg(i + loader.getDataStart(), loader.getProgDataValue(i))
l3.ENTRY_POINT = loader.getProgStart()
l3.PROGRAM_LIMIT = loader.getProgDim() + loader.getDataStart()
l3.PROGRAM_START = loader.getDataStart()

curEmu = trapwrapper.OSEmulator32(l3.getInterface())
curEmu.initSysCalls('sparc.out')
pro = trapwrapper.Profiler32(l3.getInterface(),'sparc.out')
l3.toolManager.addTool(curEmu)
l3.toolManager.addTool(pro)
run_simulation()
pro.printCsvStats('leon3.csv')

