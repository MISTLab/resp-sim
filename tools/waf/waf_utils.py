# -*- coding: iso-8859-1 -*-

import os, types, sys, glob,  stat, Options,Logs

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
        os.remove(os.path.join(task.env['RESP_HOME'], 'startSim.sh'))
    except Exception, e:
        if Logs.verbose:
            print 'Error in removing file startSim.sh --> ' + str(e)

    sys.path.append(os.path.join(task.env['RESP_HOME'], 'src', 'hci'))
    sys.path.append(os.path.join(task.env['RESP_HOME'], 'src'))
    import console

    startFile = open(os.path.join(task.env['RESP_HOME'], 'startSim.sh'), 'w')
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

    print >> startFile,  'CUR_LIB_PATH=' + os.path.join(task.env['RESP_HOME'], '_build_', 'default', 'lib', 'systemc')
    print >> startFile,  'CUR_LIB_PATH=$CUR_LIB_PATH:' + os.path.join(task.env['RESP_HOME'], '_build_', 'default', 'src', 'bfdFrontend')
    print >> startFile,  'CUR_LIB_PATH=$CUR_LIB_PATH:' + os.path.join(task.env['RESP_HOME'], '_build_', 'default', 'src', 'controller')
    print >> startFile,  'CUR_LIB_PATH=$CUR_LIB_PATH:' + os.path.join(task.env['RESP_HOME'], '_build_', 'default', 'src', 'utils')
    for extLib in task.env['NON_STD_LIBS']:
        print >> startFile,  'CUR_LIB_PATH=$CUR_LIB_PATH:' + extLib
    print >> startFile,  'export LD_LIBRARY_PATH=$CUR_LIB_PATH:$LD_LIBRARY_PATH'
    print >> startFile,  'export DYLD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DYLD_LIBRARY_PATH\n'
    print >> startFile,  'case $TERM in'
    print >> startFile,  'xterm*|rxvt*|Eterm)'
    print >> startFile,  """echo -ne \"\\033]0;ReSP: Reflective Simulation Platform\\007\""""
    print >> startFile,  ';;'
    print >> startFile,  'screen)'
    print >> startFile,  """echo -ne \"\\033_ReSP: Reflective Simulation Platform\\033\""""
    print >> startFile,  ';;'
    print >> startFile,  'esac'
    print >> startFile,  '\n[ -d ' + os.path.join(task.env['RESP_HOME'], '_build_', 'default') + ' ] && (' + os.path.join(task.env['RESP_HOME'], 'src','resp.py') + ' $* || (reset && echo \"A disruptive error has occurred during the execution of ReSP\")) || (echo \"First please compile ReSP, then you can try to execute it :-)\" && exit)'
    print >> startFile,  'echo \"\"'

    startFile.close()

    os.chmod(os.path.join(task.env['RESP_HOME'], 'startSim.sh'), stat.S_IXOTH | stat.S_IXGRP | stat.S_IXUSR | os.stat(os.path.join(task.env['RESP_HOME'], 'startSim.sh'))[stat.ST_MODE])

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
