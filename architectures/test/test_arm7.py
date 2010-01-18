a7 = arm7tdmi_funcLT_wrapper.ARM7Processor('a7',scwrapper.sc_time(10.0,scwrapper.SC_NS))
memorySize = 1024*1024*256
latencyMem = scwrapper.sc_time(10.0, scwrapper.SC_NS)
mem = MemoryLT32.MemoryLT32('mem', memorySize, latencyMem)
connectPortsForce(a7, a7.dataMem.initSocket, mem, mem.socket)
connectPortsForce(a7, a7.instrMem.initSocket, mem, mem.socket)

loader = loader_wrapper.Loader('arm.out')
#Initialization of the processors and loading in memory of the application program
for i in range(0, loader.getProgDim()):
    mem.write_byte_dbg(i + loader.getDataStart(), loader.getProgDataValue(i))
a7.ENTRY_POINT = loader.getProgStart()
a7.PROGRAM_LIMIT = loader.getProgDim() + loader.getDataStart()
a7.PROGRAM_START = loader.getDataStart()

curEmu = trapwrapper.OSEmulator32(a7.getInterface())
curEmu.initSysCalls('arm.out')
pro = trapwrapper.Profiler32(a7.getInterface(),'arm.out')
a7.toolManager.addTool(curEmu)
#a7.toolManager.addTool(pro)
run_simulation()
pro.printCsvStats('arm7.csv')

