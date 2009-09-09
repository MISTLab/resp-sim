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


import os, types, sys, glob,  stat, Options, Logs, ccroot
from TaskGen import taskgen, feature, before, after, extension

# Automatically add rpath for local dynamic uselib_local
@after('add_extra_flags')
@before('apply_obj_vars')
@feature('cc', 'cxx')
def add_rpaths(self):
    self.env[ccroot.c_attrs['rpath']] = self.to_list(self.env[ccroot.c_attrs['rpath']])

    try:
        self.uselib_local
    except:
        return

    names = self.to_list(self.uselib_local)
    while names:
        x = names.pop(0)
        y = self.name_to_obj(x)

        if os.path.splitext(ccroot.get_target_name(y))[1] != y.env['shlib_PATTERN'].split('%s')[1]:
            continue
        y.post()

        newPath = os.path.split(y.link_task.outputs[0].abspath(self.env))[0]
        if not newPath in self.env[ccroot.c_attrs['rpath']]:
            self.env[ccroot.c_attrs['rpath']].append(newPath)
        for k in self.to_list(y.env[ccroot.c_attrs['rpath']]):
            if not k in self.env[ccroot.c_attrs['rpath']]:
                self.env[ccroot.c_attrs['rpath']].append(k)

    if '' in self.env[ccroot.c_attrs['rpath']] and len(self.env[ccroot.c_attrs['rpath']]) > 1:
        self.env[ccroot.c_attrs['rpath']].remove('')

def find_files(paths, pattern, toRemove = []):
    for path in paths:
        toExamine = []
        toRemove += glob.glob(os.path.abspath(os.path.join(path, pattern)))
        for dirContent in os.listdir(path):
            if os.path.isdir(os.path.join(path, dirContent)):
                toExamine.append(os.path.join(path, dirContent))
        find_files(toExamine, pattern, toRemove)

def create_startSim(task):
    try:
        os.remove(os.path.join(task.bld.path.abspath(), 'startSim.sh'))
    except Exception, e:
        if Logs.verbose:
            print 'Error in removing file startSim.sh --> ' + str(e)

    sys.path.append(os.path.join(task.bld.path.abspath(), 'src', 'hci'))
    sys.path.append(os.path.join(task.bld.path.abspath(), 'src'))
    import console

    startFile = open(os.path.join(task.bld.path.abspath(), 'startSim.sh'), 'w')
    print >> startFile,  """#!/bin/bash"""
    print >> startFile,  '\n\n\n'
    print >> startFile,  """################################################################################"""
    print >> startFile,  '#\n#\n#'
    for line in console.banner_str.split('\n'):
        print >> startFile, """#  """ + line
    print >> startFile,  '#\n#\n#'
    for line in console.version_str.split('\n'):
        print >> startFile, """#  """ + line
    print >> startFile,  '#\n#\n#'
    for line in console.license_str.split('\n'):
        print >> startFile, """#  """ + line
    print >> startFile,  '#\n#\n#'
    print >> startFile,  """################################################################################"""
    print >> startFile,  '\n\n\n'

    if sys.platform == 'darwin':
        # In case we are under the mac-osx operating system we need
        # to add the dynamic link paths since they are not automatically
        # determined thorugh the rpath compiler directive
        for path in task.env['RPATH']:
            print >> startFile,  'CUR_LIB_PATH=$CUR_LIB_PATH:' + path
        print >> startFile,  'export DYLD_LIBRARY_PATH=$CUR_LIB_PATH:$DYLD_LIBRARY_PATH'

    print >> startFile,  'case $TERM in'
    print >> startFile,  'xterm*|rxvt*|Eterm)'
    print >> startFile,  """echo -ne \"\\033]0;ReSP: Reflective Simulation Platform\\007\""""
    print >> startFile,  ';;'
    print >> startFile,  'screen)'
    print >> startFile,  """echo -ne \"\\033_ReSP: Reflective Simulation Platform\\033\""""
    print >> startFile,  ';;'
    print >> startFile,  'esac'
    print >> startFile,  '\n[ -d ' + os.path.join(task.bld.bdir, 'default') + ' ] && (' + os.path.join(task.bld.path.abspath(), 'src','resp.py') + ' $* || (reset && echo \"A disruptive error has occurred during the execution of ReSP\")) || (echo \"First please compile ReSP, then you can try to execute it :-)\" && exit)'
    print >> startFile,  'echo \"\"'

    startFile.close()

    os.chmod(os.path.join(task.bld.path.abspath(), 'startSim.sh'), stat.S_IXOTH | stat.S_IXGRP | stat.S_IXUSR | os.stat(os.path.join(task.bld.path.abspath(), 'startSim.sh'))[stat.ST_MODE])

def rec_find(curPath,  exclude,  headerExt):
    global rec_find

    """Recursively examines all the folders inside ArchC looking for include
    files and copies them to dest"""
    import os
    directories = []
    headers= []
    directories.append(curPath)

    # First of all I examine all the files inside the current directory
    dirContent = os.listdir(curPath)
    for element in dirContent:
        if os.path.isfile(curPath + os.sep + element):
            if os.path.splitext(element)[1] in headerExt:
                headers.append(os.path.abspath(curPath + os.sep + element))

    # Then I move to recirsively examine subdirectories
    for element in dirContent:
        next = curPath + os.sep + element
        #Now I have to see if the current element has to be excluded
        toExclude = False
        for exclDirs in exclude:
            if next.find(exclDirs) != -1:
                toExclude = True
                break;
        if os.path.isdir(next) and not (next).endswith('.svn') and not toExclude:
            d , h= rec_find(next, exclude,  headerExt)
            directories += d
            headers += h

    return directories, headers
