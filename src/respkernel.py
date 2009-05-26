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

"""ReSP core code and simulation namespace"""

# General Init code ############################################################

__version__ = '0.5'
__revision__ = '$Rev: 1881 $'

import sys, os, atexit

# Import DL and set correct link flags for RTTI use
# Setting the dynamic loading flags so that all symbols in the modules are loaded, even if they are not used
# at the moment: this way we avoid circular dependeces and it is not necessary to create other ad-hoc
# dynamic libraries for each module
try:
    try:
        import dl
    except:
        try:
            import DLFCN as dl
        except:
            pass
    sys.setdlopenflags(dl.RTLD_NOW | dl.RTLD_GLOBAL)
except:
    print 'Unable to import dl or DLFCN; ReSP cannot be executed because it needs to set RTDL_NOW and RTDL_GLOBAL flags'
    sys.exit(0)

# set path in order to import the rest of the modules
# Is there any better way to do this?
# TODO: Use waf and the configuration generation!
curPath = os.path.abspath(os.path.join(os.path.dirname(sys.modules['respkernel'].__file__),'..'))
sys.path.append(os.path.abspath(os.path.join(curPath, 'src', 'manager')))
sys.path.append(os.path.abspath(os.path.join(curPath, 'src', 'hci')))
sys.path.append(os.path.abspath(os.path.join(curPath, 'src', 'injection')))
sys.path.append(os.path.abspath(os.path.join(curPath, '_build_', 'default', 'src', 'systempy')))
sys.path.append(os.path.abspath(os.path.join(curPath, '_build_', 'default', 'src', 'mysql')))
sys.path.append(os.path.abspath(os.path.join(curPath, '_build_', 'default', 'src', 'power', 'ecacti')))
sys.path.append(os.path.abspath(os.path.join(curPath, '_build_', 'default', 'src', 'systempy', 'sc_wrapper')))
sys.path.append(os.path.abspath(os.path.join(curPath, '_build_', 'default', 'src', 'softwareDebug')))
sys.path.append(os.path.abspath(os.path.join(curPath, '_build_', 'default', 'src', 'execLoader')))
sys.path.append(os.path.abspath(os.path.join(curPath, '_build_', 'default', 'src', 'profiler')))
sys.path.append(os.path.abspath(os.path.join(curPath, '_build_', 'default', 'src', 'bfdFrontend')))
sys.path.append(os.path.abspath(os.path.join(curPath, '_build_', 'default', 'src', 'SynchManager')))
sys.path.append(os.path.abspath(os.path.join(curPath, '_build_', 'default', 'src', 'SynchManager', 'ProcStubs')))
sys.path.append(os.path.abspath(os.path.join(curPath, '_build_', 'default', 'src')))
sys.path.append(os.path.abspath(os.path.join(curPath, '_build_', 'default', 'src', 'utils')))
sys.path.append(os.path.abspath(os.path.join(curPath, '_build_', 'default', 'src', 'softwareDebug', 'osStubs')))
sys.path.append(os.path.abspath(os.path.join(curPath, '_build_', 'default', 'src', 'softwareDebug', 'procStubs')))
sys.path.append(os.path.abspath(os.path.join(curPath, '_build_', 'default', 'lib', 'archc')))

from power import *

# END General Init code ########################################################

def print_stats():
    """Prints statistics about the simulation; in case the variable statsPrinter is defined
    in the global namespace (a function must be assigned to it) then that function
    is called passing the globals namespace to it. Otherwise simply the real and simulated
    times are printed; note that before printing statistics, I wait for simulation termination
    (if not otherwise specified)"""
    import time
    while not controller.isEnded():
        time.sleep(0.1)
    try:
        # Call a custom statsprinter if registered
        statsPrinter()
    except NameError:
        # Print
        print 'Real Elapsed Time (seconds):'
        print controller.print_real_time()
        print 'Simulated Elapsed Time (nano-seconds):'
        print str(controller.get_simulated_time()/1000) + '\n'
    except Exception,  e:
        print 'Error in the print of the statistics --> ' + str(e)


class RespKernel:
    """This class represents the core simulator class. This class provides a set of objects and functions to
    manipulate simulation objects. RespKernel initializes the code and sets up variables in the global dictionary,
    that can be used by InteractiveInterpreters to provide human-computer interfaces"""

    def __init__(self,  options=None):
        """Initialize the simulation and component managing kernel"""
        # Options...

        # Initialization of the main kernel interface
        # The "Manager" deals with component instantiation and life cycle
        self.manager = None

        # The "Controller" deals with simulation control (run, pause, stop, etc.)
        self.controller = None

        # Anything else?
        self.scripting_commands = []
        self.components = []
        self.fileName = None

        # Other options
        self.verbose = False

        # End handlers
        self.methods = [ print_stats ]

        try:
            import configuration
            if configuration.STATIC_PLATFORM:
                print '\nPlease compile ReSP without static platform support in order to be able to run interactive simulation through the python console\n'
                sys.exit(0)
        except  SystemExit:
            sys.exit(0)
        except:
            pass

        try:
            self.setup(options)
            self.setup_scripting_commands()
        except Exception,  e:
            print '\nError in the setup of the ReSP Kernel  --> ' + str(e) + '\n'
            sys.exit(0)

    def setup(self, options=None):
        """Sets up the ReSP interpreter: component loading, path adjustments, etc."""

        # Now I start performing the parsing of the command line options
        interaction = True
        if options:
            interaction = not (options.silent or not options.interactive)
            self.verbose = options.verbose

        # import the basic modules and turn off the warningns
        import signal #readline

        # Import modules for the respkernel namespace
        # import systemc, archc and tlm wrapper
        if not self.verbose:
            import warnings
            warnings.simplefilter('ignore', RuntimeWarning)

        global scwrapper, sc_controller, GDBStub32, ExecLoader, archcwrap, Profiler, compManager, breakpoints, injection, SynchManager, ProcIf, bfdwrapper, converters

        if self.verbose:
            print "Loading converters"
        import converters
        if self.verbose:
            print "Loading scwrapper"
        import scwrapper
        if self.verbose:
            print "Loading sc_controller"
        import sc_controller

        # Import Optional components
        if self.verbose:
            print "Loading ExecLoader"
        import ExecLoader
        if self.verbose:
            print "Loading compManager"
        import compManager

        # import components storing them into a component list
        self.components = []
        try:
            self.load_components(os.path.abspath(curPath + os.sep + '_build_' + os.sep + 'default' + os.sep + 'component'),  self.components)
        except Exception,  e:
            print >> sys.stderr, 'Error in loading the components in the simulator --> ',  e
            sys.exit(0)

        # importing of the processor stubs, they must be imported after the processors themselves
        if self.verbose:
            print "Loading bfdwrapper"
        import bfdwrapper

        if not self.verbose:
            warnings.resetwarnings()

        # create the component manager
        from compManager import ComponentManager
        global manager
        self.manager = manager = ComponentManager(self.components, scwrapper, archcwrap)
        global loadedFileName
        self.fileName = loadedFileName = ''
        # end, at then end I can instantiate the signal handler and the controller
        global controller

        self.controller = sc_controller.sc_controller.createController(os.getpid(), interaction)
        controller = self.controller
        signal.signal(10, self.end_signal_handler)

        if globals().has_key('__warningregistry__'):
            globals().pop('__warningregistry__')
        if globals().has_key('dl'):
            globals().pop('dl')

        atexit.register(self.delete_all)

    def setup_scripting_commands(self):
        """Create some convenience functions, used to run scripts without the need to refer to RespKernel, controller or manager objects"""

        global connectPortsForce, connectPorts, connectSyscPorts, connectSyscSignal, listComponents,  printComponents, getCompInstance, areConnected, \
                getSources,  getTargets, getConnected, getComponents, getAttrInstance,  getInstance, getBase, run_simulation, \
                pause_simulation, stop_simulation,  get_simulated_time, get_real_time_ms, run_up_to, reload_architecture, \
                enable_fault_injection, showArchitecture

        # Assign scripting commands to manager, helper and controller methods
        reload_architecture = self.reload_architecture
        connectPortsForce = self.manager.connectPortsForce
        connectPorts = self.manager.connectPorts
        connectSyscPorts = self.manager.connectSyscPorts
        connectSyscSignal = self.manager.connectSyscSignal
        listComponents = self.manager.listComponents
        printComponents = self.manager.printComponents
        getCompInstance = self.manager.getCompInstance
        areConnected = self.manager.areConnected
        getSources = self.manager.getSources
        getTargets = self.manager.getTargets
        getConnected = self.manager.getConnected
        getComponents = self.manager.getComponents
        showArchitecture = self.manager.showArchitecture

        import helper
        getAttrInstance =  helper.getAttrInstance
        getInstance = helper.getInstance
        getBase = helper.getBase

        run_simulation = self.controller.run_simulation
        run_up_to = self.controller.run_up_to
        pause_simulation = self.controller.pause_simulation
        stop_simulation = self.controller.stop_simulation
        get_simulated_time = self.controller.get_simulated_time
        get_real_time_ms = self.controller.get_real_time_ms

        enable_fault_injection  = self.enable_fault_injection

        # List commands in an internal array for reference
        self.scripting_commands = [connectPortsForce, connectPorts, connectSyscPorts, connectSyscSignal, listComponents,  printComponents, \
                                   getCompInstance, areConnected, getSources,  getTargets, getConnected, getComponents, \
                                   getAttrInstance,  getInstance, getBase, run_simulation, pause_simulation, \
                                   stop_simulation, get_simulated_time, get_real_time_ms, run_up_to, enable_fault_injection, showArchitecture]


    def end_signal_handler(self, signum, frame):
        """If the kernel is terminated, it will print some general usage statistics"""
        for m in self.methods:
            m()

    def addSimulationVar(self, name, value):
        globals()[name] = value

    def load_components(self, folder,  componentList):
        """Loads the wrappers of the components present in curFolder"""
        curPathLoaded = False
        dirContent = os.listdir(folder)
        for element in dirContent:
            if os.path.isfile(os.path.join(folder, element)):
                if element.endswith('.so'):
                    if not curPathLoaded:
                        curPathLoaded = True
                        sys.path.append(os.path.abspath(folder))
                    toLoadName = os.path.splitext(element)[0]
                    if self.verbose:
                        print "Loading " + toLoadName
                    temp = __import__(toLoadName)
                    componentList.append(temp)
                    globals()[toLoadName] = temp
        [self.load_components(os.path.join(folder, element),  componentList) for element in dirContent if os.path.isdir(os.path.join(folder, element))]

    def load_architecture(self, fileName):
        """Loads the architecture specified in fileName; it can either be an XML file
        or a python script; namespace represents the namespace containing the classes of components
        referenced in the architecture"""
        if fileName.endswith('.xml'):
            print 'Loading of XML files not yet supported'
            return False
        elif fileName.endswith('.py'):
            # I have to find the path for the module, then add it to the
            # system path and finally import the module: all the instructions
            # contained in the module (which are the instructions specifying how
            # to connect the architeture) will be executed
            try:
                exec open(fileName) in globals()
            except Exception, e:
                print 'Error in opening architecture file ' + fileName + ' --> ' + str(e) + '\n'
                if self.verbose:
                    import traceback
                    traceback.print_exc()
                    print ''
                print 'In order to be able to load a new architecture, the simulator must be restarted'
                return False
        else:
            #refs 114
            exec("__ARCHPATH__ = '%s'" %fileName) in globals()
            curPath = os.path.abspath(os.path.join(os.path.dirname(sys.modules['respkernel'].__file__),'..'))
            exec open(curPath + '/tools/importexport/import.py') in globals()

        global loadedFileName
        self.fileName = loadedFileName = fileName
        return True

    def reload_architecture(self):
        if self.fileName == None:
            print 'No architecture to reload'
            return

        self.reset()
        self.load_architecture(self.fileName)

    def run_silent(self, duration=-1):
        # Runs the simulation in batch mode: this means that it executes a simulation
        # in non-interactive mode and executing the specified python actions at
        # different times. In addition to the actions it is possible to change the
        # parameters of the loaded architecture
        run_simulation(duration)

    def delete_all(self):
        """Delete all the object instances in namespaces whose base type is
        Boost.Python.instance; note that in case the simulation is still
        running, the first thing I do is stopping it"""
        if not self.controller.is_finished() and self.controller.is_running() and not self.controller.error:
            if self.verbose:
                print 'killing the simulation'
            self.controller.kill()
        for name in globals().keys():
            if name == 'controller':
                continue
            comp = globals()[name]
            try:
                base = comp.__class__.__bases__[0]
                while base.__name__ != "instance" and base.__name__ != "object":
                    base = base.__bases__[0]
                if base.__name__ == "instance":
                    if self.verbose:
                        print 'deleting ' + str(name)
                    del globals()[name]
                    if self.verbose:
                        print 'deleted ' + str(name)
            except:
                pass

    def get_namespace(self):
        """ Returns the respkernel dictionary (i.e. namespace) """
        return globals()

    def reset(self):
        """ Resets the simulator to the initial state """
        reset()

    def enable_fault_injection(self, value = True):
        """ Enables oe disables the fault injection environment"""
        global manager
        if len(self.manager.getComponents()) > 0:
            print 'An architecture is instantiated. A reset is required for enabling the fault injiection'

        from injectionCompManager import InjectionComponentManager
        from compManager import ComponentManager
        if value:
            if not isinstance(self.manager,InjectionComponentManager):
                #enable fault injection environment
                self.manager = manager = InjectionComponentManager(self.components, scwrapper, archcwrap)

                global createComponent, addMemoryMapping, initProc, callComponentFunction, getSaboteurs, \
                    getNominalComponents, getGoldenComponents, getGoldenCompInstance, enableGoldenModel, injector, getProcFromId

                createComponent = self.manager.createComponent
                addMemoryMapping = self.manager.addMemoryMapping
                initProc = self.manager.initProc
                callComponentFunction = self.manager.callComponentFunction
                getSaboteurs = self.manager.getSaboteurs
                getNominalComponents = self.manager.getNominalComponents
                getGoldenComponents = self.manager.getGoldenComponents
                getGoldenCompInstance = self.manager.getGoldenCompInstance
                enableGoldenModel = self.manager.enableGoldenModel
                getProcFromId = self.manager.getProcFromId

                import faultInjector
                injector = faultInjector.faultInjector()

                self.setup_scripting_commands()
                self.scripting_commands.append(createComponent)
                self.scripting_commands.append(addMemoryMapping)
                self.scripting_commands.append(initProc)
                self.scripting_commands.append(callComponentFunction)
                self.scripting_commands.append(getSaboteurs)
                self.scripting_commands.append(getNominalComponents)
                self.scripting_commands.append(getGoldenComponents)
                self.scripting_commands.append(getGoldenCompInstance)
                self.scripting_commands.append(enableGoldenModel)
                self.scripting_commands.append(getProcFromId)
        else:
            if isinstance(self.manager,InjectionComponentManager):
                #disable fault injection environment
                self.manager = manager = ComponentManager(self.components, scwrapper, archcwrap)
                del(globals()['createComponent'])
                del(globals()['addMemoryMapping'])
                del(globals()['initProc'])
                del(globals()['callComponentFunction'])
                del(globals()['getSaboteurs'])
                del(globals()['getNominalComponents'])
                del(globals()['getGoldenComponents'])
                del(globals()['getGoldenCompInstance'])
                del(globals()['injector'])
                del(globals()['enableGoldenModel'])
                del(globals()['getProcFromId'])


def get_namespace():
    """ Returns the respkernel dictionary (i.e. namespace) """
    return globals()

def reset():
    """ Resets the simulator to the initial state """
    # Delete All
    # TODO: Refactor!

    if controller.error == True:
        print "\n\nSimulation cannot be restarted since an exception has been thrown!\n\n"
        return

    controller.stop_simulation()
    for name in globals().keys():
        if name == 'controller':
            continue
        comp = globals()[name]
        try:
            base = comp.__class__.__bases__[0]

            # Remove stray power models
            if base.__name__ == 'model':
                del globals()[name]
                continue

            # Remove all boost.python references
            while base.__name__ != "instance" and base.__name__ != "object":
                base = base.__bases__[0]
            if base.__name__ == "instance":
                del globals()[name]

        except:
            pass


    for name in globals().keys():
        if name == 'controller':
            continue
        comp = globals()[name]
        if (isinstance( comp , list) or isinstance( comp, dict )) and not name.startswith('__'):
            del globals()[name]
        elif hasattr (globals()[name], '__del__'):
            del globals()[name]

    #print 'SynchManager.reset'
    # Reset OSEmulation
    SynchManager.reset()

    #print 'archcwrap.reset'
    # Reset Syscalls
    archcwrap.reset()

    #print 'manager'
    # Reset component manager
    manager.reset()

    #print 'power'
    # Reset power framework
    import power
    power.reset()

    #print 'SystemC'
    # Reset SystemC (tricky)
    scwrapper.sc_get_curr_simcontext().reset()

    #print 'power'
    # Reset controller
    controller.reset()


def findInFolder(fileName, startFolder):
    '''Locate all files matching supplied filename pattern in and below
    supplied root directory.'''
    for path, dirs, files in os.walk(os.path.abspath(startFolder)):
        if fileName in files:
            return os.path.join(path, fileName)
    return None

class RespOutFile:
    """ File object for communication with the kernel. This is probably horribly slow and can be written
        much more elegantly. If you know how, please do it. This class is used for testing """
    def __init__(self):
        self.buffer = []

    def write( self, data ):
        self.buffer.append(data)

    def readline(self):
        value = ""
        while(value.rfind('\n') < 0 ):
            value += self.buffer[0]
            self.buffer = self.buffer[1:]

        return value

    def read(self, num = None):
        if not num:
            value = ''.join(self.buffer)
            self.buffer = []
            #return value #Instruction moved outside the else block
        else:
            value = ''.join(self.buffer)[:num]
            self.buffer = [ ''.join(self.buffer)[num:] ]
        return value
