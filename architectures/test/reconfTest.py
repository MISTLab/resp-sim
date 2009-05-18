#This is a method called at the end of the simulation (after sc_stop has been called); it is usually used to display simulation results. We decided not to
#hardcode it inside the simulation since you own componentsmay have their own way of displaying results. For instance
#this method displays statistic about execution speed of processors generated usign the ArchC language. This is a normal python method,
#you could do anything you want in it, including writing to file .....
def printArchCStats():
    outfile = open('pyBounc2.txt', 'a')
    print ''
    print '\x1b[34m\x1b[1mReal Elapsed Time (seconds):\x1b[0m'
    print '\x1b[31m' + controller.print_real_time() + '\x1b[0m'
    print >> outfile, str(controller.print_real_time())
    print '\x1b[34m\x1b[1mSimulated Elapsed Time (nano-seconds):\x1b[0m'
    print '\x1b[31m' + str(controller.get_simulated_time()/1000) + '\x1b[0m'
    print '\x1b[34m\x1b[1mNumber of processor instructions executed:\x1b[0m'
    print '\x1b[31m' + str(proc1.get_instr_counter()) + '\x1b[0m'
    print '\x1b[34m\x1b[1mNumber of clock cycles elapsed:\x1b[0m'
    print '\x1b[31m' + str(int(get_simulated_time()/latency.to_double())) + '\x1b[0m'
    print '\x1b[34m\x1b[1mInstruction Execution Speed (Kinstr/sec):\x1b[0m'
    if controller.get_real_time_ms() > 0:
        print '\x1b[31m' + str(float(proc1.get_instr_counter())/float(controller.get_real_time_ms())) + '\x1b[0m'
    else:
        print '\x1b[31mSimulation Too Fast to be precise\x1b[0m'
#This line is used to register the just created function, so that it can be called at the end of the simulation
statsPrinter = printArchCStats

# *********************************** PROCESSOR INSTANTIATION************************
#Processor instantiation; decomment the following line to add a second processor and eventually add similar lines
#for the third, fourth .... processor
latency = scwrapper.sc_time(2,  scwrapper.SC_NS)
proc1 = arm7.arm7('proc1', latency)
proc2 = arm7.arm7('proc2', latency)
proc3 = arm7.arm7('proc3', latency)
proc4 = arm7.arm7('proc4', latency)
proc5 = arm7.arm7('proc5', latency)
proc6 = arm7.arm7('proc6', latency)
proc7 = arm7.arm7('proc7', latency)
proc8 = arm7.arm7('proc8', latency)
proc9 = arm7.arm7('proc9', latency)
proc10 = arm7.arm7('proc10', latency)
proc11 = arm7.arm7('proc11', latency)
proc12 = arm7.arm7('proc12', latency)
proc13 = arm7.arm7('proc13', latency)
proc14 = arm7.arm7('proc14', latency)
proc15 = arm7.arm7('proc15', latency)
proc16 = arm7.arm7('proc16', latency)

#********************************MEMORY INSTANTIATION******************************
#So far in ReSP there are two types of memories; one with debugging capabilites (but of course slower) and
#the other without any tracing facilities, but much faster. The second parameter of the DbgMemory32 memory
#constructor is the file on which debug informations are written. In order to analyze this data you can use
#the memAnalyzer program in the bin subfolder
memorySize = 1024*1024*8
mem = SimpleMemory32.SimpleMemory32( 'mem', memorySize)
#mem = DbgMemory32.DbgMemory32('mem', 'memDump1.dmp', memorySize)

#********************************INTERCONNECTION INSTANTIATION******************************
#Creation of the arbitrated bus which will be used to connect the slave (peripherals) and master (processors) components;
#the second argument of the constructor specifies the number of master components which can be attached to the bus
latencyBus = scwrapper.sc_time(4, scwrapper.SC_NS)
bus = pv_router32.pv_router32('SimpleBus', latencyBus, 16)

#*********************************************CACHES*******************************************
cache1 = SimpleCache32.SimpleCache32("cache1", memorySize, 1024*16)
cache2 = SimpleCache32.SimpleCache32("cache2", memorySize, 1024*16)
cache3 = SimpleCache32.SimpleCache32("cache3", memorySize, 1024*16)
cache4 = SimpleCache32.SimpleCache32("cache4", memorySize, 1024*16)
cache5 = SimpleCache32.SimpleCache32("cache5", memorySize, 1024*16)
cache6 = SimpleCache32.SimpleCache32("cache6", memorySize, 1024*16)
cache7 = SimpleCache32.SimpleCache32("cache7", memorySize, 1024*16)
cache8 = SimpleCache32.SimpleCache32("cache8", memorySize, 1024*16)
cache9 = SimpleCache32.SimpleCache32("cache9", memorySize, 1024*16)
cache10 = SimpleCache32.SimpleCache32("cache10", memorySize, 1024*16)
cache11 = SimpleCache32.SimpleCache32("cache11", memorySize, 1024*16)
cache12 = SimpleCache32.SimpleCache32("cache12", memorySize, 1024*16)
cache13 = SimpleCache32.SimpleCache32("cache13", memorySize, 1024*16)
cache14 = SimpleCache32.SimpleCache32("cache14", memorySize, 1024*16)
cache15 = SimpleCache32.SimpleCache32("cache15", memorySize, 1024*16)
cache16 = SimpleCache32.SimpleCache32("cache16", memorySize, 1024*16)

#********************************OTHER COMPONENTS INSTANTIATION******************************
#Two instances of a timer (the PID platform, from which we are taking inspiration for this architecture has two timers)
#timer1 = timer32.timer32('timer1', 1)
#timer2 = timer32.timer32('timer2', 1)
#Interrupt controller
#intContr = IntrContMp32.IntrContMp32('intContr')
#Creation of the UART peripheral; this is used to the IO. In order to communicate with the UART it is
#Creation of the UART peripheral; this is used to the IO. In order to communicate with the UART it is
#both possible to use your normal xterm console or socket connections (telnet should be enough for the
#job). In order to choose among the two methods, associate the appropriate objec to the commIf variable
#In order to use the console, open a new instance of xterm, type tty in it and copy the result of the command
#in the constructor of the term_inout class
#for the socket communication interface, it waits for connections on port 2000; passing another number to
#the constructor of socket_inout class it is possible to change this port
#commIf = pc16x5x32.term_inout("/dev/pts/1")
#commIf = pc16x5x32.socket_inout()
#uart = pc16x5x32.pc16x5x32('uart', commIf)
#led: it simply writes a message to screen when the led goes on. Mainly used for debugging purposes
#led = led32.led32("led", 8)

#********************************BUS CONNECTIONS******************************
# Connecting the master components to the bus
connectPortsForce(proc1, proc1.PROG_MEM_port.memory_port, cache1, cache1.inPort)
connectPortsForce(proc2, proc2.PROG_MEM_port.memory_port, cache2, cache2.inPort)
connectPortsForce(proc1, proc1.DATA_MEM_port.memory_port, bus, bus.target_port[0])
connectPortsForce(proc2, proc2.DATA_MEM_port.memory_port, bus, bus.target_port[1])
connectPortsForce(proc3, proc3.PROG_MEM_port.memory_port, cache3, cache3.inPort)
connectPortsForce(proc4, proc4.PROG_MEM_port.memory_port, cache4, cache4.inPort)
connectPortsForce(proc3, proc3.DATA_MEM_port.memory_port, bus, bus.target_port[2])
connectPortsForce(proc4, proc4.DATA_MEM_port.memory_port, bus, bus.target_port[3])
connectPortsForce(proc5, proc5.PROG_MEM_port.memory_port, cache5, cache5.inPort)
connectPortsForce(proc6, proc6.PROG_MEM_port.memory_port, cache6, cache6.inPort)
connectPortsForce(proc5, proc5.DATA_MEM_port.memory_port, bus, bus.target_port[4])
connectPortsForce(proc6, proc6.DATA_MEM_port.memory_port, bus, bus.target_port[5])
connectPortsForce(proc7, proc7.PROG_MEM_port.memory_port, cache7, cache7.inPort)
connectPortsForce(proc8, proc8.PROG_MEM_port.memory_port, cache8, cache8.inPort)
connectPortsForce(proc7, proc7.DATA_MEM_port.memory_port, bus, bus.target_port[6])
connectPortsForce(proc8, proc8.DATA_MEM_port.memory_port, bus, bus.target_port[7])
connectPortsForce(proc9, proc9.PROG_MEM_port.memory_port, cache9, cache1.inPort)
connectPortsForce(proc10, proc10.PROG_MEM_port.memory_port, cache10, cache2.inPort)
connectPortsForce(proc9, proc9.DATA_MEM_port.memory_port, bus, bus.target_port[8])
connectPortsForce(proc10, proc10.DATA_MEM_port.memory_port, bus, bus.target_port[9])
connectPortsForce(proc11, proc11.PROG_MEM_port.memory_port, cache11, cache11.inPort)
connectPortsForce(proc12, proc12.PROG_MEM_port.memory_port, cache12, cache12.inPort)
connectPortsForce(proc11, proc11.DATA_MEM_port.memory_port, bus, bus.target_port[10])
connectPortsForce(proc12, proc12.DATA_MEM_port.memory_port, bus, bus.target_port[11])
connectPortsForce(proc13, proc13.PROG_MEM_port.memory_port, cache13, cache13.inPort)
connectPortsForce(proc14, proc14.PROG_MEM_port.memory_port, cache14, cache14.inPort)
connectPortsForce(proc13, proc13.DATA_MEM_port.memory_port, bus, bus.target_port[12])
connectPortsForce(proc14, proc14.DATA_MEM_port.memory_port, bus, bus.target_port[13])
connectPortsForce(proc15, proc15.PROG_MEM_port.memory_port, cache15, cache15.inPort)
connectPortsForce(proc16, proc16.PROG_MEM_port.memory_port, cache16, cache16.inPort)
connectPortsForce(proc15, proc15.DATA_MEM_port.memory_port, bus, bus.target_port[14])
connectPortsForce(proc16, proc16.DATA_MEM_port.memory_port, bus, bus.target_port[15])

connectPortsForce(cache1, cache1.cacheOutPort, bus, bus.target_port[0])
connectPortsForce(cache2, cache2.cacheOutPort, bus, bus.target_port[1])
connectPortsForce(cache3, cache3.cacheOutPort, bus, bus.target_port[2])
connectPortsForce(cache4, cache4.cacheOutPort, bus, bus.target_port[3])
connectPortsForce(cache5, cache5.cacheOutPort, bus, bus.target_port[4])
connectPortsForce(cache6, cache6.cacheOutPort, bus, bus.target_port[5])
connectPortsForce(cache7, cache7.cacheOutPort, bus, bus.target_port[6])
connectPortsForce(cache8, cache8.cacheOutPort, bus, bus.target_port[7])
connectPortsForce(cache9, cache9.cacheOutPort, bus, bus.target_port[8])
connectPortsForce(cache10, cache10.cacheOutPort, bus, bus.target_port[9])
connectPortsForce(cache11, cache11.cacheOutPort, bus, bus.target_port[10])
connectPortsForce(cache12, cache12.cacheOutPort, bus, bus.target_port[11])
connectPortsForce(cache13, cache13.cacheOutPort, bus, bus.target_port[12])
connectPortsForce(cache14, cache14.cacheOutPort, bus, bus.target_port[13])
connectPortsForce(cache15, cache15.cacheOutPort, bus, bus.target_port[14])
connectPortsForce(cache16, cache16.cacheOutPort, bus, bus.target_port[15])

#connectPortsForce(proc3, proc3.DATA_MEM_port.memory_port, bus, bus.target_port[2])
#connectPortsForce(proc3, proc3.PROG_MEM_port.memory_port, bus, bus.target_port[2])
#connectPortsForce(proc4, proc4.DATA_MEM_port.memory_port, bus, bus.target_port[3])
#connectPortsForce(proc4, proc4.PROG_MEM_port.memory_port, bus, bus.target_port[3])

#connecting slave components to the bus
connectPortsForce(bus, bus.initiator_port, mem, mem.memPort)

#********************************PERIPHERAL MAPPING******************************
#mapping of the address at which the memory mapped slave peripherals are registered
bus.addBinding("mem.mem_SimpleMemPort", 0x0, memorySize)

#********************************SYSTEM INITIALIZATION******************************
software = '_build_/arm/software/benchmarks/openMP/OmpSCR_v2.0/applications/c_LUreduction/c_lu'
#Initialization of the processors and loading in memory of the application program
loader = ExecLoader.ExecLoader(software)
proc1.init(0, loader.getProgStart())
proc2.init(1, loader.getProgStart())
proc3.init(2, loader.getProgStart())
proc4.init(3, loader.getProgStart())
proc5.init(4, loader.getProgStart())
proc6.init(5, loader.getProgStart())
proc7.init(6, loader.getProgStart())
proc8.init(7, loader.getProgStart())
proc9.init(8, loader.getProgStart())
proc10.init(9, loader.getProgStart())
proc11.init(10, loader.getProgStart())
proc12.init(11, loader.getProgStart())
proc13.init(12, loader.getProgStart())
proc14.init(13, loader.getProgStart())
proc15.init(14, loader.getProgStart())
proc16.init(15, loader.getProgStart())

mem.loadApplication(loader.getProgData(), loader.getDataStart(), loader.getProgDim())

# Set environmental variables; they can be read by the simulated program with the normal getenv system call
archcwrap.set_environ('OMP_NUM_THREADS', '16')
archcwrap.set_environ('OMP_NESTED', 'false')
#archcwrap.set_environ('OMP_DYNAMIC', 'false')
#archcwrap.set_environ('OMP_SCHEDULE', 'guided,1')

# Initializes the emulation of the Operating System. The second parameter specifies the
# latency of each system call. In theory it is possible to specify a different latency
# for each system call, though this features hasn't been made available through python
# yet. In order to specify it, method initSysCalls in ac_syscall.cpp must be modified
latencyMap = {}
latMut = 1376.97*2
latSem = 2745.83*2
latencyMap['^pthread_mutex_(try)?lock$'] = scwrapper.sc_time(latMut,  scwrapper.SC_NS)
latencyMap['^pthread_mutex_unlock$'] =scwrapper.sc_time(latMut,  scwrapper.SC_NS)
latencyMap['^__malloc_(un)?lock$'] = scwrapper.sc_time(latMut,  scwrapper.SC_NS)
latencyMap['^__sfp_lock_'] = scwrapper.sc_time(latMut,  scwrapper.SC_NS)
latencyMap['^__sinit_lock_'] = scwrapper.sc_time(latMut,  scwrapper.SC_NS)
latencyMap['^__fp_(un)?lock$'] = scwrapper.sc_time(latMut,  scwrapper.SC_NS)
latencyMap['^sem_post$'] = scwrapper.sc_time(latSem,  scwrapper.SC_NS)
latencyMap['^sem_wait$'] = scwrapper.sc_time(latSem,  scwrapper.SC_NS)

#latencyMap['^pthread_mutex_(try)?lock$'] = scwrapper.sc_time(8,  scwrapper.SC_NS)
#latencyMap['^pthread_mutex_unlock$'] =scwrapper.sc_time(8,  scwrapper.SC_NS)
#latencyMap['^__malloc_(un)?lock$'] = scwrapper.sc_time(8,  scwrapper.SC_NS)
#latencyMap['^__sfp_lock_'] = scwrapper.sc_time(8,  scwrapper.SC_NS)
#latencyMap['^__sinit_lock_'] = scwrapper.sc_time(8,  scwrapper.SC_NS)
#latencyMap['^__fp_(un)?lock$'] = scwrapper.sc_time(8,  scwrapper.SC_NS)
#latencyMap['^sem_post$'] = scwrapper.sc_time(8,  scwrapper.SC_NS)
#latencyMap['^sem_wait$'] = scwrapper.sc_time(8,  scwrapper.SC_NS)

latencyMap['^_?open$'] = scwrapper.sc_time(4035.61,  scwrapper.SC_NS)
latencyMap['^_?creat$'] = scwrapper.sc_time(4035.61,  scwrapper.SC_NS)
latencyMap['^_?close$'] = scwrapper.sc_time(4035.61,  scwrapper.SC_NS)
latencyMap['^_?read$'] = scwrapper.sc_time(4035.61,  scwrapper.SC_NS)
latencyMap['^_?write$'] = scwrapper.sc_time(4035.61,  scwrapper.SC_NS)
latencyMap['^_?lseek$'] = scwrapper.sc_time(4035.61,  scwrapper.SC_NS)
latencyMap['^_?f?stat$'] = scwrapper.sc_time(4035.61,  scwrapper.SC_NS)
latencyMap['^_?chmod$'] = scwrapper.sc_time(4035.61,  scwrapper.SC_NS)
latencyMap['^_?dup$'] = scwrapper.sc_time(4035.61,  scwrapper.SC_NS)
latencyMap['^_?dup2$'] = scwrapper.sc_time(4035.61,  scwrapper.SC_NS)
latencyMap['^_?chown$'] = scwrapper.sc_time(4035.61,  scwrapper.SC_NS)
latencyMap['^_?unlink$'] = scwrapper.sc_time(4035.61,  scwrapper.SC_NS)

latencyMap['^_?sbrk$'] = scwrapper.SC_ZERO_TIME

latencyMap['^pthread_mutex_destroy$'] = scwrapper.sc_time(899.54,  scwrapper.SC_NS)
latencyMap['^pthread_mutex_init$'] = scwrapper.sc_time(899.54,  scwrapper.SC_NS)
latencyMap['^sem_init$'] = scwrapper.sc_time(899.54,  scwrapper.SC_NS)
latencyMap['^sem_destroy$'] = scwrapper.sc_time(899.54,  scwrapper.SC_NS)
latencyMap['^pthread_cond_init$'] = scwrapper.sc_time(899.54,  scwrapper.SC_NS)
latencyMap['^pthread_cond_destroy$'] = scwrapper.sc_time(899.54,  scwrapper.SC_NS)

latencyMap['^sem_post$'] = scwrapper.sc_time(2745.83,  scwrapper.SC_NS)
latencyMap['^sem_wait$'] = scwrapper.sc_time(2745.83,  scwrapper.SC_NS)

latencyMap['^pthread_create$'] = scwrapper.sc_time(20020,  scwrapper.SC_NS)

archcwrap.initSysCalls(software, latencyMap)

# arguments for pbzip2 "pbzip2", "-p2", "-k", "-f", "-b1", "software/benchmarks/data/short10.inp"
#archcwrap.add_program_args(("pbzip2", "-p2", "-k", "-f", "-b1", "software/benchmarks/data/short10.inp"))

proc1If = ProcIf.arm7If(proc1)
proc2If = ProcIf.arm7If(proc2)
proc3If = ProcIf.arm7If(proc3)
proc4If = ProcIf.arm7If(proc4)
proc5If = ProcIf.arm7If(proc5)
proc6If = ProcIf.arm7If(proc6)
proc7If = ProcIf.arm7If(proc7)
proc8If = ProcIf.arm7If(proc8)
proc9If = ProcIf.arm7If(proc9)
proc10If = ProcIf.arm7If(proc10)
proc11If = ProcIf.arm7If(proc11)
proc12If = ProcIf.arm7If(proc12)
proc13If = ProcIf.arm7If(proc13)
proc14If = ProcIf.arm7If(proc14)
proc15If = ProcIf.arm7If(proc15)
proc16If = ProcIf.arm7If(proc16)

SynchManager.registerProcessorIf(proc1If)
SynchManager.registerProcessorIf(proc2If)
SynchManager.registerProcessorIf(proc3If)
SynchManager.registerProcessorIf(proc4If)
SynchManager.registerProcessorIf(proc5If)
SynchManager.registerProcessorIf(proc6If)
SynchManager.registerProcessorIf(proc7If)
SynchManager.registerProcessorIf(proc8If)
SynchManager.registerProcessorIf(proc9If)
SynchManager.registerProcessorIf(proc10If)
SynchManager.registerProcessorIf(proc11If)
SynchManager.registerProcessorIf(proc12If)
SynchManager.registerProcessorIf(proc13If)
SynchManager.registerProcessorIf(proc14If)
SynchManager.registerProcessorIf(proc15If)
SynchManager.registerProcessorIf(proc16If)

# Parameters of the initializePthreadSystem call:
# I parameter: Memory size
# II parameters: specifies whether idle processor should be paused (true) or if they should wait in a busy loop
# III parameter: stack size for each thread, if not otherwise specified at runtime using the pthread_attr_setstacksize function
# IV parameter: the latency of the pthread emulation routines
SynchManager.initializePthreadSystem(memorySize, True, 1024*20, latencyMap)

# Protects the multi-thread access to the core newlib routines. This is only useful if you are running in a multi-processor environment
# The parameters specifies the latency of the mutex-lock and unlock routines used to protect access to newlib
SynchManager.initiReentrantEmu()

#class mul(archcwrap.SyscallCB):
#    def __init__(self):
#        archcwrap.SyscallCB.__init__(self)
#    def __call__(self, procInstance):
#        param1 = procInstance.get_arg(0)
#        param2 = procInstance.get_arg(1)
#        retVal = param1*param2
#        procInstance.set_retVal(0, retVal)
#        procInstance.return_from_syscall()
#        return False
#
#callbInstance = mul()
#archcwrap.register_syscall('mul', callbInstance, {'' : scwrapper.SC_ZERO_TIME})

#********************************PROFILING******************************
#profiler1 = Profiler.Profiler(software,  True)
#proc1.setProfiler(profiler1)
#profiler2 = Profiler.Profiler(software,  True)
#proc2.setProfiler(profiler2)
