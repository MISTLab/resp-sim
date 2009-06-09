# -*- coding: iso-8859-1 -*-
###### GENERAL PARAMETERS #####
PROCESSOR_FREQUENCY = 500         # MHz
PROCESSOR_NUMBER  = 4             #

# Caches
I_CACHE_SIZE      = 32            # KBytes
I_SUBST_POLICY    = "LRU"      # Values are LRU, LRR, RANDOM
I_CACHE_ENABLE    = True          # Cache enable, True or False

D_CACHE_SIZE      = 32            # KBytes
D_WRITE_POLICY    = "THROUGH"     # Values are THROUGH or BACK
D_SUBST_POLICY    = "LRU"         # Values are LRU, LRR, RANDOM
D_CACHE_ENABLE    = False         # Cache enable, True or False

# Memory/bus
MEMORY_SIZE       = 256           # MBytes
BUS_LATENCY       = 10            # ns
MEM_LATENCY       = 10            # ns

# Software
try:
    SOFTWARE           =  benchmark
    ARGS               =  None
except:
    SOFTWARE           = 'pigz' # A full path or a filename to be searched
    ARGS               = ("pigz", "-p" , str(PROCESSOR_NUMBER),"-f" ,"-b", "64",  "-k",  "-v" , "/home/beltrame/Projects/resp/software/benchmarks/data/test.out3")
    #SOFTWARE           = 'pbzip2' # A full path or a filename to be searched
    #ARGS               = ("pbzip2", "-p"+str(PROCESSOR_NUMBER), "-k", "-f", "-b10", "-v" , "/home/beltrame/Projects/resp/software/benchmarks/data/test.out2")

OS_EMULATION       = True     # True or False
THREAD_STACK_SIZE  = 1024*512


################################################
##### AUTO VARIABLE SETUP ######################
################################################

# Find the specified software in the _build_ directory if not an absolute path
software = SOFTWARE
if not os.path.isfile(software):
    software = findInFolder(SOFTWARE, '_build_/arm')
    if not software:
        raise Exception('Unable to find program ' + SOFTWARE)
    print "loading "+software

# Check if the software is multithreaded and expects OS emulation
if OS_EMULATION:
    frontend = bfdwrapper.BFDFrontend(software)
    if frontend.exists('pthread_exit'):
        MULTI_THREADING = True
    else:
        MULTI_THREADING = False
        if OS_EMULATION:
            PROCESSOR_NUMBER = 1

################################################
##### COMPONENT CREATION #######################
################################################

import power

###### PROCESSOR INSTANTIATION #####
#Processor instantiation; decomment the following line to add a second processor and eventually add similar lines
#for the third, fourth .... processor
processors = []
latency = scwrapper.sc_time(float(1000)/float(PROCESSOR_FREQUENCY),  scwrapper.SC_NS)
for i in range(0, PROCESSOR_NUMBER):
    processors.append(arm7.arm7('processor_'+str(i), latency))
    pm = processor.simple_processor()
    pm.frequency = PROCESSOR_FREQUENCY
    connect_power_model( processors[i] , pm )

##### MEMORY INSTANTIATION #####
#So far in ReSP there are two types of memories; one with debugging capabilites (but of course slower) and
#the other without any tracing facilities, but much faster. The second parameter of the DbgMemory32 memory
#constructor is the file on which debug informations are written. In order to analyze this data you can use
#the memAnalyzer program in the bin subfolder
memorySize = 1024*1024*MEMORY_SIZE
latencyMem = scwrapper.sc_time(MEM_LATENCY, scwrapper.SC_NS)
mem = SimpleMemory32.SimpleMemory32( 'mem', memorySize, latencyMem)
#mem = DbgMemory32.DbgMemory32('mem', 'memDump1.dmp', memorySize)
from math import sqrt
mem_params = {
               'size'        : memorySize
             , 'line_width'  : sqrt(memorySize)   # Memory aspect ratio: make it square
             , 'word_size'   : 32                 # Generally we use 32 bit words
             }
connect_power_model(mem, power.memory.interpolated_memory(mem_params))

##### CACHES INSTANTIATION #####
# Caches are directly connected to processors' memory ports
d_cache_size = D_CACHE_SIZE*1024
i_cache_size = I_CACHE_SIZE*1024

# I_CACHE write policy doesn't matter, as it is never written
dcacheSubstsPolicy = getattr( UniversalCache32.UniversalCache32 , D_SUBST_POLICY )
icacheWritePolicy = dcacheWritePolicy = getattr( UniversalCache32.UniversalCache32 , D_WRITE_POLICY )
icacheSubstsPolicy = getattr( UniversalCache32.UniversalCache32 , I_SUBST_POLICY )

# Directory for coherent cache
#directory = Directory32.Directory32('mydirectory', PROCESSOR_NUMBER, scwrapper.SC_ZERO_TIME)

caches = []
for i in range(0, PROCESSOR_NUMBER):
    caches.append( UniversalCache32.UniversalCache32("cache_"+str(i), memorySize, d_cache_size, i_cache_size))

    # Cache parameters
    params = { "size":  i_cache_size, "block_size": 32, "associativity": 1, "subbanks": 2.0, "technology": 0.07,
               "read_ports": 0, "write_ports": 0, "rw_ports": 1 }

    # Set power models
    ic_model = power.cache.ecacti(params)
    ic_model.connect_probe( 'writeMissNum' , generic.probe( caches[i], 'writeMissNum' , 1 ) )
    ic_model.connect_probe( 'readMissNum'  , generic.probe( caches[i], 'readMissNum'  , 1 ) )
    ic_model.connect_probe( 'writeHitNum'  , generic.probe( caches[i], 'writeHitNum'  , 1 ) )
    ic_model.connect_probe( 'readHitNum'   , generic.probe( caches[i], 'readHitNum'   , 1 ) )
    ic_model.set_name('icache'+str(i))
    power.stats.connected_models[ic_model.name]= ic_model

    params['size'] = d_cache_size;
    dc_model = power.cache.ecacti(params)
    dc_model.connect_probe( 'writeMissNum' , generic.probe( caches[i], 'writeMissNum' , 0 ) )
    dc_model.connect_probe( 'readMissNum'  , generic.probe( caches[i], 'readMissNum'  , 0 ) )
    dc_model.connect_probe( 'writeHitNum'  , generic.probe( caches[i], 'writeHitNum'  , 0 ) )
    dc_model.connect_probe( 'readHitNum'   , generic.probe( caches[i], 'readHitNum'   , 0 ) )
    dc_model.set_name('dcache'+str(i))
    power.stats.connected_models[dc_model.name]= dc_model

    caches[i].setConfiguration(UniversalCache32.UniversalCache32.DATA_CACHE
                             , dcacheSubstsPolicy, dcacheWritePolicy, 2, False, 4, 0, 0)
    caches[i].setConfiguration(UniversalCache32.UniversalCache32.INSTR_CACHE
                             , icacheSubstsPolicy, icacheWritePolicy, 1, False, 4, 0, 0)

    # Disable 0-size caches
    if D_CACHE_ENABLE:
        caches[i].status[UniversalCache32.UniversalCache32.DATA_CACHE] = UniversalCache32.UniversalCache32.ENABLED;
    if I_CACHE_ENABLE:
        caches[i].status[UniversalCache32.UniversalCache32.INSTR_CACHE] = UniversalCache32.UniversalCache32.ENABLED;

##### INTERCONNECTION INSTANTIATION #####
#Creation of the arbitrated bus which will be used to connect the slave (peripherals) and master (processors) components;
#the second argument of the constructor specifies the number of master components which can be attached to the bus
latencyBus = scwrapper.sc_time(BUS_LATENCY, scwrapper.SC_NS)
bus = pv_router32.pv_router32('SimpleBus', latencyBus, PROCESSOR_NUMBER)

##### OTHER COMPONENTS INSTANTIATION #####
# Timers (optional)
#timer1 = timer32.timer32('timer1', 1)
#timer2 = timer32.timer32('timer2', 1)

# Interrupt controller (optional)
#intContr = IntrContMp32.IntrContMp32('intContr')

# UART (optional)
# Creation of the UART peripheral; this is used to the IO. In order to communicate with the UART it is
# both possible to use your normal xterm console or socket connections (telnet should be enough for the
# job). In order to choose among the two methods, associate the appropriate objec to the commIf variable
# for the socket communication interface, it waits for connections on port 2000; passing another number to
# the constructor of socket_inout class it is possible to change this port
if not OS_EMULATION:
    commIf = pc16x5x32.term_inout()
    # commIf = pc16x5x32.socket_inout()
    uart = pc16x5x32.pc16x5x32('uart', commIf)

# LEDs (optional)
#led: it simply writes a message to screen when the led goes on. Mainly used for debugging purposes
#led = led32.led32("led", 8)

################################################
##### INTERCONNECTIONS #########################
################################################

##### BUS CONNECTIONS #####
# Connecting the master components to the bus
# Caches
for i in range(0, PROCESSOR_NUMBER):
    connectPortsForce(processors[i], processors[i].PROG_MEM_port.memory_port, caches[i], caches[i].instrInPort)
    connectPortsForce(processors[i], processors[i].DATA_MEM_port.memory_port, caches[i], caches[i].dataInPort)
    connectPortsForce(caches[i], caches[i].cacheOutPort, bus, bus.target_port[i])
    #connectPortsForce(directory, directory.toCachePort, caches[i], caches[i].fromDirectoryPort)
    #connectPortsForce(caches[i], caches[i].toDirectoryPort, directory, directory.target_port[i])

# Memory
connectPortsForce(bus, bus.initiator_port, mem, mem.memPort)

# Other components
#connectPortsForce(bus, bus.initiator_port, timer1, timer1.timerPort)
#connectPortsForce(bus, bus.initiator_port, timer2, timer2.timerPort)
#connectPortsForce(bus, bus.initiator_port, intContr, intContr.intrPort)
#connectPortsForce(bus, bus.initiator_port, led, led.ledPort)

if not OS_EMULATION:
    connectPortsForce(bus, bus.initiator_port, uart, uart.uartPort)


##### MEMORY MAPPING #####
#mapping of the address at which the memory mapped slave peripherals are registered
bus.addBinding("mem.mem_SimpleMemPort", 0x0, memorySize)
#bus.addBinding("intContr.intContr_SimpleControllerPort", 0x0A000000, 0x0A00010C)
#bus.addBinding("timer1.timer1_SimpleTimerPort", 0x0A800000, 0x0A800010)
#bus.addBinding("timer2.timer2_SimpleTimerPort", 0x0A800020, 0x0A800030)
#bus.addBinding("led.led_SimpleLedPort", 0x0D800040, 0x0D800040)

if not OS_EMULATION:
    bus.addBinding("uart.uart_SimpleUartPort", 0x0D800000, 0x0D800020)

################################################
##### SYSTEM INIT ##############################
################################################

##### LOAD SOFTWARE #####

import os
#Initialization of the processors and loading in memory of the application program
loader = ExecLoader.ExecLoader(software)
for i in range(0, PROCESSOR_NUMBER):
    processors[i].init(i, loader.getProgStart())

mem.loadApplication(loader.getProgData(), loader.getDataStart(), loader.getProgDim())


##### OS EMULATION #####

processor_interfaces = []
if OS_EMULATION:
    # Initialization of environmental variables; they can be read by the simulated program with the normal getenv system call
    archcwrap.set_environ('OMP_NUM_THREADS', str(PROCESSOR_NUMBER))

    # Initializes the emulation of the Operating System. The second parameter specifies the
    # latency of each system call. In theory it is possible to specify a different latency
    # for each system call, though this features hasn't been made available through python
    # yet. In order to specify it, method initSysCalls in ac_syscall.cpp must be modified
    archcwrap.initSysCalls(software)

    # Initialization of the program command line arguments
    if ARGS:
        archcwrap.add_program_args(ARGS)

    for i in range(0,PROCESSOR_NUMBER):
        processor_interfaces.append(ProcIf.arm7If(processors[i]))
        SynchManager.registerProcessorIf(processor_interfaces[i])

    # Parameters of the initializePthreadSystem call:
    # I parameter: Memory size
    # II parameters: specifies whether idle processor should be paused (true) or if they should wait in a busy loop
    # III parameter: stack size for each thread, if not otherwise specified at runtime using the pthread_attr_setstacksize function
    # IV parameter: the latency of the pthread emulation routines
    if MULTI_THREADING:
        SynchManager.initializePthreadSystem(memorySize, True, THREAD_STACK_SIZE)
        # Protects the multi-thread access to the core newlib routines. This is only useful if you are running in a multi-processor environment
        # The parameters specifies the latency of the mutex-lock and unlock routines used to protect access to newlib
        if PROCESSOR_NUMBER > 1:
            SynchManager.initiReentrantEmu()

################################################
##### DEBUGGING  ###############################
################################################

##### GDB #####
#The following lines are used to instantiate the GDB debugger stub; note how there is a different
#debugger for each processor in the sytem. Note that in order to be able to used GDB, ReSP must
#have been configured with the --enable-debug flag; this is disabled by default because it slows down
#simulation. In the constructor of GDBStub32 the last parameters is the verbosity level (deafault at 1)
#and the one before is the port used for socket communication with the debugger
#inter1 = GDBProcStub32.leon2Stub(proc1, 1) #stub used by GDB to communicate with the processor
#stub1 = GDBStub32.GDBStub32(inter1, 1500, 1) #GDB server, listens for connections from GDB
#proc1.setGDBStub(stub1) #coupling among GDB and the processor
#inter2 = GDBProcStub32.arm7Stub(proc2)
#stub2 = GDBStub32.GDBStub32(inter2, 1501, 2)
#proc2.setGDBStub(stub2)
#Registration of the callback, used to couple SystemC and GDB
#callBSysC = GDBStub32.GDBSimEndCallback()
#sc_controller.register_EOS_callback(callBSysC)
#callBPause = GDBStub32.GDBPauseCallback()
#sc_controller.register_PAUSE_callback(callBPause)
#callBTime = GDBStub32.GDBTimeoutCallback()
#sc_controller.register_TIMEOUT_callback(callBTime)
#callBError = GDBStub32.GDBErrorCallback()
#sc_controller.register_ERROR_callback(callBError)

##### TRACING #####
#It is possible to configure the processors so that, at each clock cycle, they print their status
#(the value of each register) on a file. Note that in order to be able to use this feature, ReSP must
#have been configured with the --enable-tracing flag; this is disabled by default since it slows
#down simulation. When more processors are used, the ReSPTraceViewer.py utility contained
#inside the bin folder may be useful to compare the traces
#for i in range(0, PROCESSOR_NUMBER):
    #processors[i].setTraceFile('trace' + str(i) + '.txt')

##### PROFILING #####
# The profiler needs rewriting

#This is a method called at the end of the simulation (after sc_stop has been called); it is usually used to display simulation results. We decided not to
#hardcode it inside the simulation since you own componentsmay have their own way of displaying results. For instance
#this method displays statistic about execution speed of processors generated usign the ArchC language. This is a normal python method,
#you could do anything you want in it, including writing to file .....
def print_stats():
    print ''
    print '\x1b[34m\x1b[1mReal Elapsed Time (seconds):\x1b[0m'
    print '\x1b[31m' + controller.print_real_time() + '\x1b[0m'
    print '\x1b[34m\x1b[1mSimulated Elapsed Time (nano-seconds):\x1b[0m'
    print '\x1b[31m' + str(controller.get_simulated_time()/1000) + '\x1b[0m'
#This line is used to register the just created function, so that it can be called at the end of the simulation
statsPrinter = print_stats

##### SOME GENERAL CONSIDERATIONS #####
#Note how this file is a pure python file and all the commands contained in it are pure python
#commands; when you issue the "load_architecture(<filename>)" command,
#you are actually executing these python instructions in the ReSP shell. You could have issued
#the commands one by one in the shell (and sometimes you do that for debugging purposes)
#but it is easier to just write them in a separate file.
