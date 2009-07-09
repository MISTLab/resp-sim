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
#   TRAP is free software; you can redistribute it and/or modify
#   it under the terms of the GNU Lesser General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public License
#   along with this program; if not, write to the
#   Free Software Foundation, Inc.,
#   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#   or see <http://www.gnu.org/licenses/>.
#
#
#
#   (c) Giovanni Beltrame, Luca Fossati
#       Giovanni.Beltrame@esa.int fossati@elet.polimi.it
#
##############################################################################


"Python wrapper generation with Py++"

import ccroot,cxx, TaskGen
from TaskGen import taskgen,feature,before,after,extension
import Options,Utils,Task,Logs,Build
from Logs import error
import os, sys, imp

def compute_indent(line):
    indent = 0
    for i in line:
        if i != ' ':
            break
        indent += 1
    return indent

def get_code_blocks(code_string):
    # this function splits the code contained in the string
    # into different blocks (the code under and if statement,
    # a for statement, etc. is a code block)
    codeBlocks = []
    code_string = code_string.replace('\t', '    ')
    code_string_lines = code_string.split('\n')
    baseIndent = 0
    curBlock = ''
    for line in code_string_lines:
        if line.strip() == '':
            continue
        newIndent = compute_indent(line)
        if newIndent > baseIndent:
            curBlock += line + '\n'
        elif newIndent == baseIndent and (line.strip().startswith('else') or line.strip().startswith('except') or line.strip().startswith('elif')):
            curBlock += line + '\n'
        else:
            if curBlock.strip() != '':
                codeBlocks.append(curBlock)
            curBlock = line + '\n'
            if newIndent < baseIndent:
                baseIndent = newIndent

    if curBlock.strip() != '':
        codeBlocks.append(curBlock)

    return codeBlocks

CXX_METHS = ['extract_headers', 'init_cxx', 'apply_type_vars', 'apply_incpaths', 'apply_defines_cxx',
'apply_core', 'apply_lib_vars', 'apply_obj_vars_cxx', 'process_headers']

TaskGen.bind_feature('pypp', CXX_METHS)

createdLibraries = None

class pypp_taskgen(cxx.cxx_taskgen):
    """ Class for dealing with header files and generate python wrappers using py++

    This class behaves exactly like cppobj, compiling c/c++ files into objects, libraries,
    or executables. In addition, it generates a wrapper with the specified header files using
    py++ and calls it <name>.generated.cpp and compiles it.

    Parameters:
        start_decls              specifies the declarations (classes) where to start parsing
        include                  a list of all declaration files to be included as already_exposed
        define_symobls           a set of symbols that have to be defined for the GCCXML run
        custom_code              custom py++ to be executed before generating a module
        custom_declaration_code  custom C++ to be added to module declaration
        custom_registration_code custom C++ to register module components/functions
        split                    number of files to be created for the module
        virtuality               enable or disable python overrides for functions (False by default)
        templates                List of templates to be instantiated
    """

    s_default_ext = ['.h', '.hpp', '.hxx', '.H', '.cpp','.cc','.cxx']
    header_ext = ['.h', '.hpp', '.hxx','.H']

    def __setattr__(self, name, attr):
        object.__setattr__(self, name, attr)

    def __init__(self,*k,**kw):
        ccroot.ccroot_abstract.__init__(self, *k, **kw)
        self.start_decls = ''
        self.custom_code = ''
        self.custom_declaration_code = ''
        self.custom_registration_code = ''
        self.include = ""

        self.virtuality = False
        self.split = 1
        self.templates = []
        self.features.append('pypp')
        self.features.append('cxx')
        self.features.append('cshlib')


def setup(env):
    # create our action here
    Task.task_type_from_func(name='pypp',func=dopypp,color='CYAN')

@taskgen
#@feature('pypp')
@extension(['.h', '.hpp', '.hxx','.H'])
def skip_headers(self,node):
    pass

@taskgen
@feature('pypp')
@before('init_cxx')
def extract_headers(self):
    lst = self.to_list(self.source)
    self.source = ''
    find_resource = self.path.find_resource

    self.uselib_local += ' sc_controller'

    try:
        obj_ext = self.obj_ext
    except AttributeError:
        self.obj_ext = '.o'

    self.srclist = []
    targetbase, ext = os.path.splitext(ccroot.get_target_name(self))

    find_resource = self.path.find_resource
    for filename in lst:
        # -- Extract the header files to be treated separately
        #
        base, ext = os.path.splitext(filename)
        if ext in self.header_ext:
            n = find_resource(Utils.split_path(filename))

            self.source += '\n' + filename
            self.srclist.append(n)
        else:
            self.source += '\n' + filename

@taskgen
@feature('pypp')
#@after('apply_defines_cxx')
#@before('apply_link')
@before('apply_core')
def process_headers(self):
    """ This function separates header files from .cpp files, and
        treats them separately.
    """
    if sys.platform == 'darwin':
        so_ext = '.so'
    else:
        so_ext = self.env['shlib_PATTERN'].split('%s')[1]
    self.env['shlib_PATTERN'] = '%s'+so_ext

    try:
        self.custom_code = get_code_blocks(self.custom_code)
    except:
        self.custom_code = []

    targetbase, ext = os.path.splitext(ccroot.get_target_name(self))
    inc_paths = []
    for i in self.to_list(self.uselib):
        if self.env['CPPPATH_'+i]:
            inc_paths += self.to_list(self.env['CPPPATH_'+i])

    if len(self.srclist) > 0:
        # Includes are objects: to keep dependencies working, objects have
        # to be *posted* before they can be used
        include_lst = self.to_list(self.include)

        # Find the target node, i.e. the generated file
        if self.split < 1:
            raise Exception('Error, specified ' + str(self.split) + ' output files for task ' + targetbase + '; a number > 0 should be used')
        tgnodes = []
        if self.split == 1:
            tgnodes.append(self.path.find_or_declare(targetbase+'.pypp.cpp'))
        else:
            for i in range( 1 , self.split ):
                name = targetbase+'_classes_'+str(i)
                tgnodes.append(self.path.find_or_declare(name+'.pypp.cpp'))
            tgnodes.append(self.path.find_or_declare(targetbase+'.main.cpp'))

        # Create a pypp task with all the input headerfiles and one output module
        pypptask = self.create_task('pypp', self.env)

        # Apply parameters passed by the wscript to the task
        pypptask.bldpath = self.bld.bldnode.abspath()
        pypptask.srcpath = self.bld.srcnode.abspath()
        pypptask.custom_code = self.custom_code
        pypptask.custom_declaration_code = self.custom_declaration_code
        pypptask.custom_registration_code = self.custom_registration_code
        pypptask.target = ccroot.get_target_name(self)
        pypptask.split = self.split
        pypptask.start_decls = Utils.to_list(self.start_decls)
        pypptask.virtuality = self.virtuality
        pypptask.path_lst = self.env['INC_PATHS']
        pypptask.includes = [os.path.join(self.path.abspath(), i) for i in self.to_list(self.includes)]
        pypptask.templates  = self.templates

        # Now I have to parse the CPPFLAGS and CXXFLAGS environment variables and extract the synmbol definitions;
        pypptask.define_symbols = []
        for symbol in self.env['CXXFLAGS']:
            if symbol.find('-D')  == 0:
                symbol = symbol.replace('-D',  '',  1)
                if not symbol in pypptask.define_symbols:
                    pypptask.define_symbols.append(symbol)
        for symbol in self.env['CPPFLAGS']:
            if symbol.find('-D')  == 0:
                symbol = symbol.replace('-D',  '',  1)
                if not symbol in pypptask.define_symbols:
                    pypptask.define_symbols.append(symbol)

        # Create a node for every include
        incl = []
        includeCustomCode = []
        includeStartDecls = []
        if include_lst:
            for x in include_lst:
                obj = self.name_to_obj(x)
                if( not  obj ):
                    print targetbase + ' --> Warning: Object '+ x +' not found'
                    continue
                try:
                    includeCustomCode += obj.custom_code
                except AttributeError:
                    pass
                try:
                    includeStartDecls += Utils.to_list(obj.start_decls)
                except AttributeError:
                    pass

        # Now I add the custom code of the includes, so that their elements are also
        # exported; I also have to add their start declarations
        pypptask.custom_code_include = includeCustomCode
        pypptask.start_decls = includeStartDecls + pypptask.start_decls

        # ********************** TODO ************** set the dipendences well, so that only the tasks
        # ************************* depending on each other are (either directly or because they include the
        # same wrapper) are serialized *****************************************************
        # Adding dependencies to all already-processed pypp files
        all_tasks = self.bld.task_manager.groups[self.bld.task_manager.current_group].tasks
        dependencies = filter(lambda tsk: isinstance(tsk, Task.TaskBase.classes['pypp']) and (pypptask != tsk) , all_tasks)
        if dependencies:
            pypptask.set_run_after(dependencies[-1])
            #print dependencies[-1]
        for x in dependencies:
            incl.append( x.outputs[0] )

        pypptask.include = incl

        # Generate template task if templates are specified
        if self.templates:
            templatetask = self.create_task('generate_header', self.env)
            templatetask.templates = self.templates

            # The source files are augmented with the autogenerated template instantiation
            templatetask.inputs = self.srclist
            templatetask.outputs = [self.path.find_or_declare(targetbase+'_exp.hpp')]
            templatetask.target = self.target

            # If templates are enabled, py++ will process only the autogenerated header
            pypptask.inputs = templatetask.outputs
            pypptask.set_run_after(templatetask)
        else:
            # If there are no templates, the input of py++ are the source files
            pypptask.inputs = self.srclist

        #print Build.bld.bdir
        pypptask.ext_headers = map(lambda a: os.path.join(Build.bld.bdir, a.srcpath(self.env)) , self.srclist)

        pypptask.outputs = tgnodes

        # Compile the autogenerated C++ files
        if self.split == 1:
            task = self.create_task('cxx', self.env)
            task.inputs = [tgnodes[0]]
            task.outputs = [pypptask.outputs[0].change_ext(self.obj_ext)]
            task.set_run_after(pypptask)
            #task.defines  = self.scanner_defines
            self.compiled_tasks.append(task)
        else:
            for i in range (0, self.split):
                task = self.create_task('cxx',self.env)

                task.inputs = [tgnodes[i]]
                task.outputs = [pypptask.outputs[i].change_ext(self.obj_ext)]
                task.set_run_after(pypptask)
                #task.defines  = self.scanner_defines
                self.compiled_tasks.append(task)

def detect(conf):
    curDir = conf.curdir
    if os.path.exists(conf.blddir):
        newDir = os.chdir(conf.blddir)
    ##################################################
    # Check for gccxml
    ##################################################
    gccxmlFound = False
    gccxml = conf.find_program('gccxml', mandatory = 1)
    try:
        if gccxml:
            #Now I check the version
            version = os.popen(gccxml + ' 2>&1').readline().split()[2]
            if int(version.split('.')[1]) < 9 and int(version.split('.')[0]) == 0:
                conf.check_message_custom('gccxml version', '', 'Version ' + version, color='YELLOW')
                conf.fatal("Configure Failed, please install at least version 0.9.0 of gccxml as described in the online documentation")
            conf.check_message_custom('gccxml version', '', 'Version ' + version, color='GREEN')
        else:
            conf.check_message_custom('gccxml version', '', 'Not found', color='YELLOW')
            conf.fatal("Configure Failed, please install gccxml as described in the online documentation")
    except Exception,  e:
        conf.check_message_custom('gccxml version', '', 'Error in determining the version --> ' + str(e), color='YELLOW')
        conf.fatal("Configure Failed, please install gccxml as described in the online documentation")
    gccxmlpp = conf.find_program('gccxml_cc1plus', mandatory = 1)
    if gccxmlpp:
        callResult = os.popen("echo \'int temp = 0;\' | " + gccxmlpp + ' 2>&1').read()
        if 'Could not determine GCCXML_EXECUTABLE setting' in callResult:
            conf.check_message_custom('gccxml_cc1plus', '', 'Error in the setup, gccxml_cc1plus program does not run correctly', color='YELLOW')
            conf.fatal("Configure Failed, please correctly re-install gccxml")
    else:
        conf.check_message_custom('gccxml_cc1plus', '', 'Not found', color='YELLOW')
    os.chdir(curDir)

    ##################################################
    # Determine gxx version, since 4.3 cannot work with
    # gccxml, so, in this case, we need to look for another
    # complier
    ##################################################
    if not conf.env['CC_VERSION']:
        conf.fatal('Error in determining gcc version')

    if int(conf.env['CC_VERSION'][1]) >= 3:
        for i in ['.2', '.1', '.0']:
            for j in ['.4', '.3', '.2', '.1', '.0', '']:
                if conf.find_program('g++-4' + i + j):
                    conf.env['CXX_OLD_VERSION'] = ['g++-4' + i + j]
                    break
            if conf.env['CXX_OLD_VERSION']:
                break
        if not conf.env['CXX_OLD_VERSION']:
            conf.fatal('Unable to find a g++ version older than 4.3: gccxxml does not work with g++ >= 4.3. Please install an old gcc version along with the current one')

    ##################################################
    # Check for pygccxml
    ##################################################
    try:
        import pygccxml
    except:
        if Logs.verbose:
            import traceback
            traceback.print_exc(file=sys.stderr)
        conf.check_message_custom('module', 'pygccxml', 'Not found', color='YELLOW')
        conf.fatal("Configure Failed, please install pygccxml as described in the online documentation")
    else:
        conf.check_message_custom('module','pygccxml', 'ok', color='GREEN')
        try:
            version = pygccxml.__version__.split('.')
            if Logs.verbose:
                print 'Version ' + str(version)
            if int(version[0]) >= 1:
                if int(version[1]) >= 5 and int(version[2]) >= 0:
                    conf.check_message_custom('pygccxml version', '', 'Version ' + pygccxml.__version__, color='GREEN')
                else:
                    conf.check_message_custom('pygccxml version', '', 'Version ' + pygccxml.__version__ + ' too old', color='YELLOW')
                    conf.fatal("Configure Failed, pygccxml version too old; please use at least version 1.5.0")
            else:
                conf.check_message_custom('pygccxml version', '', 'Version ' + pygccxml.__version__ + ' too old', color='YELLOW')
                conf.fatal("Configure Failed, pygccxml version too old; please use at least version 1.5.0")
        except IndexError:
            conf.fatal("Configure Failed, unable to determine pygccxml version. Please check that you have version 1.5.0 correctly installed.")
        except ValueError:
            conf.fatal("Configure Failed, unable to determine pygccxml version. Please check that you have version 1.5.0 correctly installed.")

    ##################################################
    # Check for Py++
    ##################################################
    try:
        import pyplusplus
    except:
        if Logs.verbose:
            import traceback
            traceback.print_exc(file=sys.stderr)
        conf.check_message_custom('module', 'pyplusplus', 'Not found', color='YELLOW')
        conf.fatal("Configure Failed, please install pypluplus as described in the online documentation")
    else:
        conf.check_message_custom('module', 'pyplusplus', 'ok', color='GREEN')
        try:
            version = pyplusplus.__version__.split('.')
            if Logs.verbose:
                print 'Version ' + str(version)
            if int(version[0]) >= 1:
                if int(version[1]) >= 5 and int(version[2]) >= 0:
                    conf.check_message_custom('pyplusplus version', '', 'Version ' + pyplusplus.__version__, color='GREEN')
                else:
                    conf.check_message_custom('pyplusplus version', '', 'Version ' + pyplusplus.__version__ + ' too old', color='YELLOW')
                    conf.fatal("Configure Failed, pyplusplus version too old; please use at least version 1.5.0")
            else:
                conf.check_message_custom('pyplusplus version', '', 'Version ' + pyplusplus.__version__ + ' too old', color='YELLOW')
                conf.fatal("Configure Failed, pyplusplus version too old; please use at least version 1.5.0")
        except IndexError:
            conf.fatal("Configure Failed, unable to determine pyplusplus version. Please check that you have version 1.5.0 correctly installed.")
        except ValueError:
            conf.fatal("Configure Failed, unable to determine pyplusplus version. Please check that you have version 1.5.0 correctly installed.")

    conf.check_tool( 'template_builder', './tools/waf'  )

    conf.env['PYPLUSPLUS_EXT'] = ['.hpp','.h']


included_decls = ''

import threading
logger_lock = threading.Lock()

def dopypp(task):

    global included_decls

    # Set the path for importing additional tools
    if not os.path.join(task.srcpath, 'tools', 'waf') in sys.path:
        sys.path.append(os.path.join(task.srcpath, 'tools', 'waf'))

    # Increase recursion limit for tough includes
    sys.setrecursionlimit(sys.getrecursionlimit()*2)

    from pyplusplus import module_builder, messages, utils, code_repository
    from pygccxml import declarations,parser
    from pyplusplus.module_builder import call_policies
    from pypp_utils import TemplateBuilder
    import logging

    # Disable some annoying messages
    if not Logs.verbose:
        module_builder.set_logger_level(logging.FATAL)
        messages.disable(messages.W1023)
        messages.disable(messages.W1025)
        messages.disable(messages.W1026)
        messages.disable(messages.W1027)
        messages.disable(messages.W1031)
        messages.disable(messages.W1043)
    elif Logs.verbose > 2:
        module_builder.set_logger_level(logging.DEBUG)

    ## ????????????????????????????????????? What is this for ?????????????????????????????
    logger_lock.acquire()
    log = logging.getLogger()
    temp_handler = None
    if len(log.handlers) > 0:
        temp_handler = log.handlers[0]
        log.removeHandler(log.handlers[0])
    logger_lock.release()

    # Create source list with proper directories
    sources = map(lambda a: os.path.abspath(os.path.join(task.bldpath, a.srcpath(task.env))), task.inputs)

    # Create include list
    global_includes = task.includes
    global_includes += task.env['CPPPATH']
    global_includes += [os.path.abspath(os.path.join(task.bldpath, i.srcpath(task.env))) for i in task.path_lst]

    try:
        if int(task.env['CC_VERSION'][1]) >= 3:
            compilerExec = task.env['CXX_OLD_VERSION'][0]
        else:
            compilerExec = task.env['CXX'][0]
        mb = module_builder.module_builder_t(
                                files=sources
                                , start_with_declarations = task.start_decls
                                , define_symbols = task.define_symbols
                                , working_directory = task.bldpath
                                , include_paths = global_includes
                                , compiler = compilerExec
                                , cache=parser.file_cache_t(os.path.abspath(os.path.join(task.bldpath, task.outputs[0].bldpath(task.env)+'_cache' ))))
    except Exception, e:
        print e
        if temp_handler:
            log.addHandler(temp_handler)
        return 1

    # Restore the recursion limit to its previous value
    sys.setrecursionlimit(sys.getrecursionlimit()/2)

    # Append this task to the pypp task list, so that, at library
    # loading time.
    global createdLibraries
    if createdLibraries is None:
        if os.path.exists(os.path.join(task.bldpath, '.wrapper_order.py')):
            wrapperOrderFile = open(os.path.join(task.bldpath, '.wrapper_order.py'))
            createdLibraries = [a.strip() for a in wrapperOrderFile.readlines()]
            wrapperOrderFile.close()
        else:
            createdLibraries = []
    libraryName = os.path.join(task.bldpath, os.path.splitext(os.path.splitext(task.outputs[-1].bldpath(task.env))[0])[0] + task.env['shlib_PATTERN'].split('%s')[1])
    if not libraryName in createdLibraries:
        wrapperOrderFile = open(os.path.join(task.bldpath, '.wrapper_order.py'), 'a')
        wrapperOrderFile.write(libraryName + '\n')
        wrapperOrderFile.close()

    # Set a standard name for every declaration
    try:
        for decl in mb.classes():
            from pyplusplus.decl_wrappers.algorithm import create_valid_name
            decl.alias = create_valid_name(decl.name)
    except:
        pass

    # Disable virtuality unless specifically asked to do otherwise: this should avoid overrides
    # and increase simulation speed
    if( not task.virtuality ):
        members = mb.global_ns.calldefs( declarations.virtuality_type_matcher( declarations.VIRTUALITY_TYPES.VIRTUAL  ) , allow_empty=True)

        if( members ):
            members.set_virtuality( declarations.VIRTUALITY_TYPES.NOT_VIRTUAL )

    # Execute includes user-specified code
    for customCodeBlock in task.custom_code_include:
        try:
            exec customCodeBlock in locals()
        except:
            if Logs.verbose:
                import traceback
                traceback.print_exc()
                print '\nWhile Processing FILEs: \n' +  str(sources) + '\nLines: ' + str(customCodeBlock) + '\n'

    # Execute user-specified code
    try:
        for customCodeBlock in task.custom_code:
            exec customCodeBlock in locals()
    except:
        import traceback
        traceback.print_exc()
        print '\nWhile Processing FILEs: \n' +  str(sources) + '\nLines: ' + customCodeBlock + '\n'
        return 1

    # Now I protect the virtual member calls so that they behave correctly in a multi-threaded environment
    members = mb.global_ns.calldefs( (declarations.virtuality_type_matcher(declarations.VIRTUALITY_TYPES.VIRTUAL) |
                                    declarations.virtuality_type_matcher(declarations.VIRTUALITY_TYPES.PURE_VIRTUAL)) &
                                    declarations.custom_matcher_t( lambda decl: decl.ignore == False and decl.exportable ), allow_empty=True)
    if( members ):
        for member in members:
            try:
                member.add_override_precall_code('\npyplusplus::threading::gil_guard_t gil_guard( resp::sc_controller::getController().interactive );')
                if not code_repository.gil_guard.file_name in task.ext_headers:
                    task.ext_headers.append(code_repository.gil_guard.file_name)
                controller_path = os.path.join(task.srcpath, 'src', 'controller', 'controller.hpp')
                if not controller_path in task.ext_headers:
                    task.ext_headers.append(controller_path)
                if not os.path.exists(os.path.abspath(os.path.join( task.bldpath, os.path.dirname(task.outputs[0].bldpath(task.env)),  code_repository.gil_guard.file_name) )):
                    guard = open(os.path.abspath(os.path.join( task.bldpath, os.path.dirname(task.outputs[0].bldpath(task.env)),  code_repository.gil_guard.file_name) ), 'w')
                    guard.write(code_repository.gil_guard.code)
                    guard.close()
            except AttributeError:
                pass

    #Lets import the doxygen module and create the doc_extractor object
    import doxygen_extractor
    docExtractor = doxygen_extractor.doc_extractor()

    # Add declaration and registration code
    mb.add_registration_code( task.custom_registration_code, False )
    mb.add_declaration_code( task.custom_declaration_code )

    # Register module dependency
    module_list = []
    for inc in task.include:
        new_module = os.path.dirname(inc.bldpath(task.env))
        if new_module not in module_list:
            if Logs.verbose:
                print "Adding "+os.path.dirname(inc.bldpath(task.env))+" to module dependencies"
            mb.register_module_dependency(new_module)
            module_list.append(new_module)


    # Create code and assign moduyle name
    mb.build_code_creator(os.path.splitext(task.target)[0], doc_extractor=docExtractor)

    # Write the module to the target file(s)
    #print "Writing file %s:"  % (os.path.join( os.path.abspath('.'), task.m_outputs[k].bldpath(task.m_env) ) )
    #from pyplusplus import file_writers

    # Replace headers to avoid the inclusion of autogenerated template instantiation files
    # TODO: Discover the files where special sc_* or tlm_* templates are declared
    mb.code_creator.replace_included_headers(task.ext_headers)

    try:
        if task.split == 1:
            mb.write_module( os.path.abspath(os.path.join( task.bldpath, task.outputs[0].bldpath(task.env) ) ))
        else:
            from balanced_files import resp_files_t
            try:
                mb._builder_t__merge_user_code()
            except:
                mb._module_builder_t__merge_user_code()
            mfs = resp_files_t( mb.code_creator
                                    ,os.path.abspath(os.path.join( task.bldpath, os.path.dirname(task.outputs[0].bldpath(task.env))))
                                    ,task.split-1
                                    ,files_sum_repository=None
                                    ,encoding=mb.encoding )
            mfs.write()

            # When pyplusplus will be updated, this will be the syntax
            #mb.balanced_split_module( os.path.abspath(os.path.join( task.bldpath ,os.path.dirname(task.outputs[0].bldpath(task.env))))
            #                        ,task.split-1, on_unused_file_found=lambda fpath: fpath )
    except Exception, e:
        print e
        if temp_handler: log.addHandler(temp_handler)
        return 1

    if temp_handler: log.addHandler(temp_handler)
    return 0
