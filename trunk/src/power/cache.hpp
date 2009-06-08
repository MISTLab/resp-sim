#ifndef CACHE_HPP
#define CACHE_HPP

#include <power.hpp>
#include <string>
#include <vector>
#include <map>

class ECacti : public PowerModel {
    protected:
        double rhitenergy;
        double whitenergy;
        double rmissenergy;
        double wmissenergy;
        double leakage;

        std::map<std::string, std::vector<double> > ecacti_cache;
    public:
        ECacti();
        double get_energy();
        void update_parameters();
        void load_cache(std::string filename = "/tmp/ecacti_cache");
        void save_cache(std::string filename = "/tmp/ecacti_cache");
};


#endif /* CACHE_HPP */
