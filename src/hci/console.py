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

import code, sys, os, readline, atexit, signal

import colors
import respkernel

banner_str = r"""
            ___           ___           ___           ___
           /  /\         /  /\         /  /\         /  /\
          /  /::\       /  /:/_       /  /:/_       /  /::\
         /  /:/\:\     /  /:/ /\     /  /:/ /\     /  /:/\:\
        /  /:/~/:/    /  /:/ /:/_   /  /:/ /::\   /  /:/~/:/
       /__/:/ /:/___ /__/:/ /:/ /\ /__/:/ /:/\:\ /__/:/ /:/
       \  \:\/:::::/ \  \:\/:/ /:/ \  \:\/:/~/:/ \  \:\/:/
        \  \::/~~~~   \  \::/ /:/   \  \::/ /:/   \  \::/
         \  \:\        \  \:\/:/     \__\/ /:/     \  \:\
          \  \:\        \  \::/        /__/:/       \  \:\
           \__\/         \__\/         \__\/         \__\/
"""

version_str = """

       v""" + respkernel.__version__ + """ - Politecnico di Milano, European Space Agency
          This tool is distributed under the GPL License
"""

banner = colors.colorMap['bright_green'] + banner_str + colors.colorMap['none'] + version_str
command_str = """

    Type """ + colors.colorMap['red'] + """show_commands()""" + colors.colorMap['none'] + """ to get the list of available commands
"""

license_str = r"""
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the
Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA.
"""

class Console(code.InteractiveConsole):
    """
        Console class for the ReSP interactive shell
    """

    def __init__(self, resp_kernel,  verbose = False, debug = False, locals=None):
        self.verbose = verbose
        self.debug = debug
        self.started = False
        self.error = False
        self.resp_kernel = resp_kernel

        self.init_history(os.path.expanduser("~/.resp-history"))
        if not locals:
            locals = resp_kernel.get_namespace()
        code.InteractiveConsole.__init__(self,  locals = locals)
        self.init_console()

    def showtraceback(self):
        self.error = True
        code.InteractiveConsole.showtraceback(self)

    def showsyntaxerror(self, filename=None):
        self.error = True
        code.InteractiveConsole.showsyntaxerror(self, filename)

    def init_console(self):
        """initializes the console by importing all the libraries and packages
        containing the components and the wrappers"""

        # Now that the imports are finished, I can see if it is the case of starting GDB
        if self.debug:
            os.system('xterm -e gdb -p ' + str(os.getpid()) +  ' -ex continue &')

        # end, at then end I can instantiate the signal handler and the controller
        import customCompleter
        completer = customCompleter.Completer(self.locals)
        readline.set_completer(completer.complete)
        readline.parse_and_bind("tab: complete")
        readline.set_completer_delims('\t\n`~!@#$%^&*)-=+[{]}\\|;:,<>?')

        # Assign the SIGINT (ctrl-c) signal to the console handler
        signal.signal(signal.SIGINT, self.ctrl_c_handler)

        # Sneakily add some extra commands to the respkernel namespace
        self.locals['show_commands'] = self.show_commands
        self.locals['load_architecture'] = self.load_architecture
        self.locals['generate_fault_list'] = self.generate_fault_list
        
        #refs 114
        self.locals['save_architecture'] = self.save_architecture
        self.locals['generate_fault_list'] = self.generate_fault_list

    def start_console(self):
        """Starts the console interactive mode"""
        self.resp_kernel.controller.consoleInit = True
        self.interact('')

    def init_history(self, histfile):
        if hasattr(readline, "read_history_file"):
            tryBackup = False
            try:
                tryBackup = not os.path.exists(histfile)
                if not tryBackup:
                    historyFileDesc = open(histfile)
                    historyFileDesc.seek(0, 2)
                    tryBackup = (historyFileDesc.tell() == 0)
                    historyFileDesc.close()
            except:
                tryBackup = False
            try:
                if tryBackup:
                    try:
                        if os.path.exists(histfile + '_backup'):
                            import shutil
                            shutil.copy(histfile + '_backup', histfile)
                    except:
                        pass
                readline.read_history_file(histfile)
            except IOError:
                pass
            readline.set_history_length(1000)
            atexit.register(self.save_history, histfile)

    def save_history(self, histfile):
        readline.write_history_file(histfile)
        readline.write_history_file(histfile + '_backup')


    def load_architecture(self, fileName):
        """Loads the architecture specified in fileName; it can either be an XML file
        or a python script; namespace represents the namespace containing the classes of components
        referenced in the architecture; usually you can pass globals() as namespace parameter"""
        if fileName.endswith('.xml'):
            print colors.color_red + 'Error, Loading of XML files not yet supported' + colors.color_none
        elif fileName.endswith('.py'):
            # I have to find the path for the module, then add it to the
            # system path and finally import the module: all the instructions
            # contained in the module (which are the instructions specifying how
            # to connect the architeture) will be executed
            try:
                exec open(fileName)  in self.resp_kernel.get_namespace()
            except Exception, e:
                print colors.colorMap['red'] + 'Error in opening architecture file: '+colors.colorMap['none'] + fileName + ' --> ' + str(e) + '\n'
                if self.resp_kernel.verbose:
                    import traceback
                    traceback.print_exc()
                    print ''
                print 'In order to be able to load a new architecture, the simulator must be restarted'
            else:
                print '\n' + colors.colorMap['red'] + 'File ' + colors.colorMap['none'] + colors.colorMap['bright_green'] + fileName + colors.colorMap['none'] + colors.colorMap['red'] + ' correctly loaded\n' + colors.colorMap['none']
                if sys.stdout.isatty():
                    print "\033]0;ReSP - " + os.path.basename(fileName) + "\007"
        else:
            #refs 114
            exec("__ARCHPATH__ = '%s'" %fileName) in self.resp_kernel.get_namespace()
            curPath = os.path.abspath(os.path.join(os.path.dirname(sys.modules['respkernel'].__file__),'..'))
            exec open(curPath + '/tools/importexport/import.py') in self.resp_kernel.get_namespace()

        self.resp_kernel.get_namespace()['loadedFileName'] = fileName
        self.resp_kernel.fileName =  fileName
    
    #refs 114
    def save_architecture(self, fileName):
        exec("__ARCHPATH__ = '%s'" %fileName) in self.resp_kernel.get_namespace()
        curPath = os.path.abspath(os.path.join(os.path.dirname(sys.modules['respkernel'].__file__),'..'))
        exec open(curPath + '/tools/importexport/export.py') in self.resp_kernel.get_namespace()

    def generate_fault_list(self):
        import injection.listgeneration
        from injection.listgeneration import fault_list_generation
        try:
            fault_list_generation(self)
        except BaseException, e:
            print "Error occurred during fault list generation wizard: " + str(e)

    def ctrl_c_handler(self, signum, frame):
        """Custom handler of the CTRL-C (SIGINT)
        action: it pauses the simulation"""
        self.resp_kernel.controller.pause_simulation()
        print '\n' + colors.colorMap['red'] + 'Simulation Paused' + colors.colorMap['none'] + '\n'


    def show_commands(self):
        """It returns a string containing the list of the available commands with
        a short description on how they work: usefull to get started"""

        print '\n'+colors.colorMap['red'] + 'The available commands are:\n' + colors.colorMap['none']
        lineWidth = 90

        #This function looks recursively inside the specified elements and, if the publicCommands list
        #is found, it prints the element members whose name matches one of the elements of publicCommands

        # Lets star with the commands of the command manager
        import helper
        methods =  [ i.__name__ for i in self.resp_kernel.scripting_commands]
        for i in [self.resp_kernel.manager, helper,  self.resp_kernel.controller , colors, self]:
            funclist = dir(i)
            for func in funclist:
                if func in methods:
                    element = getattr(i, func)
                    if callable(element):
                        print (go_new_line('\t' + colors.colorMap['cyan'] + '** ' + func + '()' + colors.colorMap['none'] + ':', lineWidth) + '\n\t\t' + go_new_line(clean_docstr(element.__doc__ or 'no doc found...'), lineWidth) + '\n')


def clean_docstr(toClean):
    """Given a documentation string with tabulations and newlines it eliminates both
    of them every time they are consecutive"""
    import re
    return re.sub('\n[\t ]+', ' ', toClean)

def go_new_line(toModify, lineWidth = 90, numTabs = 2):
    """Given a documentation string the function introduces newline characters to
    respect the line width constraint"""
    tabs = ''
    for i in range(0, numTabs):
        tabs += '\t'
    # first of all I have to get the nearest white space character
    if len(toModify) < lineWidth:
        return toModify
    endToCheck = toModify.find('\n')
    if endToCheck < 0:
        endToCheck = len(toModify)
    i = endToCheck
    for i in range(lineWidth, endToCheck):
        if toModify[i] == ' ':
            break
    if i < endToCheck:
        return toModify[0:i] + '\n' + tabs + go_new_line(toModify[(i + 1):endToCheck], lineWidth, numTabs)
    else:
        if i < len(toModify) - 1:
            return toModify[0:(endToCheck + 1)] + go_new_line(toModify[(endToCheck + 1):len(toModify)], lineWidth, numTabs)
        else:
            return toModify
