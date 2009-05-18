# Copyright 2004 Giovanni Beltrame
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

"""defines a class that writes L{code_creators.module_t} to multiple files"""

import os
import math
from pyplusplus.file_writers import balanced_files
from pyplusplus import messages
from pyplusplus import _logging_
from pygccxml import declarations
from pyplusplus import decl_wrappers
from pyplusplus import code_creators
from pyplusplus.utils import split_sequence

#TODO: to add namespace_alias_t classes
class resp_files_t(balanced_files.balanced_files_t):
    """
    This class splits only classes and not the rest
    """
    HEADER_EXT = '.pypp.hpp'
    SOURCE_EXT = '.pypp.cpp'

    def __init__( self
                  , extmodule
                  , directory_path
                  , number_of_buckets
                  , write_main=True
                  , files_sum_repository=None
                  , encoding='ascii'):
        """Constructor.

        @param extmodule: The root of a code creator tree
        @type extmodule: module_t
        @param directory_path: The output directory where the source files are written
        @type directory_path: str

        @param write_main:  if it is True, the class will write out a main file
            that calls all the registration methods.
        @type write_main: boolean
        """
        balanced_files.balanced_files_t.__init__( self, extmodule, directory_path, number_of_buckets,  write_main, files_sum_repository, encoding)

    def split_classes( self ):
        class_creators = filter( lambda x: isinstance(x, ( code_creators.class_t, code_creators.class_declaration_t ) )
                                 , self.extmodule.body.creators )

        class_creators = filter( lambda cc: not cc.declaration.already_exposed
                                 , class_creators )

        buckets = split_sequence(class_creators, len(class_creators)/self.number_of_buckets )
        if len(buckets) > self.number_of_buckets:
            buckets[len(buckets)-2] += buckets[len(buckets)-1]
            buckets = buckets[:len(buckets)-1]

        for index, bucket in enumerate( buckets ):
            self.split_creators( bucket
                                 , '_classes_%d' % (index+1)
                                 #,'_register_classes_%d' % (index+1)
                                 , self.extmodule.body.name +'_register_classes_%d' % (index+1)
                                 , -1 )


    #TODO: move write_main to __init__
    def write(self):
        """ Write out the module.
            Creates a separate source/header combo for each class and for enums, globals,
            and free functions.
            If write_main is True it writes out a main file that calls all the registration methods.
            After this call split_header_names and split_method_names will contain
            all the header files and registration methods used.  This can be used by
            user code to create custom registration methods if main is not written.
        """

        self.write_code_repository( self._multiple_files_t__directory_path )
        self.save_exposed_decls_db( self._multiple_files_t__directory_path )

        self.extmodule.do_include_dirs_optimization()

        #self.split_values_traits()
        self.split_classes()
        #self.split_enums()
        #self.split_global_variables()
        #self.split_free_functions()

        if self.write_main:
            self.include_creators.sort( cmp=lambda ic1, ic2: cmp( ic1.header, ic2.header ) )
            map( lambda creator: self.extmodule.adopt_include( creator )
                 , self.include_creators )
            main_cpp = os.path.join( self.directory_path, self.extmodule.body.name + '.main.cpp')
            self.write_file( main_cpp, self.extmodule.create() + os.linesep)
        self.files_sum_repository.save_values()
