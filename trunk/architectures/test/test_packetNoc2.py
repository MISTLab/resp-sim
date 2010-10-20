# -*- coding: iso-8859-1 -*-

NUM_OF_SP          = 8
SM_FREQUENCY       = 333
SM_TRANSMISSIONS   = 10000
NOC_FREQUENCY      = 166             # MHz
NOC_DESCRIPTION    = 'architectures/test/test_packetNoc2.xml'
MEM1_SIZE          = 256             # KBytes
MEM1_FREQUENCY     = 166             # MHz
MEM2_SIZE          = 33              # MBytes
MEM2_FREQUENCY     = 166              # MHz


# Modified stats auto-printer
def statsPrinter():
    print '\x1b[34m\x1b[1mReal Elapsed Time (seconds):\x1b[0m'
    print '\x1b[31m' + str(controller.print_real_time()) + '\x1b[0m'
    print '\x1b[34m\x1b[1mSimulated Elapsed Time (nano-seconds):\x1b[0m'
    print '\x1b[31m' + str(controller.get_simulated_time()) + '\x1b[0m'
    print ''
    print 'NOC Accesses:'
    noc.printStats()


################################################
##### COMPONENT CREATION #######################
################################################

#memories instantiation
memorySize1 = 1024*MEM1_SIZE
latencyMem1 = scwrapper.sc_time(float(1000)/float(MEM1_FREQUENCY), scwrapper.SC_NS)
mem1 = MemoryLT32.MemoryLT32('mem1', memorySize1, latencyMem1)
memorySize2 = 1024*1024*MEM2_SIZE/2
latencyMem2 = scwrapper.sc_time(float(1000)/float(MEM2_FREQUENCY), scwrapper.SC_NS)
mem2 = MemoryLT32.MemoryLT32('mem2', memorySize2, latencyMem2)
latencyMem3 = scwrapper.sc_time(float(1000)/float(MEM2_FREQUENCY), scwrapper.SC_NS)
mem3 = MemoryLT32.MemoryLT32('mem3', memorySize2, latencyMem3)

#masters instantiation and configuration
masterLatency1 = scwrapper.sc_time(float(1000)/float(SM_FREQUENCY), scwrapper.SC_NS)
m1 = trafficGenerator.trafficGenerator('master1', masterLatency1)
m2 = trafficGenerator.trafficGenerator('master2', masterLatency1)
m3 = trafficGenerator.trafficGenerator('master3', masterLatency1)
m4 = trafficGenerator.trafficGenerator('master4', masterLatency1)
m5 = trafficGenerator.trafficGenerator('master5', masterLatency1)
m6 = trafficGenerator.trafficGenerator('master6', masterLatency1)
m7 = trafficGenerator.trafficGenerator('master7', masterLatency1)
m8 = trafficGenerator.trafficGenerator('master8', masterLatency1)

endMessageAddress = memorySize1+memorySize2*2
m1.setParameters(SM_TRANSMISSIONS, 0x0, memorySize1+memorySize2*2-1, memorySize1/16, 32, endMessageAddress)
m2.setParameters(SM_TRANSMISSIONS, 0x800, memorySize1+memorySize2*2-1, memorySize1/256, 128, endMessageAddress)
m3.setParameters(SM_TRANSMISSIONS, 0x0, memorySize1+memorySize2*2-1, memorySize1/16, 16, endMessageAddress)
m4.setParameters(SM_TRANSMISSIONS, 0x0, memorySize1+memorySize2*2-1, memorySize1/64, 4096, endMessageAddress)
m5.setParameters(SM_TRANSMISSIONS, 0x400, memorySize1+memorySize2*2-1, memorySize1/16, 64, endMessageAddress)
m6.setParameters(SM_TRANSMISSIONS, 0x0, memorySize1+memorySize2*2-1, memorySize1/8, 8, endMessageAddress)
m7.setParameters(SM_TRANSMISSIONS, 0x0, memorySize1+memorySize2-2, memorySize1/16, 8, endMessageAddress)
m8.setParameters(SM_TRANSMISSIONS, 0x0, memorySize1, memorySize1/16, 16, endMessageAddress)

#dummy end instantiation
de1 = dummyEnd_wrapper.DummyEnd("dummyEnd1",NUM_OF_SP, latencyMem1)

################################################
##### INTERCONNECTIONS #########################
################################################

clockNoc = scwrapper.sc_time(float(1000)/float(NOC_FREQUENCY), scwrapper.SC_NS)
noc = packetNoc32.packetNoc('noc',NOC_DESCRIPTION,clockNoc)

#connecting memories
connectPorts(noc, noc.initiatorSocket, mem1, mem1.targetSocket)
connectPorts(noc, noc.initiatorSocket, mem2, mem2.targetSocket)
connectPorts(noc, noc.initiatorSocket, mem3, mem3.targetSocket)
connectPorts(noc, noc.initiatorSocket, de1, de1.targetSocket)

# Add memory mapping
# three memories and a dummy component receiving end messages and stopping the simulation 
noc.addBinding(8,0x0,memorySize1-1)
noc.addBinding(9,memorySize1,memorySize1+memorySize2-1)
noc.addBinding(10,memorySize1+memorySize2,memorySize1+memorySize2*2-1)
noc.addBinding(11,memorySize1+memorySize2*2,memorySize1+memorySize2*2+4)

#connecting masters
connectPorts(m1, m1.initSocket, noc, noc.targetSocket)
connectPorts(m2, m2.initSocket, noc, noc.targetSocket)
connectPorts(m3, m3.initSocket, noc, noc.targetSocket)
connectPorts(m4, m4.initSocket, noc, noc.targetSocket)
connectPorts(m5, m5.initSocket, noc, noc.targetSocket)
connectPorts(m6, m6.initSocket, noc, noc.targetSocket)
connectPorts(m7, m7.initSocket, noc, noc.targetSocket)
connectPorts(m8, m8.initSocket, noc, noc.targetSocket)

#configure governor
BUFFER_SIZE = 2
INIT_THRESHOLD = 50
MIN_THRESHOLD = 40
MAX_THRESHOLD = 160
ENABLE = True
clockGNoc = scwrapper.sc_time(float(1000)/float(NOC_FREQUENCY), scwrapper.SC_NS)
governor = packetNocGovernor.packetNocGovernor('governor',noc,clockGNoc)
governor.enableGovernor(ENABLE)
governor.initialize(BUFFER_SIZE, INIT_THRESHOLD, MAX_THRESHOLD, MIN_THRESHOLD)

# Printing some useful statistics before the simulation begins
noc.printBindings()
noc.printGRT()
noc.changeAllTimeouts(scwrapper.sc_time(1000, scwrapper.SC_NS))

