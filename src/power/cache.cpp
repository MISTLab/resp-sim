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
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 *
 *   (c) Giovanni Beltrame, Luca Fossati
 *       Giovanni.Beltrame@esa.int fossati@elet.polimi.it
 *
\***************************************************************************/


#include <cache.hpp>
#include <ecacti_wrapper.h>
#include <boost/filesystem/fstream.hpp>
#include <boost/tokenizer.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <sstream>

ECacti::ECacti() : ecacti_cache() {
    this->parameters["size"] = 16384;                  // Bytes
    this->parameters["block_size"] = 32;               // Block size in bytes
    this->parameters["associativity"] = 1;             // Power of two
    this->parameters["subbanks"] = 1.0;                // Power of two
    this->parameters["technology"] = 0.07;             // In micron (e.g. 0.07)
    this->parameters["read_ports"] = 0;                // Number
    this->parameters["write_ports"] = 0;               // Number
    this->parameters["rw_ports"] = 1;                  // Number

    this->probes["readHitNum"] = 0.0;
    this->probes["writeHitNum"] = 0.0;
    this->probes["readMissNum"] = 0.0;
    this->probes["writeMissNum"] = 0.0;
    this->probes["execution_time"] = 0.0;

    this->rhitenergy = 1.5260595272768383e-10;
    this->whitenergy = 1.2664516868313481e-10;
    this->rmissenergy = 3.2881930812357897e-10;
    this->wmissenergy = 1.5498319357070943e-10;
    this->leakage = 0.01638873861267354;

    this->load_cache();
}

void ECacti::update_parameters() {
    // (self.size , self.block_size , self.associativity, self.technology , self.subbanks, self.read_ports, self.write_ports, self.rw_ports)
    std::string key= str(boost::format("%d,%d,%d,%1.6f,%d,%d,%d,%d,")
                     % parameters["size"]
                     % parameters["block_size"]
                     % parameters["associativity"]
                     % parameters["technology"]
                     % parameters["subbanks"]
                     % parameters["read_ports"]
                     % parameters["write_ports"]
                     % parameters["rw_ports"]);

    std::map<std::string, std::vector<double> >::iterator item = ecacti_cache.find(key);
    if( item != ecacti_cache.end() ) {
        this->rhitenergy = item->second[0];
        this->whitenergy = item->second[1];
        this->rmissenergy = item->second[2];
        this->wmissenergy = item->second[3];
        this->leakage = item->second[4];
    } else {
/*        for( std::map<std::string, std::vector<double> >::iterator it = ecacti_cache.begin(); it != ecacti_cache.end() ; it++ )
            std::cout << "##" << it->first << "##" << std::endl;*/

        resp::cache_power_data data;
        data = resp::get_power_data( parameters["size"]
                                            , parameters["block_size"]
                                            , parameters["associativity"]
                                            , parameters["technology"]
                                            , parameters["subbanks"]
                                            , parameters["read_ports"]
                                            , parameters["write_ports"]
                                            , parameters["rw_ports"] );

        this->rhitenergy = data.read_hit;
        this->whitenergy = data.write_hit;
        this->rmissenergy = data.read_miss;
        this->wmissenergy = data.write_miss;
        this->leakage = data.leakage;

        std::vector<double> vec;
        vec.push_back(data.read_hit);
        vec.push_back(data.write_hit);
        vec.push_back(data.read_miss);
        vec.push_back(data.write_miss);
        vec.push_back(data.leakage);

        ecacti_cache[key] = vec;

        //this->save_cache();
    }

}

double ECacti::get_energy() {
    double energy =  (this->probes["readHitNum"]*this->rhitenergy + this->probes["writeHitNum"]*this->whitenergy
                +this->probes["readMissNum"]*this->rmissenergy + this->probes["writeMissNum"]*this->wmissenergy )*1e3;   // J -> mJ
    energy += this->leakage*this->probes["execution_time"]*1e-6;  // W -> mW

    return energy;
}
// std::map<std::string, std::vector<double> > ecacti_cache;
void ECacti::load_cache(std::string filename) {
    boost::filesystem::ifstream cacheReader(filename);
    std::string line;
    // Clear the cache
    ecacti_cache.clear();

    // Get a cache line
    while( std::getline( cacheReader, line ) ) {
        // Vector for values
        std::vector<double> values;
        std::stringstream key;

        // Tokenize line
        boost::char_separator<char> sep(",");
        boost::tokenizer<boost::char_separator<char> > tok(line,sep);
        boost::tokenizer<boost::char_separator<char> >::iterator beg=tok.begin();

        // size block associativity subbanks technology readp writep rwp || rhite white rmisse wmiss leak
        for( int i = 0 ; i < 8 ; i++ ) {
            key << *(beg++) << ",";
        }

        for( int i = 0 ; i < 5 ; i++ ) {
            values.push_back(atof(beg->c_str()));
            beg++;
        }
        ecacti_cache[key.str()] = values;
    }

}

void ECacti::save_cache(std::string filename) {
    boost::filesystem::ofstream cacheWriter(filename);
    // size block associativity subbanks technology readp writep rwp || rhite white rmisse wmiss leak
     for( std::map<std::string, std::vector<double> >::iterator it = ecacti_cache.begin(); it != ecacti_cache.end() ; it++ ) {
        cacheWriter << it->first;
        for( int i = 0 ; i < 4 ; i++ )
            cacheWriter << it->second[i] << ",";
        cacheWriter << it->second[5] << std::endl;
     }
}
