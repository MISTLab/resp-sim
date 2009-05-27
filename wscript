#! /usr/bin/env python
# -*- coding: iso-8859-1 -*-

# ReSP Build file

# these variables are mandatory ('/' are converted automatically)
srcdir = '.'
blddir = '_build_'
VERSION = '0.5'
APPNAME = 'ReSP'

# make sure waf has the version we want
import Build,Utils,Options,TaskGen,Configure,Logs
from Tools import cc
from Tools import cxx
import os, types, sys, copy,  stat

validProcessors = ['arm', 'leon3', 'microblaze']

# Build methods
def build(bld):
    """
    All building is delegated to subdirectories.
    """
    sys.path.append( os.path.join('tools', 'waf') )
    from waf_utils import create_startSim

    # Now I have to correct the extensions which are accepted by the tools since they forgot
    # assembly files
    #TaskGen.declare_extension(['.S','.s'], cc.c_hook)

    # process subfolders from here
    bld.add_subdirs('lib tools src components dse')

    # Creates the startSim script
    bld.add_group()
    obj = bld.new_task_gen('cmd')
    obj.fun = create_startSim

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
    if int(conf.env['CC_VERSION'][0]) < 4:
        conf.fatal('Error, gcc compiler at leat version 4.0 required')

    conf.check_tool('misc')

    ##################################################################
    # Correcting custom environment vars to lists as required by waf
    ##################################################################
    if type(conf.env['CXX']) == type(''):
        conf.env['CXX'] = conf.env['CXX'].split(' ')
    if type(conf.env['CC']) == type(''):
        conf.env['CC'] = conf.env['CC'].split(' ')
    if type(conf.env['CCFLAGS']) == type(''):
        conf.env['CCFLAGS'] = conf.env['CCFLAGS'].split(' ')
    if type(conf.env['CXXFLAGS']) == type(''):
        conf.env['CXXFLAGS'] = conf.env['CXXFLAGS'].split(' ')
    if type(conf.env['CPPFLAGS']) == type(''):
        conf.env['CPPFLAGS'] = conf.env['CPPFLAGS'].split(' ')
    if type(conf.env['LINKFLAGS']) == type(''):
        conf.env['LINKFLAGS'] = conf.env['LINKFLAGS'].split(' ')
    if type(conf.env['STLINKFLAGS']) == type(''):
        conf.env['STLINKFLAGS'] = conf.env['STLINKFLAGS'].split(' ')
    if type(conf.env['RPATH']) == type(''):
        conf.env['RPATH'] = conf.env['RPATH'].split(' ')

    ########################################
    # Set and Check for special gcc flags
    ########################################
    if not '-g' in conf.env['CCFLAGS'] and not '-g3' in conf.env['CCFLAGS']:
        conf.env.append_unique('CCFLAGS', '-O2')
        conf.env.append_unique('CPPFLAGS', '-DNDEBUG')
    if not '-g' in conf.env['CXXFLAGS'] and not '-g3' in conf.env['CXXFLAGS']:
        conf.env.append_unique('CXXFLAGS', '-O2')
        conf.env.append_unique('CPPFLAGS', '-DNDEBUG')

    defaultFlags = ['-fstrict-aliasing']
    defaultFlags += ['-fPIC']
    conf.env.append_unique('LINKFLAGS','-fPIC' )
    if sys.platform != 'darwin':
        conf.env.append_unique('LINKFLAGS','-Wl,-E')

    conf.env.append_unique('CXXFLAGS', defaultFlags)
    conf.env.append_unique('CFLAGS', defaultFlags)
    conf.env.append_unique('CCFLAGS', defaultFlags)
    conf.env.append_unique('CPPFLAGS', '-DPIC')
    conf.env.append_unique('ARFLAGS', 'rc')

    if sys.platform == 'darwin':
        conf.env.append_unique('shlib_LINKFLAGS', ['-undefined suppress', '-flat_namespace'] )

    if not Options.options.enable_tools:
        conf.env.append_unique('CPPFLAGS','-DDISABLE_TOOLS')

    if sys.byteorder == "little":
        conf.env.append_unique('CPPFLAGS','-DLITTLE_ENDIAN_BO')
        conf.check_message_custom('endianness', '', 'little')
    else:
        conf.env.append_unique('CPPFLAGS','-DBIG_ENDIAN_BO')
        conf.check_message_custom('endianness', '', 'big')

    if conf.env['CPPFLAGS']:
        conf.check_cc(cflags=conf.env['CPPFLAGS'])
    if conf.env['CCFLAGS']:
        conf.check_cc(cflags=conf.env['CCFLAGS'])
    if conf.env['CXXFLAGS']:
        conf.check_cxx(cxxflags=conf.env['CXXFLAGS'])
    if conf.env['LINKFLAGS']:
        conf.check_cxx(linkflags=conf.env['LINKFLAGS'])
    if conf.env['STLINKFLAGS']:
        conf.check_cxx(linkflags=conf.env['STLINKFLAGS'], mandatory=1)

    ##################################################
    # Check for boost libraries
    ##################################################
    boostlibs = 'thread regex date_time program_options filesystem system'
    boostlibs += ' python unit_test_framework'

    conf.check_tool('boost')
    conf.check_boost(lib=boostlibs, static='both', min_version='1.35.0', mandatory = 1, errmsg = 'Unable to find ' + boostlibs + ' boost libraries of at least version 1.35, please install them and specify their location with the --boost-includes and --boost-libs configuration options')

    conf.env['RESP_HOME'] = os.path.abspath(os.path.dirname(sys.modules['__main__'].__file__))

    ########################################
    # Now I permanently save some compilation options specified at configure time
    ########################################
    conf.env['NON_STD_LIBS'] = []
    momh_libs = getattr(Options.options, 'momh_libs', '')
    if momh_libs:
        conf.env['NON_STD_LIBS'].append(os.path.normpath(os.path.expandvars(os.path.expanduser(momh_libs))))
    gmp_libs = getattr(Options.options, 'gmp_libs', '')
    if gmp_libs:
        conf.env['NON_STD_LIBS'].append(os.path.normpath(os.path.expandvars(os.path.expanduser(gmp_libs))))
    mpfr_libs = getattr(Options.options, 'mpfr_libs', '')
    if mpfr_libs:
        conf.env['NON_STD_LIBS'].append(os.path.normpath(os.path.expandvars(os.path.expanduser(mpfr_libs))))

    mpfrcpp_libs = getattr(Options.options, 'mpfrcpp_libs', '')
    if mpfrcpp_libs:
        conf.env['NON_STD_LIBS'].append(os.path.normpath(os.path.expandvars(os.path.expanduser(mpfrcpp_libs))))
    boostlibs = getattr(Options.options, 'boostlibs', '')
    if boostlibs:
        conf.env['NON_STD_LIBS'].append(os.path.normpath(os.path.expandvars(os.path.expanduser(boostlibs))))

    ########################################
    # Check for python
    ########################################
    conf.check_tool('python')
    conf.check_python_version((2,4))
    conf.check_python_headers()
    if float(conf.env['PYTHON_VERSION']) < 2.5:
        conf.env.append_unique('CPPFLAGS' , '-DPy_ssize_t=long' )

    ########################################
    # Check for special tools
    ########################################
    conf.check_tool('pypp', os.path.join( '.' , 'tools' , 'waf'))
    conf.check_tool('mkshared ', os.path.join( '.' , 'tools' , 'waf'))

    ##################################################
    # Check for standard libraries
    ##################################################
    conf.check_cc(lib='m', uselib_store='MATH', mandatory=1)
    conf.check_cc(lib='dl', uselib_store='DL', mandatory=1)

    #######################################################
    # Determining gcc search dirs
    #######################################################
    compilerExecutable = ''
    if len(conf.env['CXX']):
        compilerExecutable = conf.env['CXX'][0]
    elif len(conf.env['CC']):
        compilerExecutable = conf.env['CC'][0]
    else:
        conf.fatal('CC or CXX environment variables not defined: Error, is the compiler correctly detected?')

    result = os.popen(compilerExecutable + ' -print-search-dirs')
    searchDirs = []
    localLibPath = os.path.join('/', 'usr','local','lib')
    if os.path.exists(localLibPath):
        searchDirs.append(localLibPath)
    gccLines = result.readlines()
    for curLine in gccLines:
        startFound = curLine.find('libraries: =')
        if startFound != -1:
            curLine = curLine[startFound + 12:-1]
            searchDirs_ = curLine.split(':')
            for i in searchDirs_:
                if os.path.exists(i) and not os.path.abspath(i) in searchDirs:
                    searchDirs.append(os.path.abspath(i))
            break
    conf.check_message_custom('gcc search path', '', 'ok')

    ###########################################################
    # Check for IBERTY library
    ###########################################################
    if Options.options.bfddir:
        searchDirs.append(os.path.abspath(os.path.expanduser(os.path.expandvars(os.path.join(Options.options.bfddir, 'lib')))))

    import glob
    foundStatic = []
    foundShared = []
    for directory in searchDirs:
        foundShared += glob.glob(os.path.join(directory, conf.env['shlib_PATTERN'].split('%s')[0] + 'iberty*' + conf.env['shlib_PATTERN'].split('%s')[1]))
        foundStatic += glob.glob(os.path.join(directory, conf.env['staticlib_PATTERN'].split('%s')[0] + 'iberty*' + conf.env['staticlib_PATTERN'].split('%s')[1]))
    if not foundStatic and not foundShared:
        conf.fatal('IBERTY library not found, install binutils development package for your distribution')
    tempLibs = []
    for ibertylib in foundStatic:
        tempLibs.append(os.path.basename(ibertylib)[3:os.path.basename(ibertylib).rfind('.')])
    foundStatic = tempLibs
    tempLibs = []
    for ibertylib in foundShared:
        tempLibs.append(os.path.basename(ibertylib)[3:os.path.basename(ibertylib).rfind('.')])
    foundShared = tempLibs
    iberty_lib_name = ''
    for ibertylib in foundStatic:
        if ibertylib in foundShared:
            iberty_lib_name = ibertylib
            break
    if not iberty_lib_name:
        if foundShared:
            iberty_lib_name = foundShared[0]
        else:
            iberty_lib_name = foundStatic[0]

    conf.check_cc(lib=iberty_lib_name, uselib_store='BFD', mandatory=1, libpath=searchDirs, errmsg='not found, use --with-bfd option')

    ###########################################################
    # Check for BFD library and header
    ###########################################################
    if Options.options.bfddir:
        searchDirs.append(os.path.abspath(os.path.expanduser(os.path.expandvars(os.path.join(Options.options.bfddir, 'lib')))))

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

    conf.check_cc(lib=bfd_lib_name, uselib_store='BFD', mandatory=1, libpath=searchDirs, errmsg='not found, use --with-bfd option')
    if Options.options.bfddir:
        conf.check_cc(header_name='bfd.h', uselib='BFD', uselib_store='BFD', mandatory=1, includes=[os.path.abspath(os.path.expanduser(os.path.expandvars(os.path.join(Options.options.bfddir, 'include'))))])
    else:
        conf.check_cc(header_name='bfd.h', uselib='BFD', uselib_store='BFD', mandatory=1)

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
    # Notice that we can't rely on lib-linux,
    # therefore I have to find the actual platform...
    ##################################################
    # First I set the clafgs needed by TLM 2.0 for including systemc dynamic process
    # creation
    conf.env.append_unique('CPPFLAGS','-DSC_INCLUDE_DYNAMIC_PROCESSES')

    syscpath = None
    if Options.options.systemcdir:
        syscpath = ([os.path.abspath(os.path.expanduser(os.path.expandvars(os.path.join(Options.options.systemcdir, 'include'))))])
    elif 'SYSTEMC' in os.environ:
        syscpath = ([os.path.abspath(os.path.expanduser(os.path.expandvars(os.path.join(os.environ['SYSTEMC'], 'include'))))])

    import glob
    sysclib = ''
    if syscpath:
        sysclib = glob.glob(os.path.join(os.path.abspath(os.path.join(syscpath[0], '..')), 'lib-*'))
    conf.check_cxx(lib='systemc', uselib_store='SYSTEMC_STATIC', mandatory=1, libpath=sysclib, errmsg='not found, use --with-systemc option')

    ######################################################
    # Check if systemc is compiled with quick threads or not
    ######################################################
    if not os.path.exists(os.path.join(syscpath[0] , 'sysc' , 'qt')):
        conf.env.append_unique('CPPFLAGS', '-DSC_USE_PTHREADS')

    ##################################################
    # Check for SystemC header and test the library
    ##################################################
    conf.check_cxx(header_name='systemc.h', uselib='SYSTEMC_STATIC', uselib_store='SYSTEMC_H', mandatory=1, includes=syscpath)

    conf.check_cxx(fragment="""
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
    """, msg='Check for SystemC version', uselib='SYSTEMC_H SYSTEMC_STATIC', mandatory=1, errmsg='Error, at least version 2.2.0 required')

    ##################################################
    # Check for TLM header
    ##################################################
    tlmPath = ''
    if Options.options.tlmdir:
        tlmPath = os.path.normpath(os.path.abspath(os.path.expanduser(os.path.expandvars(Options.options.tlmdir))))
    elif 'TLM' in os.environ:
        tlmPath = os.path.normpath(os.path.abspath(os.path.expanduser(os.path.expandvars(os.environ['TLM']))))
    if not tlmPath.endswith('include'):
        tlmPath = os.path.join(tlmPath, 'include')
    tlmPath = [os.path.join(tlmPath, 'tlm')]

    conf.check_cxx(header_name='tlm.h', uselib='SYSTEMC_H SYSTEMC_STATIC', uselib_store='TLM', mandatory=1, includes=tlmPath, errmsg='not found, use --with-tlm option')
    conf.check_cxx(fragment='''
        #include <systemc.h>
        #include <tlm.h>

        #ifndef TLM_VERSION_MAJOR
        #error TLM_VERSION_MAJOR undefined in the TLM library
        #endif
        #ifndef TLM_VERSION_MINOR
        #error TLM_VERSION_MINOR undefined in the TLM library
        #endif
        #ifndef TLM_VERSION_PATCH
        #error TLM_VERSION_PATCH undefined in the TLM library
        #endif

        #if TLM_VERSION_MAJOR < 2
        #error Wrong TLM version; required 2.0
        #endif

        extern "C" int sc_main(int argc, char **argv){
            return 0;
        }
    ''', msg='Check for TLM version', uselib='SYSTEMC_H SYSTEMC_STATIC TLM', mandatory=1, errmsg='Error, at least version 2.0 required')

    ##################################################
    # Check for TRAP runtime libraries and headers
    ##################################################
    trapDirLib = ''
    trapDirInc = ''
    if Options.options.trapdir:
        trapDirLib = os.path.abspath(os.path.expandvars(os.path.expanduser(os.path.join(Options.options.trapdir, 'lib'))))
        trapDirInc = os.path.abspath(os.path.expandvars(os.path.expanduser(os.path.join(Options.options.trapdir, 'include'))))
        conf.check_cxx(lib='trap', uselib_store='TRAP', mandatory=1, libpath=trapDirLib)
        conf.check_cxx(header_name='trap.hpp', uselib='TRAP', uselib_store='TRAP', mandatory=1, includes=trapDirInc)
        conf.check_cxx(fragment='''
            #include "trap.hpp"

            #ifndef TRAP_REVISION
            #error TRAP_REVISION not defined in file trap.hpp
            #endif

            #if TRAP_REVISION < 420
            #error Wrong version of the TRAP runtime: too old
            #endif
            int main(int argc, char * argv[]){return 0;}
        ''', msg='Check for TRAP version', uselib='TRAP', mandatory=1, includes=trapDirInc, errmsg='Error, at least revision 420 required')
    else:
        conf.check_cxx(lib='trap', uselib_store='TRAP', mandatory=1)
        conf.check_cxx(header_name='trap.hpp', uselib='TRAP', uselib_store='TRAP', mandatory=1, errmsg='not found, use --with-trap option')
        conf.check_cxx(fragment='''
            #include "trap.hpp"

            #ifndef TRAP_REVISION
            #error TRAP_REVISION not defined in file trap.hpp
            #endif

            #if TRAP_REVISION < 420
            #error Wrong version of the TRAP runtime: too old
            #endif
            int main(int argc, char * argv[]){return 0;}
        ''', msg='Check for TRAP version', uselib='TRAP', mandatory=1, errmsg='Error, at least revision 420 required')

    ##################################################
    # Check for GMP & GMPXX libraries and headers
    # used to represent precise floating point values
    ##################################################
    foundGMPLibs = False
    if Options.options.gmp_libs:
        foundGMPLibs = conf.check_cc(lib='gmp', uselib_store='GMP', libpath=Options.options.gmp_libs)
    else:
        foundGMPLibs = conf.check_cc(lib='gmp', uselib_store='GMP')
    if foundGMPLibs:
        if Options.options.gmp_header:
            foundGMPLibs = conf.check_cc(header_name='gmp.h', uselib_store='GMP', uselib='GMP',  includes=Options.options.gmp_header)
        else:
            foundGMPLibs = conf.check_cc(header_name='gmp.h', uselib_store='GMP', uselib='GMP')
    if foundGMPLibs:
        if Options.options.gmpxx_libs:
            foundGMPLibs = conf.check_cxx(lib='gmpxx', uselib_store='GMPXX', libpath=Options.options.gmpxx_libs)
        else:
            foundGMPLibs = conf.check_cxx(lib='gmpxx', uselib_store='GMPXX')
    if foundGMPLibs:
        if Options.options.gmpxx_header:
            foundGMPLibs = conf.check_cxx(header_name='gmpxx.h', uselib_store='GMPXX', uselib='GMPXX GMP', includes=Options.options.gmpxx_header)
        else:
            foundGMPLibs = conf.check_cxx(header_name='gmpxx.h', uselib_store='GMPXX', uselib='GMPXX GMP')

    ##################################################
    # Check for MPFR & MPFRCPP libraries and headers
    # used to represent precise floating point values
    ##################################################
    foundMPFR = False
    if Options.options.mpfr_libs:
        foundMPFR = conf.check_cc(lib='mpfr', uselib_store='MPFR', libpath=Options.options.mpfr_libs)
    else:
        foundMPFR = conf.check_cc(lib='mpfr', uselib_store='MPFR')
    if foundMPFR:
        if Options.options.mpfr_header:
            foundMPFR = conf.check_cc(header_name='mpfr.h', uselib_store='MPFR', uselib='MPFR', includes=Options.options.mpfr_header)
        else:
            foundMPFR = conf.check_cc(header_name='mpfr.h', uselib_store='MPFR', uselib='MPFR')
    if foundMPFR:
        if Options.options.mpfrcpp_libs:
            foundMPFR = conf.check_cxx(lib='mpfrcpp', uselib_store='MPFRCPP', libpath=Options.options.mpfrcpp_libs)
        else:
            foundMPFR = conf.check_cxx(lib='mpfrcpp', uselib_store='MPFRCPP')
    if foundMPFR:
        if Options.options.mpfrcpp_header:
            foundMPFR = conf.check_cxx(header_name='mpfrcpp/mpfrcpp.hpp', uselib='MPFR MPFRCPP', uselib_store='MPFRCPP', includes=Options.options.mpfrcpp_header)
        else:
            foundMPFR = conf.check_cxx(header_name='mpfrcpp/mpfrcpp.hpp', uselib='MPFR MPFRCPP', uselib_store='MPFRCPP')

    ##################################################
    # Check for the SigC++ library
    ##################################################
    conf.check_cfg(package='sigc++-2.0', uselib_store='SIGCPP',args='--cflags --libs')

    ##################################################
    # Check for the MOMHLib++ library
    ##################################################
    foundMOMH = False
    if Options.options.momh_libs:
        foundMOMH = conf.check_cxx(lib='momh', uselib_store='MOMH', libpath=Options.options.momh_libs)
    else:
        foundMOMH = conf.check_cxx(lib='momh', uselib_store='MOMH')
    if foundMOMH:
        if Options.options.momh_header:
            foundMOMH = conf.check_cxx(header_name='momh/libmomh.h', uselib='MOMH SIGCPP', uselib_store='MOMH', includes=Options.options.momh_header)
        else:
            foundMOMH = conf.check_cxx(header_name='momh/libmomh.h', uselib='MOMH SIGCPP', uselib_store='MOMH')
            conf.env["MOMH_FOUND"]=1
            if not foundMOMH:
                foundMOMH = conf.check_cxx(header_name='momh/libmomh.h', uselib='MOMH SIGCPP', uselib_store='MOMH', includes=['/usr/include/libmomh', '/usr/local/include/libmomh'])
    if foundMOMH:
        conf.env["HAVE_MOMH"]=1

    #################################################
    # Finally I set the path so that it is possible to include file utils.hpp in all the project
    #################################################
    conf.env.append_unique('CPPPATH', os.path.abspath(os.path.join('src', 'utils')))
    conf.env.append_unique('LIBPATH', os.path.abspath(os.path.join('_build_', 'default', 'src', 'utils')))
    conf.env.append_unique('LIB', 'utils')

def set_options(opt):
    """
    Allow some user configurability for the build environment
    """
    waf_tools = os.path.join('.' , 'tools' , 'waf')

    # Default tools options
    opt.tool_options('python')
    opt.tool_options('compiler_cc')
    opt.tool_options('compiler_cxx')
    opt.tool_options('gcc')
    opt.tool_options('g++')
    opt.tool_options('boost')

    # Specify SystemC path
    opt.add_option('--with-systemc', type='string', help='SystemC installation directory', dest='systemcdir' )
    # Specify TLM path
    opt.add_option('--with-tlm', type='string', help='TLM installation directory', dest='tlmdir')
    # Specify TRAP path
    opt.add_option('--with-trap', type='string', help='TRAP libraries and headers installation directory', dest='trapdir')
    # Specify BFD and IBERTY libraries path
    opt.add_option('--with-bfd', type='string', help='BFD installation directory', dest='bfddir' )

    # Specify if tools (debugger, profiler etc.) support should be compiled inside processor models
    opt.add_option('-T', '--disable-tools', default=True, action="store_false", help='Disables support for support tools (debuger, os-emulator, etc.) (switch)', dest='enable_tools')

    # Specifies whether to compile the extensions for DSE or not
    opt.add_option('--with-momh-header', type='string', help='Specifies the location of the headers for the ', dest='momh_header')
    opt.add_option('--with-momh-libs', type='string', help='Specifies the location of the headers for the ', dest='momh_libs')

    # Specifies whether to compile the FPU and the directory containing the required libraries
    opt.add_option('--with-gmp-header', type='string', help='Specifies the location of the headers for GMP', dest='gmp_header')
    opt.add_option('--with-gmpxx-header', type='string', help='Specifies the location of the headers for GMPXX', dest='gmpxx_header')
    opt.add_option('--with-mpfr-header', type='string', help='Specifies the location of the headers for MPFR', dest='mpfr_header')
    opt.add_option('--with-mpfrcpp-header', type='string', help='Specifies the location of the headers for MPFRCPP', dest='mpfrcpp_header')
    opt.add_option('--with-gmp-libs', type='string', help='Specifies the location of the libraries for GMP', dest='gmp_libs')
    opt.add_option('--with-gmpxx-libs', type='string', help='Specifies the location of the libraries for GMPXX', dest='gmpxx_libs')
    opt.add_option('--with-mpfr-libs', type='string', help='Specifies the location of the library for MPFR', dest='mpfr_libs')
    opt.add_option('--with-mpfrcpp-libs', type='string', help='Specifies the location of the library for MPFRCPP', dest='mpfrcpp_libs')

def shutdown():

    if Options.commands['build'] == 1:
        import os
        # Finally, lets print some usefull messages to the user
        print('\nTo start the simulator type ' + Logs.colors.BOLD + Logs.colors.GREEN + os.path.join('.' , 'startSim.sh') + Logs.colors.NORMAL + ' at the command prompt\n')

#Installing custom distclean function so that some files are
def distclean():

    # Removing SystemC dynamic library
    try:
        import glob
        toRemove = glob.glob(os.path.abspath(os.path.join('lib' , 'systemc' , 'libsystemc*')))
        for i in toRemove:
            try:
                os.remove(i)
            except:
                pass
    except:
        pass

    # Removing compiled python files
    try:
        sys.path.append( os.path.join('tools','waf') )
        from waf_utils import find_files

        toRemFiles = find_files(['.'], '*.pyc')
        for i in toRemFiles:
            try:
                os.remove(i)
            except:
                pass
    except:
        pass

    # Removing simulator start script
    try:
        os.remove('startSim.sh')
    except:
        pass

    # Now execute the standard cleaning up
    import Scripting
    Scripting.distclean()
