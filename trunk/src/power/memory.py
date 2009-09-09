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


from generic import model

class simple_memory(model):
    """ A simple memory model using read and write costs """

    def __init__(self, parameters=None):
        """ Set the defaults for the arm7tdmi """

        # Set default parameters (ARM7TDMI 0.13um)
        if not parameters:
            parameters = dict()
            parameters["read_energy"] = 0.1318707   # pJ
            parameters["write_energy"] = 0.1546865  # pJ
            parameters["clock_frequency"] = 500     # MHz

        # Set probe names
        probes = [ 'num_reads', 'num_writes' ]               # NUmbers of reads and writes

        # Specify the model type
        self.kind = 'memory'

        # Init model (model.PULL mode)
        model.__init__(self, model.PULL, probes, parameters)


    def update_values(self):
        """ To be reimplemented by each model """
        num_reads = self["num_reads"]
        num_writes = self["num_writes"]
        period = self["clock_period"]
        renergy = self["read_energy"]
        wenergy = self["write_energy"]

        self.energy =  (num_reads*renergy + num_writes*wenergy)*1e-9   # pJ -> mJ
        self.power =  ((renergy*period + wenergy*period)*1e6)/2     # pJ = uW * MHz -> mW


class interpolated_memory(model):
    """ An interpolated memory model using a samples file """

    def __init__(self, parameters=None):
        """ Set the defaults for a 16KB memory in 4 bytes lines without multiplexers """

        # Set default parameters (ARM7TDMI 0.13um)
        if not parameters:
            parameters = dict()
            parameters["size"] = 16384                 # Bytes
            parameters["line_width"] = 4               # Bytes
            parameters["word_size"] = 32               # Bits

        # Set probe names
        probes = [ 'num_reads', 'num_writes' ]                  # NUmbers of reads and writes

        # Specify the model type
        self.kind = 'memory'

        # Init model (model.PULL mode)
        model.__init__(self, model.PULL, probes, parameters)

        # Standard energy and power values
        self.renergy = 23.21
        self.wenergy = 27.34

        # Default coefficients
        # best fit: XXXPow = c[0] * words + c[1] * bits + c[2] * mux + c[3] * area + c[4]"
        self.c_read = [ -1.09167887e-05 ,  6.05436716e-01 ,  2.04028667e-02 ,  2.76256799e-06 ,  3.49943207e+00]
        self.c_write = [ -1.79541119e-05 ,  7.16287132e-01 ,  3.25601331e-02 ,  3.91476756e-06 ,  3.94571979e+00]
        self.c_leakage = [  2.92556486e-02 , 1.05218644e+01 , 2.02997888e+00 , 1.86243641e-03 , 1.27200413e+03]

        self.update_parameters()


    def __setitem__(self, key, value):
        """ Sets a parameter and recomputes associated parameters """
        self.parameters[key] = value
        if key == 'sample_file': self.update_coefficients()
        self.update_parameters()


    def update_parameters(self):
        """ Recalculates the model's constants when parameters are changed """
        import math
        words = (self["size"]/self["line_width"])
        bits = self["word_size"]
        area = words * bits
        mux = math.log(words, 2) + math.log(self["line_width"],2)

        self.renergy = words*self.c_read[0] + bits*self.c_read[1] + mux*self.c_read[2] + area*self.c_read[3] + self.c_read[4]
        self.wenergy = words*self.c_write[0] + bits*self.c_write[1] + mux*self.c_write[2] + area*self.c_write[3] + self.c_write[4]
        self.leakage = words*self.c_leakage[0] + bits*self.c_leakage[1] + mux*self.c_leakage[2] + area*self.c_leakage[3] + self.c_leakage[4]


    def update_coefficients(self):
        """ Uses the samples file to extract parameters using linear multifit """
        import pygsl
        import pygsl._numobj as Numeric
        import pygsl.rng
        import pygsl.multifit

        from scipy.io import read_array
        from numpy import array

        data = read_array(file(self['sample_file']))
        dep_var = data[:,-3:]        # Last 3 columns are supposed to be Renergy, Wenergy, Leakage
        indep_var = data[:,:5]       # First 3 columns are of interest, 2 are just used as fillers

        indep_var[:,-2] = indep_var[:,0] * indep_var[:,1]  # We also consider words * bits as a parameter
        indep_var[:,-1:] = 1                               # Coefficient used as a constant

        work = pygsl.multifit.linear_workspace(len(indep_var), 5)
        self.c_read, cov, chisq = pygsl.multifit.linear(indep_var, dep_var[:,0], work)
        self.c_write, cov, chisq = pygsl.multifit.linear(indep_var, dep_var[:,1], work)
        self.c_leakage, cov, chisq = pygsl.multifit.linear(indep_var, dep_var[:,2], work)

        #print "# best fit: ReadPow = %g * words + %g * bits + %g * mux + %g * area + %g" % tuple(c_read)
        #print "# best fit: WritePow = %g * words + %g * bits + %g * mux + %g * area + %g" % tuple(c_write)
        #print "# best fit: Leakage = %g * words + %g * bits + %g * mux + %g * area + %g" % tuple(c_leakage)

    def update_values(self):
        """ To be reimplemented by each model """
        # No need to interpolate every time, use the stored values
        current_time = self.get_current_time()*1e-3 # ps -> ns

        # leakage (uW) * current_time (ns) => fJ => pJ * 1e-3
        energy =  (self.num_reads*self.renergy + self.num_writes*self.wenergy + self.leakage*current_time*1e-3)*1e-9    # pJ -> mJ

        if current_time == 0:
            self.power = 0
        else:
            self.power =  energy/(current_time*1e-9)     # pJ = uW * MHz -> mW
        self.energy = energy
