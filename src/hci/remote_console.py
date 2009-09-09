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

"""Remote console"""

import os, sys, code
from optparse import OptionParser
curPath = os.path.abspath(os.path.join(os.path.dirname(sys.modules['__main__'].__file__),'..','..'))
sys.path.append(os.path.abspath(os.path.join(curPath, 'src')))
sys.path.append(os.path.abspath(os.path.join(curPath, 'src', 'hci')))

import hci
from server_api import *
import server
from hci import console
from console import Console

class RemoteConsole(Console):
    """
        Console class for the ReSP interactive shell
    """

    def __init__(self, resp_port=2200, resp_server = 'localhost' , locals=None):
        #dummy stuff required by the Console...
        self.verbose = False
        self.debug = False
        self.started = False
        self.error = False

        self.client = RespClient( resp_server, resp_port )
        self.init_history(os.path.expanduser("~/.resp-history"))
        code.InteractiveConsole.__init__(self,  locals = locals)
        self.init_console()

    def runsource(self, source, filename="<input>", symbol="single"):
        # Try to compile incomplete code
        try:
            code = self.compile(source, filename, symbol)
        except (OverflowError, SyntaxError, ValueError):
            self.showsyntaxerror(filename)
            return False

        # If code is incomplete, do nothing
        if code is None:
            return True

        # Execute command
        ####################################################################
        # Check for local commands!!!
        ####################################################################
        if source == 'quit' or source == 'exit()':
          self.client.quit()
          exit()
        try:
            print server.decode_compound(self.client.execute(source))
        except Exception, e:
            print e
        return False

    def ctrl_c_handler(self, signum, frame):
        """Custom handler of the CTRL-C (SIGINT)
        action: it pauses the simulation"""
        self.client.shutdown()
        print '\n' + colors.colorMap['red'] + 'Simulation Paused' + colors.colorMap['none'] + '\n'


    def show_commands(self):
        """It returns a string containing the list of the available commands with
        a short description on how they work: usefull to get started"""

        ########## WRITE SOMETHING INTERESTING HERE
        pass


if __name__ == "__main__":
    usage = "usage: remote_console [options]"
    import respkernel
    parser = OptionParser(usage=usage, version = 'Reflective Simulation Platform. Version: ' + respkernel.__version__ + ' Revision ' + respkernel.__revision__[6:-2])
    parser.description = 'Reflective Simulation Platform; version ' + respkernel.__version__ + '. Released under the GPL license'
    parser.add_option("-p", "--port", dest="port", type="int", default=2200, help="specifies the server port")
    parser.add_option("-s", "--server", dest="server", type="string", default='localhost', help="specifies the server address")

    try:
        (options, args) = parser.parse_args()
        if not isinstance(options.port,int):
            parser.error("port argument must be an integer number")
        if int(options.port) < 0:
            parser.error("port argument must be a positive integer number")
    except BaseException, e:
        #in case of bad parameter specification resp exits.
        #error message is just printed by the parser.
        sys.exit(0)

    RemoteConsole(int(options.port),options.server).interact(hci.console.banner)