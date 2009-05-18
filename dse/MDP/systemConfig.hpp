/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *                                                                         *
 ***************************************************************************/

#ifndef SOLUTION_HPP
#define SOLUTION_HPP

#include <map>
#include <string>
#include <ostream>
#include <iostream>

#include "map_defs.hpp"

#ifdef MEMORY_DEBUG
#include <mpatrol.h>
#endif

///Represents a system configuration of the MDP; such a configuration
///is determined through simulation, thus is has precise values for the metrics
struct SystemConfig{
    bool dominated;

    ///maps a parameter to its current value
    int_map param2Value;

    ///maps a metric to its value
    double_map metric2Value;

    ///holds the statistics simulated for each parameter
    stats_map param2Stats;

    ///holds the target alpha
    double alpha;

    SystemConfig();

    ///Compares two solutions to see if one dominates the other
    ///Returns true of the current solution dominates the other
    bool dominates(const SystemConfig & other);

    ///Print CSV file containin the solution
    void printcsv(std::ostream & stream) const;

    ///Print the current solution to an output stream
    void print(std::ostream & stream) const;
    std::ostream& operator<< (std::ostream &os) const;

    ///Operator for comparing two solutions
    bool operator ==( const SystemConfig & other ) const;
};

std::ostream& operator<< (std::ostream &os, const SystemConfig& conf);

#endif
