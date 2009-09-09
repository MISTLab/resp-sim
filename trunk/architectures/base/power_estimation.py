# -*- coding: iso-8859-1 -*-
##############################################################################
#
#
#         ___           ___           ___           ___
#        /  /\         /  /\         /  /\         /  /\
#       /  /::\       /  /:/_       /  /:/_       /  /::\
#      /  /:/\:\     /  /:/ /\     /  /:/ /\     /  /:/\:\
#     /  /:/~/:/    /  /:/ /:/_   /  /:/ /::\   /  /:/~/:/
#    /__/:/ /:/___ /__/:/ /:/ /\ /__/:/ /:/\:\ /__/:/ /:/
#    \  \:\/:::::/ \  \:\/:/ /:/ \  \:\/:/~/:/ \  \:\/:/
#     \  \::/~~~~   \  \::/ /:/   \  \::/ /:/   \  \::/
#      \  \:\        \  \:\/:/     \__\/ /:/     \  \:\
#       \  \:\        \  \::/        /__/:/       \  \:\
#        \__\/         \__\/         \__\/         \__\/
#
#
#
#
#   This file is part of ReSP.
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#
#
#   (c) Giovanni Beltrame, Luca Fossati
#       Giovanni.Beltrame@esa.int fossati@elet.polimi.it
#
##############################################################################

d_cache_size = i_cache_size = 1024*16
icacheWritePolicy = UnifiedCache32.UnifiedCache32.THROUGH
icacheSubstsPolicy = UnifiedCache32.UnifiedCache32.LRU
dcacheWritePolicy = UnifiedCache32.UnifiedCache32.THROUGH
dcacheSubstsPolicy = UnifiedCache32.UnifiedCache32.LRU

def benchmarking(test,  numProc):
    global proc, caches, procIf, powerModels
    proc = []
    caches = []
    procIf = []

    memsize = 1024*1024*64

    # Create processors
    for i in range(0, numProc):
        proc.append(arm7.arm7('proc_' + str(i)))
        caches.append(UnifiedCache32.UnifiedCache32("cache_"+str(i), 0x09000000, d_cache_size, i_cache_size))
        caches[i].setConfiguration(UnifiedCache32.UnifiedCache32.DATA_CACHE
                       , dcacheSubstsPolicy, dcacheWritePolicy, 2, False, 4, 0, 0)
        caches[i].setConfiguration(UnifiedCache32.UnifiedCache32.INSTR_CACHE
                       , icacheSubstsPolicy, icacheWritePolicy, 2, False, 4, 0, 0)
        caches[i].status[UnifiedCache32.UnifiedCache32.DATA_CACHE] = UnifiedCache32.UnifiedCache32.ENABLED;
        caches[i].status[UnifiedCache32.UnifiedCache32.INSTR_CACHE] = UnifiedCache32.UnifiedCache32.ENABLED;
        connectPortsForce(proc[i], proc[i].DATA_MEM_port.memory_port, caches[i], caches[i].dataInPort)
        connectPortsForce(proc[i], proc[i].PROG_MEM_port.memory_port, caches[i], caches[i].instrInPort)


    # Create memories
    mem = SimpleMemory32.SimpleMemory32( 'mem' , memsize)

    # Create busses
    bus = pv_router32.pv_router32('SimpleBus', scwrapper.SC_ZERO_TIME, numProc)

    # Connect components
    for i in range(0, numProc):
        connectPortsForce(caches[i], caches[i].cacheOutPort, bus, bus.target_port[i])

    manager.connectPortsForce(bus, bus.initiator_port, mem, mem.memPort)

    # Add memory maps
    bus.addBinding("mem.mem_SimpleMemPort", 0x0, memsize)

    # Load application
    loader = ExecLoader.ExecLoader(test)
    for i in range(0, numProc):
        proc[i].init(i, loader.getProgStart())
        procIf.append(ProcIf.arm7If(proc[i]))
        SynchManager.registerProcessorIf(procIf[i])
    mem.loadApplication(loader.getProgData(), loader.getDataStart(), loader.getProgDim())

    # Define additional variables
    if numProc > 1:
        archcwrap.set_environ('OMP_NUM_THREADS', str(numProc))
    else:
        archcwrap.set_environ('OMP_NUM_THREADS', '2')
    archcwrap.set_environ('OMP_NESTED', 'true')
    archcwrap.set_environ('OMP_DYNAMIC', 'false')
    archcwrap.initSysCalls(test)
    archcwrap.add_program_args([os.path.basename(test)])


    # Activate pthread emulation
    try:
        SynchManager.initializePthreadSystem(memsize, True,  1024*64)
        if numProc > 1:
            SynchManager.initiReentrantEmu()
    except Exception, e:
        if numProc == 1:
            pass
        else:
            raise e
