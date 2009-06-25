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

from generic import model
import csv

class ecacti(model):
    """ A cache model using eCACTI 1.0 """

    def __init__(self, parameters=None):
        """ Set the defaults for the arm7tdmi """

        # Set default parameters (ARM7TDMI 0.13um)
        if not parameters:
            parameters = dict()
            parameters["size"] = 16384                  # Bytes
            parameters["block_size"] = 32               # Block size in bytes
            parameters["associativity"] = 2             # Power of two
            parameters["subbanks"] = 1.0                # Power of two
            parameters["technology"] = 0.07             # In micron (e.g. 0.07)
            parameters["read_ports"] = 0                # Number
            parameters["write_ports"] = 0               # Number
            parameters["rw_ports"] = 1                  # Number
            parameters["param_cache"] = True            # Bool


        # Set probe names
        probes = [ 'readHitNum', 'writeHitNum', 'readMissNum', 'writeMissNum' ]               # NUmbers of reads and writes

        # Default values for the default parameters (J)
        self.rhitenergy = 1.5260595272768383e-10
        self.whitenergy = 1.2664516868313481e-10
        self.rmissenergy = 3.2881930812357897e-10
        self.wmissenergy = 1.5498319357070943e-10
        self.leakage = 0.01638873861267354 # <=== W!

        # Specify the model type
        self.kind = 'cache'

        # Load parameter cache
        #if parameters["param_cache"]:
        self.load_cache()

        # Init model (model.PULL mode)
        model.__init__(self, model.PULL, probes, parameters)
        if parameters is not None: self.update_parameters()


    def __setitem__(self, key, value):
        """ Sets a parameter and recomputes associated parameters """
        self.parameters[key] = value
        self.update_parameters()

    def load_cache(self):
        self.ecacti_cache = dict()

        try:
            cacheReader = csv.reader(open('/tmp/ecacti_cache'))
        except IOError:
            return

        # size block associativity subbanks technology readp writep rwp || rhite white rmisse wmiss leak
        for row in cacheReader:
            self.ecacti_cache[ ' '.join(row[:8]) ] = row[8:]


    def save_cache(self):
        cacheWriter = csv.writer(open('/tmp/ecacti_cache', 'w'))
        # size block associativity subbanks technology readp writep rwp || rhite white rmisse wmiss leak
        for k,v in self.ecacti_cache.items():
            cacheWriter.writerow( k.split() + v )

    def update_parameters(self):
        """ Update the model parameters using eCACTI """
        import ecacti_wrapper
        key = "%d %d %d %f %d %d %d %d" % (self.size , self.block_size , self.associativity, self.technology , self.subbanks, self.read_ports, self.write_ports, self.rw_ports)
        try:

            config = self.ecacti_cache[key]
            self.rhitenergy = float(config[0])
            self.whitenergy = float(config[1])
            self.rmissenergy = float(config[2])
            self.wmissenergy = float(config[3])
            self.leakage = float(config[4])
        except KeyError:
            data = ecacti_wrapper.get_power_data( self.size , self.block_size , self.associativity, self.technology
                                    , self.subbanks, self.read_ports, self.write_ports, self.rw_ports )
            self.rhitenergy = data.read_hit
            self.whitenergy = data.write_hit
            self.rmissenergy = data.read_miss
            self.wmissenergy = data.write_miss
            self.leakage = data.leakage

            self.ecacti_cache[key] = [ data.read_hit , data.write_hit , data.read_miss , data.write_miss , data.leakage ]
            self.save_cache()


    def update_values(self):
        """ To be reimplemented by each model """
        energy =  (self.readHitNum*self.rhitenergy + self.writeHitNum*self.whitenergy
                  +self.readMissNum*self.rmissenergy + self.writeMissNum*self.wmissenergy )*1e3   # J -> mJ
        energy += self.leakage*self.get_current_time()*1e-9  # W -> mW

        self.power =  (energy/(self.get_current_time()*1e-12))    # mW = mJ*Hz
        self.energy = energy
