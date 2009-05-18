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
            std::map<std::string, float> statistics;
            std::string application;

            void print(std::ostream & stream) const;
            void read(std::string &line);
            bool operator ==( const SystemConfig & other ) const;
            bool equal( const SystemConfig & other ) const;
            bool equal(const std::vector<std::string> &objectiveNames,
                const std::map<std::string, std::string> &param2Value, const std::string &application) const;
        };

        std::vector<SystemConfig> cacheConfigs;
    public:
        SimulationCache(std::string cacheFileName);
        ~SimulationCache();
        /// Looks in the simulation cache for the specified solution
        bool find(const std::vector<std::string> &objectiveNames, const std::map<std::string, std::string> &param2Value,
                const std::string &application, std::map<std::string, double> &objectiveValues, std::map<std::string, float> &statistics);
        /// Adds an element to the simulation cache and dumps it on the cache file
        void add(const std::map<std::string, double> &objective2Value,
                const std::map<std::string, std::string> &param2Value, const std::map<std::string, float> &statistics, const std::string &application);
};

#endif
