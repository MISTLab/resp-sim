#! /usr/bin/env python
# -*- coding: iso-8859-1 -*-
# encoding: utf-8

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
    sys.path.append( os.path.join('tools','waf') )
    from waf_utils import create_startSim

    # Now I have to correct the extensions which are accepted by the tools since they forgot
    # assembly files
    #TaskGen.declare_extension(['.S','.s'], cc.c_hook)

    # process subfolders from here
    bld.add_subdirs('lib component tools src')
    bld.add_subdirs(os.path.join('dse', 'MDP'))
    if bld.env['MOMH_FOUND']:
        bld.add_subdirs(os.path.join('dse', 'MOMH'))

    # Creates the startSim script
    if not bld.env['STATIC_PLATFORM']:
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
    if conf.env['STLINKFLAGS']:
        conf.check_cxx(linkflags=conf.env['STLINKFLAGS'], mandatory=1)

    ##################################################
    # Check for boost libraries
    ##################################################
    boostlibs = 'thread regex date_time program_options filesystem system'
    if conf.env['STATIC_PLATFORM'] != 1:
        boostlibs += ' python unit_test_framework'

    conf.check_tool('boost')
    conf.check_boost(lib=boostlibs, static='both', min_version='1.35.0', mandatory = 1, errmsg = 'Unable to find ' + boostlibs + ' boost libraries of at least version 1.35, please install them and specify their location with the --boost-includes and --boost-libs configuration options')

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
    if conf.env['MPFRCPP_LIBS']:
        conf.env['NON_STD_LIBS'].append(os.path.normpath(os.path.expandvars(os.path.expanduser(mpfrcpp_libs))))
    boostlibs = getattr(Options.options, 'boostlibs', '')
    if boostlibs:
        conf.env['NON_STD_LIBS'].append(os.path.normpath(os.path.expandvars(os.path.expanduser(getattr(Options.options, 'boostlibs', '')))))
    if Options.options.en_processors:
        conf.env['ENABLE_PROCESSORS'] = Options.options.en_processors.split()
        for proc in conf.env['ENABLE_PROCESSORS']:
            if not proc in validProcessors:
                Logs.error("please specify a path inside the main ReSP folder for file " + Options.options.static_plat)

    if not Options.options.enable_tools:
        conf.env.append_unique('CPPFLAGS','-DDISABLE_TOOLS')

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
    conf.check_tool('mkshared ', os.path.join( '.' , 'tools' , 'waf'))

    ##################################################
    # Check for standard libraries
    ##################################################
    conf.check_cc(lib='m', uselib_store='MATH', mandatory=1)
    conf.check_cc(lib='dl', uselib_store='DL', mandatory=1)

    ###########################################################
    # Check for BFD library and header and for LIBERTY library
    ###########################################################
    compilerExecutable = ''
    if len(conf.env['CXX']):
        compilerExecutable = conf.env['CXX'][0]
    elif len(conf.env['CC']):
        compilerExecutable = conf.env['CC'][0]
    else:
        conf.fatal('CC or CXX environment variables not defined: Error, is the compiler correctly detected?')

    result = os.popen(compilerExecutable + ' -print-search-dirs')
    searchDirs = []
    gccLines = result.readlines()
    for curLine in gccLines:
        startFound = curLine.find('libraries: =')
        if startFound != -1:
            curLine = curLine[startFound + 12:-1]
            searchDirs_ = curLine.split(':')
            for i in searchDirs_:
                if not os.path.abspath(i) in searchDirs:
                    searchDirs.append(os.path.abspath(i))
            break
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

    conf.check_cc(lib=bfd_lib_name, uselib_store='BFD', mandatory=1, libpath=searchDirs)
    if Options.options.bfddir:
        conf.check_cc(header_name='bfd.h', uselib_store='BFD', mandatory=1, includes=[os.path.abspath(os.path.expanduser(os.path.expandvars(os.path.join(Options.options.bfddir, 'include'))))])
    else:
        conf.check_cc(header_name='bfd.h', uselib_store='BFD', mandatory=1)

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
    syscpath = None
    if Options.options.systemcdir:
        syscpath = ([os.path.abspath(os.path.expanduser(os.path.expandvars(os.path.join(Options.options.systemcdir, 'include'))))])
    elif 'SYSTEMC' in os.environ:
        syscpath = ([os.path.abspath(os.path.expanduser(os.path.expandvars(os.path.join(os.environ['SYSTEMC'], 'include'))))])

    import glob
    sysclib = ''
    if syscpath:
        sysclib = glob.glob(os.path.join(os.path.abspath(os.path.join(syscpath[0], '..')), 'lib-*'))
    conf.check_cxx(lib='systemc', uselib_store='SYSTEMC', mandatory=1, libpath=sysclib)
    ######################################################
    # Check if systemc is compiled with quick threads or not
    ######################################################
    if not os.path.exists(os.path.join(syscpath[0] , 'sysc' , 'qt')):
        conf.env.append_unique('CPPFLAGS', '-DSC_USE_PTHREADS')

    ##################################################
    # Check for SystemC header and test the library
    ##################################################
    conf.check_cxx(header_name='systemc.h', uselib='SYSTEMC', uselib_store='SYSTEMC', mandatory=1, includes=syscpath)
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
    """, msg='Check for SystemC version (2.2.0 or greater required)', uselib='SYSTEMC', mandatory=1)

    ##################################################
    # Check for TLM header
    ##################################################
    tlmPath = ''
    if Options.options.tlmdir:
        tlmPath = [os.path.abspath(os.path.expanduser(os.path.expandvars(os.path.join(Options.options.tlmdir, 'tlm'))))]
    elif 'TLM' in os.environ:
        tlmPath = [os.path.abspath(os.path.expanduser(os.path.expandvars(os.path.join(os.environ['TLM'], 'tlm'))))]

    conf.check_cxx(header_name='tlm.h', uselib='SYSTEMC', uselib_store='TLM', mandatory=1, includes=tlmPath)
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
    ''', msg='Check for TLM version (2.0 or greater required)', uselib='SYSTEMC TLM', mandatory=1)

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

            #if TRAP_REVISION < 419
            #error Wrong version of the TRAP runtime: too old
            #endif
            int main(int argc, char * argv[]){return 0;}
        ''', msg='Check for TRAP version', uselib='TRAP', mandatory=1, includes=trapDirInc)
    else:
        conf.check_cxx(lib='trap', uselib_store='TRAP', mandatory=1)
        conf.check_cxx(header_name='trap.hpp', uselib='TRAP', uselib_store='TRAP', mandatory=1)
        conf.check_cxx(fragment='''
            #include "trap.hpp"

            #ifndef TRAP_REVISION
            #error TRAP_REVISION not defined in file trap.hpp
            #endif

            #if TRAP_REVISION < 63
            #error Wrong version of the TRAP runtime: too old
            #endif
            int main(int argc, char * argv[]){return 0;}
        ''', msg='Check for TRAP version', uselib='TRAP', mandatory=1)

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
    conf.env.append_unique('CPPPATH', os.path.abspath(os.path.join('src', 'utils')))
    conf.env.append_unique('CPPPATH', os.path.abspath(os.path.join('src', 'systempy')))
    conf.env.append_unique('LIBPATH', os.path.abspath(os.path.join('_build_','default','src', 'utils')))
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

    # Specify SystemC path
    opt.add_option('--with-systemc', type='string', help='SystemC installation directory', dest='systemcdir' )
    # Specify TLM path
    opt.add_option('--with-tlm', type='string', help='TLM installation directory', dest='tlmdir')

    # Processor-related options
    opt.sub_options('component/processor', opt.add_option_group( "Processor model options" ))

    #Specifies the main file for the construction of the static platform
    opt.add_option('--static-platform', type='string', help='Specifies the main file of the static platform: instead of building the reflective simulator, it builds a static simulator using the main file provided by the user', dest='static_plat')

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

def shutdown():
    import os

    if Options.commands['build'] == 1:

        # Finally, lets print some usefull messages to the user
        if not Build.bld.env['STATIC_PLATFORM']:
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
        os.remove('startSim.sh')
    except:
        pass

    import Scripting
    Scripting.distclean()
