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

import csv, sys, os
from pyx import *

# Now I read all the CSV files in the folder and, for each of them, I print
# the graph with all the simulated points and with the pareto curve for the
# non-dominated points
if __name__ == "__main__":
    pareto_style = [graph.style.line([style.linewidth.Thick, style.linestyle.solid, color.rgb.blue]), graph.style.symbol()]
    dominated_style = [graph.style.symbol(graph.style.symbol.circle)]

    files = os.listdir('.')
    files = filter(lambda a: os.path.splitext(a)[1] == '.csv' , files)
    for i in files:
        print 'Analyzing file ' + str(i)
        fileHandle = open(i, 'r')
        fileHeader = fileHandle.readline().split(';')
        if len(fileHeader) != 3:
            print 'Error, header of file ' + i + ' not valid'
            fileHandle.close()
            continue
        numParameters = int(fileHeader[0])
        numNumObj = int(fileHeader[1])
        numNonDom = int(fileHeader[2])
        if numNumObj != 2:
            print 'Error, there are ' + str(numNumObj) + ' metrics, unable to create the graph for file ' + i
            fileHandle.close()
            continue
        fileCsvReader = csv.reader(fileHandle, delimiter = ';')
        paramMetricsName = fileCsvReader.next()
        if len(paramMetricsName) != numParameters + numNumObj:
            print 'Error, the header of file ' + i + ' is different from its actual content'
            fileHandle.close()
            continue
        g_cat = graph.graphxy(width=8, x=graph.axis.linear(title=paramMetricsName[numParameters],painter=graph.axis.painter.regular(gridattrs=[style.linestyle.dashed,style.linewidth.Thin,color.gray(0.5)])), y=graph.axis.linear(title=paramMetricsName[numParameters + 1],painter=graph.axis.painter.regular(gridattrs=[style.linestyle.dashed,style.linewidth.Thin,color.gray(0.5)])))
        # Now I get the points on the pareto curve
        graphPoints = []
        for j in range(0, numNonDom):
            paramMetricsVals = fileCsvReader.next()
            graphPoints.append( (float(paramMetricsVals[numParameters]), float(paramMetricsVals[numParameters + 1])) )
        graphPoints = sorted(graphPoints)
        totalPoints = len(graphPoints)
        g_cat.plot([graph.data.points(graphPoints, x=1, y=2)], styles=pareto_style)
        # Finally I get the dominated points
        graphPoints = []
        for paramMetricsVals in fileCsvReader:
            graphPoints.append( (float(paramMetricsVals[numParameters]), float(paramMetricsVals[numParameters + 1])) )
        totalPoints += len(graphPoints)
        if totalPoints == 1:
            raise Exception('Problems in determining the axis limits since there is only one point in the whole graph')
        if graphPoints:
            g_cat.plot([graph.data.points(graphPoints, x=1, y=2)], styles=dominated_style)
        # Lets finally print the graphs
        g_cat.writePDFfile(os.path.splitext(i)[0])
        g_cat.writeEPSfile(os.path.splitext(i)[0])
        fileHandle.close()
