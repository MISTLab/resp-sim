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


import socket
import server


class RespClient:
    """ General class to ease the connection to the ReSP command server """

    def __init__(self, servername = 'localhost', port = 2200 ):
        # Create a client
        self.serverHost = servername            # servername is localhost
        self.serverPort = port             # use arbitrary port > 1024
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)    # create a TCP socket

        self.s.connect((self.serverHost, self.serverPort)) # connect to server on the port
        #self.s.settimeout(1)

    def shutdown(self,  string):
        self.s.shutdown()

    def send(self,  string):
        packet = server.encode_string(string)
        self.s.send(packet)

    def receive(self):
        # Receive packet
        buffer = []
        c = ''
        while( c != '#'):
            try:
                c = self.s.recv(1)
            except Exception,  e:
                pass
            buffer.append(c)
        return ''.join(buffer)

    def quit(self):
        self.send('QUIT')
        msg = server.parse_message(self.receive())
        self.s.close()
        return msg

    def list_components(self):
        # LISTCOMPONENTS
        self.send('LISTCOMP')
        return server.parse_message(self.receive())

    def dir_element(self, item):
        # DIR
        self.send('DIR-'+server.encode_compound(item))
        stuff = server.parse_message(self.receive())
        return server.decode_compound(stuff[2:])

    def create_component(self, typename, name, params ):
        # CREATE
        self.send("CREATE-"+server.encode_compound([typename, name]+params))
        message = server.parse_message(self.receive())
        if message != 'OK': raise Exception('Failed to create object: '+message)

    def connect_tlm(self, item1, port1, item2, port2):
        # CONNECTTLM
        self.send('CONNECTTLM-'+server.encode_compound([item1, port1 , item2, port2]))
        message = server.parse_message(self.receive())
        if message != 'OK': raise Exception('Failed to connect objects: '+message)

    def connect_sysc(self, item1, port1, item2, port2):
        # CONNECTTLM
        self.send('CONNECTSYSC-'+server.encode_compound([item1, port1 , item2, port2]))
        message = server.parse_message(self.receive())
        if message != 'OK': raise Exception('Failed to connect objects: '+message)

    def connect_sig(self, item1, port1, item2, port2):
        # CONNECTTLM
        self.send('CONNECTSIG-'+server.encode_compound([item1, port1 , item2, port2]))
        message = server.parse_message(self.receive())
        if message != 'OK': raise Exception('Failed to connect objects: '+message)

    def load_application(self, app):
        # LoadApp
        self.send("LOADAPP-"+server.encode_compound([app]))
        message = server.parse_message(self.receive())
        if message != 'OK': raise Exceotuib('Failed to load application: '+message)

    def load_architecture(self, arch):
        # LOADARCH
        self.send('LOADARCH-S'+arch)
        message = server.parse_message(self.receive())
        print message
        if message != 'OK': raise Exception('Failed to load architecture: '+message)

    def run_simulation(self, time = -1):
        # RUN
        self.send('RUN-S'+str(time))
        if not server.parse_message(self.receive()) == 'OK':
            raise Exception('Error running the RUN command')

    def stop_simulation(self):
        self.send('STOP')
        message = server.parse_message(self.receive())
        if not server.parse_message(self.receive()) == 'OK':
            raise Exception('Error running the STOP command')

    def get_times(self):
        # GETTIMES
        self.send('GETTIMES')
        sim_time = server.parse_message(self.receive())
        return float(sim_time[2:])

    def get_timer(self):
        # GETTIMER
        self.send('GETTIMER')
        real_time = server.parse_message(self.receive())
        return float(real_time[2:])

    def execute(self, string):
        # EXEC
        self.send('EXEC-'+server.encode_compound(string))
        response = server.parse_message(self.receive())
        if not response[0:2] == 'OK':
            raise Exception(response[2:])
        return response[2:]