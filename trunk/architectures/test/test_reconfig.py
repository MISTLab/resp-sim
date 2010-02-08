def statsPrinter():
    print '\x1b[34m\x1b[1mSimulated Elapsed Time (nano-seconds):\x1b[0m'
    print '\x1b[31m' + str(controller.get_simulated_time()) + '\x1b[0m'
#    print '\x1b[34m\x1b[1mNumber of processor instructions executed:\x1b[0m'
#    print '\x1b[31m' + str(proc1.get_instr_counter()) + '\x1b[0m'

a9 = arm9tdmi_funcLT_wrapper.Processor_arm9tdmi_funclt('a9',scwrapper.sc_time(10.0,scwrapper.SC_NS))
#bus  = BusLT32.BusLT32('bus',scwrapper.SC_ZERO_TIME)
bus  = BusLT32.BusLT32('bus',scwrapper.sc_time(0.001, scwrapper.SC_NS),3)
memorySize = 1024*1024*256
latencyMem = scwrapper.sc_time(10.0, scwrapper.SC_NS)
#mem = MemoryLT32.MemoryLT32('mem', memorySize, scwrapper.SC_ZERO_TIME)
mem = MemoryLT32.MemoryLT32('mem', memorySize, latencyMem)

connectPortsForce(a9, a9.dataMem.initSocket, bus, bus.targetSocket)
connectPortsForce(a9, a9.instrMem.initSocket, bus, bus.targetSocket)
connectPortsForce(bus, bus.initiatorSocket, mem, mem.targetSocket)
bus.addBinding("mem",0x0,1024*1024*256)

bS = bS_wrapper.bitstreamSink32('bS')
cE = cE_wrapper.configEngine('cE',a9.getInterface(),0x0,1024*1024*256+1,cE_wrapper.RANDOM)
eF1 = eFPGA_wrapper.eFPGA('eF1',50,100,scwrapper.sc_time(0.025, scwrapper.SC_NS),2,5)
eF2 = eFPGA_wrapper.eFPGA('eF2',60,200,scwrapper.sc_time(0.005, scwrapper.SC_NS),4,10)
connectPortsForce(cE, cE.busSocket, bus, bus.targetSocket)
connectPortsForce(bus, bus.initiatorSocket, bS, bS.targetSocket)
bus.addBinding("bS", 1024*1024*256+1, 1024*1024*256+1)
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

bus.printBindings()
cE.recEmu.printRegisteredFunctions()
a9.toolManager.addTool(osEmu)
a9.toolManager.addTool(cE.recEmu)

run_simulation()

