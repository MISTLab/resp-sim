WRITE_ADDR=0x0F000010
SIZE=32
# Processor
# create an instance of the C++ class which implements the processor. The class is defined in file component/processor/arm7tdmi/functional/arm7.H.
armProc = arm7.arm7('arm')

# Memory
# Instantiation of the memory. Again, this line creates an instance of a C++ class (defined in component/memory/simple/simpleMemory.hpp)
mem = SimpleMemory32.SimpleMemory32( 'mem', 1024*1024*8)
# Bus
# Instantiation of the functional bus
bus = pv_router32.pv_router32('SimpleBus')
# Matlab component
instSlave = MatlabSlave_fft.MatlabSlave_fft('matlab_fft')

# Connection of the components;
# connectPortsForce is a function of ReSP and is used to connect together two TLM ports.
# Connecting the ports to the main bus
connectPortsForce(armProc, armProc.DATA_MEM_port.memory_port, bus, bus.target_port[0])
connectPortsForce(armProc, armProc.PROG_MEM_port.memory_port, bus, bus.target_port[0])
connectPortsForce(bus, bus.initiator_port, mem, mem.memPort)
connectPortsForce(bus, bus.initiator_port, instSlave, instSlave.matlabPort)

# addBinding is, instead, a method of the pv router class and it is used to specify the memory map of the slave components connected to the bus; it takes as parameters the first address mapped to each device and the width of the memory area used by the device
bus.addBinding("mem.mem_SimpleMemPort", 0x0, 1024*1024*8)
bus.addBinding("matlab_fft.matlab_fft_SimpleMatlabPort", WRITE_ADDR, WRITE_ADDR+SIZE)

# Application
software = '_build_/arm/software/application/testmatlab_fft'
loader = ExecLoader.ExecLoader(software)
armProc.init(0, loader.getProgStart())
mem.loadApplication(loader.getProgData(), loader.getDataStart(), loader.getProgDim())
archcwrap.initSysCalls(software)

# Run simulation
run_simulation()







