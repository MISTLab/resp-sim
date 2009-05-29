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


"Make a shared library from a static one"

import Task, ccroot, Options, Logs
import os, shutil, filecmp

from TaskGen import after, before, taskgen, feature, extension

unpack_ar = 'cd ${SRC[0].bld_dir(env)} && ${AR} -x ${SRC[0].abspath()}' # && rm ${MKSHARED_SUBST}'

@taskgen
@extension(['.a'])
@after('default_cc')
def do_unpack_ar(self, node):
        unpack_task = self.create_task('unpack_ar', self.env)
        unpack_task.set_inputs(node)
        unpack_task.set_outputs(self.lib_obj[node.name])

@taskgen
@feature('cxx')
@feature('cc')
@before('sequence_order')
def copy_libs(self):
    # Copy library and update source list
    files = self.to_list(self.source)

    for filename in files:
        if self.env['LIB_'+filename]:

            if type(self.env['LIB_'+filename]) == type(''):
                self.env['LIB_'+filename] = [self.env['LIB_'+filename]]
            if type(self.env['LIBPATH_'+filename]) == type(''):
                self.env['LIBPATH_'+filename] = [self.env['LIBPATH_'+filename]]

            for baselibname in self.env['LIB_' + filename]:
                # I make sure that the library actually exists and I determine its path
                libname =  self.env['staticlib_PATTERN'] % baselibname
                for path in self.env['LIBPATH_' + filename]:
                    libpath = os.path.join(path, libname)
                    if os.path.exists(libpath):
                        break

                if not os.path.exists(libpath):
                    continue

                # Copy the library if not present in the source tree
                self.library = os.path.join(self.path.abspath(), libname)
                if (not os.path.isfile( self.library )) or (not filecmp.cmp(self.library, libpath, 0)):
                    if Logs.verbose:
                        print('copying ' + libpath + ' --> '+ self.path.abspath())
                    shutil.copyfile( libpath, self.library )

@taskgen
@feature('cxx')
@feature('cc')
@before('apply_core')
@after('copy_libs')
def parse_libs(self):
    # Copy library and update source list
    files = self.to_list(self.source)

    self.source = ''

    for filename in files:
        if self.env['LIB_'+filename]:

            if type(self.env['LIB_'+filename]) == type(''):
                self.env['LIB_'+filename] = [self.env['LIB_'+filename]]

            for baselibname in self.env['LIB_' + filename]:
                # I make sure that the library actually exists and I determine its path
                libname =  self.env['staticlib_PATTERN'] % baselibname
                if not os.path.exists(os.path.join(self.path.abspath(), libname)):
                    continue

                # Check the contents
                # UGLY!
                try:
                    if type(self.additional_objs) == type(''):
                        self.additional_objs = self.additional_objs.split(' ')
                except AttributeError:
                    self.additional_objs = []

                f = os.popen( self.env['AR'] + ' t ' + os.path.join(self.path.abspath(), libname) )
                newObj = [ self.path.find_or_declare(t.strip()) for t in f ]
                try:
                    self.lib_obj
                except AttributeError:
                    self.lib_obj = {}
                self.lib_obj[libname] = newObj
                filenames = map( lambda x : os.path.splitext(x)[0] , files)
                newObj = filter(lambda x: not os.path.splitext(x.name)[0] in filenames , newObj)
                newObj = filter(lambda x: not x in self.additional_objs , newObj)
                self.additional_objs += newObj

                self.source += ' ' + libname
        else:
            self.source += ' ' + filename

@taskgen
@feature('cshlib')
@after('apply_link')
def add_elements(self):
    if not hasattr(self, 'additional_objs'):
        return
    for obj in self.additional_objs:
        self.link_task.inputs.append(obj)

def setup(env):
    Task.simple_task_type('unpack_ar', unpack_ar, color='BLUE')

def detect(conf):
    conf.check_tool('cxx')
    conf.check_tool('cc')
    conf.check_tool('ar')
