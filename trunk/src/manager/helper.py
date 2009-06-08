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

""" Module containing some generic helper methods used in the manager classes"""

from connectionNode import ConnectionNode
from exc import *
#import types
import string
import sys
import exceptions

def getClass(className, symbolNamespace):
    """Given a class name it looks for the instance of the class itself; it looks
    inside the ArchC wrapper and in all the elements in the component folder"""
    for i in symbolNamespace:
        try:
            if className in dir(i):
                return getattr(i, className)
        except:
            pass
    return None

def getAttrInstance(path, env):
    """Given the string representing the full path of an attribute inside an object,
    it returns the instance of the object and the name of the attribute; the
    instance of the object pointed by path must be present in the current namespace"""
    if not path:
        print 'Error, the path of which we are trying to get an instance is None'
        sys.exit()
    splitPath = path.split('.')
    attributeName = splitPath.pop()
    
    try:
        objInstance = env[splitPath.pop(0)]
        for pathElem in splitPath:
            objInstance = getattr(objInstance, pathElem)
    except Exception, e:
        print 'Unable to get attribute ' + path + ' --> ' + str(e)
        sys.exit()
        
    return [objInstance, attributeName]

def getAttrInstance2(component, attribute):
    """Given a component instance reference and a string representing the path of 
    the attribute in the object, it return the reference of the attribute"""
    if type(attribute) is not str:
        raise exceptions.Exception(str(attribute) + 'is not a valid name of an attribute')
    try:
        splittedAttributeName = attribute.split('.')
        attributeRef = component
        for i in splittedAttributeName:
            attributeRef = getattr(attributeRef, i)
    except:
            raise exceptions.Exception('There is no attribute with name ' + attribute + ' in the component ' + str(component))
    return attributeRef


def getInstance(name, params, components):
    """given the name of a class it builds an instance
    of that class and returns it; the name itself is returned if no
    class matching name is found in the global namespace"""
    # first of all, name may contain the indication of a module
    # so I split it to separate the module indications from
    # the real name of the class
    modNames = name.split('.')
    modNamesRev = modNames
    modNamesRev.reverse()
    foundNameSpace = None
    found = False
    for elemMod in modNamesRev:
        if found:
            break
        for comp in components:
            if elemMod == comp:
                foundNameSpace = modNames[modNames.index(elemMod) + 1:]
                curModule = comp
                found = True
                break
    
    # Now I have to go up in the namespace of the object until
    # I get to the object itself: then I can create the necessary
    # instance
    if not foundNameSpace:
        return name
    for modules in foundNameSpace:
        try:
            curModule = getattr(curModule, modules)
        except Exception:
            return name
    return curModule()

def getBase(startingName, classToCheck):
    """given an object, it checks if in its inheritance chain there
    is a class whose name is starting with startingName; in case it
    is found the name itself is returned, otherwise an exception is
    raised"""
    if classToCheck.__name__.startswith(startingName):
        return classToCheck.__name__
    else:
        baseClasses = classToCheck.__bases__
        if len(baseClasses) == 0:
            raise BaseNotFound('No classes in the inheritance chain of ' +
                               classToCheck.__name__ + ' starts with ' + startingName)
        foundBases = []
        for base in baseClasses:
            foundBases.append(getBaseRecusrive(startingName, base))
        
        for curBase in foundBases:
            if curBase != '':
                return curBase
        
        raise BaseNotFound('No classes in the inheritance chain of ' +
                           classToCheck.__name__ + ' starts with ' + startingName)

        
def getBaseRecusrive(startingName, classToCheck):
    """given an object, it checks if in its inheritance chain there
    is a class whose name is starting with startingName; in case it
    is found the name itself is returned"""
    if classToCheck.__name__.startswith(startingName):
        return classToCheck.__name__
    else:
        baseClasses = classToCheck.__bases__

        foundBases = []
        for base in baseClasses:
            foundBases.append(getBaseRecusrive(startingName, base))
        
        for curBase in foundBases:
            if curBase != '':
                return curBase
        
        return ''

def getTLMInPort(object):
    ports = []
    if hasattr(object, "get_child_objects"):
        vet = object.get_child_objects()
        for i in range(0,len(vet)):
            curr = vet[i]
            if hasattr(curr,'__class__'):
                if isTLMInPort(curr.__class__):
                    ports.append(curr)
            else:
                subports = getTLMInPort(curr)
                for sp in subports:
                    ports.append(sp)
    return ports        

def getTLMOutPort(object):
    ports = []
    if hasattr(object, "get_child_objects"):
        vet = object.get_child_objects()
        for i in range(0,len(vet)):
            curr = vet[i]
            if hasattr(curr,'__class__'):
                if isTLMOutPort(curr.__class__):
                    ports.append(curr)
                else:
                    subports = getTLMOutPort(curr)
                    for sp in subports:
                        ports.append(sp)
    return ports        

def isTLMInPort(portClass):
    """Given a class it returns true if this can be a TLM port,
    false otherwise"""
    try:
        getBase('tlm_target_port', portClass)
        return True
    except:
        return False

def isTLMOutPort(portClass):
    """Given a class it returns true if this can be a TLM port,
    false otherwise"""
    try:
        getBase('tlm_initiator_port', portClass)
        return True
    except:
        return False
    
def isSystemCInPort(portClass):
    """Given a class it returns true if this can be a SystemC port,
    false otherwise"""
    try:
        getBase('sc_in', portClass)
        return True
    except:
        return False

def isSystemCOutPort(portClass):
    """Given a class it returns true if this can be a SystemC port,
    false otherwise"""
    try:
        getBase('sc_out', portClass)
        return True
    except:
        return False

def isSystemCInOutPort(portClass):
    """Given a class it returns true if this can be a SystemC port,
    false otherwise"""
    try:
        getBase('sc_inout', portClass)
        return True
    except:
        return False

def isSystemCSignal(portClass):
    """Given a class it returns true if this can be a SystemC port,
    false otherwise"""
    try:
        getBase('sc_signal', portClass)
        return True
    except:
        return False

def extractFunctionRetVal(function):
    """Given a function it extracts a string representing its
    C++ return value"""
    #In order to determine the attribute type, I use the docstring
    if not '__doc__' in dir(function):
        return ('', '')
    docStr = function.__doc__
    if not docStr:
        return ('', '')
    retValStart = docStr.find(' -> ')
    if retValStart == -1:
        return ('', '')
    retValEnd = docStr.find(' ', retValStart + 5)
    if retValEnd == -1:
        return ('', '')
    docStr = docStr[retValStart + 4:retValEnd]
    #Ok: now my string contains the return value with cbrace, comma etc
    #substrings: I have to replace them
    pyDocString = docStr
    docStr = docStr.replace('_scope_', '::')
    docStr = docStr.replace('_comma__', ', ')
    docStr = docStr.replace('_comma_', ',')
    docStr = docStr.replace('_less_', '<')
    docStr = docStr.replace('_greater__', '> ')
    docStr = docStr.replace('__greater_', ' >')
    docStr = docStr.replace('_greater_', '>')
    docStr = docStr.replace('_obrace_', '(')
    docStr = docStr.replace('_cbrace_', ')')
    return (docStr, pyDocString)

def extractAttrType(attribute, classNames):
    """Given an attribute it extracts a string representing its C++ type"""
    #In order to determine the attribute type, I use the docstring
    if not '__doc__' in dir(attribute):
        return ''
    docStr = attribute.__doc__
    if not docStr:
        return ''
    if docStr.find('Help on ') != 0:
        return ''
    docStr = docStr.replace('Help on ','')
    #Ok: now my string is separated into three parts: the type of the attribute,
    #the name of the attribute and the documentation description; I'm interested in
    #the type
    varId = docStr.find('[variable]')
    if varId == -1:
        return ''
    docStr = docStr[:varId - 1]
    #Ok, Now I have the type and then the variable name: I look for the name
    #of the class containing this method: this is the end of the atribute name
    lastSpace = -1
    for className in classNames:
        lastSpace = docStr.rfind(className + '<')
        if lastSpace == -1:
            lastSpace = docStr.rfind(className + ':')
        if lastSpace == -1:
            continue
        else:
            break

    if lastSpace == -1:
        return ''
    #print ' DOC before ' + docStr
    docStr = docStr[:lastSpace -1]
    #print ' DOC after ' + docStr
    import re
    namspaceSpec = re.compile('[a-zA-Z_]\w*::')
    match = namspaceSpec.match(docStr)
    if not match:
        return docStr
    else:
        return docStr[match.end():]

def extractClassName(classs):
    """Given an attribute it extracts a string representing its C++ type"""
    #In order to determine the attribute type, I use the docstring
    if not '__doc__' in dir(classs):
        return ''
    docStr = classs.__doc__
    if not docStr:
        return ''
    if docStr.find('Help on ') != 0:
        return ''
    docStr = docStr.replace('Help on ','')
    #Ok: now my string is separated into three parts: the type of the attribute,
    #the name of the attribute and the documentation description; I'm interested in
    #the type
    varId = docStr.find('[class]')
    if varId == -1:
        return ''
    docStr = docStr[:varId - 1]
    #Ok, Now I have the type and then the variable name: I look for the name
    #of the class containing this method: this is the end of the atribute name
    templStart = docStr.find('<')
    if templStart == -1:
        return docStr
    return docStr[:templStart]

def extractClassNames(classs):
    """Given an attribute it extracts a list of strings, each one representing one of its C++ types
    in the type hierarchy"""
    namesList = []
    if not '__doc__' in dir(classs):
        return namesList
    foundName = extractClassName(classs)
    if foundName != '':
        namesList.append(foundName)
    try:
        baseCl = classs.__bases__
    except:
        return namesList
    for i in baseCl:
        namesList += extractClassNames(i)
    return namesList
    
def extractConstrSig(constructor):
    """Given an attribute it extracts a string representing its C++ type"""
    #In order to determine the attribute type, I use the docstring
    if not '__doc__' in dir(constructor):
        return []
    docStr = constructor.__doc__
    if not docStr:
        return []
    try:
        docStr = docStr.split('\n')[1]
    except:
        return []
    startParams = docStr.find('arg1,')
    if startParams == -1:
        return []
    endParams = docStr.find(') -> None')
    if endParams == -1:
        return []
    docStr = docStr[startParams + 6:endParams]
    defValBegin = docStr.find('[')
    if defValBegin != -1:
        normal_params = docStr[:defValBegin]
        def_params = docStr[defValBegin + 1:-1]
    else:
        normal_params = docStr
        def_params = ''
    # Now I start separating the parameters
    params = []
    for param in normal_params.split(','):
        params.append(param.lstrip().rstrip())
    for param in def_params.split(','):
        equal = param.find('=')
        if equal != -1:
            params.append((param[:equal].lstrip().rstrip(), param[equal + 1:].lstrip().rstrip()))
    return params
    
def getTLMPortType(portName, type = None):
    """it parses the name of the port to extract the type of the
    data communicated over the port; the name of the type is returned
    type is a string that specifies which port must be returned: if
    type == 'target' then only the type for TLM target ports must
    be returned, if type == 'source' then only the type for TLM
    source ports, if type == None all the ports"""
    tempStr = string.split(portName, '_less_')
    # ok, now the first element of the list should be the type of
    # the port, then the second element should be the type of the
    # template.
    if len(tempStr) < 2:
        raise NotAPort(portName + 'doesn\'t represent a TLM port: no _less_ found')
    if tempStr[0] == 'tlm_initiator_port' and (type == 'source' or not type):
        tempStr = string.split(tempStr[1], '_greater_')
        if len(tempStr) < 2:
            raise NotAPort(portName + 'doesn\'t represent a TLM port: no _greater_ found')
        templTypes = string.split(tempStr[0], '_comma_')
        if len(templTypes) < 2:
            raise NotAPort(portName + 'doesn\'t represent a TLM port: no _comma_ found')
        return templTypes[0]
    elif tempStr[0] == 'tlm_target_port' and (type == 'target' or not type):
        templTypes = string.split(tempStr[1], '_greater_')
        if len(tempStr) < 2:
            raise NotAPort(portName + 'doesn\'t represent a TLM port: no _greater_ 2 found')
        return templTypes[0]
    else:
        raise NotAPort(portName + 'doesn\'t represent a TLM port')
    
def getSysCPortType(portName):
    """it parses the name of the port to extract the type of the
    data communicated over the port; the name of the type is returned"""
    tempStr = string.split(portName, '_less_')
    # ok, now the first element of the list should be the type of
    # the port, then the second element should be the type of the
    # template.
    if len(tempStr) < 2:
        raise NotAPort(portName + 'doesn\'t represent a SystemC port: no _less_ found')
    if tempStr[0] == 'sc_in':
        templTypes = string.split(tempStr[1], '_greater')
        if len(templTypes) < 2:
            raise NotAPort(portName + 'doesn\'t represent a SystemC port: no _greater found')
        return templTypes[0]
    elif tempStr[0] == 'sc_out':
        templTypes = string.split(tempStr[1], '_greater')
        if len(tempStr) < 2:
            raise NotAPort(portName + 'doesn\'t represent a SystemC port: no _greater 2 found')
        return templTypes[0]
    elif tempStr[0] == 'sc_inout':
        templTypes = string.split(tempStr[1], '_greater')
        if len(tempStr) < 2:
            raise NotAPort(portName + 'doesn\'t represent a SystemC port: no _greater 3 found')
        return templTypes[0]
    else:
        raise NotAPort(portName + 'doesn\'t represent a SystemC port')

def getSysCSignalType(signalName):
    """it parses the name of the port to extract the type of the
    data communicated over the port; the name of the type is returned"""
    tempStr = string.split(signalName, '_less_')
    # ok, now the first element of the list should be the type of
    # the port, then the second element should be the type of the
    # template.
    if len(tempStr) < 2:
        raise NotAPort(signalName + 'doesn\'t represent a SystemC signal: no _less_ found')
    if tempStr[0] == 'sc_signal':
        templTypes = string.split(tempStr[1], '_greater')
        if len(templTypes) < 2:
            raise NotAPort(signalName + 'doesn\'t represent a SystemC signal: no _greater found')
        return templTypes[0]
    else:
        raise NotAPort(signalName + 'doesn\'t represent a SystemC signal')

def findConnectionsRecv(comp, realComp, pathList,  scwrapper, symbolNamespace):
    """Given a component and its symbolic representation
    comp, it examines component and all the classes of its attributes
    looking for SystemC or TLM ports. The list represents the list of
    the attributes currently followed starting from the main
    component"""
    import pyplusplus
    import component
    try:
        getBase('instance', realComp)
    except:
        return
    for i in dir(realComp):
        try:
            attr = getattr(realComp, i)
            if callable(attr):
                # I have to check if the return value is a port: in case it is a port generator
                retVal, pyRetVal = extractFunctionRetVal(attr)
                if pyRetVal in dir(scwrapper):
                    #ok, it is a SystemC or TLM component: now I have to check which
                    #kind of port it is
                    pyRetVal = getattr(scwrapper, pyRetVal)
                    cm = component.ClassMethod()
                    cm.name = i
                    cm.instance = attr
                    cm.retTypeName = retVal
                    cm.pyRetType = pyRetVal
                    cm.path = pathList
                    if isTLMInPort(pyRetVal):
                        # Is it a TLM input port
                        comp.in_tlm_port_gens.append(cm)
                    elif isTLMOutPort(pyRetVal):
                        # Is it a TLM output port
                        comp.out_tlm_port_gens.append(cm)
                    elif isSystemCOutPort(pyRetVal):
                        # Is it a SystemC output port
                        comp.out_sysc_port_gens.append(cm)
                    elif isSystemCInPort(pyRetVal):
                        # Is it a SystemC input port
                        comp.in_sysc_port_gens.append(cm)
                    elif isSystemCInOutPort(pyRetVal):
                        # Is it a SystemC in-out port
                        comp.inout_sysc_port_gens.append(cm)
                    elif isSystemCSignal(pyRetVal):
                        # Is it a SystemC signal
                        comp.sysc_signal_gens.append(cm)
            else:
                attrType = extractAttrType(attr, extractClassNames(realComp))
                pyAttrType = pyplusplus.decl_wrappers.algorithm.create_valid_name(attrType)
                if pyAttrType in dir(scwrapper):
                    pyAttrType = getattr(scwrapper, pyAttrType)
                    ca = component.ClassAttr()
                    ca.name = i
                    ca.instance = attr
                    ca.typeName = attrType
                    ca.pyAttrType = pyAttrType
                    ca.path = pathList
                    if isTLMInPort(pyAttrType):
                        # Is it a TLM input port
                        comp.in_tlm_ports.append(ca)
                    elif isTLMOutPort(pyAttrType):
                        # Is it a TLM output port
                        comp.out_tlm_ports.append(ca)
                    elif isSystemCOutPort(pyAttrType):
                        # Is it a SystemC output port
                        comp.out_sysc_ports.append(ca)
                    elif isSystemCInPort(pyAttrType):
                        # Is it a SystemC input port
                        comp.in_sysc_ports.append(ca)
                    elif isSystemCInOutPort(pyAttrType):
                        # Is it a SystemC in-out port
                        comp.inout_sysc_ports.append(ca)
                    elif isSystemCSignal(pyAttrType):
                        # Is it a SystemC signal
                        comp.sysc_signals.append(ca)
                else:
                    # Lets go in depth to see if the class for this attribute contains other ports
                    pathListCopy = pathList[:]
                    pathListCopy.append(i)
                    pyAttrTypeClass = getClass(pyAttrType, symbolNamespace)
                    if pyAttrTypeClass:
                        findConnectionsRecv(comp, pyAttrTypeClass, pathListCopy, scwrapper, symbolNamespace)
        except:
            import traceback
            traceback.print_exc()
            pass

