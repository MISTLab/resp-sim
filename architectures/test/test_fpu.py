def printArchCStats():
    print '\x1b[34m\x1b[1mReal Elapsed Time (milli-seconds):\x1b[0m'
    print '\x1b[31m' + controller.print_real_time() + '\x1b[0m'
    print '\x1b[34m\x1b[1mSimulated Elapsed Time (nano-seconds):\x1b[0m'
    print '\x1b[31m' + str(controller.get_simulated_time()) + '\x1b[0m'
    print '\x1b[34m\x1b[1mNumber of processor instructions executed:\x1b[0m'
    print '\x1b[31m' + str(proc1.get_instr_counter()) + '\x1b[0m'
    print '\x1b[34m\x1b[1mInstruction Execution Speed (Kinstr/sec):\x1b[0m'    
    if controller.get_simulated_time() > 0:
        print '\x1b[31m' + str(float(proc1.get_instr_counter())/float(controller.get_simulated_time())*1000000) + '\x1b[0m'
    else:
        print '\x1b[31mSimulation Too Fast to be precise\x1b[0m'
#This line is used to register the just created function, so that it can be called at the end of the simulation
statsPrinter = printArchCStats

proc1 = arm7.arm7('proc1')
mem = SimpleMemory32.SimpleMemory32( 'mem', 1024*1024*8)
bus = pv_router32.pv_router32('SimpleBus',  scwrapper.SC_ZERO_TIME, 1)
connectPortsForce(proc1, proc1.DATA_MEM_port.memory_port, bus, bus.target_port[0])
connectPortsForce(proc1, proc1.PROG_MEM_port.memory_port, bus, bus.target_port[0])
connectPortsForce(bus, bus.initiator_port, mem, mem.memPort)
bus.addBinding("mem.mem_SimpleMemPort", 0x0, 1024*1024*8)

software = '/home/arlix/a.out'
loader = ExecLoader.ExecLoader(software)
proc1.init(0, loader.getProgStart())
mem.loadApplication(loader.getProgData(), loader.getDataStart(), loader.getProgDim())
archcwrap.initSysCalls(software)

proc1.setTraceFile("trace1.txt")

run_simulation()

