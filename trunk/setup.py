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

    This is a wizard setup script for the platform compilation and installation
"""

import os, sys
import readline

class Completer:
    def __init__(self, namespace = None):
        """Create a new completer for the command line."""

        self.matches = []

    def complete(self, text, state):
        """Return the next possible completion for 'text'.

        This is called successively with state == 0, 1, 2, ... until it
        returns None.  The completion should begin with 'text'.

        """
        if state == 0:
            import os
            import re
            text = os.path.expanduser(text)
            if text == '':
                text = './'
            dirName = os.path.dirname(text)
            baseName = os.path.basename(text)
            if not os.path.exists(dirName):
                return None
            files = os.listdir(dirName)
            if not baseName == '':
                files = filter( lambda x: os.path.basename(x).startswith(baseName) , files )
            self.matches = []
            for i in files:
                curPath = os.path.join(dirName, i)
                if os.path.isdir(curPath):
                    self.matches.append(curPath + os.sep)
                else:
                    self.matches.append(curPath)
        try:
            return self.matches[state]
        except:
            return None

if __name__ == '__main__':
    completer = Completer()
    readline.set_completer(completer.complete)
    readline.parse_and_bind("tab: complete")
    readline.set_completer_delims('\t\n`!@#$%^&*)=+[{]}\\|;:,<>?')

    curPath = os.path.abspath(os.path.dirname(sys.modules['__main__'].__file__) + os.sep + 'src'  + os.sep + 'hci')
    if curPath not in sys.path:
        sys.path.append(curPath)
    curPath = os.path.abspath(os.path.dirname(sys.modules['__main__'].__file__) + os.sep + 'src' )
    if curPath not in sys.path:
        sys.path.append(curPath)

    import colors

    if (sys.platform=='win32') or ('NOCOLOR' in os.environ) \
        or (os.environ.get('TERM', 'dumb') in ['dumb', 'emacs']) \
        or (not sys.stdout.isatty()):
        colors.set_colors(False)
    else:
        colors.set_colors(True)

    import console

    print colors.colorMap['bright_green'] + console.banner_str + colors.colorMap['none'] + console.version_str
    print '\n\n' + colors.colorMap['bright_green'] + 'Welcome to the installation script of the ' + colors.colorMap['magenta'] + 'ReSP Reflective Simulation Platform' + colors.colorMap['none'] + '\n\n'

    # Configuring the project for compilation
    try:
        print "You will now be asked to enter some information on your environment\n"
        systemc_dir = os.path.expanduser((raw_input('Enter the location of the ' + colors.colorMap['bright_green'] + 'SystemC library' + colors.colorMap['none'] + ': ')).replace('\n', ''))
        while not os.path.exists(systemc_dir):
            systemc_dir = os.path.expanduser((raw_input('Please specify an existing path for the ' + colors.colorMap['bright_green'] + 'SystemC library' + colors.colorMap['none'] + ': ')).replace('\n', ''))
        tlm_dir = os.path.expanduser((raw_input('Enter the location of the ' + colors.colorMap['bright_green'] + 'TLM library' + colors.colorMap['none'] + ': ')).replace('\n', ''))
        while not os.path.exists(tlm_dir):
            tlm_dir = os.path.expanduser((raw_input('Please specify an existing path for the ' + colors.colorMap['bright_green'] + 'TLM library' + colors.colorMap['none'] + ': ')).replace('\n', ''))
#MATLAB
        matlab_dir = os.path.expanduser((raw_input('Enter the location of the ' + colors.colorMap['bright_green'] + 'MATLAB ' + colors.colorMap['none'] + ' (just press RETURN if you do not need it): ')).replace('\n', ''))      
        boost_head_dir = os.path.expanduser((raw_input('Enter the location of the ' + colors.colorMap['bright_green'] + 'boost library headers' + colors.colorMap['none'] + ' (just press RETURN for their automatic detection): ')).replace('\n', ''))
        while not os.path.exists(boost_head_dir) and boost_head_dir != '':
            boost_head_dir = os.path.expanduser((raw_input('Please specify an existing path for the ' + colors.colorMap['bright_green'] + 'boost library headers' + colors.colorMap['none'] + ': ')).replace('\n', ''))
        boost_lib_dir = os.path.expanduser((raw_input('Enter the location of the ' + colors.colorMap['bright_green'] + 'boost library libraries' + colors.colorMap['none'] + ' (just press RETURN for their automatic detection): ')).replace('\n', ''))
        while not os.path.exists(boost_lib_dir) and boost_lib_dir != '':
            boost_head_dir = os.path.expanduser((raw_input('Please specify an existing path for the ' + colors.colorMap['bright_green'] + 'boost library libraries' + colors.colorMap['none'] + ': ')).replace('\n', ''))
        debug = (raw_input('Do you want to have the possibility of ' + colors.colorMap['bright_green'] + 'debugging' + colors.colorMap['none'] + ' simulated programs (it slows down execution) [y/N]: ')).replace('\n', '')
        profile = (raw_input('Do you want to have the possibility of ' + colors.colorMap['bright_green'] + 'profiling' + colors.colorMap['none'] + ' simulated programs (it slows down execution) [y/N]: ')).replace('\n', '')
        trace = (raw_input('Do you want to have the possibility of ' + colors.colorMap['bright_green'] + 'producing debug traces' + colors.colorMap['none'] + ' of the instructions executed by the processors (it slows down execution) [y/N]: ')).replace('\n', '')
        os_emu = (raw_input('Do you want to enable ' + colors.colorMap['bright_green'] + 'Operating System Emulation' + colors.colorMap['none'] + ' (if you only need to run a real OS, such as eCos, this option will only slow you down) [Y/n]: ')).replace('\n', '')
        processors = (raw_input('Specify the ' + colors.colorMap['bright_green'] + 'processors' + colors.colorMap['none'] + ' that you want to use (separate the names with a space, nothing to enable all of them) [arm7 arm7ca leon2 ppc405 powerpc mips1]: ')).replace('\n', '').lower()
        arm_cross_path = os.path.expanduser((raw_input('Specify the path of the' + colors.colorMap['bright_green'] + ' arm cross-compiler' + colors.colorMap['none'] + ' (just press RETURN if you do not need it): ')).replace('\n', ''))
        sparc_cross_path = os.path.expanduser((raw_input('Specify the path of the' + colors.colorMap['bright_green'] + ' sparc cross-compiler' + colors.colorMap['none'] + ' (just press RETURN if you do not need it): ')).replace('\n', ''))
        mips_cross_path = os.path.expanduser((raw_input('Specify the path of the' + colors.colorMap['bright_green'] + ' mips cross-compiler' + colors.colorMap['none'] + ' (just press RETURN if you do not need it): ')).replace('\n', ''))
        ppc_cross_path = os.path.expanduser((raw_input('Specify the path of the' + colors.colorMap['bright_green'] + ' powerpc cross-compiler' + colors.colorMap['none'] + ' (just press RETURN if you do not need it): ')).replace('\n', ''))
        debug_level = (raw_input('Specify the ' + colors.colorMap['bright_green'] + 'compilation flags' + colors.colorMap['none'] + ' [debug/release/OPTIMIZED]: ')).replace('\n', '')
        eCos_path = os.path.expanduser((raw_input('Specify the ' + colors.colorMap['bright_green'] + 'eCos source folder' + colors.colorMap['none'] + ' (just press RETURN if you do not need eCos): ')).replace('\n', ''))
        if eCos_path != '':
            wxGTK_path = os.path.expanduser((raw_input('Specify the ' + colors.colorMap['bright_green'] + 'wxGTK-2.4.0' + colors.colorMap['none'] + ' (just press RETURN if you do not need the graphical eCos support tools): ')).replace('\n', ''))
            eCos_dest = os.path.expanduser((raw_input('Specify the ' + colors.colorMap['bright_green'] + 'destination for the eCos support binaries' + colors.colorMap['none'] + ' [~/ecos-bin]: ')).replace('\n', ''))
        tests = (raw_input('Do you want to ' + colors.colorMap['bright_green'] + 'run the tests' + colors.colorMap['none'] + ' at the end of the compilation (it will take a lot of time) [Y/n]: ')).replace('\n', '')

        options = "--with-systemc=" + systemc_dir + " --with-tlm=" + tlm_dir 
        if matlab_dir != "":
            options += " --with-matlab=" + matlab_dir
        if boost_head_dir != "":
            options += " --boost-includes=" + boost_head_dir
        if boost_lib_dir != "":
            options += " --boost-libs=" + boost_lib_dir
        if debug.lower() == "y":
            options += " -D"
        if profile.lower() == "y":
            options += " -P"
        if trace.lower() == "y":
            options += " -T"
        if os_emu.lower() == "n":
            options += " -S"
        if processors != "":
            options += " --enable-processor=\"" + processors + "\""
        if arm_cross_path != "":
            options += " --arm-compiler=" + arm_cross_path
        if sparc_cross_path != "":
            options += " --sparc-compiler=" + sparc_cross_path
        if mips_cross_path != "":
            options += " --mips-compiler=" + mips_cross_path
        if ppc_cross_path != "":
            options += " --ppc-compiler=" + ppc_cross_path
        if debug_level != "":
            options += " --debug " + debug_level
        if eCos_path != "":
            options += " --ecos-path=" + eCos_path
            if wxGTK_path != "":
                options += " --wxgtk-path=" + wxGTK_path
            else:
                print colors.colorMap['bright_green'] + 'Warning:' + colors.colorMap['none'] + ' the wxGTK path was specified, but eCos is not going to be built'
            if eCos_dest != "":
                options += " --ecos-bin-dest=" + eCos_dest

        raw_input('\nPress RETURN to ' + colors.colorMap['bright_brown'] + 'start the project configuration' + colors.colorMap['none'])
        print ''

        conf_result = os.system("./waf configure " + options + " --nocache")

        if conf_result != 0:
            print '\n' + colors.colorMap['bright_red'] + 'An error occurred during ReSP configuration process' + colors.colorMap['none'] + '\n'
            print 'The options used to configure the system were:\n' + options + '\n'
            sys.exit()

        conf_result = raw_input('\nPress RETURN to ' + colors.colorMap['bright_brown'] + 'start the project compilation' + colors.colorMap['none'] + ' (it takes a while, so we suggest you have a coffee in the meantime ...)')
        print ''
    except EOFError,e:
        print '\n\n' + colors.colorMap['bright_red'] + 'Installation process aborted' + colors.colorMap['none'] + '\n'
        sys.exit(0)
    except KeyboardInterrupt,e:
        print '\n\n' + colors.colorMap['bright_red'] + 'Installation process aborted' + colors.colorMap['none'] + '\n'
        sys.exit(0)

    if tests.lower() == "n":
        conf_result = os.system("./waf")
    else:
        conf_result = os.system("./waf --execute-tests")

    if conf_result != 0:
        print '\n' + colors.colorMap['bright_red'] + 'An error occurred during ReSP compilation process' + colors.colorMap['none']
        print 'The options used to configure the system were:\n' + options
        sys.exit()

    print '\nCompilation succesfully completed'
    print 'In future, if you want to compile modifications you make to the code just run the command \'./waf\' at the command prompt'
    print '\n' + colors.colorMap['bright_red'] + 'Type ' + colors.colorMap['bright_brown'] + '\'./startSim.sh -a architectures/test_arch.py\'' + colors.colorMap['bright_red'] + ' at the command prompt to quickly test ReSP installation' + colors.colorMap['none'] + '\n'
