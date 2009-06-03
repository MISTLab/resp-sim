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

#ifndef OBJECTIVEFUN_HPP
#define OBJECTIVEFUN_HPP

#include <map>
#include <string>

///Note how, so far, in order to change the objective function
///we need to change this class. In future a better mechanism
///could be used and we could think of using a plugin architecture
///such as the one employed for the transformations
class ObjectiveFunction{
  public:
    double alpha;
    ObjectiveFunction(const std::map<std::string, std::string> &objFunParams);
    ///Given the metrics values, it estimates the scalarizing function for
    ///those metrics
    double estimate(const std::map<std::string, float> &metrics);
    double estimate(const std::map<std::string, double> &metrics);
};

#endif
