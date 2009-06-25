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

""" Power

    This package contains all the python code for the power analyzer,
    i.e. all the classes and routines which keep track of power modeling
"""

#################### HELPER FUNCTIONS ####################

from generic import *
import stats
import processor, memory, cache

models = dict()
models['processor'] = filter( lambda x: not x.startswith('_') and x != 'model' , dir(processor))
models['memory'] = filter( lambda x: not x.startswith('_') and x != 'model' , dir(memory))
models['cache'] = filter( lambda x: not x.startswith('_') and x != 'model' , dir(cache))

def connect_power_model( obj , model , throw = False):
    """ connect_model
        Connects an object to a given power model if possible
    """
    # TODO Should check probe kind...
    # Get the necessary probes from the model
    probes = model.get_probes()

    for p in probes:
        try:
            # Check that the probes exists in the object
            #print "Looking for probe %s" % (p)
            attribute = getattr( obj, p )
            #print "Found probe %s" % (p)

            # If present, connect it to the model
            #print "Connecting probe %s" % (p)
            setattr( model, p , obj )
            #print "Connected probe %s" % (p)

        except Exception, e:
            # Bail out if probe not present
            del model
            if throw: raise Exception("Model not compatible")
            else:
                print e
                return

    model.set_name(obj.name)
    stats.connected_models[model.name]= model


def add_power_model( obj ):
    # For all the models in all packages
    # TODO: Use all classes that subclass generic.model...
    for key in models.keys():
        for m in models[key]:
            try:
                # Try to connect a newly created model
                #print "Trying", key, m
                connect_power_model( obj , getattr( globals()[key] , m )() , True )
                #print "Model found", key, m
                return
            except Exception, e:
                #print e
                pass
    print "No compatible model found for object ", obj

def reset():
    for model in stats.connected_models.values():
        del model.probes
        del model
    stats.connected_models = {}

__all__ = [ "generic", "stats", "processor" , "memory", "cache" , "connect_power_model", "add_power_model" ]
