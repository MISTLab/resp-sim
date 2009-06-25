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

"""Word completion for GNU readline 2.0.

This requires the latest extension to the readline module. The completer
completes keywords, built-ins and globals in a selectable namespace (which
defaults to __main__); when completing NAME.NAME..., it evaluates (!) the
expression up to the last dot and completes its attributes.

It's very cool to do "import sys" type "sys.", hit the
completion key (twice), and see the list of names defined by the
sys module!

Tip: to use the tab key as the completion key, call

    readline.parse_and_bind("tab: complete")

Notes:

- Exceptions raised by the completer function are *ignored* (and
generally cause the completion to fail).  This is a feature -- since
readline sets the tty device in raw (or cbreak) mode, printing a
traceback wouldn't work well without some complicated hoopla to save,
reset and restore the tty state.

- The evaluation of the NAME.NAME... form may cause arbitrary
application defined code to be executed if an object with a
__getattr__ hook is found.  Since it is the responsibility of the
application (or the user) to enable this feature, I consider this an
acceptable risk.  More complicated expressions (e.g. function calls or
indexing operations) are *not* evaluated.

- GNU readline is also used by the built-in functions input() and
raw_input(), and thus these also benefit/suffer from the completer
features.  Clearly an interactive application can benefit by
specifying its own completer function and using raw_input() for all
its input.

- When the original stdin is not a tty device, GNU readline is never
used, and this module (and the readline module) are silently inactive.

"""

import __builtin__
import __main__

__all__ = ["Completer"]

class Completer:
    def __init__(self, namespace = None):
        """Create a new completer for the command line.

        Completer([namespace]) -> completer instance.

        If unspecified, the default namespace where completions are performed
        is __main__ (technically, __main__.__dict__). Namespaces should be
        given as dictionaries.

        Completer instances should be used as the completion mechanism of
        readline via the set_completer() call:

        readline.set_completer(Completer(my_namespace).complete)
        """

        if namespace and not isinstance(namespace, dict):
            raise TypeError,'namespace must be a dictionary'

        self.matches = []

        # Don't bind to namespace quite yet, but flag whether the user wants a
        # specific namespace or to use __main__.__dict__. This will allow us
        # to bind to __main__.__dict__ at completion time, not now.
        if not namespace:
            self.use_main_ns = 1
        else:
            self.use_main_ns = 0
            self.namespace = namespace

    def complete(self, text, state):
        """Return the next possible completion for 'text'.

        This is called successively with state == 0, 1, 2, ... until it
        returns None.  The completion should begin with 'text'.

        """
        #The completion is performed onl;y when the simulation is stopped
        if 'controller' in self.namespace.keys():
            if (not self.namespace['controller'].is_paused()) and (not self.namespace['controller'].isEnded()) and (self.namespace['controller'].is_running()):
                return None
        if self.use_main_ns:
            self.namespace = __main__.__dict__

        if state == 0:
            if "." in text and not text.startswith('load_architecture'):
                quote1 = text.find('\'')
                quote2 = text.find('\"')
                bracket = text.find('(')
                space = text.find(' ')
                if quote1 > bracket:
                    delim = quote1
                else:
                    delim = bracket
                if quote2 > delim:
                    delim = quote2
                if space > delim:
                    delim = space
                if delim != -1:
                    self.matches = []
                    tempRes = self.attr_matches(text[delim + 1:])
                    for i in tempRes:
                        self.matches.append(text[:delim + 1] + i)
                else:
                    self.matches = self.attr_matches(text)
            else:
                if text.startswith('load_architecture'):
                    try:
                        self.matches = self.loadArchMatch(text)
                    except Exception,  e:
                        print 'Error ' + str(e)
                else:
                    quote1 = text.find('\'')
                    quote2 = text.find('\"')
                    bracket = text.find('(')
                    space = text.find(' ')
                    if quote1 > bracket:
                        delim = quote1
                    else:
                        delim = bracket
                    if quote2 > delim:
                        delim = quote2
                    if space > delim:
                        delim = space
                    if delim != -1:
                        self.matches = []
                        tempRes = self.global_matches(text[delim + 1:])
                        for i in tempRes:
                            self.matches.append(text[:delim + 1] + i)
                    else:
                        self.matches = self.global_matches(text)
        try:
            return self.matches[state]
        except:
            return None

    def global_matches(self, text):
        """Compute matches when text is a simple name.

        Return a list of all keywords, built-in functions and names currently
        defined in self.namespace that match.

        """
        #print 'gbl match'
        import keyword
        matches = []
        n = len(text)
        for list in [keyword.kwlist,
                     __builtin__.__dict__,
                     self.namespace]:
            for word in list:
                if word[:n] == text and word != "__builtins__":
                    matches.append(word)
        return matches

    def loadArchMatch(self, text):
        """Compute matches when text is a simple name.

        Return a list of all keywords, built-in functions and names currently
        defined in self.namespace that match.

        """
        #print 'load a rch match'
        import os
        import re
        matches = []
        remText = text[len('load_architecture'):]
        matchStartStr = re.match('\(( )*(\'|\")?', remText)
        if len(remText) == 0 or (matchStartStr and matchStartStr.end() == len(remText)):
            files = os.listdir('.')
            files = filter( lambda x: ((x.endswith('.py') or os.path.isdir(x)) and not x.startswith('.')) , files )
            if not '(' in text:
                text += '(\''
            elif not '\"' in text and not '\'' in text:
                text += '\''
            for i in files:
                matches.append(text + i)
            return matches
        if matchStartStr:
            if remText.find('.py') == -1:
                partialPath = remText[matchStartStr.end():]
                #Ok, now I simply have to complete the path
                lastSep = partialPath.rfind(os.sep)
                if lastSep == -1 and not os.path.exists(partialPath):
                    # I simply complete the current path
                    files = os.listdir('.')
                    files = filter( lambda x: ((x.endswith('.py') or os.path.isdir(x)) and x.startswith(partialPath)), files )
                    for i in files:
                        matches.append('load_architecture' + remText[:matchStartStr.end()] + i)
                    return matches
                else:
                    #I have to move in the specified directory and complete the path
                    if lastSep != -1:
                        crPath = partialPath[:lastSep]
                    else:
                        crPath = partialPath
                    if os.path.exists(crPath):
                        files = os.listdir(crPath)
                        if lastSep != -1 and lastSep < len(partialPath) - 1:
                            files = filter( lambda x: ((x.endswith('.py') or os.path.isdir(os.path.join(crPath, x))) and x.startswith(partialPath[lastSep + 1:])), files )
                        else:
                            files = filter( lambda x: (x.endswith('.py') or os.path.isdir(os.path.join(crPath, x))), files )
                        for i in files:
                            matches.append('load_architecture' + remText[:matchStartStr.end()] + crPath + os.sep + i)
                        return matches

        return matches

    def attr_matches(self, text):
        """Compute matches when text contains a dot.

        Assuming the text is of the form NAME.NAME....[NAME], and is
        evaluatable in self.namespace, it will be evaluated and its attributes
        (as revealed by dir()) are used as possible completions.  (For class
        instances, class members are also considered.)

        WARNING: this can still invoke arbitrary C code, if an object
        with a __getattr__ hook is evaluated.

        """
        #print 'attrbiute match'
        import re
        m = re.match(r"(\w+(\.\w+)*)\.(\w*)", text)
        if not m:
            return
        expr, attr = m.group(1, 3)
        object = eval(expr, self.namespace)
        words = dir(object)
        if hasattr(object,'__class__'):
            words.append('__class__')
            words = words + get_class_members(object.__class__)
        matches = []
        n = len(attr)
        for word in words:
            if word[:n] == attr and word != "__builtins__":
                matches.append("%s.%s" % (expr, word))
        return matches

def get_class_members(klass):
    ret = dir(klass)
    if hasattr(klass,'__bases__'):
        for base in klass.__bases__:
            ret = ret + get_class_members(base)
    return ret

try:
    import readline
except ImportError:
    pass
else:
    readline.set_completer(Completer().complete)
