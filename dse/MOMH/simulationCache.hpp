#ifndef SIMULATIONCACHE_HPP
#define SIMULATIONCACHE_HPP

#include <string>
#include <map>
#include <vector>
#include <iostream>

class SimulationCache{
    private:
        std::string cacheFileName;

        struct SystemConfig{
            std::map<std::string, double> objective2Value;
            std::map<std::string, std::string> param2Value;
            std::string application;

            void print(std::ostream & stream) const;
            void read(std::string line);
            bool operator ==( const SystemConfig & other ) const;
            bool equal( const SystemConfig & other ) const;
            bool equal(const std::vector<std::string> &objectiveNames,
                const std::map<std::string, std::string> &param2Value, const std::string &application) const;
        };

        std::vector<SystemConfig> cacheConfigs;
    public:
        SimulationCache(std::string cacheFileName);
        ~SimulationCache();
        /// Adds an element to the simulation cache and dumps it on the cache file
        void add(const std::vector<std::string> &objectiveNames, const std::vector<double> &objectiveValues,
                const std::map<std::string, std::string> &param2Value, const std::string &application);
        /// Looks in the simulation cache for the specified solution
        std::map<std::string, double> find(const std::vector<std::string> &objectiveNames,
                const std::map<std::string, std::string> &param2Value, const std::string &application);
        /// Adds an element to the simulation cache and dumps it on the cache file
        void add(const std::map<std::string, double> &objective2Value,
                const std::map<std::string, std::string> &param2Value, const std::string &application);
};

#endif
