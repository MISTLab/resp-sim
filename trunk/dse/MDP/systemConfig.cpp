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

#include <cmath>
#include <map>
#include <string>
#include <ostream>

#include "systemConfig.hpp"
#include "configuration.hpp"
#include "pluginIf.hpp"

SystemConfig::SystemConfig(){
    this->dominated = false;
}

///Compares two solutions to see if one dominates the other
///Returns true of the current solution dominates the other
bool SystemConfig::dominates(const SystemConfig & other){
    std::map<std::string, double>::iterator metricIter, metricEnd;
    for(metricIter = this->metric2Value.begin(), metricEnd = this->metric2Value.end(); metricIter != metricEnd; metricIter++){
        if(metricIter->second > other.metric2Value.find(metricIter->first)->second)
            return false;
    }
    return true;
}

///Print CSV file containin the solution
void SystemConfig::printcsv(std::ostream & stream) const{
    stream << alpha << ";";

    std::map<std::string, int>::const_iterator paramIter, paramEnd;
    for(paramIter = this->param2Value.begin(), paramEnd = this->param2Value.end(); paramIter != paramEnd; paramIter++){
        stream << plugin_handler[paramIter->first]->getParameterName(paramIter->second) << ";";
    }
    std::map<std::string, double>::const_reverse_iterator metricIter, metricEnd;
    std::map<std::string, double>::const_reverse_iterator metricIterTemp;
    for (metricIter = this->metric2Value.rbegin(), metricIterTemp = metricIter, metricIterTemp++, metricEnd = this->metric2Value.rend(); metricIter != metricEnd; metricIter++, metricIterTemp++){
        stream << metricIter->second;
        if(metricIterTemp != metricEnd)
            stream << ";";
    }
    stream << std::endl;
}

///Print the current solution to an output stream
void SystemConfig::print(std::ostream & stream) const{
    std::map<std::string, int>::const_iterator paramIter, paramEnd;
    stream << "Parameters:" << std::endl;
    for(paramIter = this->param2Value.begin(), paramEnd = this->param2Value.end(); paramIter != paramEnd; paramIter++){
        stream << '\x09' << paramIter->first << " = " << plugin_handler[paramIter->first]->getParameterName(paramIter->second) << std::endl;
    }
    stream << "Metrics:" << std::endl;
    std::map<std::string, double>::const_iterator metricIter, metricEnd;
    for (metricIter = this->metric2Value.begin(), metricEnd = this->metric2Value.end(); metricIter != metricEnd; metricIter++){
        stream << '\x09' << metricIter->first << "=" << metricIter->second << std::endl;
    }
}

std::ostream& SystemConfig::operator<< (std::ostream &os) const{
    this->print(os);
    return os;
}

std::ostream& operator<< (std::ostream &os, const SystemConfig& conf){
    conf.print(os);
    return os;
}

bool SystemConfig::operator ==( const SystemConfig & other ) const{
    std::map<std::string, int>::const_iterator param2ValueIter, param2ValueEnd, foundParameter;
    std::map<std::string, double>::const_iterator metric2ValueIter, metric2ValueEnd, foundMetric;

    for(param2ValueIter = this->param2Value.begin(), param2ValueEnd = this->param2Value.end(); param2ValueIter != param2ValueEnd; param2ValueIter++){
        foundParameter = other.param2Value.find(param2ValueIter->first);
        if(foundParameter == other.param2Value.end())
            return false;
        if(foundParameter->second != param2ValueIter->second)
            return false;
    }

    for(metric2ValueIter = this->metric2Value.begin(), metric2ValueEnd = this->metric2Value.end(); metric2ValueIter != metric2ValueEnd; metric2ValueIter++){
        foundMetric = other.metric2Value.find(metric2ValueIter->first);
        if(foundMetric == other.metric2Value.end())
            return false;
        if(fabs(foundMetric->second - metric2ValueIter->second) > config.eps*std::min(foundMetric->second,metric2ValueIter->second)){
            return false;
        }
    }

    return true;
}
