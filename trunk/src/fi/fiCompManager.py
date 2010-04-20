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
#   (c) Antonio Miele
#       miele@elet.polimi.it
#
##############################################################################


""" This module contains an enhanced component manager for fault simulations and analysis.
    It manages the fault locations and the automatic insertion of probes on the interconnections.
"""

import manager
import helper
import exceptions
from manager import compManager
from compManager import ComponentManager
import attributeWrapper
from attributeWrapper import *
import scwrapper
import types

class ProbeConnectionNode:
    """Utility class used for storing information on the position of each instantiated probe"""
    def __init__(self, probe, source, sourcePort, target, targetPort):
        self.probe = probe
        self.source = source
        self.sourcePort = sourcePort
        self.target = target
        self.targetPort = targetPort

class FaultInjectionComponentManager(ComponentManager):
    """This manager extends the standard component mananger and has to be used in place of the basic one in case
    of simulations for reliability analysis. It manages the list of fault locations and the the instantiation of 
    the probes on the interconnections"""
    
    def __init__(self, components):
        """Constructor for the fault injection component manager"""
        ComponentManager.__init__(self, components) # init the standard component manager
        import ProbeLT32
        self.__probeClass = ProbeLT32.ProbeLT32
        self.__probeID = 0
        self.__locationsDescriptors = self.__loadComponentFaultLocationDescriptors() #load the decriptors of the fault location of each component.
        self.__faultLocations = []
        self.__probes = {}
    
    def reset(self):
        """Reset the fault injection component manager"""
        ComponentManager.reset()
        import ProbeLT32
        self.__probeClass = ProbeLT32.ProbeLT32
        self.__probeID = 0
        self.__locationsDescriptors = self.__loadComponentFaultLocationDescriptors() #load the decriptors of the fault location of each component.
        self.__faultLocations = []        
        self.__probes = {} # the key is a tuple containing the names of the involved source and target ports. The value is a ProbleConnectionNode object
        
    def __loadComponentFaultLocationDescriptors(self, filename = 'src/fi/faultlocations.txt'):
        """Loads from a text file the descriptors of the locations for each component class. This list is used
        for automatically creating the list of fault locations for the instantiated architecture"""
        try:
            fp = open(filename,'r')
        except:
            raise exceptions.Exception('Error while loading ' + filename + ' file')

        #descriptors are stored in a dictinary. each entry contains the descriptors for a single component class. 
        #the value is a list of all locations each location is a dictionary which entries are the attribute name, 
        #the wrapper class, the number of lines and the word size
        classLocations = {}

        #scan all lines contained into the file
        i = 1
        for line in fp:
            #split line and check if the number of elements is ok
            line = line.replace('\n','')
            entry = line.split(',')
            if len(entry) != 5:
                raise exceptions.Exception('Error on line ' + str(i) + ': ' + line + '\nNot valid number of parameters')
            
            #get module and class name of the current component
            (componentModule, componentName) = entry[0].split('.')
            
            #get component reference
            component = None
            for m in self.components:
                if m.__name__ == componentModule:
                    component = getattr(m,componentName)
                    break
            if component == None:
                raise exceptions.Exception('Error on line ' + str(i) + ': ' + line + '\nNot valid component class')
        
            #store data of current location in a dictionary
            #they will be checked when they are used
            currLocationsDescriptor = {}
            currLocationsDescriptor["attribute"] = entry[1]
            currLocationsDescriptor["wrapperClass"] = entry[2]
            currLocationsDescriptor["lines"] = entry[3]
            currLocationsDescriptor["wordSize"] = entry[4]
            
            #store the dictionary in the list of the current component
            if classLocations.has_key(component):
                classLocations[component].append(currLocationsDescriptor)
            else:
                classLocations[component] = [currLocationsDescriptor]
        
            #increment the line number
            i=i+1
        #return the locaion
        return classLocations

    def connectPortsForce(self, source, sourcePort, target, targetPort, enableProbe = True):
        """Connects two specified components on the specified ports. The references of the components and the ports to be connected
           are required. Moreover, if specified, a probe is introduced on the connection, i.e., the source component is connected to the target
           port of the probe and the target component is connected to the initiator port of the sabouter"""
        #check if ports are already connected
        key = (sourcePort.name(),targetPort.name())
        if self.__probes.has_key(key):
            return
        if ComponentManager.areConnected(self, source, sourcePort, target, targetPort):
            return
        
        if enableProbe:
            #instantiate the probe
            probeInst = self.__probeClass('probe_'+str(self.__probeID))
            self.__probeID = self.__probeID + 1
            probeInPort = helper.getTLMInPort(probeInst)
            if len(probeInPort) != 1:
                raise exceptions.Exception('The probe must have exactly one target port')
            probeOutPort = helper.getTLMOutPort(probeInst)
            if len(probeOutPort) != 1:
                raise exceptions.Exception('The probe must have exactly one initiator port')
            #interconnect the components
            ComponentManager.connectPortsForce(self, source, sourcePort, probeInst, probeInPort[0])
            ComponentManager.connectPortsForce(self, probeInst, probeOutPort[0], target, targetPort)
            #keep track of the connected probe
            key = (sourcePort.name(), targetPort.name())
            self.__probes[key] = ProbeConnectionNode(probeInst, source.name(), sourcePort.name(), target.name(), targetPort.name()) 
            return probeInst
        else:
            ComponentManager.connectPortsForce(self, source, sourcePort, target, targetPort)
                            
    def getProbe(self, sourcePort, targetPort):
        """Returns the probe connected between the two specified ports. If there is no probe or the two ports are not connected, it returns None"""
        sourcePortName = ''
        if type(sourcePort) == types.StringType:
            sourcePortName = sourcePort
        else:
            if helper.isTLMOutPort(sourcePort):
                sourcePortName = sourcePort.name()
            else:
                raise exceptions.Exception(str(sourcePort) + ' is not a valid source port')
                
        targetPortName = ''
        if type(targetPort) == types.StringType:
            targetPortName = targetPort
        else:
            if helper.isTLMInPort(targetPort):
                targetPortName = targetPort.name()
            else:
                raise exceptions.Exception(str(sourcePort) + ' is not a valid source port')
        
        key = (sourcePortName,targetPortName)
        if self.__probes.has_key(key):
            return self.__probes[key].probe
        return None

    def getProbePosition(self, probe):
        """Returns the probeConnectionNode. If there is no probe or the two ports are not connected, it returns None"""
        if helper.isSystemCComponent(probe) and probe.__class__ == self.__probeClass:        
            for i in self.__probes.items():
                if i.probe.name() == probe.name():
                    return i
        else:
            raise exceptions.Exception(str(probe) + ' is not a valid probe')
        
    def getProbes(self):
        """Returns the list of all probes introduced in the architecture"""
        l = []
        for i in self.__probes.items():
            l.append(i.probe)
        return l
        
    def registerComponent(self, component, attributes=None): 
        #check if component is a valid object
        if not(helper.isSystemCComponent(component) and ComponentManager.getInstantiatedComponents(self).count(component.name()) == 1):
            raise exception.Exception(str(component) + ' is not a valid SystemC component instantiated in the architecture')
                    
        #it is necessary to create a descriptor for each instance of the component
        if self.__locationsDescriptors.has_key(component.__class__):            
            #check if specified attributes exist in the component class
            compAttributes = []
            for currLocationsDescriptor in self.__locationsDescriptors[component.__class__]:
                compAttributes.append(currLocationsDescriptor["attribute"])
            if attributes == None:
                attributes = compAttributes 
            else:
                for currAttr in attributes:
                    if compAttributes.count(currAttr) == 0:
                        raise exceptions.Exception(str(currAttr) + ' is not a valid attribute for component ' + str(component))
            
            #generate location descriptors for the specified component
            for currLocationsDescriptor in self.__locationsDescriptors[component.__class__]:
                if attributes.count(currLocationsDescriptor["attribute"]):
                    attribute = currLocationsDescriptor["attribute"]
                    lines = currLocationsDescriptor["lines"]
                    wordSize = currLocationsDescriptor["wordSize"]
                    wrapperClass = currLocationsDescriptor["wrapperClass"]
                    
                    #get the number of lines. 
                    #it may be a number or also the name of a function to be called in order to load the value dinamically  
                    try:
                        #the number of lines is specified into the file
                        lines = long(lines)
                    except:
                        #the number of lines is retrieved dinamically by reading an attribute or calling a method of the object
                        try:
                            pars = lines.split('.')
                            parRef = component
                            for nextParStr in pars:
                                parRef = getattr(parRef,nextParStr)
                            if callable(parRef):
                                lines = parRef()
                            else:
                                lines = parRef
                        except:
                            raise exceptions.Exception(str(lines) + ' attribute has not been found in component ' + str(component))
        
                    #get the word size. 
                    #it may be a number or also the name of a function to be called in order to load the value dinamically
                    try:
                        wordSize = long(wordSize)
                    except:
                        try:
                            pars = lines.split('.')
                            parRef = component
                            for nextParStr in pars:
                                parRef = getattr(parRef,nextParStr)
                            if callable(parRef):
                                wordSize = parRef()
                            else:
                                wordSize = parRef
                        except:
                            raise exceptions.Exception(str(lines) + ' attribute has not been found in component ' + str(component))
                            
                    #create the location descriptor
                    ld = locationDescriptor(component, attribute, wrapperClass, lines, wordSize)
                    self.__faultLocations.append(ld)

    def connectPorts(self, source, sourcePortName, target, targetPortName, sourcePortId = None, targetPortId = None, enableProbe = True):
        raise exceptions.Exception('connect method is not supported in the fiComponentManager. Please use other connection methods')      

    def connect(self, source, target):
        raise exceptions.Exception('connect method is not supported in the fiComponentManager. Please use other connection methods')      
 
    def connectSyscPorts(self, source, sourcePort, target, targetPort, sourceParams = None, targParams = None):
        raise exceptions.Exception('connectSyscPorts method is not supported in the fiComponentManager')      

    def connectSyscSignal(self, source, sourceSignal, target, targetPort, sourceParams = None, targParams = None):
        raise exceptions.Exception('connectSyscSignal method is not supported in the fiComponentManager')      

    def getConnectionSignal(self, sourceComp, targetComp, sourcePort, targetPort):
        raise exceptions.Exception('getConnectionSignal method is not supported in the fiComponentManager')      

