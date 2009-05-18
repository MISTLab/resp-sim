#! /usr/bin/env python
# -*- coding: iso-8859-1 -*-

##############################################################################
#      ___           ___           ___           ___
#     /  /\         /  /\         /  /\         /  /\
#    /  /::\       /  /:/_       /  /:/_       /  /::\
#   /  /:/\:\     /  /:/ /\     /  /:/ /\     /  /:/\:\
#  /  /:/~/:/    /  /:/ /:/_   /  /:/ /::\   /  /:/~/:/
# /__/:/ /:/___ /__/:/ /:/ /\ /__/:/ /:/\:\ /__/:/ /:/
# \  \:\/:::::/ \  \:\/:/ /:/ \  \:\/:/~/:/ \  \:\/:/
#  \  \::/~~~~   \  \::/ /:/   \  \::/ /:/   \  \::/
#   \  \:\        \  \:\/:/     \__\/ /:/     \  \:\
#    \  \:\        \  \::/        /__/:/       \  \:\
#     \__\/         \__\/         \__\/         \__\/
#
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the
#   Free Software Foundation, Inc.,
#   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA.
#
###############################################################################


""" Reflective Simulation Platform.

    ReSP is a virtual platform for embedded system development. It provides
    tools to create, connect and simulate some of the typical components of
    a Multi-Processor System-on-Chip. ReSP is based on the Python programming
    language and the SystemC TLM 2.0 library
"""

# Global imports
import os,  sys #readline
from optparse import OptionParser

# Kernel imports
import respkernel
from respkernel import RespKernel

def checkOptions(options, parser):
    """Checks that the options passed to the script are consistent with each other"""
    if options.batch and not options.archFile:
        parser.error("If batch execution is required, the input architectural file must be specified")
    if options.batch and options.command:
        parser.error("If you execute a batch simulation, the command will be automatically computed, so it shouldn't be specified on the command line")
    #TODO: is any option check missing??


if __name__ == '__main__':
    ####################### Command Line Options #######################
    usage = "usage: startSim.sh [options]"
    parser = OptionParser(usage=usage, version = 'Reflective Simulation Platform. Version: ' + respkernel.__version__ + ' Revision ' + respkernel.__revision__[6:-2])
    parser.description = 'Reflective Simulation Platform; version ' + respkernel.__version__ + '. Released under the GPL license'
    parser.add_option("-a", "--arch", type="string", dest="archFile", metavar="FILE",
                  help="file containing the architecture which must be loaded. This file must be a python script")
    parser.add_option("-o", "--output", type="string", dest="outFile",
                  help="file used to save the output of the simulation.", metavar="FILE")
    parser.add_option("-v", "--verbose", dest="verbose", default=False, action="store_true",
                  help="specifies whether verbose information will be printed during non-interactive simulation: [default: %default]")
    parser.add_option("--no-color", dest="color", default=True, action="store_false",
                  help="specifies to disable colored output")
    parser.add_option("--no-banner", dest="banner", default=True, action="store_false",
                  help="specifies to disable the ReSP banner")
    parser.add_option("-g", "--debug", dest="debug", default=False, action="store_true",
                  help="specifies whether a GDB instance should be opened together with the simulator (it may be used to detect unespected segfaults): [default: %default]")
    parser.add_option("-s", "--server", dest="server", type="int", default=0, help="starts a command server on the specified port")
    parser.add_option("-i", "--no-interactive", dest="interactive", default=True, action="store_false", help="disables interactive mode")
    parser.add_option("-b", "--batch", dest="batch", type="string",
                  help="Starts the simulation in batch mode: a python script containing the main loop of the batch simulation has to be passed to this option",  metavar="FILE")
    parser.add_option("--silent", dest="silent", default=False, action="store_true",
                  help="Starts the simulation in silent mode: this means that no interactive console is fired-up; this mode is usually used in combination with the batch option")
    parser.add_option("-d", "--additional-command", type="string", dest="command",
                  help="Additional python code which is executed in the global name space after the loading of the architecture (in case the -a option is used); also a file can be used and in that case all the python commands contained in that file are executed", metavar="COMMAND")

    #parse specified options...
    try:
        (options, args) = parser.parse_args()
        checkOptions(options, parser)
    except BaseException, e:
        #in case of bad parameter specification resp exits.
        #error message is just printed by the parser.
        #TODO: I was not able to catch the specific exception but only the BaseException...
        sys.exit(0)
    ################### End Of Command Line Options #####################

    if options.verbose:
        print 'Command line options --> ' + ' '.join(sys.argv)

    # First of all I check if I have to start a fault injection campaign
    # Now let's check if I have to simply start some batch simulations
    # and the parameters for each simulation
    if options.batch:
        options.silent = True
        try:
            if os.path.exists(options.batch):
                exec open(options.batch) in globals()
            else:
                exec options.batch in globals()
        except Exception, e:
            print 'Error in the execution of the batch script --> ' + str(e)
        sys.exit(0)

    # Create a new HCI
    if (sys.platform=='win32') or ('NOCOLOR' in os.environ) \
        or (os.environ.get('TERM', 'dumb') in ['dumb', 'emacs']) \
        or (not sys.stdout.isatty()):
        options.color = False

    from hci import colors
    colors.set_colors(options.color)

    # Start the simulation kernel
    try:
        resp_kernel = RespKernel(options)
    except  SystemExit:
        sys.exit(0)
    except BaseException, e:
        print 'Error occurred during setup of resp_kernel --> ' + str(e)
        sys.exit(0)

    if not options.batch:
        from hci import console
        if options.banner:
            if options.silent:
                print console.banner
            else:
                print console.banner + console.command_str

    #Check if a predefined architecture have to be loaded
    if options.archFile:
        if not resp_kernel.load_architecture(options.archFile):
            print 'Error during loading of file ' + options.archFile
            sys.exit(0)

    # Now, if necessary, I execute the specified command
    if options.command:
        if options.verbose:
            print 'executing command ' + options.command
        if os.path.exists(options.command):
            exec open(options.command) in resp_kernel.get_namespace()
        else :
            exec options.command in resp_kernel.get_namespace()

    # Start the command server if requested
    try:
        if options.server:
            from hci import server
            srv = server.RespCommandServer(resp_kernel, options.server, not options.silent)
            if options.verbose:
                print 'Server correctly started on port ' + str(options.server)
    except Exception, e:
        print '\nErrror during the start of the ReSP server ' + str(e)
        sys.exit(0)
    except:
        sys.exit(0)
    try:
        if options.server:
            if options.silent:
               srv.tcpserver.serve_forever()
    except Exception, e:
        print '\nErrror during the start of the ReSP server ' + str(e)
        sys.exit(0)
    except:
        sys.exit(0)

    # Start the simulation, determining whether it has to
    # run in automatic or manual mode
    if options.silent:
        resp_kernel.run_silent()
    else:
        # Start HCI interfaces
        try:
            console = console.Console(resp_kernel, options.verbose,  options.debug, locals=resp_kernel.get_namespace())
            console.start_console()
        except SystemExit:
            if options.server:
                srv.stop()
                del srv
            sys.exit(0)
        except BaseException, e:
            print 'Error occurred during the start of the console --> ' + str(e)
            sys.exit(0)
