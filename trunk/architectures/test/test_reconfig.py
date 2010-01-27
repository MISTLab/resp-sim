def statsPrinter():
    print '\x1b[34m\x1b[1mSimulated Elapsed Time (nano-seconds):\x1b[0m'
    print '\x1b[31m' + str(controller.get_simulated_time()) + '\x1b[0m'
#    print '\x1b[34m\x1b[1mNumber of processor instructions executed:\x1b[0m'
#    print '\x1b[31m' + str(proc1.get_instr_counter()) + '\x1b[0m'

a9 = arm9tdmi_funcLT_wrapper.ARM9Processor('a9',scwrapper.sc_time(10.0,scwrapper.SC_NS))
memorySize = 1024*1024*256
latencyMem = scwrapper.sc_time(10.0, scwrapper.SC_NS)
#mem = MemoryLT32.MemoryLT32('mem', memorySize, latencyMem)
mem = MemoryLT32.MemoryLT32('mem', memorySize, scwrapper.SC_ZERO_TIME)

# BUS!!!
#bus = pv_router32.pv_router32('SimpleBus', scwrapper.SC_ZERO_TIME, 2)
#bus = pv_router32.pv_router32('SimpleBus', scwrapper.sc_time(0.001, scwrapper.SC_NS), 2)
#connectPortsForce(proc1, proc1.DATA_MEM_port.memory_port, bus, bus.target_port[0])
#connectPortsForce(proc1, proc1.PROG_MEM_port.memory_port, bus, bus.target_port[0])
#connectPortsForce(bus, bus.initiator_port, mem, mem.memPort)
#bus.addBinding("mem.mem_SimpleMemPort", 0x0, 1024*1024*8)

connectPortsForce(a9, a9.dataMem.initSocket, mem, mem.socket)
connectPortsForce(a9, a9.instrMem.initSocket, mem, mem.socket)

#bS = bitstreamSink.bitstreamSink('bS')
cE = cE_wrapper.configEngine('cE',a9.getInterface(),1024*1024*8+1,cE_wrapper.RANDOM)
eF1 = eFPGA_wrapper.eFPGA('eF1',50,100,scwrapper.sc_time(0.025, scwrapper.SC_NS),2,5)
eF2 = eFPGA_wrapper.eFPGA('eF2',60,200,scwrapper.sc_time(0.005, scwrapper.SC_NS),4,10)
#connectPortsForce(cE, cE.bus_init_port, bus, bus.target_port[0])
#connectPortsForce(bus, bus.initiator_port, bS, bS.configPort)
#bus.addBinding("bS.bS_configPort", 1024*1024*8+1, 1024*1024*8+1)
connectPortsForce(cE, cE.initiatorSocket, eF1, eF1.targetSocket)
connectPortsForce(cE, cE.initiatorSocket, eF2, eF2.targetSocket)
#cE.printSystemStatus()
#cE.setRequestDelay(scwrapper.sc_time(100000, scwrapper.SC_NS))
#cE.setExecDelay(scwrapper.sc_time(100000, scwrapper.SC_NS))
#cE.setConfigDelay(scwrapper.sc_time(100000, scwrapper.SC_NS))
#cE.setRemoveDelay(scwrapper.sc_time(100000, scwrapper.SC_NS))

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

software = 'test_reconfig'
loader = loader_wrapper.Loader(software)
for i in range(0, loader.getProgDim()):
    mem.write_byte_dbg(i + loader.getDataStart(), loader.getProgDataValue(i))
a9.ENTRY_POINT = loader.getProgStart()
a9.PROGRAM_LIMIT = loader.getProgDim() + loader.getDataStart()
a9.PROGRAM_START = loader.getDataStart()

osEmu = trapwrapper.OSEmulator32(a9.getInterface())
osEmu.initSysCalls(software)

cE.recEmu.initExec(software)
printValueInstance = printValueCall(scwrapper.sc_time(0.100, scwrapper.SC_MS),50,100)			# REMEMBER: PYTHON USES ONLY NS!!
cE.registerPythonCall('printValue', printValueInstance);
#cE.registerCppCall('printValue', scwrapper.sc_time(0.050, scwrapper.SC_NS),25,50)
#cE.registerCppCall('printValue')

cE.registerCppCall('generate', scwrapper.sc_time(1, scwrapper.SC_NS),1,1)

cE.registerCppCall('sum');

cE.recEmu.printRegisteredFunctions()
a9.toolManager.addTool(osEmu)
a9.toolManager.addTool(cE.recEmu)

run_simulation()

