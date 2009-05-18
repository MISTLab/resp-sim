# -*- coding: iso-8859-1 -*-
leonProc = leon2.leon2('leonProc')

mem = SimpleMemory32.SimpleMemory32( 'mem', 33637892*8)
bus = pv_router32.pv_router32('SimpleBus')

# Connecting the ports to the main bus
manager.connectPortsForce(leonProc, leonProc.DATA_MEM_port.memory_port, bus, bus.target_port[0])
manager.connectPortsForce(leonProc, leonProc.PROG_MEM_port.memory_port, bus, bus.target_port[0])
manager.connectPortsForce(bus, bus.initiator_port, mem, mem.memPort)

#Connecting the timer to the interrupt port of the LEON2
bus.addBinding("mem.mem_SimpleMemPort", 0x0, 2048*2048*8)

#software = '_build_/sparc/software/application/helloworld'
software = '/home/luke/SVN/trap/testsuite/testDiv1'
loader = ExecLoader.ExecLoader(software)
leonProc.init(0, loader.getProgStart())
mem.loadApplication(loader.getProgData(), loader.getDataStart(), loader.getProgDim())
archcwrap.initSysCalls(software)
leonProc.setTraceFile('traceLeonOK.txt')
