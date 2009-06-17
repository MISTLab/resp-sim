# -*- coding: iso-8859-1 -*-
###### GENERAL PARAMETERS #####
PROCESSOR_FREQUENCY = 500         # MHz
PROCESSOR_NUMBER  = 1             #
try:
    PROCESSOR_NAMESPACE
except:
    PROCESSOR_NAMESPACE = leon3_funcLT_wrapper

# Memory/bus
MEMORY_SIZE       = 256           # MBytes
MEM_LATENCY       = 10            # ns


# Software
try:
    SOFTWARE
except:
    SOFTWARE = None

if SOFTWARE:
    try:
        ARGS
    except:
        ARGS = None

OS_EMULATION = True     # True or False

################################################
##### AUTO VARIABLE SETUP ######################
################################################

# Find the specified software in the _build_ directory if not an absolute path
if not SOFTWARE or not os.path.isfile(SOFTWARE):
    SOFTWARE = findInFolder(SOFTWARE, '_build_/arm')
    if not SOFTWARE:
        raise Exception('Unable to find program ' + SOFTWARE)

################################################
##### COMPONENT CREATION #######################
################################################

###### PROCESSOR INSTANTIATION #####
#Processor instantiation; decomment the following line to add a second processor and eventually add similar lines
#for the third, fourth .... processor
processors = []
latency = scwrapper.sc_time(float(1000)/float(PROCESSOR_FREQUENCY),  scwrapper.SC_NS)
for i in range(0, PROCESSOR_NUMBER):
    processors.append(PROCESSOR_NAMESPACE.Processor('processor_' + str(i), latency))

##### MEMORY INSTANTIATION #####
memorySize = 1024*1024*MEMORY_SIZE
latencyMem = scwrapper.sc_time(MEM_LATENCY, scwrapper.SC_NS)
mem = MemoryLT32.MemoryLT32( 'mem', memorySize, latencyMem)

################################################
##### INTERCONNECTIONS #########################
################################################

##### BUS CONNECTIONS #####
# Connecting the master components to the bus
# Caches
for i in range(0, PROCESSOR_NUMBER):
    processors[i].dataMem.initSocket.bind(mem.socket)
    processors[i].instrMem.initSocket.bind(mem.socket)

################################################
##### SYSTEM INIT ##############################
################################################

##### LOAD SOFTWARE #####

import os
if not os.path.exists(SOFTWARE):
    raise Exception('Error, ' + str(SOFTWARE) + ' does not exists')

loader = loader_wrapper.Loader(SOFTWARE)
#Initialization of the processors and loading in memory of the application program
for i in range(0, loader.getProgDim()):
    mem.write_byte_dbg(i + loader.getDataStart(), loader.getProgDataValue(i))

for i in range(0, PROCESSOR_NUMBER):
    processors[i].ENTRY_POINT = loader.getProgStart()
    processors[i].PROGRAM_LIMIT = loader.getProgDim() + loader.getDataStart()
    processors[i].PROGRAM_START = loader.getDataStart()
