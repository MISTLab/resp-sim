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

""" Module containing some generic helper methods used in the manager classes"""

class NotAPort(Exception):
    """Exception indicating that the current element doesn't represent
    a tlm port"""
    def __init__(self, value):
        self.value = value
    def __repr__(self):
        return repr(self.value)
    def __str__(self):
        return repr(self)
    
class BaseNotFound(Exception):
    """Exception indicating that a determined base class was not found"""
    def __init__(self, value):
        self.value = value
    def __repr__(self):
        return repr(self.value)
    def __str__(self):
        return repr(self)

class ComponentNotFound(Exception):
    """Exception indicating that a component wasn't found"""
    def __init__(self, value):
        self.value = value
    def __repr__(self):
        return repr(self.value)
    def __str__(self):
        return repr(self)

class NotSystemCPort(Exception):
    """Exception indicating that a port is not """
    def __init__(self, value):
        self.value = value
    def __repr__(self):
        return repr(self.value)
    def __str__(self):
        return repr(self)
