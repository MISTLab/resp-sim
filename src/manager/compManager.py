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


""" Module containing the classes use to manage the components present in the
architecture"""

from connectionNode import ConnectionNode
import exceptions
from exc import *
import component
import types
import os
#import string
import sys
import helper


class ComponentManager:
    """Manages the components present in the loaded architecture: this class
    is mainly used during two phases of the simulation: at the beggining it
    it used to load and connect together all the components, at the end of
    the simulation it can be used to debug the components themselves.
    The internal representation of the components is a directed graph,
    where the source of an edge represents the initiator component of the
    connection identified by that edge, and the target represents the target
    port."""

    def __init__(self, components, scwrapper, archc):
        """Inizializer of the class; components is a list containing the
        imported component modules"""
        # maps the name of an instance of a component (represented by a name) to a connection
        # node
        self.compToConnection = {}
        # counts how many unnamed components has been inserted so far
        # in the simulator
        self.unnamedId = 0
        # keeps track of the names associated to the unnamed components
        # encountered so far
        self.unknownComNames = {}
        # keeps track of the imported components modules
        self.components = components
        self.scwrapper = scwrapper
        self.archc = archc

    def connect(self, source, target,
                sourceParams = None, targParams = None,
                sourceConnParams = None, targConnParams = None):
        """Connects the source component with target; in case only one
        initiator port is present in source and one target port in target
        these two ports are connected, otherwise the user is requested
        to specify the correct ports. Parameters can be specified for the
        source and target connection; these parameters are a couple of
        values which specify the starting and ending mapping value of
        the connection.
        Note that source and target must represent the instances of the components
        that we wish to connect together, not their names
        sourceParams and targParams are a list of the parameters which must be
        passed to the component during its construction; it this list is not
        None, then it must have at least an element, the name of the component expressed
        as a string"""
        # First of all I get all the initiator ports from source and all
        # the target ports from target: they are maps: the index of the
        # map is formed the tlm of the tlm interface and the content of
        # the map is formed by the port itself. both the port and the type
        # are identified by strings
        sourcePorts = {}
        targetPorts = {}
        # Check the type of the TLM initiator port, there are two templates
        for srcP in dir(source):
            try:
                portInstance = getattr(source, srcP)
            except TypeError:
                continue
            except Exception:
                raise exceptions.Exception('Error in getting the instance of source port with name ' + srcP +
                      ' from component ' + str(source))
            varName = portInstance.__class__.__name__
            print portInstance.__class__.__name__
            portType = ''
            try:
                portType = helper.getTLMPortType(varName, 'source')
            except NotAPort:
                continue
            if sourcePorts.has_key(portType):
                sourcePorts[portType].append([portInstance, srcP])
            else:
                sourcePorts[portType] = [[portInstance, srcP]]
        # Now i repeate the same trick for the target port; note that this has
        # only one template
        for tgtP in dir(target):
            try:
                portInstance = getattr(target, tgtP)
            except TypeError:
                continue
            except Exception:
                raise exceptions.Exception('Error in getting the instance of source port with name ' + tgtP +
                      ' from component ' + str(target))
            varName = portInstance.__class__.__name__
            print portInstance.__class__.__name__
            portType = ''
            try:
                portType = helper.getTLMPortType(varName, 'target')
            except NotAPort:
                continue
            if targetPorts.has_key(portType):
                targetPorts[portType].append([portInstance, tgtP])
            else:
                targetPorts[portType] = [[portInstance, tgtP]]

        # ok, now I have to check if there are ports whose template types
        # matches and which hasn't been connected together:
        # if there is only one such pair then I can connect the two ports
        # together, otherwise I have to ask the user for the port names which
        # must be connected together.
        candidates = []
        for type, foundSrcPorts in sourcePorts.items():
            if targetPorts.has_key(type):
                foundTgPorts = targetPorts[type]
                # Now I have to see if there are some ports which hasn't
                # been connected together yet: they are the candidates for
                # this connection
                for srcPort in foundSrcPorts:
                    for tgtPort in foundTgPorts:
                        if not self.areConnected(source, srcPort[0], target, tgtPort[0]):
                            candidates.append([srcPort, tgtPort])
        if len(candidates) == 0:
            raise exceptions.Exception('Found no possible connections among ' +
                  str(source) + ' and ' + str(target))
        elif len(candidates) > 1:
            errorString = 'There are too many possible connections among' + str(source) + ' and ' + str(target)
            errorString = errorString + 'The candidates sourcePort, targetPort are:'
            for candTemp in candidates:
                errorString = errorString + candTemp[0][1] + ' -> ' + candTemp[1][1]
            errorString = errorString + 'Please call connectPorts or connectPortsForce specifying ' + 'the ports you wish to connect'
            raise exceptions.Exception(errorString)
        else:
            print('Connecting ' + str(source) + ' and ' + str(target) +
                  ' repectively using ports ' + candidates[0][0][1] + ' and ' + candidates[0][1][1])
            self.connectPorts(source, candidates[0][0][1], target, candidates[0][1][1],
                              sourceParams, targParams, sourceConnParams, targConnParams)

    def connectPorts(self, source, sourcePortName, target, targetPortName,
                     sourceParams = None, targParams = None,
                     sourceConnParams = None, targConnParams = None,
                     sourcePortId = None, targetPortId = None):
        """Connects the source component with target; the name of
        the initiator port (in the source component) and target port
        (in the target component) are specified. Parameters can be specified for the
        source and target connection; these parameters are a couple of
        values which specify the starting and ending mapping value of
        the connection. Note that a check is performed to be sure that
        the type of the source and target port match: I go down on the
        inheritace chain of the types of the ports, and if I find the same
        type at some point OK, otherwise if I get to the empty tuple it
        means that the two ports are not compatibile, so they are not
        connected together.
        It is also possible to specify the Ids of the ports: if they are different
        from None it means that sourcePortName and targetPortName are the names
        for arrays of ports and the port id refers to the array index of the
        desired port
        Note that source and target may either be the name of the classes of the source
        and target component or the instance of the component itself; the methods
        should work in both the situations
        sourceParams and targParams are a list of the parameters which must be
        passed to the component during its construction; it this list is not
        None, then it must have at least an element, the name of the component"""
        # first of all I check to see if source and target are strings and in
        # case I create the instance of the corresponding component
        if type(source) == types.StringType:
            source = helper.getInstance(source, sourceParams, self.components)
            if type(source) == types.StringType:
                raise exceptions.Exception('Error, there is no class with the name ' + source + ' to be used as source component')
        if type(target) == types.StringType:
            target = helper.getInstance(target, targParams, self.components)
            if type(target) == types.StringType:
                raise exceptions.Exception('Error, there is no class with the name ' + target + ' to be used as target component')
            

        # ok, now I can check if the components contain the port with the
        # specified name
        try:
            sourcePort = helper.getAttrInstance2(source, sourcePortName)
        except Exception:
            raise exceptions.Exception('There is no port with name ' + sourcePortName + ' in the source component')
        if sourcePortId is not None and hasattr(sourcePort,'__getitem__'):
            if type(sourcePortId) is int:
                sourcePort = sourcePort[sourcePortId]
            else:
                raise exceptions.Exception('The source port ID must be an integer')
                
        try:
            targetPort = helper.getAttrInstance2(target, targetPortName)
        except Exception:
            raise exceptions.Exception('There is no port with name ' + targetPortName + ' in the target component')
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
            sourcePortTypeName = helper.getBase('tlm_initiator_port', sourcePort.__class__)
            targetPortTypeName = helper.getBase('tlm_target_port', targetPort.__class__)
        except BaseNotFound, e:
            raise exceptions.Exception('ports ' + sourcePortName + ' and ' + targetPortName + ' cannot be connected because' +
                  'of the following error ' + str(e))

        if helper.getTLMPortType(sourcePortTypeName) != helper.getTLMPortType(targetPortTypeName):
            raise exceptions.Exception('ports ' + sourcePortName + ' and ' + targetPortName + ' are not compatible')

        sourceName = ''
        targetName = ''
        if sourceParams:
            sourceName = sourceParams[0]
        else:
            if not self.unknownComNames.has_key(source):
                self.unknownComNames[source] = 'component' + str(self.unnamedId)
                self.unnamedId = self.unnamedId + 1
            sourceName = self.unknownComNames[source]
        if targParams:
            targetName = targParams[0]
        else:
            if not self.unknownComNames.has_key(target):
                self.unknownComNames[target] = 'component' + str(self.unnamedId)
                self.unnamedId = self.unnamedId + 1
            targetName = self.unknownComNames[target]


        self.connectPortsForce(source, sourcePort,
                                    target, targetPort,
                                sourceConnParams, targConnParams)

    def connectPortsForce(self, source, sourcePort, target,
                           targetPort, sourceConnParams = None, targConnParams = None):
        """Method used to connect two ports whose name is not directly visibile from
        python (such as for example ports which are inserted inside a vector): in this
        case we can't specify the name of the ports which must be connected, but we
        directly have to pass the instance of the port itself to python. Note that in
        these situations it is not possible to use the xml input configuration
        file, but we must directly use the python script to specify the architecture
        Note that in this case no checks are performed to be sure that the ports
        are compatible
        Actually this method may also be used to generically connect two components
        without checking that the types of the ports we are connectins match"""
        # ok, I simply have to allocate a new element in the compToConnection; note
        # that if an element already exists in this map I don't have to allocate a
        # new one, but I have to add a connection to the node
        if not 'name' in dir(target):
            raise NotAPort('Component ' + str(target) + ' doesn\'t have the method name; probably it is not an sc_module')

        if not 'name' in dir(source):
            raise NotAPort('Component ' + str(source) + ' doesn\'t have the method name; probably it is not an sc_module')

        sourceName = super(self.scwrapper.sc_module, source).name()
        if self.compToConnection.has_key(sourceName):
            sourceNode = self.compToConnection[sourceName]
        else:
            sourceNode = ConnectionNode(source, sourceName)
            self.compToConnection[sourceName] = sourceNode
        try:
            targetName = super(self.scwrapper.sc_module, target).name()
        except TypeError:
            targetName = 'Unnamed'
        if self.compToConnection.has_key(targetName):
            targetNode = self.compToConnection[targetName]
        else:
            targetNode = ConnectionNode(target, targetName)
            self.compToConnection[targetName] = targetNode

        if not 'bind' in dir(sourcePort):
            raise NotAPort('Component ' + sourceName + ' doesn\'t have the method bind among the ones of the port ' + str(sourcePort))

        sourceNode.addTarget(targetName, targetPort, sourcePort)
        targetNode.addSource(sourceName, sourcePort, targetPort)

        #TODO: I would need to go down in the hierarchy of classes on the port and cast the sourcePort to the class which is really the port
        #something similar to what I did to detrmine the name of the components
        sourcePort.bind(targetPort)

    def connectSyscPorts(self, source, sourcePort, target,
                           targetPort, sourceParams = None, targParams = None):
        """Connects together two systemc (NOT TLM) ports; a signal
        is used to connect together the ports, sourceParams[0] and targParams[0] are the
        names of the components we want to connect; target and source ports may either
        be the name of the port or the instance of it. Also the source and target
        components may either be strings (the names of the components) or the
        instances of those components
        sourceParams and targParams represent the parameters which must be passed to
        the constructor of the source and target components in case they have to
        be built (so in case source and target are string)"""
        # first of all I check to see if source and target are strings and in
        # case I create the instance of the corresponding component
        if type(source) == types.StringType:
            source = helper.getInstance(source, sourceParams, self.components)
            if type(source) == types.StringType:
                raise exceptions.Exception('Error, there is no class with the name ' + source + ' to be used as source component')
        if type(target) == types.StringType:
            target = helper.getInstance(target, targParams, self.components)
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
            raise NotAPort('Component ' + str(target) + ' doesn\'t have the method name; probably it is not an sc_module')

        if not 'name' in dir(source):
            raise NotAPort('Component ' + str(source) + ' doesn\'t have the method name; probably it is not an sc_module')

        sourceName = super(self.scwrapper.sc_module, source).name()
        targetName = super(self.scwrapper.sc_module, target).name()

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
            raise NotAPort('Component ' + sourceName + ' doesn\'t have the method bind among the ones of the port ' + str(sourcePort))
        if not 'bind' in dir(targetPort):
            raise NotAPort('Component ' + targetName + ' doesn\'t have the method bind among the ones of the port ' + str(targetPort))

        # now I create the signal, connect the components with it and finally
        # add the connection node; note that the signal depends on the type of
        # the ports we are trying to connect: so, first of all I have to
        # deterimine it
        try:
            sourcePortTypeName = helper.getBase('sc_out', sourcePort.__class__)
        except BaseNotFound:
            try:
                sourcePortTypeName = helper.getBase('sc_inout', sourcePort.__class__)
            except BaseNotFound:
                raise exceptions.Exception('ports ' + str(sourcePort) + ' and ' + str(targetPort) + ' cannot be connected because' +
                      'No classes in the inheritance chain of ' + str(sourcePort.__class__.__name__) + ' starts with sc_inout or sc_in')
        except Exception, e:
            raise exceptions.Exception('ports ' + str(sourcePort) + ' and ' + str(targetPort) + ' cannot be connected because' +
                  'of the following error ' + str(e))

        try:
            targetPortTypeName = helper.getBase('sc_in', targetPort.__class__)
        except BaseNotFound:
            try:
                targetPortTypeName = helper.getBase('sc_inout', targetPort.__class__)
            except BaseNotFound:
                raise exceptions.Exception('ports ' + str(sourcePort) + ' and ' + str(targetPort) + ' cannot be connected because' +
                      'No classes in the inheritance chain of ' + str(targetPort.__class__.__name__) + ' starts with sc_inout or sc_out')
        except Exception, e:
            raise exceptions.Exception('ports ' + str(sourcePort) + ' and ' + str(targetPort) + ' cannot be connected because' +
                  'of the following error ' + str(e))

        sourcePortType = helper.getSysCPortType(sourcePortTypeName)
        targetPortType = helper.getSysCPortType(targetPortTypeName)
        if sourcePortType != targetPortType:
            raise exceptions.Exception('ports ' + str(sourcePort) + ' and ' + str(targetPort) + ' are not compatible')

        # Finally I create the signal and connect the ports
        try:
            connSignalType = getattr(self.scwrapper, 'sc_signal_less_' + sourcePortType + '_greater_')
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
        # first of all I check to see if source and target are strings and in
        # case I create the instance of the corresponding component
        if type(source) == types.StringType:
            source = helper.getInstance(source, sourceParams, self.components)
            if type(source) == types.StringType:
                raise exceptions.Exception('Error, there is no class with the name ' + source + ' to be used as source component')
        if type(target) == types.StringType:
            target = helper.getInstance(target, targParams, self.components)
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
            raise NotAPort('Component ' + str(target) + ' doesn\'t have the method name; probably it is not an sc_module')

        if not 'name' in dir(source):
            raise NotAPort('Component ' + str(source) + ' doesn\'t have the method name; probably it is not an sc_module')

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
            raise NotAPort('Component ' + targetName + ' doesn\'t have the method bind among the ones of the port ' + str(targetPort))

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
        except BaseNotFound:
            try:
                targetPortTypeName = helper.getBase('sc_inout', targetPort.__class__)
            except BaseNotFound, e:
                try:
                   targetPortTypeName = helper.getBase('sc_in', targetPort.__class__)
                except BaseNotFound, e:
                   raise exceptions.Exception(str(sourceSignal) + ' and ' + str(targetPort) + ' cannot be connected because' +
                         'of the following error ' + str(e))
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
        pass

    def getComponents(self):
        """Returns a tuple with the names of the components present in the architecture;
        actually only their name is returned"""
        components = []
        for comp in self.compToConnection.keys():
            components.append(comp)
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
                raise ComponentNotFound('Component ' + component + ' not present in the loaded architecture')
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
                raise ComponentNotFound('Component ' + component + ' not present in the loaded architecture')
            return self.compToConnection[component].getSources()
        else:
            #I now have to discover the the name of the component
            for comp, connNode in self.compToConnection.items():
                if component == connNode.component:
                    return self.compToConnection[connNode.componentName].getSources()
        return ()

    def areConnected(self, source, srcPortName, target, tgtPortName):
        """Checks whether source and target are already connected together
        on ports srcPort and tgtPort. source and target may represent
        both the instance of the component or its name. On the other
        hand srcPortName and tgtPortName represents the name of
        the source and target ports inside those components"""
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
        if not self.compToConnection.has_key(sourceName):
            return False
        if targetName in self.compToConnection[sourceName].getTargets():
            return True
        else:
            return False


    def getCompInstance(self, name):
        """Given the component's name this method returns its instance"""
        if self.compToConnection.has_key(name):
            return self.compToConnection[name].component
        else:
            return None

    def setParameters(self, name, value, env):
        """Takes as input two strings: the first one represents the full path
        of a python object representing a parameters inside a component; the second
        one represents the value which has to be assigned to that component"""
        [instance, attrName] = helper.getAttrInstance(name, env)
        try:
            castedVal = getattr(instance, attrName).__class__(value)
        except:
            print 'It is not possible to convert value ' + value + ' to an object of type ' + str(getattr(instance, attrName).__class__)
            sys.exit()
        setattr(instance, attrName, value)

    def __repr__(self):
        """Returns a formal representation of this class"""
        representation = ''
        for elem in self.compToConnection.values():
            representation += repr(elem) + '\n'
        return representation

    def __str__(self):
        """Returns an informal string representation of this class"""
        return repr(self)

    def listComponents(self):
        """Returns the list of the current available architectural components"""

        # ok: the components should have already been imported in the current python
        # session: I simply have to get the path of all the imported modules : if it contains
        # the __build__/default/component string, then this is a component and I can list it
        # Note actually that the module containing the component is listed, not the class
        # defining the component itself
        symbolNamespace = [self.archc]
        symbolNamespace += self.components
        foundComponents = []
        for module in self.components:
            if '__file__' in dir(module) and '__name__' in dir(module):
                if module.__file__.find('_build_' + os.sep + 'default' + os.sep + 'component') != -1:
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
                        helper.findConnectionsRecv(comp, realComp, [], self.scwrapper, symbolNamespace)
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
        for c in ComponentManager.getComponents(self):
            nodes = nodes + "\t"+str(c)+"[shape=box]\n"
            nodes = nodes + "\n"
            for out in ComponentManager.getTargets(self,c):
                edges = edges + "\t" + str(c) + "->"+ str(out) +"\n"
        
        code = "digraph simple_hierarchy {\n" + nodes + "\n" + edges + "\n}"
        f = open("__tmpGraph.dot",'w')
        f.write(code)
        f.close()
        import os
        os.system('dotty __tmpGraph.dot')
        os.system('rm __tmpGraph.dot')

    def reset(self):
        self.compToConnection = {}
        self.unknownComNames = {}
        self.unnamedId = 0