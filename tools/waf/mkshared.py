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
    # Copy library
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
                sharedlibname =  self.env['shlib_PATTERN'] % baselibname
                foundStatic = False
                foundShared = False
                for path in self.env['LIBPATH_' + filename]:
                    libpath = os.path.join(path, libname)
                    if os.path.exists(libpath):
                        foundStatic = True
                        break

                for path in self.env['LIBPATH_' + filename]:
                    sharedlibpath = os.path.join(path, sharedlibname)
                    if os.path.exists(sharedlibpath):
                        foundShared = True
                        break

                if not foundStatic:
                    continue
                if foundShared:
                    self.uselib += ' ' + filename
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
    conf.find_program('objdump', mandatory=1)

def check_dyn_library(conf, libfile, libpaths):
    if not conf.env['AR']:
        conf.fatal('Please configure the mkshared tool before calling the check_dyn_library method')

    # Now I have to check that the object files in the library are compiled with -fPIC option.
    # actually this only seems to affect 64 bits systems, while compilation on 32 bits only
    # yields warnings, but perfectly usable libraries. So ... I simply have to check if we are on
    # 64 bits systems and in case, I issue the objdump -r command: if there are symbols of type
    # R_X86_64_32S, then the library was compiled without the -fPIC option and it is not possible
    # to create a shared library out of it.

    # Are we on 64 bit systems?
    import struct
    if struct.calcsize("P") > 4:
        # are we actually processing a shared library?
        if os.path.splitext(libfile)[1] == conf.env['staticlib_PATTERN'].split('%s')[1]:
            # Now lets check for the presence of symbols of type R_X86_64_32S:
            # in case we have an error.
            for libpath in libpaths:
                if os.path.exists(os.path.join(libfile, libpath)):
                    print os.path.join(libfile, libpath)
                    libDump = os.popen('objdump -r ' + os.path.join(libfile, libpath)).readlines()
                    for line in libDump:
                        if 'R_X86_64_32S' in line:
                            return False
                    break
    return True
