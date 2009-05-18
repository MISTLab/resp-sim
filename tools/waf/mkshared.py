#! /usr/bin/env python
# encoding: utf-8

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

import Task,ccroot,Options,Logs
import os,shutil,filecmp

from TaskGen import after, before, taskgen, feature, extension

unpack_ar = 'cd ${SRC[0].bld_dir(env)} && ${AR} -x ${SRC[0].abspath()}' # && rm ${MKSHARED_SUBST}'

@taskgen
@extension(['.a'])
@after('default_cc')
#@feature('shlib')
def do_unpack_ar(self, node):
        unpack_task = self.create_task('unpack_ar', self.env)
        unpack_task.set_inputs(node)
        unpack_task.set_outputs(self.additional_objs)


@taskgen
@feature('shlib')
@before('apply_core')
def parse_libs(self):
    # Copy library and update source list
    files = self.to_list(self.source)
    try:
        filename = files[0]
    except:
        return
    if self.env['LIB_'+filename]:
        baselibname = self.env['LIB_' + filename][0]
        libname =  self.env['staticlib_PATTERN'] % baselibname
        libpath = os.path.join(self.env['LIBPATH_' + filename][0], libname)

        # Copy the systemc library if not present in the source tree
        self.library = os.path.join(self.path.abspath(),libname )
        if (not os.path.isfile( self.library )) or (not filecmp.cmp(self.library, libpath, 0)):
            if Logs.verbose:
                print "copying "+ libpath + ' --> '+ self.path.abspath()
            shutil.copyfile( libpath, self.library )

        # Check the contents
        # UGLY!
        f = os.popen( self.env['AR']+' t '+libpath )
        self.additional_objs = [ self.path.find_or_declare(t.strip()) for t in f ]
        filenames = map( lambda x : os.path.splitext(x)[0] , files[1:])
        self.additional_objs = filter(lambda x: not os.path.splitext(x.name)[0] in filenames , self.additional_objs)

        self.source = ' '.join([libname] + files[1:])

@taskgen
@feature('shlib')
@after('apply_link')
def add_elements(self):
    if not hasattr(self, 'additional_objs'): return
    for obj in self.additional_objs:
        self.link_task.inputs.append(obj)

def setup(env):
    Task.simple_task_type('unpack_ar', unpack_ar, color='BLUE')

def detect(conf):
    conf.check_tool('cxx')
    return 1
