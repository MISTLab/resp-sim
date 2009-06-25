/***************************************************************************\
 *
 *
 *         ___           ___           ___           ___
 *        /  /\         /  /\         /  /\         /  /\
 *       /  /::\       /  /:/_       /  /:/_       /  /::\
 *      /  /:/\:\     /  /:/ /\     /  /:/ /\     /  /:/\:\
 *     /  /:/~/:/    /  /:/ /:/_   /  /:/ /::\   /  /:/~/:/
 *    /__/:/ /:/___ /__/:/ /:/ /\ /__/:/ /:/\:\ /__/:/ /:/
 *    \  \:\/:::::/ \  \:\/:/ /:/ \  \:\/:/~/:/ \  \:\/:/
 *     \  \::/~~~~   \  \::/ /:/   \  \::/ /:/   \  \::/
 *      \  \:\        \  \:\/:/     \__\/ /:/     \  \:\
 *       \  \:\        \  \::/        /__/:/       \  \:\
 *        \__\/         \__\/         \__\/         \__\/
 *
 *
 *
 *
 *   This file is part of ReSP.
 *
 *   TRAP is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *   or see <http://www.gnu.org/licenses/>.
 *
 *
 *
 *   (c) Giovanni Beltrame, Luca Fossati
 *       Giovanni.Beltrame@esa.int fossati@elet.polimi.it
 *
\***************************************************************************/

#include <map>
#include <string>
#include <cmath>

#include <boost/lexical_cast.hpp>

#include "ObjectiveFun.hpp"

#include "utils.hpp"

ObjectiveFunction::ObjectiveFunction(const std::map<std::string, std::string> & objFunParams){
    std::map<std::string, std::string>::const_iterator foundAlpha = objFunParams.find("alpha");
    if(foundAlpha == objFunParams.end())
        THROW_EXCEPTION("The objective function requires parameter alpha to be defined in the configuration file");
    this->alpha = boost::lexical_cast<double>(foundAlpha->second);
}

double ObjectiveFunction::estimate(const std::map<std::string, float> &metrics){
    double outValue = 1;
    std::map<std::string, float>::const_iterator metricIter, metricEnd;
    for(metricIter = metrics.begin(), metricEnd = metrics.end(); metricIter != metricEnd; metricIter++){
        if(metricIter->first == "execTime")
            outValue *= metricIter->second;
        else if(metricIter->first == "energy")
            outValue *= pow(metricIter->second, this->alpha);
        else
            THROW_EXCEPTION(metricIter->first << " is an unknown metric value");
    }
    return outValue;
}

double ObjectiveFunction::estimate(const std::map<std::string, double> &metrics){
    double outValue = 1;
    std::map<std::string, double>::const_iterator metricIter, metricEnd;
    for(metricIter = metrics.begin(), metricEnd = metrics.end(); metricIter != metricEnd; metricIter++){
        if(metricIter->first == "execTime")
            outValue *= pow(metricIter->second, 1-this->alpha);
        else if(metricIter->first == "energy")
            outValue *= pow(metricIter->second, this->alpha);
        else
            THROW_EXCEPTION(metricIter->first << " is an unknown metric value");
    }
    return outValue;
}
