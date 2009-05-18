#! /usr/bin/env python
# -*- coding: iso-8859-1 -*-
# encoding: utf-8

# ReSP Build file

# these variables are mandatory ('/' are converted automatically)
srcdir = '.'
blddir = '_build_'

# make sure waf has the version we want
import Build,Utils,Options,TaskGen,Configure,Logs
from Tools import cc
from Tools import cxx
import os, types, sys, copy,  stat

## Force autoreconfiguration when part of the configuration wscripts change
#Configure.autoconfig=1

# Build methods

def build(bld):
    """
    All building is delegated to subdirectories.
    """
    sys.path.append( os.path.join('tools','waf') )
    from waf_utils import clear_database, create_startSim

    # Now I have to correct the extensions which are accepted by the tools since they forgot
    # assembly files
    TaskGen.declare_extension(['.S','.s'], cc.c_hook)

    # process subfolders from here
    bld.add_subdirs('lib component test tools src software')
    if os.path.exists(os.path.join('other','xynoc')):
        bld.add_subdirs(os.path.join('other','xynoc'))
    bld.add_subdirs(os.path.join('dse', 'MDP'))
    if bld.env['MOMH_FOUND']:
        bld.add_subdirs(os.path.join('dse', 'MOMH'))

    #Check if MySQL database has to be cleared
    if Options.options.clearMySql:
        bld.add_group()
        obj = bld.new_task_gen('cmd')
        obj.fun = clear_database

    # Builds Doxygen Documentation
#    if bld.env['DOXYGEN'] != '' and not bld.env['STATIC_PLATFORM']:
#        if Options.options.rebuild_doc or not os.path.exists(os.path.join('_build_' , 'default' , 'documentation')):
#            bld.add_group()
#            obj = bld.new_task_gen('cmd')
#            obj.fun = doxygen_create

    # Creates the startSim script
    if not bld.env['STATIC_PLATFORM']:
        bld.add_group()
        obj = bld.new_task_gen('cmd')
        obj.fun = create_startSim

    # builds the static platform: if has to compile the main filed passed on the command line and also
    # all the libraries
    if bld.env['STATIC_PLATFORM']:
        obj = bld.new_task_gen('cxx', 'program')
        obj.env['FULLSTATIC'] = True
        obj.env.append_unique('LINKFLAGS', '-static')
        obj.source = bld.env['STATIC_MAIN']
        obj.uselib='SYSTEMC BOOST_ASIO BOOST BOOST_PROGRAM_OPTIONS BOOST_REGEX BOOST_SYSTEM BOOST_THREAD OPCODES_CUSTOM BFD_CUSTOM LIBERTY_CUSTOM'
#MATLAB
        if Options.options.matlabdir:
            obj.uselib += ' MATLAB'
        if '-DENABLE_FPU' in bld.env['CXXFLAGS']:
            obj.uselib += ' MPFRCPP MPFR GMPXX GMP'
        obj.uselib_local='ExecLoader Profiler GDBProcStub32 sc_controller'
        if bld.env['ENABLE_PROCESSORS'] == []:
            obj.uselib_local += ' arm7 arm7ca mips1 powerpc ppc405 leon2 '
        else:
            if 'arm7' in bld.env['ENABLE_PROCESSORS']:
                    obj.uselib_local += ' arm7 '
            if 'leon2' in bld.env['ENABLE_PROCESSORS']:
                    obj.uselib_local += ' leon2 '
            if 'arm7ca' in bld.env['ENABLE_PROCESSORS']:
                    obj.uselib_local += ' arm7ca '
            if 'powerpc' in bld.env['ENABLE_PROCESSORS']:
                    obj.uselib_local += ' powerpc '
            if 'ppc405' in bld.env['ENABLE_PROCESSORS']:
                    obj.uselib_local += ' ppc405 '
            if 'mips1' in bld.env['ENABLE_PROCESSORS']:
                    obj.uselib_local += ' mips1 '
        obj.uselib_local += 'SimpleMaster SimpleMasterSysc SimpleSlave SimpleSlaveSysc commIf SynchManager ProcIf BFDFrontend GDBStub32'
        obj.includes='lib/archc/ac_includes src/execLoader src/profiler src/softwareDebug src/softwareDebug/procStubs src/systempy src/mysql component/interconnect/simplebus component/memory/caches component/memory/simple component/memory/dbgMemory component/misc/simpleMaster component/misc/simpleSlave component/misc/synchronization component/processor/arm7tdmi/functional component/processor/arm7tdmi/cycleAcc component/processor/leon2/functional component/processor/powerpc/functional component/processor/powerpc/cycleAcc component/processor/mips/functional component/misc/uart/pc16x5x component/misc/led component/misc/timer/at697 component/misc/timer/armpid7 component/misc/IntrController/armpid7 component/misc/IntrController/armpid7-mp component/misc/uart/ src/SynchManager/ src/SynchManager/ProcStubs lib/binutils-2.18/build/include'
        if bld.env['ENABLE_MYSQL']:
            obj.uselib_local += ' MySQL'
        obj.name = os.path.splitext(os.path.basename(bld.env['STATIC_MAIN']))[0]
        obj.target = obj.name
        obj.env.append_unique('LINKFLAGS', '-Wl,-Bstatic')


def package(bld):
    """
    There we are supposed to build ReSP binary package, to be implemented
    """
    pass

########################################
# Provide initial configuration
########################################
def configure(conf):
    """
    Process ReSP initial configuration: looks for SystemC, gcc, etc.
    """

    ########################################
    # Check for standard tools
    ########################################
    conf.check_tool('gcc g++')
    conf.check_tool('flex bison misc')

    ##################################################################
    # Correcting custom environment vars to lists as required by waf
    ##################################################################
    if type(conf.env['CCFLAGS']) == type(''):
        conf.env['CCFLAGS'] = conf.env['CCFLAGS'].split(' ')
    if type(conf.env['CXXFLAGS']) == type(''):
        conf.env['CXXFLAGS'] = conf.env['CXXFLAGS'].split(' ')
    if type(conf.env['CPPFLAGS']) == type(''):
        conf.env['CPPFLAGS'] = conf.env['CPPFLAGS'].split(' ')
    if type(conf.env['LINKFLAGS']) == type(''):
        conf.env['LINKFLAGS'] = conf.env['LINKFLAGS'].split(' ')

    ########################################
    # Check for special gcc flags
    ########################################
    if not '-g' in conf.env['CCFLAGS'] and not '-g3' in conf.env['CCFLAGS']:
        conf.env.append_unique('CCFLAGS', '-O2 -DNDEBUG -g')
    if not '-g' in conf.env['CXXFLAGS'] and not '-g3' in conf.env['CXXFLAGS']:
        conf.env.append_unique('CXXFLAGS', '-O2 -DNDEBUG -g')
    if conf.env['CPPFLAGS']:
        conf.check_cc(cflags=conf.env['CPPFLAGS'])
    if conf.env['CCFLAGS']:
        conf.check_cc(cflags=conf.env['CCFLAGS'])
    if conf.env['CXXFLAGS']:
        conf.check_cxx(cxxflags=conf.env['CXXFLAGS'])
    if conf.env['LINKFLAGS']:
        conf.check_cxx(linkflags=conf.env['LINKFLAGS'])

    ##################################################
    # Check for boost libraries
    ##################################################
    boostlibs = 'thread regex date_time program_options filesystem system'
    if conf.env['STATIC_PLATFORM'] != 1:
        boostlibs += ' python unit_test_framework'

    conf.check_tool('boost')
    conf.check_boost(lib=boostlibs, static='both', min_version='1.35.0', mandatory = 1, errmsg = 'Unable to find ' + boostlibs + ' boost libraries, please install them and specify their location with the --boost-includes and --boost-libs configuration options')

    conf.env['RESP_HOME'] = os.path.abspath(os.path.dirname(sys.modules['__main__'].__file__))

    defaultFlags = ['-fstrict-aliasing']
    if not Options.options.static_plat:
        defaultFlags += ['-fPIC']
        conf.env.append_unique('LINKFLAGS','-fPIC' )
        if sys.platform != 'darwin':
            conf.env.append_unique('LINKFLAGS','-Wl,-E')

    conf.env.append_unique('CXXFLAGS',defaultFlags)
    conf.env.append_unique('CFLAGS',defaultFlags)
    conf.env.append_unique('CCFLAGS',defaultFlags)
    conf.env.append_unique('CPPFLAGS','-DPIC')
    conf.env.append_unique('ARFLAGS','rc')

    if sys.platform == 'darwin':
        conf.env.append_unique('shlib_LINKFLAGS', ['-undefined suppress', '-flat_namespace'] )

    ########################################
    # Now I eliminate the -DDEBUG compilation flag, it creates problems with the TLM library
    # TODO: use filter
    ########################################
    #if '-DDEBUG' in conf.env['CCFLAGS']:
        #conf.env['CCFLAGS'].remove('-DDEBUG')
    #if '-DDEBUG' in conf.env['CFLAGS']:
        #conf.env['CFLAGS'].remove('-DDEBUG')
    #if '-DDEBUG' in conf.env['CXXFLAGS']:
        #conf.env['CXXFLAGS'].remove('-DDEBUG')
    #if '-DDEBUG' in conf.env['CPPFLAGS']:
        #conf.env['CPPFLAGS'].remove('-DDEBUG')

    ########################################
    # Now I permanently save some compilation options specified at configure time
    ########################################
    conf.env['NON_STD_LIBS'] = []
    conf.env['ENABLE_GUI'] = getattr(Options.options, 'enable_gui', 1)
    conf.env['ENABLE_DEBUG'] = getattr(Options.options, 'enable_debug', 1)
    conf.env['ENABLE_PROFILE'] = getattr(Options.options, 'enable_profile', 1)
    conf.env['ENABLE_OS_EMU'] = getattr(Options.options, 'enable_os_emu', 1)
    conf.env['ENABLE_TRACE'] = getattr(Options.options, 'enable_tracing', 1)
    conf.env['ENABLE_HISTORY'] = getattr(Options.options, 'enable_history', 1)
    conf.env['MOM_HEADER'] = getattr(Options.options, 'momh_header', '')
    conf.env['MOM_LIBS'] = getattr(Options.options, 'momh_libs', '')
    if conf.env['MOM_LIBS']:
        conf.env['NON_STD_LIBS'].append(os.path.normpath(os.path.expandvars(os.path.expanduser(conf.env['MOM_LIBS']))))
    conf.env['GMP_HEADER'] = getattr(Options.options, 'gmp_header', '')
    conf.env['GMP_LIBS'] = getattr(Options.options, 'gmp_libs', '')
    if conf.env['GMP_LIBS']:
        conf.env['NON_STD_LIBS'].append(os.path.normpath(os.path.expandvars(os.path.expanduser(conf.env['GMP_LIBS']))))
    conf.env['MPFR_HEADER'] = getattr(Options.options, 'mpfr_header', '')
    conf.env['MPFR_LIBS'] = getattr(Options.options, 'mpfr_libs', '')
    if conf.env['MPFR_LIBS']:
        conf.env['NON_STD_LIBS'].append(os.path.normpath(os.path.expandvars(os.path.expanduser(conf.env['MPFR_LIBS']))))
#MATLAB
    if Options.options.matlabdir:
        conf.env['NON_STD_LIBS'].append(os.path.abspath(os.path.join(os.path.normpath(os.path.expandvars(os.path.expanduser(Options.options.matlabdir))), 'bin','glnx86')))

    conf.env['MPFRCPP_HEADER'] = getattr(Options.options, 'mpfrcpp_header', '')
    conf.env['MPFRCPP_LIBS'] = getattr(Options.options, 'mpfrcpp_libs', '')
    if conf.env['MPFRCPP_LIBS']:
        conf.env['NON_STD_LIBS'].append(os.path.normpath(os.path.expandvars(os.path.expanduser(conf.env['MPFRCPP_LIBS']))))
    if getattr(Options.options, 'boostlibs', ''):
        conf.env['NON_STD_LIBS'].append(os.path.normpath(os.path.expandvars(os.path.expanduser(getattr(Options.options, 'boostlibs', '')))))
    if Options.options.en_processors:
        conf.env['ENABLE_PROCESSORS'] = Options.options.en_processors.split()
    if Options.options.static_plat:
        conf.env['STATIC_PLATFORM'] = 1
        conf.env.append_unique('CPPFLAGS','-DSTATIC_PLATFORM')

        if os.path.exists(Options.options.static_plat):
            commonPath = os.path.commonprefix([os.path.abspath(Options.options.static_plat), conf.env['RESP_HOME']])
            purgedPath = os.path.abspath(Options.options.static_plat).replace(commonPath, '')
            if purgedPath.find(os.sep) == 0:
                purgedPath = purgedPath[1:]
            purgedReSPPath = conf.env['RESP_HOME'].replace(commonPath, '')
            if purgedReSPPath.find(os.sep) == 0:
                purgedReSPPath = purgedReSPPath[1:]
            if purgedReSPPath != '':
                Logs.error("please specify a path inside the main ReSP folder for file " + Options.options.static_plat)
            conf.env['STATIC_MAIN'] = purgedPath
        else:
            Logs.error("please specify an existing path inside the main ReSP folder for file " + Options.options.static_plat)

    if Options.options.enable_debug:
        conf.env.append_unique('CPPFLAGS','-DSW_DEBUG')
    if Options.options.enable_profile or Options.options.enable_history:
        conf.env.append_unique('CPPFLAGS','-DSW_PROFILE')
    if Options.options.enable_os_emu:
        conf.env.append_unique('CPPFLAGS','-DENABLE_OS_EMU')
    if Options.options.enable_tracing:
        conf.env.append_unique('CPPFLAGS','-DSW_TRACE')
    if Options.options.enable_history:
        conf.env.append_unique('CPPFLAGS','-DGBL_HISTORY')

    ########################################
    # Setting the host endianess
    ########################################
    if sys.byteorder == "little":
        conf.env.append_unique('CPPFLAGS','-DLITTLE_ENDIAN_BO')
        conf.check_message_custom('endianness', '', 'little')
    else:
        conf.env.append_unique('CPPFLAGS','-DBIG_ENDIAN_BO')
        conf.check_message_custom('endianness', '', 'big')

    ########################################
    # Check for python
    ########################################
    conf.check_tool('python')
    conf.check_python_version((2,4))
    if conf.env['STATIC_PLATFORM'] != 1:
        conf.check_python_headers()

    ########################################
    # Check for special tools
    ########################################
    if conf.env['STATIC_PLATFORM'] != 1:
        conf.check_tool('py++', os.path.join( '.' , 'tools' , 'waf'))
    conf.check_tool('mkshared mysql ecos', os.path.join( '.' , 'tools' , 'waf'))

    ##################################################
    # Check for standard libraries
    ##################################################
    conf.check_cc(lib='m', uselib_store='MATH', mandatory=1)
    conf.check_cc(lib='dl', uselib_store='DL', mandatory=1)

    ##################################################
    # Check for pthread library/flag
    ##################################################
    if conf.check_cxx(linkflags='-pthread') is None or conf.check_cxx(cxxflags='-pthread') is None:
        conf.env.append_unique('LIB', 'pthread')
    else:
        conf.env.append_unique('LINKFLAGS', '-pthread')
        conf.env.append_unique('CXXFLAGS', '-pthread')
        conf.env.append_unique('CFLAGS', '-pthread')
        conf.env.append_unique('CCFLAGS', '-pthread')
        pthread_uselib = []

    ##################################################
    # Is SystemC compiled? Check for SystemC library
    # Notice that we can't rely on lib-linux, therefore I have to find the actual platform...
    ##################################################
    # First I set the clafgs needed by TLM 2.0 for including systemc dynamic process
    # creation
    #conf.env.append_unique('CPPFLAGS','-DSC_INCLUDE_DYNAMIC_PROCESSES')
    syscpath = None
    if Options.options.systemcdir:
        syscpath = ([os.path.abspath(os.path.join(Options.options.systemcdir, 'include'))])
    elif 'SYSTEMC' in os.environ:
        syscpath = ([os.path.abspath(os.path.join(os.environ['SYSTEMC'], 'include'))])

    import glob
    sysclib = ''
    if syscpath:
        sysclib = glob.glob(os.path.join(os.path.abspath(os.path.join(syscpath[0], '..')), 'lib-*'))
    conf.check_cxx(lib='systemc', uselib_store='SYSTEMC', mandatory=1, libpath=sysclib)

    ######################################################
    # Check if systemc is compiled with quick threads or not
    ######################################################
    if not os.path.exists(os.path.join(syscpath[0] , 'sysc' , 'qt')):
        conf.env.append_unique( 'CPPFLAGS','-DSC_USE_PTHREADS')

    ##################################################
    # Check for SystemC header and test the library
    ##################################################
    conf.check_cxx(header_name='systemc.h', uselib='SYSTEMC', uselib_store='SYSTEMC_H', mandatory=1, includes=syscpath)
    conf.check_cxx(fragment='''
        #include <systemc.h>

        #ifndef SYSTEMC_VERSION
        #error SYSTEMC_VERSION not defined in file sc_ver.h
        #endif

        #if SYSTEMC_VERSION < 20070314
        #error Wrong SystemC version
        #endif

        extern "C" {
            int sc_main(int argc, char** argv) {
                wif_trace_file trace("");
                trace.set_time_unit(1, SC_NS);
                return 0;
            };
        }
    ''', msg='Check for SystemC version (2.2.0 or greater required)', uselib='SYSTEMC SYSTEMC_H', mandatory=1)

    ##################################################
    # Check for TLM header
    ##################################################
    tlmPath = ''
    if Options.options.tlmdir:
        tlmPath = [os.path.abspath(os.path.join(Options.options.tlmdir, 'tlm'))]
    elif 'TLM' in os.environ:
        tlmPath = [os.path.abspath(os.path.join(os.environ['TLM'], 'tlm'))]

    conf.check_cxx(header_name='tlm.h', uselib='SYSTEMC SYSTEMC_H', uselib_store='TLM_H', mandatory=1, includes=tlmPath)
    #conf.check_cxx(fragment='''
        ##include <systemc.h>
        ##include <tlm.h>

        ##ifndef TLM_VERSION_MAJOR
        ##error TLM_VERSION_MAJOR undefined in the TLM library
        ##endif
        ##ifndef TLM_VERSION_MINOR
        ##error TLM_VERSION_MINOR undefined in the TLM library
        ##endif
        ##ifndef TLM_VERSION_PATCH
        ##error TLM_VERSION_PATCH undefined in the TLM library
        ##endif

        ##if TLM_VERSION_MAJOR < 2
        ##error Wrong TLM version; required 2.0
        ##endif

        #extern "C" int sc_main(int argc, char **argv){
            #return 0;
        #}
    #''', msg='Check for TLM version (2.0 or greater required)', uselib='SYSTEMC SYSTEMC_H TLM TLM_H', mandatory=1)

    ##################################################
    # Check for TRAP runtime libraries and headers
    ##################################################
    trapDirLib = ''
    trapDirInc = ''
    if Options.options.trapdir:
        trapDirLib = os.path.expandvars(os.path.expanduser(os.path.join(Options.options.trapdir, 'lib')))
        trapDirInc = os.path.expandvars(os.path.expanduser(os.path.join(Options.options.trapdir, 'include')))
    if conf.check_cxx(lib='trap', uselib_store='TRAP', mandatory=0, libpath=trapDirLib):
        if conf.check_cxx(header_name='trap.hpp', uselib='TRAP', uselib_store='TRAP', mandatory=0, includes=trapDirInc):
            conf.check_cxx(fragment='''
        #include "trap.hpp"

        #ifndef TRAP_REVISION
        #error TRAP_REVISION not defined in file trap.hpp
        #endif

        #if TRAP_REVISION < 63
        #error Wrong version of the TRAP runtime: too old
        #endif
        int main(int argc, char * argv[]){return 0;}
    ''', msg='Check for TRAP version', uselib='TRAP', mandatory=0)

    ######################################################
    # Finally I can set the environment for the BFD, LIBERTY and OPSOCDES libraries
    ######################################################
    conf.env['CPPPATH_BFD_CUSTOM'] = [os.path.abspath('lib' + os.sep + 'binutils-2.18' + os.sep + 'build' + os.sep + 'include')]
    conf.env['CPPPATH_LIBERTY_CUSTOM'] = [os.path.abspath('lib' + os.sep + 'binutils-2.18' + os.sep + 'build' + os.sep + 'include')]
    conf.env['CPPPATH_OPCODES_CUSTOM'] = [os.path.abspath('lib' + os.sep + 'binutils-2.18' + os.sep + 'build' + os.sep + 'include')]
    conf.env['LIBPATH_LIBERTY_CUSTOM'] = [os.path.abspath('lib' + os.sep + 'binutils-2.18' + os.sep + 'build' + os.sep + 'lib')]
    conf.env['LIBPATH_OPCODES_CUSTOM'] = [os.path.abspath('lib' + os.sep + 'binutils-2.18' + os.sep + 'build' + os.sep + 'lib')]
    conf.env['LIBPATH_BFD_CUSTOM'] = [os.path.abspath('lib' + os.sep + 'binutils-2.18' + os.sep + 'build' + os.sep + 'lib')]
    conf.env['LIB_LIBERTY_CUSTOM'] = ['iberty-2.18']
    conf.env['LIB_OPCODES_CUSTOM'] = ['opcodes-2.18']
    conf.env['LIB_BFD_CUSTOM'] = ['bfd-2.18']

    ######################################################
    # Set the environment for the MATLAB libraries
    ######################################################
    if Options.options.matlabdir:
         conf.env['CPPPATH_MATLAB'] = [os.path.abspath(os.path.join(os.path.normpath(os.path.expandvars(os.path.expanduser(Options.options.matlabdir))), 'extern','include'))]
         conf.env['LIBPATH_MATLAB'] = [os.path.abspath(os.path.join(os.path.normpath(os.path.expandvars(os.path.expanduser(Options.options.matlabdir))), 'bin','glnx86'))]
         conf.env['LIB_MATLAB'] = ['eng','mx']

    ##################################################
    # Check for documentation related tools
    ##################################################
    if not conf.find_program(filename='doxygen',var='DOXYGEN'):
        Logs.warn('doxygen not present, the documentation will not be compiled')

    ##################################################
    # GUI Section
    ##################################################
    if Options.options.enable_gui:
        conf.check_tool('pyqt4', os.path.join( '.' , 'tools' , 'waf'))

    #################################################
    # Special configuration for the software packages
    #################################################
    conf.sub_config('software')

    ##################################################
    # Check for GMP & GMPXX libraries and headers
    # used to represent precise floating point values
    ##################################################
    foundGMPLibs = False
    if conf.check_cc(lib='gmp', uselib_store='GMP', libpath=conf.env['GMP_LIBS']):
        if conf.check_cc(header_name='gmp.h', uselib_store='GMP', includes=conf.env['GMP_HEADER']):
            if conf.check_cxx(lib='gmpxx', uselib_store='GMPXX', libpath=conf.env['GMP_LIBS']):
                if conf.check_cxx(header_name='gmpxx.h', uselib_store='GMPXX', includes=conf.env['GMP_HEADER']):
                    foundGMPLibs = True

    ##################################################
    # Check for MPFR & MPFRCPP libraries and headers
    # used to represent precise floating point values
    ##################################################
    if foundGMPLibs:
        if conf.check_cc(lib='mpfr', uselib_store='MPFR', libpath=conf.env['MPFR_LIBS']):
            if conf.check_cc(header_name='mpfr.h', uselib_store='MPFR', includes=conf.env['MPFR_HEADER']):
                if conf.check_cxx(lib='mpfrcpp', uselib_store='MPFR', libpath=conf.env['MPFRCPP_LIBS']):
                    if conf.check_cxx(header_name='mpfrcpp/mpfrcpp.hpp', uselib='MPFR', uselib_store='MPFR', includes=conf.env['MPFRCPP_HEADER']):
                        conf.env.append_unique('CPPFLAGS','-DENABLE_FPU')

    ##################################################
    # Check for the MPATROL library and headers
    ##################################################
    if Options.options.enable_mpatrol:
        compilerExecutable = ''
        if len(conf.env['CXX']):
            compilerExecutable = conf.env['CXX']
        elif len(conf.env['CC']):
            compilerExecutable = conf.env['CC']
        else:
            conf.fatal('CC or CXX environment variables not defined: Error, is the compiler correctly detected?')

        result = os.popen(compilerExecutable + ' -print-search-dirs')
        curLine = result.readline()
        while curLine.find('libraries: =') == -1:
            curLine = result.readline()
            startFound = curLine.find('libraries: =')
            searchDirs = []
            if startFound != -1:
                curLine = curLine[startFound + 12:-1]
                searchDirs_ = curLine.split(':')
                for i in searchDirs_:
                    if not os.path.abspath(i) in searchDirs:
                        searchDirs.append(os.path.abspath(i))
                break
        import glob
        foundStatic = []
        foundShared = []
        for directory in searchDirs:
            foundShared += glob.glob(os.path.join(directory, conf.env['shlib_PATTERN'].split('%s')[0] + 'bfd*' + conf.env['shlib_PATTERN'].split('%s')[1]))
            foundStatic += glob.glob(os.path.join(directory, conf.env['staticlib_PATTERN'].split('%s')[0] + 'bfd*' + conf.env['staticlib_PATTERN'].split('%s')[1]))
        if not foundStatic and not foundShared:
            conf.fatal('BFD library not found, install binutils development package for your distribution')
        tempLibs = []
        for bfdlib in foundStatic:
            tempLibs.append(os.path.basename(bfdlib)[3:os.path.basename(bfdlib).rfind('.')])
        foundStatic = tempLibs
        tempLibs = []
        for bfdlib in foundShared:
            tempLibs.append(os.path.basename(bfdlib)[3:os.path.basename(bfdlib).rfind('.')])
        foundShared = tempLibs
        bfd_lib_name = ''
        for bfdlib in foundStatic:
            if bfdlib in foundShared:
                bfd_lib_name = bfdlib
            break
        if not bfd_lib_name:
            if foundShared:
                bfd_lib_name = foundShared[0]
            else:
                bfd_lib_name = foundStatic[0]

        if conf.check_cxx(lib=bfd_lib_name, uselib_store='BFD', libpath=searchDirs):
            if conf.check_cxx(header_name='bfd.h', uselib_store='BFD'):
                if conf.check_cxx(lib='mpatrolmt', uselib='BFD', uselib_store='MPATROL'):
                    if conf.check_cxx(header_name='mpatrol.h', uselib='MPATROL BFD', uselib_store='MPATROL'):
                        conf.env.append_unique('CPPFLAGS','-DMEMORY_DEBUG')

    ##################################################
    # Check for the SigC++ library
    ##################################################
    conf.check_cfg(package='sigc++-2.0', uselib_store='SIGCPP',args='--cflags --libs')

    ##################################################
    # Check for the MOMHLib++ library
    ##################################################
    if conf.check_cxx(lib='momh', uselib_store='MOMH', libpath=conf.env['MOM_LIBS']):
        if conf.check_cxx(header_name='momh/libmomh.h', uselib='MOMH SIGCPP', uselib_store='MOMH', includes=conf.env['MOM_HEADER']):
            conf.env["MOMH_FOUND"]=1
        elif conf.check_cxx(header_name='momh/libmomh.h', uselib='MOMH SIGCPP', uselib_store='MOMH', includes=['/usr/include/libmomh', '/usr/local/include/libmomh']):
            conf.env["MOMH_FOUND"]=1

    #################################################
    # Finally I set the path so that it is possible to include file utils.hpp in all the project
    #################################################
    conf.env.append_unique('CPPPATH',os.path.abspath(os.path.join('src', 'utils')))
    conf.env.append_unique('CPPPATH',os.path.abspath(os.path.join('src', 'systempy')))
    conf.env.append_unique('LIBPATH',os.path.abspath(os.path.join('_build_','default','src', 'utils')))
    conf.env.append_unique('LIB', 'utils')

def set_options(opt):
    """
    Allow some user configurability for the build environment
    """
    waf_tools = os.path.join('.' , 'tools' , 'waf')

    build_options = opt.add_option_group('General Build Options')

    opt.tool_options('python', option_group=build_options) # options for disabling pyc or pyo compilation
    opt.tool_options('compiler_cc')
    opt.tool_options('compiler_cxx')
    opt.tool_options('gcc', option_group=build_options)
    opt.tool_options('g++', option_group=build_options)
    #opt.tool_options('msvc', option_group=build_options)
    opt.tool_options('boost', option_group=build_options)
    opt.tool_options('pyqt4', waf_tools , opt.add_option_group('PYQT GUI Options'))
    opt.tool_options('mysql', waf_tools,  opt.add_option_group('MYSQL Options') )

    # Specify SystemC path
    opt.add_option('--with-systemc', type='string', help='SystemC installation directory', dest='systemcdir' )

    # Specify TLM path
    opt.add_option('--with-tlm', type='string', help='TLM installation directory', dest='tlmdir')
    # Specify TLM2 path
    opt.add_option('--with-tlm2', type='string', help='TLM2 installation directory', dest='tlm2dir')

    # Specify MATLAB path
    opt.add_option('--with-matlab', type='string', help='MATLAB installation directory', dest='matlabdir')

    # Processor-related options
    opt.sub_options('component/processor', opt.add_option_group( "Processor model options" ))

    # Software-related options
    sw_group = opt.add_option_group( "Software-related options" )
    opt.sub_options('software', sw_group )
    opt.tool_options('ecos',  waf_tools , option_group=opt.add_option_group('ECOS (OS) Options'))

    # Specify if the whole system should be put in event tracing: this automatically also enables the profiler
    opt.add_option('-H', '--enable-history', default=False, action="store_true", help='Enable tracing capabilites in the whole system; this automatically also enables the profiler(switch)', dest='enable_history')

    opt.add_option('-X', '--rebuild-doc', default=False, action="store_true", help='Rebuilds Doxygen documentation', dest='rebuild_doc')

    # Specify if the GUI should be compiled
    # opt.add_option('-G', '--disable-gui', default=True, action="store_false", help='Disables the GUI (switch)', dest='enable_gui')
    opt.add_option('-G', '--enable-gui', default=False, action="store_true", help='Enable the GUI (configuration)', dest='enable_gui')

    #Specifies the main file for the construction of the static platform
    opt.add_option('--static-platform', type='string', help='Specifies the main file of the static platform: instead of building the reflective simulator, it builds a static simulator using the main file provided by the user', dest='static_plat')

    # After the compilation it executes the tests to check for the correct system functionality
    opt.add_option('-C','--execute-tests', default=False, action="store_true", help='Specifies whether tests have to be executed at the end of the compilation process; this is a compilation option, not a configuration one', dest='exec_tests')

    # Specifies whether to compile the extensions for DSE or not
    opt.add_option('--with-momh-header', type='string', help='Specifies the location of the headers for the ', dest='momh_header')
    opt.add_option('--with-momh-libs', type='string', help='Specifies the location of the headers for the ', dest='momh_libs')

    # Specifies whether to compile the FPU and the directory containing the required libraries
    opt.add_option('--with-gmp-header', type='string', help='Specifies the location of the headers for GMP', dest='gmp_header')
    opt.add_option('--with-mpfr-header', type='string', help='Specifies the location of the headers for MPFR', dest='mpfr_header')
    opt.add_option('--with-mpfrcpp-header', type='string', help='Specifies the location of the headers for MPFRCPP', dest='mpfrcpp_header')
    opt.add_option('--with-gmp-libs', type='string', help='Specifies the location of the libraries for GMP', dest='gmp_libs')
    opt.add_option('--with-mpfr-libs', type='string', help='Specifies the location of the library for MPFR', dest='mpfr_libs')
    opt.add_option('--with-mpfrcpp-libs', type='string', help='Specifies the location of the library for MPFRCPP', dest='mpfrcpp_libs')

    # Memory profiling library
    opt.add_option('-M', '--enable-mpatrol', default=False, action="store_true", help='Enable the GUI (configuration)', dest='enable_mpatrol')

def shutdown():
    import os

    if Options.commands['build'] == 1:

        if Options.options.exec_tests and not Build.bld.env['STATIC_PLATFORM']:
            print Logs.colors.BOLD + Logs.colors.GREEN + '\n\tExecuting Python Tests ...\n' + Logs.colors.NORMAL
            os.system(os.path.join('.' , 'test' , 'unittests' , 'launchAll.sh'))
            print Logs.colors.BOLD + Logs.colors.GREEN + '\n\tPython Tests Executed\n' + Logs.colors.NORMAL
            #Now it is time to execute the C++ tests
            print Logs.colors.BOLD + Logs.colors.GREEN + '\n\tExecuting C++ Tests ...\n' + Logs.colors.NORMAL
            files = [os.path.join('_build_','default','test','cppTest') + os.sep + f for f in os.listdir(os.path.join('_build_','default','test','cppTest'))]
            files = filter( lambda x: os.access(x, os.X_OK), files)
            for file in files:
                os.system('LD_LIBRARY_PATH=' + os.path.abspath(os.path.join('_build_','default','src','bfdFrontend')) + ' ' + file + ' --log_level=test_suite')
            print Logs.colors.BOLD + Logs.colors.GREEN + '\n\tC++ Tests Executed\n' + Logs.colors.NORMAL


        # Finally, lets print some usefull messages to the user
        if not Build.bld.env['STATIC_PLATFORM']:
            if Build.bld.env['ENABLE_CVS']:
                print '\nIn order to properly use the eCos configuration tools please add the following command to the ' + os.path.join('~' , '.bashrc') + ' file: ' + Logs.colors['BOLD'] + Logs.colors['YELLOW'] + 'export ECOS_REPOSITORY=' + Build.bld.env()['ECOS_PATH'] , 'packages' + Logs.colors.NORMAL
            print '\nTo start the simulator type ' + Logs.colors.BOLD + Logs.colors.GREEN + os.path.join('.' , 'startSim.sh') + Logs.colors.NORMAL + ' at the command prompt\n'
        else:
            print '\nStatic platform correctly created in the executable ' + Logs.colors.BOLD + Logs.colors.GREEN + os.path.join('_build_' , 'default' , os.path.splitext(os.path.basename(Build.bld.env['STATIC_MAIN']))[0]) + Logs.colors.NORMAL + '\n'

#Installing custom distclean function so that some files are
def distclean():
    import glob
    try:
        toRemove = glob.glob(os.path.abspath(os.path.join('lib' , 'systemc' , 'libsystemc*')))
        for i  in toRemove:
            try:
                os.remove(i)
            except:
                pass
    except:
        pass
    try:
        os.system('for i in `find . -path \'.' , '_build_\' -prune -o -iname *.pyc | grep pyc`;do rm $i; done')
    except:
        pass
    try:
        toRemove = glob.glob(os.path.abspath(os.path.join('lib' , 'binutils-2.18' , 'build' , 'include')) + os.sep + '*.h') + \
                glob.glob(os.path.abspath(os.path.join('lib' , 'binutils-2.18' , 'build' , 'lib'))  + os.sep +  '*.*')
        for i  in toRemove:
            try:
                os.remove(i)
            except:
                pass
    except:
        pass
    try:
        os.remove('startSim.sh')
    except:
        pass

    import Scripting
    Scripting.distclean()
