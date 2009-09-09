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

def checkDominance(sol1, sol2):
    # This function check if sol1 dominates sol2: in case
    # it returns true, false otherwise; the solutions are simply
    # lists of floating point values; in order for sol1 to dominate sol2
    # all values of sol1 must be smaller than values of sol2
    if len(sol1) != len(sol2):
        raise Exception('Different number of objectives in the solutions')
    for i in range(0, len(sol1)):
        if sol1[i] > sol2[i]:
            return False
    return True

# Now I read all the CSV files in the folder and, for each of them, I print
# the graph with all the simulated points and with the pareto curve for the
# non-dominated points
if __name__ == "__main__":
    if len(sys.argv) < 2:
        raise Exception('The csv cache file to be examined should be provided on the command line')
    if not os.path.exists(sys.argv[1]):
        raise Exception('File ' + sys.argv[1] + ' does not exist')

    pareto_style = [graph.style.line([style.linewidth.Thick, style.linestyle.solid, color.rgb.blue]), graph.style.symbol()]

    fileHandle = open(sys.argv[1], 'r')
    fileCsvReader = csv.reader(fileHandle, delimiter = ';')

    paretoSolutions = []
    paramMetricsName = []
    for solution in fileCsvReader:
        # The format of each solution is numParams;numObj;paramList;objList
        # we simply have to get the objectives
        numObjectives = int(solution[1])
        curSolution = []
        if not paramMetricsName:
            for i in solution[len(solution) - numObjectives:]:
                paramMetricsName.append(i[:i.find('=')])
	temp = solution[len(solution) - numObjectives:]
	temp.reverse()
        for i in temp:
            curSolution.append(float(i[i.find('=') + 1:]))
        # Now I check that the current solution is not dominated by
        # any of the already seen solutions
        dominated = False
        for i in paretoSolutions:
            if checkDominance(i, curSolution):
                dominated = True
                break;
        if not dominated:
            # Now I have to check that the current solution does not
            # domnitate any of the already added solutions
            toRemoveSol = []
            for i in paretoSolutions:
                if checkDominance(curSolution, i):
                    toRemoveSol.append(i)
            for i in toRemoveSol:
                paretoSolutions.remove(i)
            paretoSolutions.append(curSolution)
    fileHandle.close()
    # Now I also print a csv file with the best solutions obtained
    # so far
    fileHandle = open(os.path.splitext(sys.argv[1])[0] + '_pareto.csv', 'w')
    fileCsvWriter = csv.writer(fileHandle, delimiter = ';')
    fileCsvWriter.writerow(['0', str(len(paramMetricsName)), str(len(paretoSolutions))])
    fileCsvWriter.writerow(paramMetricsName)
    for solution in paretoSolutions:
        fileCsvWriter.writerow([str(i) for i in solution])
    fileHandle.close()
    # Lets finally move to graph printing
    g_cat = graph.graphxy(width=8, x=graph.axis.linear(title=paramMetricsName[0],painter=graph.axis.painter.regular(gridattrs=[style.linestyle.dashed,style.linewidth.Thin,color.gray(0.5)])), y=graph.axis.linear(title=paramMetricsName[1],painter=graph.axis.painter.regular(gridattrs=[style.linestyle.dashed,style.linewidth.Thin,color.gray(0.5)])))
    # Now I get the points on the pareto curve
    graphPoints = sorted(paretoSolutions)
    g_cat.plot([graph.data.points(graphPoints, x=1, y=2)], styles=pareto_style)
    # Lets finally print the graphs
    g_cat.writePDFfile(os.path.splitext(sys.argv[1])[0])
    g_cat.writeEPSfile(os.path.splitext(sys.argv[1])[0])
