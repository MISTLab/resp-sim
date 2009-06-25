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


"""
extracting from C++ doxygen documented file
Author G.D.
"""

class doc_extractor:
    """
    extracts doxigen styled documentation from source
    or generates from description
    """
    def __init__(self):
        #for caching source
        self.file_name  = None
        self.source = None

    def __call__(self, declaration):
        try:
            if self.file_name != declaration.location.file_name:
                self.file_name = declaration.location.file_name
                self.source = open(declaration.location.file_name).readlines()

            find_block_end = False
            doc_lines = []
            for lcount in xrange(declaration.location.line - 2, -1, -1):
                line = self.source[lcount]
                if not find_block_end:
                    try:
                        if line.rstrip()[-2:] == "*/":
                            find_block_end = True
                    except:
                        pass
                if  find_block_end:
                    try:
                        if line.lstrip()[:2] == "/*":
                            find_block_end = False
                    except:
                        pass
                final_str = clear_str(line)
                if not find_block_end and code(line):
                    break
                if final_str:
                    doc_lines.insert(0, final_str)

            if len(doc_lines) > 0:
                doc_lines.insert(0, self.get_generic_doc(declaration))
                return '\"' + ''.join(doc_lines) + '\"\n'
        except:
            pass

        return '\"' + self.get_generic_doc(declaration) + '\"\n'

    def get_generic_doc(self, declaration):
        """
        generate call information about function or method
        """
        try:
            retStr = ''
            try:
                retStr += str(declaration.type) + ' '
            except:
                pass
            retStr += str(declaration)
            return "Help on %s" % retStr.replace('\"', '\\\"')
        except:
            return ''

def clear_str(str):
    """
    replace */! by Space and \breaf, \fn, \param, ...
    """
    clean = lambda str, sym, change2 = '': str.replace(sym, change2)

    str = reduce(clean, [str, '/', '*', '!', "\brief", "\fn",\
     "@brief", "@fn", "@ref", "\ref"])

    str = clean(str, "@param", "Param: ")
    str = clean(str, "\param", "Param: ")
    str = clean(str, "@ingroup", "Group")
    str = clean(str, "\ingroup", "Group")
    str = clean(str, "@return", "It returns")
    str = clean(str, "\return", "It returns")

    str = str.replace("\"", "\"\"")

    return "  " + (str.lstrip()).rstrip('\n ')


def code(str):
    """
    detect str is code?
    """
    try:
        beg = str.lstrip()[:2]
        return  beg != "//" and beg != "/*"
    except:
        pass
    return False

if __name__ == '__main__':
    class loc:
        def __init__(self, f, l):
            self.file_name = f
            self.line = l

    class x_decl:
        def __init__(self, str, file_name, line):
            self.str  = str
            self.location = loc(file_name, line)

        def __str__(self):
            return self.str

    print doc_extractor()(x_decl("myfunc(int x, int y)","core.h",45))
    print doc_extractor()(x_decl("","core.h",209))
