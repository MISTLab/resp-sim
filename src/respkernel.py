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

"""ReSP core code and simulation namespace"""

# General Init code ############################################################

__version__ = '0.5'
__revision__ = '$Rev$'

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
curPath = os.path.abspath(os.path.join(os.path.dirname(sys.modules['respkernel'].__file__),'..'))

global blddir, srcdir
lockFile = open(os.path.join(curPath, '.lock-wscript'))
for line in lockFile.readlines():
    try:
        exec line
    except:
        pass
lockFile.close()

sys.path.append(os.path.abspath(os.path.join(srcdir, 'src', 'manager')))
sys.path.append(os.path.abspath(os.path.join(srcdir, 'src', 'controller')))
sys.path.append(os.path.abspath(os.path.join(srcdir, 'src', 'hci')))
sys.path.append(os.path.abspath(os.path.join(srcdir, 'src', 'power')))
sys.path.append(os.path.abspath(os.path.join(srcdir, 'src', 'fi')))
sys.path.append(os.path.abspath(os.path.join(blddir, 'default', 'src', 'controller')))
sys.path.append(os.path.abspath(os.path.join(blddir, 'default', 'src', 'sc_wrapper')))
sys.path.append(os.path.abspath(os.path.join(blddir, 'default', 'src', 'tlm_wrapper')))
sys.path.append(os.path.abspath(os.path.join(blddir, 'default', 'src', 'trap_wrapper')))
sys.path.append(os.path.abspath(os.path.join(blddir, 'default', 'src')))
sys.path.append(os.path.abspath(os.path.join(blddir, 'default', 'src', 'utils')))
sys.path.append(os.path.abspath(os.path.join(blddir, 'default', 'src', 'bfdFrontend')))
sys.path.append(os.path.abspath(os.path.join(blddir, 'default', 'src', 'loader')))

import power
import colors

# END General Init code ########################################################

def getCppName(pypp_name):
    #TODO never used
    return pypp_name.replace('_comma_', ',').replace('_less_', '<').replace('_greater_', '>').replace('_scope_', '::').replace(',_', ', ')

def filterNames(namespaceToFilter):
    '''This function receives as parameter a namespace (systemc, TRAP and TLM libraries, or a related component) and
    filter the names contained in it and derived from the systemc, tlm and trap libraries. The objective is to set more 
    user-friendly names'''
        
    TRAP_PREFIX = "TRAPW_"
    TLM_PREFIX = "TLMW_"
    SC_PREFIX = "SYSCW_"

    TRAP_WRAPPER = 'trapwrapper'
    TLM_WRAPPER = 'tlmwrapper'
    SC_WRAPPER = 'scwrapper'

    global scwrapper, tlmwrapper, trapwrapper
    from types import BuiltinFunctionType # all built-ins functions are instance of this type

    # Prefixes of the three libraries and corresponding wrapper names
    prefixes = [TRAP_PREFIX, TLM_PREFIX, SC_PREFIX]
    wrapperNames = {TRAP_PREFIX: TRAP_WRAPPER, TLM_PREFIX: TLM_WRAPPER, SC_PREFIX: SC_WRAPPER}

    # Filtering names for each one of the three libs (TRAP, TLM, systemc)
    for prefix in prefixes: # Iterate each library to filter the related names in the namespace passed as parameter
        currWrapperName = wrapperNames[prefix]
        for j in filter(lambda x: x.startswith(prefix), dir(namespaceToFilter)):
            item = getattr(namespaceToFilter, j)
            if not isinstance(item, BuiltinFunctionType):
                try:
                    item.__name__ = j[len(prefix):]
                    item.__module__ = currWrapperName
                except:
                    print 'Error converting name of ' + j
            currWrapper = globals()[currWrapperName]
            if not j[len(prefix):] in dir(currWrapper):
                setattr(currWrapper, j[len(prefix):], item)
            delattr(namespaceToFilter, j)

    # Now I have again to go over the components and eliminate all those elements which are
    # contained also in scwrapper, in TLM wrapper, and in TRAP wrapper
    toFilterNames_names = []
    for prefix in prefixes:
        currWrapperName = wrapperNames[prefix]
        if namespaceToFilter.__name__ != currWrapperName:
            try:
                currWrapper = globals()[currWrapperName]
                toFilterNames_names += dir(currWrapper)
            except:
                pass
    for j in dir(namespaceToFilter):
        if j in toFilterNames_names and not j.startswith('_'):
            delattr(namespaceToFilter, j)

class RespKernel:
    """This class represents the core simulator class. This class provides a set of objects and functions to
    manipulate simulation objects. RespKernel initializes the code and sets up variables in the global dictionary,
    that can be used by InteractiveInterpreters to provide human-computer interfaces"""

    def __init__(self,  options=None):
        """Initializes the simulation and component managing kernel"""

        # Initialization of the main kernel interface
        # The "Manager" deals with component instantiation and life cycle
        self.manager = None

        # The "Controller" deals with simulation control (run, pause, stop, etc.)
        self.controller = None
        
        self.scripting_commands = []
        self.components = [] # The list of all available components
        self.fileName = None # The name of the loaded architecture
        self.interactive = True # States id the simulation mode is interactive or not
        self.verbose = False # States if the execution is verbose or not
        self.debugger = None # This is a reference to the process running the debugger

        # Check if the libraries have been correctly compiled
        try:
            import configuration
            if configuration.STATIC_PLATFORM:
                print '\nPlease compile ReSP without static platform support in order to be able to run interactive simulation through the python console\n'
                sys.exit(0)
        except  SystemExit:
            sys.exit(0)
        except:
            pass

        #setup ReSP kernel
        try:
            self.setup(options)
            self.setup_scripting_commands()
        except Exception,  e:
            print '\nError in the setup of the ReSP Kernel  --> ' + str(e) + '\n'
            if self.verbose:
                import traceback
                traceback.print_exc()
                print ''
            sys.exit(0)

    def setup(self, options=None):
        """Sets up the ReSP interpreter: library and component loading, simulation controller instantiation, etc."""

        # Now I start performing the parsing of the command line options
        if options:
            self.interactive = not (options.silent) # or not options.interactive) #TODO ????? check it
            self.verbose = options.verbose
            if options.color:                
                colors.enable_colors()

        #turn off the warnings
        if not self.verbose:
            import warnings
            warnings.simplefilter('ignore', RuntimeWarning)

        # Import modules for the respkernel namespace
        # import systemc, trap and tlm wrapper

        global tlmwrapper, scwrapper, trapwrapper, sc_controller, compManager, converters, bfdwrapper, loader_wrapper

        if self.verbose:
            print "Loading converters"
        import converters
        if self.verbose:
            print "Loading scwrapper"
        import scwrapper
        filterNames(scwrapper)
        if self.verbose:
            print "Loading tlmwrapper"
        import tlmwrapper
        filterNames(tlmwrapper)
        if self.verbose:
            print "Loading trapwrapper"
        import trapwrapper
        filterNames(trapwrapper)
        if self.verbose:
            print "Loading sc_controller_wrapper"
        import sc_controller_wrapper
        filterNames(sc_controller_wrapper)

        # Import Optional components
        if self.verbose:
            print "Loading compManager"
        import compManager
        if self.verbose:
            print "Loading bfdwrapper"
        import bfdwrapper
        if self.verbose:
            print "Loading loader_wrapper"
        import loader_wrapper

        # import components storing them into the component list
        self.components = []
        componentListFile = os.path.abspath(os.path.join(blddir, '.wrapper_order.py'))
        if os.path.exists(componentListFile):
            try:
                self.load_components(componentListFile)
            except Exception,  e:
                print >> sys.stderr, 'Error in loading the components in the simulator --> ',  e
                sys.exit(0)
        else:
            print >> sys.stderr, 'File ' + componentListFile + ' does not exists'
            sys.exit(0)

        # turn on the warnings
        if not self.verbose:
            warnings.resetwarnings()

        # create the component manager
        from compManager import ComponentManager
        global manager
        self.manager = manager = ComponentManager(self.components)
        self.fileName = ''

        # instantiate controller and register the
        # callback for the end of simulation        
        if self.verbose:
            print "Setting up the controller"        
        self.setup_controller(self.interactive)        

        if globals().has_key('__warningregistry__'):
            globals().pop('__warningregistry__')
        if globals().has_key('dl'):
            globals().pop('dl')
            
        #start debugger
        if options.debug:
            self.start_debugger()
            if self.verbose:
                print "Starting debugger..."
        
        #register in the exit handler the function destroing all imported stuff
        atexit.register(self.delete_all)

    def setup_controller(self, interactive):
        '''Instantiates the controller and register the callback for the end of simulation'''
        #instantiate controller
        if self.verbose:
            print "Instantiating the controller"        
        global controller
        try:
            self.controller = sc_controller_wrapper.sc_controller.getController(interactive)
        except:
            if interactive:
                self.controller = sc_controller_wrapper.sc_controller.getController()
            else:
                self.controller = sc_controller_wrapper.sc_controller.getController(None)
        controller = self.controller
        
        # register callback for end of simulation
        if self.verbose:
            print "Registering eosCb"        
        from print_stats import print_stats_cb
        global eosCb
        eosCb = print_stats_cb(self.controller)
        sc_controller_wrapper.registerEosCallback(eosCb)
        
    def setup_scripting_commands(self):
        """Creates some convenience functions, used to run scripts without the need to refer to RespKernel, controller or manager objects"""

        global connect, connectPorts, connectPortsByPathName, listComponents, printComponents, getCompInstance, \
                areConnected, getInstantiatedComponents, getAttrInstance,  instantiateComponent, run_simulation, \
                pause_simulation, stop_simulation,  get_simulated_time, get_real_time, run_up_to, reload_architecture, \
                enable_fault_injection, showArchitecture, reset, load_architecture, reload_architecture, get_architecture_filename, register_breakpoint

        # Assign scripting commands to manager, helper and controller methods
        reload_architecture = self.reload_architecture
        connect = self.manager.connect
        connectPorts = self.manager.connectPorts
        connectPortsByPathName = self.manager.connectPortsByPathName
        listComponents = self.manager.listComponents
        printComponents = self.manager.printComponents
        getCompInstance = self.manager.getCompInstance
        areConnected = self.manager.areConnected
        getInstantiatedComponents = self.manager.getInstantiatedComponents
        showArchitecture = self.manager.showArchitecture
        
        import breakpoints
        register_breakpoint = breakpoints.register_breakpoint       
      
        import helper
        getAttrInstance =  helper.getAttrInstance
        instantiateComponent = helper.instantiateComponent

        run_simulation = self.controller.run_simulation
        run_up_to = self.controller.run_up_to
        pause_simulation = self.controller.pause_simulation
        stop_simulation = self.controller.stop_simulation
        get_simulated_time = self.controller.get_simulated_time
        get_real_time = self.controller.get_real_time
        reset = self.reset
        
        load_architecture = self.load_architecture
        reload_architecture = self.reload_architecture
        get_architecture_filename = self.get_architecture_filename

        enable_fault_injection  = self.enable_fault_injection

        # List commands in an internal array for reference
        self.scripting_commands = [connect, connectPorts, connectPortsByPathName, listComponents,  printComponents, \
                                   getCompInstance, areConnected, getInstantiatedComponents, \
                                   getAttrInstance,  instantiateComponent, run_simulation, pause_simulation, \
                                   stop_simulation, get_simulated_time, get_real_time, run_up_to, enable_fault_injection, showArchitecture, reset, \
                                   load_architecture, reload_architecture, get_architecture_filename, register_breakpoint]

        
    def load_components(self, componentListFile):
        """Loads all components included in the ReSP. The complete list is contained in the componentListFile that has been built during compiling"""
        componentPathList = [line.strip() for line in open(componentListFile).readlines()]
        for component in componentPathList:
            if not os.path.exists(component):
                if self.verbose:
                    print 'Component ' + component + ' appears in the compilation list, but the library is missing'
            else:
                componentPath, componentFile = os.path.split(component)
                componentName = os.path.splitext(componentFile)[0]
                if not componentName in globals():
                    if not componentPath in sys.path:
                        sys.path.append(componentPath)
                    if self.verbose:
                        print 'Loading ' + componentName
                    temp = __import__(componentName)
                    try:
                        filterNames(temp)
                    except:
                        print "Error in renaming during loading of component " + componentName
                        if self.verbose:
                            import traceback
                            traceback.print_exc()
                    self.components.append(temp)
                    globals()[componentName] = temp

    def load_architecture(self, fileName, returnCheck = False):
        """Loads the architecture specified in fileName; currently it must be a python script. If returnCheck
        parameter is enabled, the function returns True in case of success or False if any problem occurred during
        architecture loading"""
        if fileName.endswith('.py'):
            # I have to find the path for the module, then add it to the
            # system path and finally import the module: all the instructions
            # contained in the module (which are the instructions specifying how
            # to connect the architeture) will be executed
            try:
                exec open(fileName) in globals()
            except Exception, e:
                print colors.colorMap['red'] + 'Error in opening architecture file: '+colors.colorMap['none'] + fileName + ' --> ' + str(e) + '\n'
                if self.verbose:
                    import traceback
                    traceback.print_exc()
                    print ''
                print 'In order to be able to load a new architecture, the simulator must be restarted'
                if returnCheck:
                    return False
                else:
                    return
            else:
                print '\n' + colors.colorMap['red'] + 'File ' + colors.colorMap['none'] + colors.colorMap['bright_green'] + fileName + colors.colorMap['none'] + colors.colorMap['red'] + ' correctly loaded\n' + colors.colorMap['none']
                if sys.stdout.isatty():
                    print "\033]0;ReSP - " + os.path.basename(fileName) + "\007"
        else:
            print 'Currelty only files specified in Python are supported'
            if returnCheck:
                return False
            else:
                return
        
        self.fileName = fileName
        if returnCheck:
            return True
        else:
            return
        
    def reload_architecture(self):
        """Reloads the current architecture after resetting the simulator"""
        if self.fileName == None:
            print 'No architecture to reload'
            return
        self.reset()
        self.load_architecture(self.fileName)

    def get_architecture_filename(self):
        '''Return the name of the file containing the loaded architecture'''
        return self.fileName

    def run_silent(self, duration=-1):
        '''Runs the simulation in batch mode'''
        # it executes a simulation in non-interactive mode and executing the specified python 
        # actions at different times. TODO In addition to the actions it is possible to change the
        # parameters of the loaded architecture
        
        #if simulation commands are not specified in the architecture file, execute the whole simulation
        if not controller.has_started():
            run_simulation(duration)
	statsPrinter()

    def start_debugger(self):
        '''Starts the GBD debugger'''
        import subprocess
        try:
            self.debugger = subprocess.Popen(['xterm', '-e', 'gdb', '-p', str(os.getpid()), '-ex', 'continue', '&'])
        except Exception, e:
            print '\n--> Error while opening GDB !!!\n'
            if self.verbose:
                import traceback
                traceback.print_exc()
                print ''
                
    def end_debugger(self):
        '''Ends GBD debugger, if started'''
        if self.debugger != None:
            os.system('kill -9 ' + str(self.debugger.pid))
        
    def get_namespace(self):
        """ Returns the respkernel dictionary (i.e. namespace) """
        return globals()

    def __delete_all_helper(self):
        """ Deletes all the object instances in namespaces whose base type is
        Boost.Python.instance. DO NOTE: it should not be called directly since it
        may cause ReSP crash if the simulation is still running"""
        
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
                    if self.verbose:
                        print 'deleting ' + str(name)
                    del globals()[name]
                    if self.verbose:
                        print 'deleted ' + str(name)
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

      
    def delete_all(self):
        """Deletes all the object instances in namespaces whose base type is
        Boost.Python.instance; note that in case the simulation is still
        running, the first thing I do is stopping it"""
        #if (not self.controller.is_finished()) and self.controller.is_running() and (not self.controller.error):
        if ((self.controller.has_started() and not self.controller.is_ended()) and not self.controller.error):
            if self.verbose:
                print 'killing the simulation'
            self.controller.stop_simulation()
       
        # kill the debugger process if the simulation is in debugging mode
        if self.debugger != None:
          self.end_debugger()
          
        self.__delete_all_helper()

    def reset(self):
        """ Resets the simulator to the initial state """ 
        global controller

        #check if an exception has occurred in systemc...
        if controller.error == True:
            print "\n\nSimulation cannot be restarted since an exception has been thrown!\n\n"
            return
            
        # Stop simulation
        if (controller.has_started() and not controller.is_ended()):
          controller.stop_simulation()
        
        # Delete All
        self.__delete_all_helper()

        # Reset OSEmulation
        #cm.reset()

        # Reset component manager
        manager.reset()

        # Reset power framework
        import power
        power.reset()
        
        # Reset breakpoints
        import breakpoints
        breakpoints.reset()

        # Reset SystemC (tricky)
        scwrapper.sc_get_curr_simcontext().reset()

        # Reset controller
        controller.reset_controller()
        
        del globals()['controller']
        for script in self.scripting_commands:
            del globals()[script.__name__]
                
        #instantiate a new controller and setup scripting commands (for simplicity all commands are restored...)
        self.setup_controller(self.interactive)
        self.setup_scripting_commands()

    def enable_fault_injection(self, value = True):
        """ Enables oe disables the fault injection environment"""
        try:
            from fi import fiCompManager
        except:
            print "Fault injection not supported"
            return
        
        from compManager import ComponentManager
        global manager

        if len(self.manager.getInstantiatedComponents()) > 0:
            print 'An architecture is instantiated. A reset is required for enabling the fault injiection'

        from fiCompManager import FaultInjectionComponentManager
        if value:
            if not isinstance(self.manager,FaultInjectionComponentManager):
                #enable fault injection environment
                self.manager = manager = FaultInjectionComponentManager(self.components)                
                self.setup_scripting_commands()
                
                global getProbe, getProbes, getProbePosition, registerComponent
                getProbe = manager.getProbe
                getProbes = manager.getProbes
                getProbePosition = manager.getProbePosition
                registerComponent = manager.registerComponent
                self.scripting_commands.append(getProbe)
                self.scripting_commands.append(getProbes)
                self.scripting_commands.append(getProbePosition)
                self.scripting_commands.append(registerComponent)
        else:
            if isinstance(self.manager,FaultInjectionComponentManager):
                #disable fault injection environment
                self.manager = manager = ComponentManager(self.components)
                self.setup_scripting_commands()
                del globals()['getProbe']
                del globals()['getProbes']
                del globals()['getProbePosition']
                del globals()['registerComponent']


def get_namespace():
    """ Returns the respkernel dictionary (i.e. namespace) """
    return globals()

def findInFolder(fileName, startFolder):
    '''Locates all files matching supplied filename pattern in and below
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
