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

import SocketServer
from threading import Thread

resp_kernel = None
globalServer = None

class RespRequestHandler(SocketServer.StreamRequestHandler):
    """Manage a request from a client"""

    name_index = 0

    ############################################################
    # RESP COMMANDS
    ############################################################
    def stop_simulation(self, arguments):
        resp_kernel.controller.stop_simulation()
        self.wfile.write(encode_string('OK'))

    def pause_simulation(self, arguments):
        resp_kernel.controller.pause_simulation()
        self.wfile.write(encode_string('OK'))

    def run_simulation(self,  arguments):
        resp_kernel.controller.run_simulation(int(arguments))
        self.wfile.write(encode_string('OK'))

    def list_components(self, arguments):
        list_ = resp_kernel.manager.listComponents()
        self.wfile.write(encode_string('OK'+encode_compound(list_)))

    def dir_entity(self, arguments):
        result = dir(resp_kernel.get_namespace()[arguments])
        self.wfile.write(encode_string('OK'+encode_compound(result)))

    def load_architecture(self, arguments):
        result = resp_kernel.load_architecture(arguments)
        if result: self.wfile.write(encode_string('OK'))
        else: raise Exception("Load failed")

    def create_component(self, arguments):
        name = arguments[1]
        code = name +'='+arguments[0]+"('"+arguments[1]+"',"
        # Add the constructor arguments
        for param in arguments[2:]:
            code += param+','
        code = code[:-1]+')\n'
        exec code in resp_kernel.get_namespace()
        self.wfile.write(encode_string('OK'+encode_compound(name)))

    def load_app(self, arguments):
        code = "loader = ExecLoader.ExecLoader('"+arguments[0]+"')\n"
        code += arguments[1] + ".init("+arguments[2]+", loader.getProgStart())\n"
        code += arguments[3] + ".loadApplication(loader.getProgData(), loader.getDataStart(), loader.getProgDim())"
        exec code in resp_kernel.get_namespace()
        self.wfile.write(encode_string('OK'))

    def attach_gdb(self, arguments):
        # Get processor type
        stub_type = resp_kernel.get_namespace()[arguments[0]].__class__.__name__
        code = "inter = GDBProcStub32."+stub_type+"Stub("+arguments[0]+")\n"
        code += "stub = GDBStub32.GDBStub32(inter, "+arguments[1]
        if len(arguments)>2: code+=",test=True"
        code += ")\n"
        code += arguments[0]+ ".setGDBStub(stub)"
        exec code in resp_kernel.get_namespace()
        self.wfile.write(encode_string('OK'))

    def attach_prof(self, arguments):
        code = "profiler = Profiler.Profiler('"+arguments['exec']+"',"+arguments['callgraph']+")\n"
        code += arguments['proc']+".setProfiler(profiler)\n"
        exec code in resp_kernel.get_namespace()
        self.wfile.write(encode_string('OK'))

    def execute(self, arguments):
        # First of all I try to evaluate the expression; in case
        # an error is returned it means that it is not an expression,
        # so I execute it

        try:
            retVal = eval(arguments, resp_kernel.get_namespace())
            if retVal != None:
                self.wfile.write(encode_string('OK'+encode_compound(retVal)))
            else:
                self.wfile.write(encode_string('OK'))
        except SyntaxError:
            try:
                exec arguments in resp_kernel.get_namespace()
                self.wfile.write(encode_string('OK'))
            except Exception, e:
                self.wfile.write(encode_string('EE' + str(e)))
        except Exception, e:
            self.wfile.write(encode_string('EE' + str(e)))

    def get_simulation_time(self, arguments):
        self.wfile.write(encode_string('OK'+'S'+str(resp_kernel.controller.get_simulated_time()))+')')

    def get_real_time(self, arguments):
        self.wfile.write(encode_string('OK'+'S'+str(resp_kernel.controller.get_real_time_ms()))+')')

    def connect_tlm(self, arguments):
        #name = arguments[1]
        code = "connectPortsForce( "
        # Add the constructor arguments
        for param in arguments:
            code += param+','
        code = code[:-1]+')\n'
        exec code in resp_kernel.get_namespace()
        self.wfile.write(encode_string('OK'))

    def connect_sysc(self, arguments):
        #name = arguments[1]
        code = "connectSyscPorts( "
        # Add the constructor arguments
        for param in arguments:
            code += param+','
        code = code[:-1]+')\n'
        exec code in resp_kernel.get_namespace()
        self.wfile.write(encode_string('OK'))

    def connect_sig(self, arguments):
        #name = arguments[1]
        code = "connectSyscSignal( "
        # Add the constructor arguments
        for param in arguments:
            code += param+','
        code = code[:-1]+')\n'
        exec code in resp_kernel.get_namespace()
        self.wfile.write(encode_string('OK'))

    def reset_resp(self, arguments):
        resp_kernel.reset()
        self.wfile.write(encode_string('OK'))

    def notify(self):
        """ Asynchronous notification, hardly used """
        if self.wait_for_end:
            self.wfile.write(encode_string('OK'))
            wait_for_end = False;
        resp_kernel.methods.remove(self.notify)

    def wait_end(self,args):
        """ Waits for simulation end, asynchronously if necessary (impossible unless in interactive mode)"""
        if resp_kernel.controller.is_running():
            resp_kernel.methods.append(self.notify)
            self.wait_for_end = True
        #else:
        self.wfile.write(encode_string('OK'))

    def get_energy(self, arguments):
        import power
        e , p = power.stats.get_global_values()
        self.wfile.write(encode_string('OK'+'S'+str(e))+')')

    def quit(self, arguments):
        self.wfile.write(encode_string('OK'))
        globalServer.server_close()
        import sys
        sys.exit(0)

    resp_commands = {
        "STOP" : stop_simulation,
        "PAUSE" : pause_simulation,
        "RUN" : run_simulation,
        "LISTCOMP" : list_components,
        "DIR" : dir_entity,
        "LOADARCH" : load_architecture,
        "CREATE" : create_component,
        "LOADAPP" : load_app,
        "ATTACHGDB" : attach_gdb,
        "ATTACHPROF" : attach_prof,
        "EXEC" : execute,
        "GETTIMES" : get_simulation_time,
        "GETTIMER"  : get_real_time,
        "CONNECTTLM" : connect_tlm,
        "CONNECTSYSC" : connect_sysc,
        "CONNECTSIG" : connect_sig,
        "RESET" : reset_resp,
        "WAITEND" : wait_end,
        "GETENERGY" : get_energy,
        "QUIT" : quit
    }

    ############################################################3

    def get_arguments(self,  command):
        """Extract the arguments from a user command"""
        index = command.find('-')
        if index < 0: return command,  ''
        else: return command[:index],  decode_compound(command[index+1:])

    def handle(self):
        """Handle a signle request"""
        end_request = False

        while( not end_request ):
            # Read message from input
            try:
                mbuffer = [self.rfile.read(1)]
                while not mbuffer[0]  == '$':
                    mbuffer = [self.rfile.read(1)]
            except:
                return

            while( not mbuffer[len(mbuffer)-1] == '#'):
                mbuffer.append( self.rfile.read(1) )

            # Parse command
            command = parse_message(''.join(mbuffer))
            command , arguments = self.get_arguments( command )

            # Execute command
            try:
                self.resp_commands[command](self, arguments)
            except SystemExit:
                try:
                    globalServer.server_close()
                except:
                    pass
                import os
                os._exit(0)
            except Exception, e:
                self.wfile.write(encode_string('EE' + repr(e)))


class RespCommandServer:
    """Handle communication using whatever protocol we have defined"""
    def __init__(self,  kernel,  port=2000, daemon=True):
        """Start the socket server, in another thread"""
        global resp_kernel, globalServer
        globalServer = None
        resp_kernel = kernel
        try:
            SocketServer.TCPServer.allow_reuse_address = True
            globalServer = self.tcpserver = SocketServer.TCPServer(('localhost', port), RespRequestHandler)
        except Exception, e:
            import traceback
            traceback.print_exc()
            raise e
        print "Server waiting on port " + str(port)

        if daemon:
            # Start the server thread
            server_thread = Thread( target= self.tcpserver.serve_forever ,  name="ServerThread")
            server_thread.setDaemon(True)
            server_thread.start()

            import atexit
            atexit.register(self.stop)

    def stop(self):
        """Stop the server and kill all connections"""
        try:
            self.tcpserver.server_close()
        except:
            pass


def encode_string(message):
    """Encodes a message in hex + ascii"""
    mbuffer = ["$"]
    for i in range(0, len(message)):
        hexRepr = hex(ord(message[i]))[2:]
        if len(hexRepr) == 1:
            hexRepr = '0' + hexRepr
        mbuffer.append(hexRepr)
    return ''.join(mbuffer)+'#'


def encode_compound(lst):
    """Encodes a complex structure made by integer and long numbers, strings, dictionaries and lists"""
    # If the item is not a composite, we might be at the end of the recursion
    if not type(lst) == dict and not type(lst) == list:
        # If it is a string, return it
        if type(lst)  == str:
            return 'S' + lst + ')'
        # If it is a number (int or long), return it
        elif type(lst) == bool: # pay attention: bool are int: isinstance(True,int) results True !!!!!!
            return 'B' + str(lst) + ')'
        elif type(lst) == int:
            return 'I' + str(lst) + ')'
        elif type(lst) == float:
            return 'F' + str(lst) + ')'
        elif type(lst) == long:
            return 'G' + str(lst) + ')'
        # Else, try to get the contents of the object in a reasonable way
        # repr seems to be the best option, but requires parsing from the remote side
        else:
            return 'S' + repr(lst)+ ')'

    mbuffer = []

    # Dictionaries are handled writing key: value
    if isinstance(lst, dict):
        mbuffer += ['D']
        for i in lst.keys():
            if encode_compound(i).find(':') != -1: raise Exception('Encoding of dictionary keys containing the character \':\' not supported...')
            mbuffer.append(encode_compound(i)+":")
            mbuffer.append(encode_compound(lst[i]))
            mbuffer.append(';')
        if len(lst.keys()) > 0:
            mbuffer.pop()
        mbuffer.append('}')
    # Lists are added as element,element,...
    else:
        mbuffer += ['L']
        for i in lst:
            mbuffer.append(encode_compound(i))
            mbuffer.append(';')
        if len(lst) > 0:
            mbuffer.pop()
        mbuffer.append(']')

    return ''.join(mbuffer)


def find_index(message):
    """Finds the end of the current compound"""
    dicts = 0
    lists = 0
    i = 0
    escape = False
    in_basicType = False

    # For every character in the message
    for c in message:
        # If there is a nested dictionary, count it
        if escape :
            escape = False
            i += 1
            continue
        elif c == '\\': escape = True
        # If we parse a string or a number or an object do not count anything
        elif (c == 'S' or c == 'F' or c == 'I' or c == 'G' or c == 'B') and not in_basicType : in_basicType = True
        # The string or the number or the object is ended, restart counting
        elif c == ')' : in_basicType = False
        elif c == 'D' and not in_basicType: dicts += 1
        # If there is a nested list, count it
        elif c == 'L' and not in_basicType: lists += 1
        # If ] is found, exit from the nested list
        elif c == ']' and lists > 0 and not in_basicType: lists -= 1
        # If ] is found, exit from the nested dictionary
        elif c == '}' and dicts > 0 and not in_basicType: dicts -= 1
        # If we found a delimiter and we are not in a nested list or dict,
        # we have our index
        elif c == ';' and lists == 0 and dicts == 0 and not in_basicType:
            break
        i += 1
    return i


def decode_compound(message):
    """Decodes a complex structure made by integer and long numbers, strings, dictionaries and lists"""
    mbuffer = None
    key = None

    # Get message type
    if ( not message or message == ''): return ''
    mtype = message[0]

    if mtype == 'D':
        mbuffer = dict()
    elif mtype == 'L':
        mbuffer = list()
    elif mtype == 'S':
        return message[1:-1].replace('\\', '')
    elif mtype == 'F':
        return float(message[1:-1].replace('\\', ''))
    elif mtype == 'I':
        return int(message[1:-1].replace('\\', ''))
    elif mtype == 'G':
        return long(message[1:-1].replace('\\', ''))
    elif mtype == 'B':
        if message[1:-1].replace('\\', '') == 'True':
            return True
        else:
            return False
    else:
        return ''

    message = message[1:-1]
    # Repeat until all keys and values have been removed
    while( len(message) > 0 ):

        if( mtype == 'D' ):
            # Extract key from dictionay
            key = decode_compound(message[:message.find(':')])
            # Remove key from message
            message = message[message.find(':')+1:]

        # Find the end of the value content
        index = find_index(message)
        # Encode the value
        value = decode_compound(message[:index])

        if( mtype == 'D') :
            # Add the value to the return dictionary
            mbuffer[key] = value
        else:
            mbuffer.append(value)

        # Remove the value from the message
        message = message[index+1:]
    return mbuffer


def parse_message(message):
    """Parse an incoming message"""

    # Strip the payload delimiters
    message = message[1:-1]

    # Convert numbers to ascii
    buffer = []
    for i in range(0, len(message)-1):
        if( i%2 ): continue
        buffer.append(chr(int(message[i]+message[i+1], 16)))

    return ''.join(buffer)
