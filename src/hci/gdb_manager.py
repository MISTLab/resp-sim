#!/usr/bin/env python

import subprocess

class GDBManager():
    """ Opens and controls a GDB instance on a given process """

    def __init__(self, target):
        self.gdb = None
        self.target = target

    def start_gdb(self):
        """ Open a GDB subprocess, controlled via pipes """
        self.gdb = subprocess.Popen( ['gdb', '--interpreter=mi' , '--quiet'], stdin=subprocess.PIPE, stdout=subprocess.PIPE )

    def read_gdb_output(self):
        while( self.gdb.stdout.read(1) != '\n' ):
            pass

    def set_watchpoint(self, var):
        """ Set a hardware breakpoint/watchpoint on the simulation """
        pass

    def set_condition(self, var, comparator):
        pass

    def continue(self):
        """ Send a continue command """
        pass