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
#   (c) Giovanni Beltrame, Luca Fossati
#       Giovanni.Beltrame@esa.int fossati@elet.polimi.it
#
##############################################################################


""" Module containing the classes use to manage the components present in the
architecture"""

from connectionNode import ConnectionNode
from respkernel import blddir
import exceptions
import component
import types
import os
import sys
import helper
import scwrapper

class ComponentManager:
    """Manages the components present in the loaded architecture: it
    offers functionalities for connecting components and for keeping track of these connections.
    """

    def __init__(self, components):
        """Inizializer of the class; components is a list containing the imported component modules"""
        # maps the name of an instance of a component (represented by a name) to a connection node 
        #(connection node contains the list of all connections of the component)
        self.compToConnection = {}
        # keeps track of the imported components modules
        self.components = components

    def __repr__(self):
        """Returns a formal representation of this class"""
        #TODO improve it to consider components not yet connected to anyone
        representation = 'Component manager:\n\n'
        compList = self.getInstantiatedComponents()
        for elem in compList:
            if self.compToConnection.has_key(elem):
                representation += repr(self.compToConnection[elem]) + '\n'
            else:
                tmp = ConnectionNode(self.getCompInstance(elem).name(),elem)
                representation += repr(tmp) + '\n'
        #for elem in self.compToConnection.values():
        #    representation += repr(elem) + '\n'
        return representation

    def __str__(self):
        """Returns an informal string representation of this class"""
        return repr(self)

    def connect(self, source, target):
        """Connects the source component with target; in case only one initiator port is present in source and one target port in target
        these two ports are connected, otherwise the user is requested to specify the correct ports by using the connectPortsByPathName method. 
        Note that source and target must represent the instances of the components that we wish to connect together, not their names
        """
        
        #check if source and target are SystemC components
        if not helper.isSystemCComponent(source):
            exceptions.Exception(str(source) + ' is not a SystemC component')        
        if not helper.isSystemCComponent(target):
            exceptions.Exception(str(target) + ' is not a SystemC component')        
        
        # First of all I get all the initiator ports from source and all the target ports from target
        sourcePorts = helper.getTLMOutPort(source)
        targetPorts = helper.getTLMInPort(target)

        # check if ports are already connected. TODO improve check considering single and multi_pass_through ports
        candidates = []
        for srcPort in sourcePorts:
            for trgPort in targetPorts:
                if not self.areConnected(source, srcPort.name(), target, trgPort.name()):
                    candidates.append((srcPort,trgPort))
        
        #connect if possible
        if len(candidates) == 0:
            raise exceptions.Exception('Found no possible connections among ' +
                  str(source) + ' and ' + str(target))
        elif len(candidates) > 1:
            errorString = 'There are too many possible connections among' + str(source) + ' and ' + str(target)
            errorString = errorString + 'The candidates sourcePort, targetPort are:'
            for candTemp in candidates:
                errorString = errorString + candTemp[0][1] + ' -> ' + candTemp[1][1]
            errorString = errorString + 'Please call connectPortsByPathName or connectPorts specifying ' + 'the ports you wish to connect'
            raise exceptions.Exception(errorString)
        else: #connect the two identified ports
            print('Connecting ' + source.name() + ' and ' + target.name() +
                  ' repectively using ports ' + candidates[0][0].name() + ' and ' + candidates[0][1].name())
            self.connectPorts(source, candidates[0][0], target, candidates[0][1])

    def connectPortsByPathName(self, source, sourcePortName, target, targetPortName, sourcePortId = None, targetPortId = None):
        """Connects the source component with a target one; the path of the initiator port (in the source component) and target port
        (in the target component) are specified. Note that a check is performed to be sure that the type of the source and target port match.
        It is also possible to specify the Ids of the ports: if they are different from None it means that sourcePortName and 
        targetPortName are the names for arrays of ports and the port id refers to the array index of the desired port
        """
        #check if source and target are SystemC components
        if not helper.isSystemCComponent(source):
            exceptions.Exception(str(source) + ' is not a SystemC component')        
        if not helper.isSystemCComponent(target):
            exceptions.Exception(str(target) + ' is not a SystemC component')        

        #check if the source components contains the port with the specified name
        try:
            sourcePort = helper.getAttrInstance(source, sourcePortName)
        except Exception:
            raise exceptions.Exception('There is no port with name ' + sourcePortName + ' in the source component')
        #use the port id in case the port is contained in a vector
        if sourcePortId is not None and hasattr(sourcePort,'__getitem__'):
            if type(sourcePortId) is int:
                sourcePort = sourcePort[sourcePortId]
            else:
                raise exceptions.Exception('The source port ID must be an integer')

        #check if the target component contains the port with the specified name
        try:
            targetPort = helper.getAttrInstance(target, targetPortName)
        except Exception:
            raise exceptions.Exception('There is no port with name ' + targetPortName + ' in the target component')
        #use the port id in case the port is contained in a vector
        if targetPortId is not None and hasattr(targetPort,'__getitem__'):
            if type(targetPortId) is int:
                targetPort = targetPort[targetPortId]
            else:
                raise exceptions.Exception('The target port ID must be an integer')

        # Now I have to check that the ports are compatible, so that they carry the same
        # type of information; in order to do this I move down on the inheritance chain
        # of each of the two ports, until the name of the class starts with
        # tlm_initiator_port or tlm_target_port; then I can check the types and
        # connect them
        try:
            sourcePortTypeName = helper.getBase('tlm_base_initiator_socket_b', sourcePort.__class__)
        except Exception, e:
            raise exceptions.Exception('ports ' + sourcePortName + ' and ' + targetPortName + ' cannot be connected because' +
                  'of the following error ' + str(e))
        try:
            targetPortTypeName = helper.getBase('tlm_base_target_socket_b', targetPort.__class__)
        except Exception, e:
            raise exceptions.Exception('ports ' + sourcePortName + ' and ' + targetPortName + ' cannot be connected because' +
                  'of the following error ' + str(e))

        if helper.getTLMPortType(sourcePortTypeName) != helper.getTLMPortType(targetPortTypeName):
            raise exceptions.Exception('ports ' + sourcePortName + ' and ' + targetPortName + ' are not compatible')

        self.connectPorts(source, sourcePort, target, targetPort)


    def connectPorts(self, source, sourcePort, target, targetPort):
        """Connect two ports given their instances
        """
        #check if source and target are SystemC components
        if not helper.isSystemCComponent(source):
            exceptions.Exception(str(source) + ' is not a SystemC component')        
        if not helper.isSystemCComponent(target):
            exceptions.Exception(str(target) + ' is not a SystemC component')        
      
        #check if they are already connected
        if self.areConnected(source, sourcePort, target, targetPort):
            print '\nThe two ports are already connected!\n'
            return

        #check for name method        
        if not hasattr(target,'name'):
            raise exceptions.Exception('Component ' + str(target) + ' doesn\'t have the method name; probably it is not an sc_module')

        if not hasattr(source,'name'):
            raise exceptions.Exception('Component ' + str(source) + ' doesn\'t have the method name; probably it is not an sc_module')

        if not hasattr(targetPort,'name'):
            raise exceptions.Exception('Port ' + str(targetPort) + ' doesn\'t have the method name; probably it is not an valid port object')

        if not hasattr(sourcePort,'name'):
            raise exceptions.Exception('Port ' + str(sourcePort) + ' doesn\'t have the method name; probably it is not an valid port object')

        #get sc_name (actually the only unique identifier for sc_objects)
        sourceName = source.name()
        targetName = target.name()
        sourcePortName = sourcePort.name()
        targetPortName = targetPort.name() #super(scwrapper.sc_module, targetPort).name()

        # check of the port types
        try:
            sourcePortTypeName = helper.getBase('tlm_base_initiator_socket_b', sourcePort.__class__)
        except Exception, e:
            raise exceptions.Exception('ports ' + sourcePortName + ' and ' + targetPortName + ' cannot be connected because' +
                  'of the following error ' + str(e))
        try:
            targetPortTypeName = helper.getBase('tlm_base_target_socket_b', targetPort.__class__)
        except Exception, e:
            raise exceptions.Exception('ports ' + sourcePortName + ' and ' + targetPortName + ' cannot be connected because' +
                  'of the following error ' + str(e))

        #check if the component actually contains the port
        if helper.getAllChildScObjects(source).count(sourcePortName) == 0:
            raise exceptions.Exception('Component ' + sourceName + ' doesn\'t contain port ' + sourcePortName)
        if helper.getAllChildScObjects(target).count(targetPortName) == 0:
            raise exceptions.Exception('Component ' + targetName + ' doesn\'t contain port ' + targetPortName)

        if not 'bind' in dir(sourcePort):
            raise exceptions.Exception('Component ' + sourceName + ' doesn\'t have the method bind among the ones of the port ' + str(sourcePort))

        #connect the ports
        sourcePort.bind(targetPort)

        #update the internal representation of the interconnections
        if self.compToConnection.has_key(sourceName):
            sourceNode = self.compToConnection[sourceName]
        else:
            sourceNode = ConnectionNode(source, sourceName)
            self.compToConnection[sourceName] = sourceNode

        if self.compToConnection.has_key(targetName):
            targetNode = self.compToConnection[targetName]
        else:
            targetNode = ConnectionNode(target, targetName)
            self.compToConnection[targetName] = targetNode

        sourceNode.addTarget(targetName, targetPortName, sourcePortName)
        targetNode.addSource(sourceName, sourcePortName, targetPortName)

    def connectSyscPorts(self, source, sourcePort, target, targetPort, sourceParams = None, targParams = None):
        """Connects together two systemc (NOT TLM) ports; a signal
        is used to connect together the ports, sourceParams[0] and targParams[0] are the
        names of the components we want to connect; target and source ports may either
        be the name of the port or the instance of it. Also the source and target
        components may either be strings (the names of the components) or the
        instances of those components
        sourceParams and targParams represent the parameters which must be passed to
        the constructor of the source and target components in case they have to
        be built (so in case source and target are string)"""
        print "DEPRECATED: method not mantained"
        # first of all I check to see if source and target are strings and in
        # case I create the instance of the corresponding component
        if type(source) == types.StringType:
            source = helper.instantiateComponent(source, sourceParams, self.components)
            if type(source) == types.StringType:
                raise exceptions.Exception('Error, there is no class with the name ' + source + ' to be used as source component')
        if type(target) == types.StringType:
            target = helper.instantiateComponent(target, targParams, self.components)
            if type(target) == types.StringType:
                raise exceptions.Exception('Error, there is no class with the name ' + target + ' to be used as target component')

        # ok, now I can check if the components contain the port with the
        # specified name (in case the name is specified). In case we already have
        # the instance, nothing is done
        if type(sourcePort) == types.StringType:
            try:
                sourcePort = getattr(source, sourcePort)
            except Exception:
                raise exceptions.Exception('There is no port with name ' + str(sourcePort) + ' in the source component')

        if type(targetPort) == types.StringType:
            try:
                targetPort = getattr(target, targetPort)
            except Exception:
                raise exceptions.Exception('There is no port with name ' + str(targetPort) + ' in the target component')

        if not 'name' in dir(target):
            raise exceptions.Exception('Component ' + str(target) + ' doesn\'t have the method name; probably it is not an sc_module')

        if not 'name' in dir(source):
            raise exceptions.Exception('Component ' + str(source) + ' doesn\'t have the method name; probably it is not an sc_module')

        sourceName = source.name()
        targetName = target.name()

        # Now I can connect the ports; first of all I allocate a new element
        # which keeps track of this connection
        # ok, I simply have to allocate a new element in the compToConnection; note
        # that if an element already exists in this map I don't have to allocate a
        # new one, but I have to add a connection to the node
        if self.compToConnection.has_key(sourceName):
            sourceNode = self.compToConnection[sourceName]
        else:
            sourceNode = ConnectionNode(source, sourceName)
            self.compToConnection[sourceName] = sourceNode
        if self.compToConnection.has_key(targetName):
            targetNode = self.compToConnection[targetName]
        else:
            targetNode = ConnectionNode(target, targetName)
            self.compToConnection[targetName] = targetNode

        if not 'bind' in dir(sourcePort):
            raise exceptions.Exception('Component ' + sourceName + ' doesn\'t have the method bind among the ones of the port ' + str(sourcePort))
        if not 'bind' in dir(targetPort):
            raise exceptions.Exception('Component ' + targetName + ' doesn\'t have the method bind among the ones of the port ' + str(targetPort))

        # now I create the signal, connect the components with it and finally
        # add the connection node; note that the signal depends on the type of
        # the ports we are trying to connect: so, first of all I have to
        # deterimine it
        try:
            sourcePortTypeName = helper.getBase('sc_out', sourcePort.__class__)
        except Exception, e:
            try:
                sourcePortTypeName = helper.getBase('sc_inout', sourcePort.__class__)
            except Exception, e:
                raise exceptions.Exception('ports ' + str(sourcePort) + ' and ' + str(targetPort) + ' cannot be connected because' +
                      'No classes in the inheritance chain of ' + str(sourcePort.__class__.__name__) + ' starts with sc_inout or sc_in')

        try:
            targetPortTypeName = helper.getBase('sc_in', targetPort.__class__)
        except Exception, e:
            try:
                targetPortTypeName = helper.getBase('sc_inout', targetPort.__class__)
            except Exception, e:
                raise exceptions.Exception('ports ' + str(sourcePort) + ' and ' + str(targetPort) + ' cannot be connected because' +
                      'No classes in the inheritance chain of ' + str(targetPort.__class__.__name__) + ' starts with sc_inout or sc_out')

        sourcePortType = helper.getSysCPortType(sourcePortTypeName)
        targetPortType = helper.getSysCPortType(targetPortTypeName)
        if sourcePortType != targetPortType:
            raise exceptions.Exception('ports ' + str(sourcePort) + ' and ' + str(targetPort) + ' are not compatible')

        # Finally I create the signal and connect the ports
        try:
            connSignalType = getattr(scwrapper, 'sc_signal_less_' + sourcePortType + '_greater_')
            connSignal = connSignalType()
        except:
            raise exceptions.Exception('ports ' + str(sourcePort) + ' and ' + str(targetPort) + ' cannot be connected because' +
                  ' unable to create an instance of signal sc_signal_less_' + sourcePortType + '_greater_')

        sourceNode.addTarget(targetName, targetPort, sourcePort, connSignal)
        targetNode.addSource(sourceName, sourcePort, targetPort, connSignal)

        sourcePort.bind(connSignal)
        targetPort.bind(connSignal)

    def connectSyscSignal(self, source, sourceSignal, target,
                           targetPort, sourceParams = None, targParams = None):
        """Connects together a signal with a systemc port; sourceParams[0] and targParams[0] are the
        names of the components we want to connect; target and source ports may either
        be the name of the port or the instance of it. Also the source and target
        components may either be strings (the names of the components) or the
        instances of those components
        sourceParams and targParams represent the parameters which must be passed to
        the constructor of the source and target components in case they have to
        be built (so in case source and target are string)"""
        print "DEPRECATED: method not mantained"
        # first of all I check to see if source and target are strings and in
        # case I create the instance of the corresponding component
        if type(source) == types.StringType:
            source = helper.instantiateComponent(source, sourceParams, self.components)
            if type(source) == types.StringType:
                raise exceptions.Exception('Error, there is no class with the name ' + source + ' to be used as source component')
        if type(target) == types.StringType:
            target = helper.instantiateComponent(target, targParams, self.components)
            if type(target) == types.StringType:
                raise exceptions.Exception('Error, there is no class with the name ' + target + ' to be used as target component')

        # ok, now I can check if the components contain the port with the
        # specified name (in case the name is specified). In case we already have
        # the instance, nothing is done
        if type(sourceSignal) == types.StringType:
            try:
                sourceSignal = getattr(source, sourceSignal)
            except Exception:
                raise exceptions.Exception('There is no signal with name ' + sourceSignal + ' in the source component')

        if type(targetPort) == types.StringType:
            try:
                targetPort = getattr(target, targetPort)
            except Exception:
                raise exceptions.Exception('There is no port with name ' + targetPort + ' in the target component')

        if not 'name' in dir(target):
            raise exceptions.Exception('Component ' + str(target) + ' doesn\'t have the method name; probably it is not an sc_module')

        if not 'name' in dir(source):
            raise exceptions.Exception('Component ' + str(source) + ' doesn\'t have the method name; probably it is not an sc_module')

        sourceName = source.name()
        targetName = target.name()

        # Now I can connect the ports; first of all I allocate a new element
        # which keeps track of this connection
        # ok, I simply have to allocate a new element in the compToConnection; note
        # that if an element already exists in this map I don't have to allocate a
        # new one, but I have to add a connection to the node
        if self.compToConnection.has_key(sourceName):
            sourceNode = self.compToConnection[sourceName]
        else:
            sourceNode = ConnectionNode(source, sourceName)
            self.compToConnection[sourceName] = sourceNode
        if self.compToConnection.has_key(targetName):
            targetNode = self.compToConnection[targetName]
        else:
            targetNode = ConnectionNode(target, targetName)
            self.compToConnection[targetName] = targetNode

        if not 'bind' in dir(targetPort):
            raise exceptions.Exception('Component ' + targetName + ' doesn\'t have the method bind among the ones of the port ' + str(targetPort))

        # now I create the signal, connect the components with it and finally
        # add the connection node; note that the signal depends on the type of
        # the ports we are trying to connect: so, first of all I have to
        # deterimine it
        try:
            sourceSignalTypeName = helper.getBase('sc_signal', sourceSignal.__class__)
        except Exception, e:
            raise exceptions.Exception(str(sourceSignal) + ' and ' + str(targetPort) + ' cannot be connected because' +
                  'of the following error ' + str(e))

        try:
            targetPortTypeName = helper.getBase('sc_out', targetPort.__class__)
        except Exception, e:
            try:
                targetPortTypeName = helper.getBase('sc_inout', targetPort.__class__)
            except Exception, e:
                try:
                   targetPortTypeName = helper.getBase('sc_in', targetPort.__class__)
                except Exception, e:
                   raise exceptions.Exception(str(sourceSignal) + ' and ' + str(targetPort) + ' cannot be connected because' +
                         'of the following error ' + str(e))

        sourceSignalType = helper.getSysCSignalType(sourceSignalTypeName)
        targetPortType = helper.getSysCPortType(targetPortTypeName)
        if sourceSignalType != targetPortType:
            raise exceptions.Exception(str(sourceSignal) + ' and ' + str(targetPort) + ' are not compatible')

        sourceNode.addTarget(targetName, targetPort, None, sourceSignal)
        targetNode.addSource(sourceName, None, targetPort, sourceSignal)

        targetPort.bind(sourceSignal)

    def getConnectionSignal(self, sourceComp, targetComp, sourcePort, targetPort):
        """In case sourcePort and targetPort are connected through a SystemC port, the
        signal used to connect the ports is returned"""
        #TODO to be implemented
        print "DEPRECATED: method not mantained"
        pass

    def getInstantiatedComponents(self):
        """Returns a tuple with the names of the components present in the architecture;
        actually only their name is returned"""
        
        #get the list of all component classes
        componentClasses = []
        for elem in self.components:
            for attr in dir(elem):
                attr = getattr(elem,attr)
                if hasattr(attr,'__class__'):
                    try:
                        if issubclass(attr,scwrapper.sc_object):
                            componentClasses.append(attr)
                    except:
                        pass
        
        #get components
        components = []
        tmpList = scwrapper.sc_get_curr_simcontext().get_child_objects()
        for i in range(0,len(tmpList)):
            currComp = tmpList[i]
            if hasattr(currComp,'name'):
                if componentClasses.count(currComp.__class__):
                    components.append(currComp.name())
        ##old strategy for getting components
        #for comp in self.compToConnection.keys():
        #    components.append(comp)
        return tuple(components)

    def getConnected(self, component):
        """Returns a tuple with the components connected to component;
        component may either be the name of the component or the instance
        of the component"""
        return self.getTargets(component) + self.getSources(component)

    def getTargets(self, component):
        """Returns a tuple with the components connected to component with slave role
        component may either be the name of the component or the instance
        of the component"""
        if type(component) == types.StringType:
            if not self.compToConnection.has_key(component):
                raise exceptions.Exception('Component ' + component + ' not present in the loaded architecture')
            return self.compToConnection[component].getTargets()
        else:
            #I now have to discover the the name of the component
            for comp, connNode in self.compToConnection.items():
                if component == connNode.component:
                    return self.compToConnection[connNode.componentName].getTargets()
        return ()

    def getSources(self, component):
        """Returna a tuple with the component connected to component with master role
        component may either be the name of the component or the instance
        of the component"""
        if type(component) == types.StringType:
            if not self.compToConnection.has_key(component):
                raise exceptions.Exception('Component ' + component + ' not present in the loaded architecture')
            return self.compToConnection[component].getSources()
        else:
            #I now have to discover the the name of the component
            for comp, connNode in self.compToConnection.items():
                if component == connNode.component:
                    return self.compToConnection[connNode.componentName].getSources()
        return ()

    def areConnected(self, source, sourcePort, target, targetPort):
        """Checks whether source and target are already connected together
        on ports sourcePort and targetPort. source and target may represent
        both the instance of the component or its systemc name. On the other
        hand sourcePort and targetPort represents the instance or the SystemC name of
        the source and target ports inside those components"""
        #it cannot compare port references. In fact when a port is saved in the compToConnection the python wrapper is copied
        #thus it is a different object w.r.t. the python wrapper called when accessing the reference of the port inside the component instance
        #it can be solved comparing the names of the ports but name method is not exported for tlm ports
 
        #get components names
        sourceName = ''
        targetName = ''
        if type(source) == types.StringType:
            sourceName = source
        else:
            for comp, connNode in self.compToConnection.items():
                if source == connNode.component:
                    sourceName = connNode.componentName
                    break
        if type(target) == types.StringType:
            targetName = target
        else:
            for comp, connNode in self.compToConnection.items():
                if target == connNode.component:
                    targetName = connNode.componentName
                    break
                           
        #both the target and the source are checked to be present in the compToConnection
        #it is necessary to avoid following computation
        if not self.compToConnection.has_key(sourceName):
            return False
        if not self.compToConnection.has_key(targetName):
            return False
        
        #get components references. Both the components are included in the compToConnection 
        #thus it is possible to get their reference by using getCompInstance method
        if type(source) == types.StringType:
            source = self.getCompInstance(source)
        if type(target) == types.StringType:
            target = self.getCompInstance(target)
        
        #get ports names
        targetPortName = ''
        sourcePortName = ''
        if type(sourcePort) == types.StringType:
            sourcePortName = sourcePort
        else:
            sourcePortName = sourcePort.name()
        if type(targetPort) == types.StringType:
            targetPortName = targetPort
        else:
            targetPortName = targetPort.name()
        
        #final check if the ports are connected
        sourceNode = self.compToConnection[sourceName]
        if sourceNode.checkTarget(targetName,targetPortName,sourcePortName):
            return True
        else:
            return False

    def getCompInstance(self, name):
        """Given the component's SystemC name this method returns its instance"""
        tmpList = scwrapper.sc_get_curr_simcontext().get_child_objects()
        for i in range(0,len(tmpList)):
            currComp = tmpList[i]
            if hasattr(currComp,'name'):
                if currComp.name() == name:
                    return currComp
        return None
        
        #Old strategy based on the compToConnection
        #if self.compToConnection.has_key(name):
        #    return self.compToConnection[name].component
        #else:
        #    return None

    def setParameters(self, name, value, env):
        """Takes as input two strings: the first one represents the full path
        of a python object representing a parameters inside a component; the second
        one represents the value which has to be assigned to that component"""
        [instance, attrName] = helper.getAttrInstanceHelper(name, env)
        try:
            castedVal = getattr(instance, attrName).__class__(value)
        except:
            print 'It is not possible to convert value ' + value + ' to an object of type ' + str(getattr(instance, attrName).__class__)
            sys.exit()
        setattr(instance, attrName, value)

    def listComponents(self):
        """Returns the list of the current available architectural components"""

        # ok: the components should have already been imported in the current python
        # session: I simply have to get the path of all the imported modules : if it contains
        # the __build__/default/component string, then this is a component and I can list it
        # Note actually that the module containing the component is listed, not the class
        # defining the component itself
        ####symbolNamespace = [self.archc]
        symbolNamespace = []
        symbolNamespace += self.components
        foundComponents = []
        for module in self.components:
            if '__file__' in dir(module) and '__name__' in dir(module):
                if module.__file__.find(os.path.split(blddir)[1] + os.sep + 'default' + os.sep + 'component') != -1:
                    for classes in dir(module):
                        realComp = getattr(module, classes, module)
                        try:
                            #It is a component usefull to the user if it a subclass of sc_module
                            #and if it can be instantiated
                            helper.getBase('sc_module', realComp)
                            if realComp.__init__.__doc__ == 'Raises an exception\nThis class cannot be instantiated from Python\n':
                                continue
                        except:
                            continue
                        comp = component.Component()
                        comp.module = module
                        comp.classs = realComp
                        try:
                            comp.constructor = helper.extractConstrSig(realComp.__init__)
                        except:
                            import traceback
                            traceback.print_exc()
                            continue
                        helper.findConnectionsRecv(comp, realComp, [], scwrapper, symbolNamespace)
                        foundComponents.append(comp)
        return foundComponents

    def printComponents(self):
        """prints a string containing the list of names of the available the architectural
        components together with a short description of them"""
        compString = ''
        components = self.listComponents()
        components.sort(key=component.Component.getName)
        for i in components:
            compString += str(i) + '\n'
        print compString

    def showArchitecture(self):
        """Shows the graph of the archtiecture using dotty"""
        nodes = ""
        edges = ""
        for c in self.getInstantiatedComponents():
            nodes = nodes + "\t"+str(c)+"[shape=box]\n"
            nodes = nodes + "\n"
            for out in self.getTargets(c):
                if self.getInstantiatedComponents().count(out) != 0:
                    edges = edges + "\t" + str(c) + "->"+ str(out) +"\n"

        code = "digraph simple_hierarchy {\n" + nodes + "\n" + edges + "\n}"
        f = open("__tmpGraph.dot",'w')
        f.write(code)
        f.close()
        import os
        os.system('dotty __tmpGraph.dot')
        os.system('rm __tmpGraph.dot')

    def reset(self):
        """Resets the component manager"""
        self.compToConnection = {}
        self.unknownComNames = {}
        self.unnamedId = 0
