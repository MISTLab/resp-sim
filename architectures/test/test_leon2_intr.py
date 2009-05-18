leonProc = leon2.leon2('leonProc')

mem = SimpleMemory32.SimpleMemory32( 'mem', 33637892*8)


timerA=leon2timer24.leon2timer24('timerA',10)

bus = pv_router32.pv_router32('SimpleBus')

# Connecting the ports to the main bus
manager.connectPortsForce(leonProc, leonProc.DATA_MEM_port.memory_port, bus, bus.target_port[0])
manager.connectPortsForce(leonProc, leonProc.PROG_MEM_port.memory_port, bus, bus.target_port[0])
manager.connectPortsForce(bus, bus.initiator_port, mem, mem.memPort)

#Connecting the timer to the interrupt port of the LEON2
#manager.connectPortsForce(timer1, timer1.irqPort, leonProc, leonProc.getINTERRUPT_PORT_5_port())
#manager.connectPortsForce(timer10, timer10.irqPort, leonProc, leonProc.getINTERRUPT_PORT_13_port())
#manager.connectPortsForce(timer3, timer3.irqPort, leonProc, leonProc.getINTERRUPT_PORT_7_port())
#manager.connectPortsForce(timer4, timer4.irqPort, leonProc, leonProc.getINTERRUPT_PORT_15_port())
manager.connectPortsForce(timerA, timerA.irqPortTimer1, leonProc, leonProc.getINTERRUPT_PORT_8_port())
manager.connectPortsForce(timerA, timerA.irqPortTimer2, leonProc, leonProc.getINTERRUPT_PORT_9_port())
manager.connectPortsForce(timerA, timerA.WDOG, leonProc, leonProc.getINTERRUPT_PORT_7_port())

bus.addBinding("mem.mem_SimpleMemPort", 0x0, 2048*2048*8)

software = '_build_/sparc/software/application/test_intrs'
loader = ExecLoader.ExecLoader(software)
leonProc.init(0, loader.getProgStart())
mem.loadApplication(loader.getProgData(), loader.getDataStart(), loader.getProgDim())
archcwrap.initSysCalls(software)

leonProc.setTraceFile("trace_a.txt")


