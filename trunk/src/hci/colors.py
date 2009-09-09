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

class ColorDict(dict):
    def __init__(self):
        dict.__init__(self)
    def __getitem__(self, key):
        val = self.get(key)
        if val:
            return val
        return ''

colorMap = ColorDict()

def is_color_enabled():
    """It states wheter colors are enabled or not"""
    return (len(colorMap)>0)

def set_colors(enable=True):
    """ It enables/disables colors in console printing"""

    if enable:
        colorMap['black'] = """\x1b[30m"""
        colorMap['blue'] = """\x1b[34m"""
        colorMap['bright_blue'] = """\x1b[34;1m"""
        colorMap['bright_cyan'] = """\x1b[36;1m"""
        colorMap['bright_green'] = """\x1b[32;1m"""
        colorMap['bright_red'] = """\x1b[31;1m"""
        colorMap['bright_brown'] = """\x1b[33;1m"""
        colorMap['brown'] = """\x1b[33m"""
        colorMap['cyan'] = """\x1b[36m"""
        colorMap['dark_gray'] = """\x1b[30;1m"""
        colorMap['green'] = """\x1b[32m"""
        colorMap['light_gray'] = """\x1b[37m"""
        colorMap['magenta'] = """\x1b[35m"""
        colorMap['none'] = """\x1b[0m"""
        colorMap['purple'] = """\x1b[35;1m"""
        colorMap['red'] = """\x1b[31m"""
        colorMap['white'] = """\x1b[37;1m"""
        colorMap['yellow'] = """\x1b[33;1m"""
    else:
        colorMap.clear()

def disable_colors():
    """ It disables colors in console printing"""
    set_colors(False)

def enable_colors():
    """ It enables colors in console printing"""
    set_colors(True)
