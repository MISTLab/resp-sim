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

""" Gemeric

    This modules contains the base code for the power analyzer,
    i.e. all the classes and routines which keep track of power modeling
"""

import respkernel

####################      PROBES      ####################

class probe():
    """ Probe

        A basic connection between variables and power models
    """
    def __init__(self, object, attribute=None, element=None):
        """ Instantiate a probe type """
        self.object = object
        self.attribute = attribute
        self.element = element
        self.timestamp = 0

    def __call__(self):
        """ Returns the probe value """
        if self.attribute is not None:
            if self.element is not None:
                value = getattr( self.object , self.attribute )[self.element]
            else:
                value = getattr( self.object , self.attribute )
        else:
            value = self.object

        if callable(value) : v = value()
        else: v = value
        self.timestamp = respkernel.get_simulated_time()

        return v


class model(object):
    """ PowerModel

        A basic power model class
    """

    PUSH=0
    PULL=1

    def __init__(self, mode, probes, parameters=[]):
        self.mode = mode
        self.power = 0
        self.energy = 0

        # Set initial probes
        self.probes = dict()
        for p in probes:
            self.probes[p] = None

        self.name = None
        self.parameters = parameters

    def set_name(self, name):
        if callable(name):
            self.name = name()
        else:
            self.name = name

    def update_values(self):
        """ To be reimplemented by each model """
        print "WARNING: Unimplemented model!"

    def __getitem__(self, key):
        """ Returns a probe if available, otherwise a parameter """
        try:
            return self.probes[key]
        except KeyError:
            return self.parameters[key]

    def __getattribute__(self, key):
        """ Returns a probe or parameter """
        if key == 'PULL'or key == 'PUSH':
            return object.__getattribute__(self, key)
        try:
            if (object.__getattribute__(self, 'mode') == self.PULL) and (key == 'energy' or key == 'power'):
                object.__getattribute__(self, 'update_values')()
                return object.__getattribute__(self, key)
        except:
            return object.__getattribute__(self, key)

        try:
            return super(model,self).__getattribute__(key)
        except AttributeError:
            try:
                return super(model,self).__getattribute__('probes')[key]()
            except KeyError:
                return super(model,self).__getattribute__('parameters')[key]


    def __setitem__(self, key, value):
        """ Sets a parameter (probes are defined statically) """
        self.parameters[key] = value

    def __setattr__(self, attr, value):
        """ Connects a probe (tricky!) """

        # We have to try as self.probes might not be defined yet
        #print "Setting " , attr
        try:

            # If the attribute is a probe, set it
            if attr not in self.probes:
                object.__setattr__(self, attr, value)
            else:
                # If it is a function, wrap it in a probe, otherwise use a lambda
                if callable(value):
                    self.connect_probe( attr, probe( value ) )
                else:
                    # Check if the passed value is an object => i.e. we should get the probe
                    # out of the object
                    try:
                        getattr( value, attr )
                        self.connect_probe( attr, probe( value, attr ) )
                    except:
                        # Otherwise fallback to a lambda
                        self.connect_probe( attr, probe( lambda: value ) )
        except:
            # Fallback to the standard method
            object.__setattr__(self, attr, value)

    def get_probes(self):
        return self.probes.keys()

    def connect_probe(self, name, _probe):
        if isinstance( _probe, probe ):
            self.probes[name] = _probe
        else:
            self.probes[name] = probe( _probe )

    def get_current_time(self):
        time = respkernel.get_simulated_time()
        if not time:
            return 1
        else:
            return time

    # Pretty printing
    def __str__(self):
        return str(self.__class__)

from stats import connected_models

class Estimator(object):
    """ Estimator
        Globally allows power estimation based on data
    """

    def __init__(self):
        """ Nothing to be done here """
        pass

    def set_estimated_probe(name, value, type_restriction=None):
        """ Sets all probes of a given name to the specified value """
        for m in connected_models.keys():
            if type_restriction and type_restriction != connected_models[m].kind: continue
            if name in connected_models[m].probes:
                setattr(connected_models[m], name, value )

    def set_global_parameter(name, value , type_restriction=None):
        """ Sets all parameters of a given name to the specified value """
        for m in connected_models.keys():
            if type_restriction and type_restriction != connected_models[m].kind: continue
            if name in connected_models[m].parameters.keys():
                connected_models[m][name] = value

    def apply_probe_modifier(name, modifier , type_restriction=None ):
        for m in connected_models.keys():
            if type_restriction and type_restriction != connected_models[m].kind: continue
            if name in connected_models[m].probes:
                exec ('connected_models["%s"].%s = connected_models["%s"].%s%s' % ( m, name, m , name, modifier)) in globals()
