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

/*
 *   Main driver for the Multi-Objective optimization algorithms
 *   used in ReSP for the design space exploration. In order to use
 *   this code you need the MOMHLib++ library downloadable from
 *   http://home.gna.org/momh/index.html
*/

#include <signal.h>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

#include <boost/program_options.hpp>

#include <momh/mosa.h>
#include <momh/psa.h>
#include <momh/smosa.h>
#include <momh/mogls.h>
#include <momh/pma.h>
#include <momh/immogls.h>
#include <momh/momsls.h>
#include <momh/spea.h>
#include <momh/nsga.h>
#include <momh/nsgaii.h>
#include <momh/nsgaiic.h>

#include "problem.hpp"
#include "solution.hpp"

struct AlgorithmPreferences{
    float sa_start_temperature; // = 1.00;
    float sa_final_temperature; // = 0.01;
    float sa_temperature_decrease_coefficient; // = 0.90;
    int sa_moves_on_temperature_level; // = 100;
    int sa_generating_population_size; // = 10;
    float sa_weights_change_coefficient; // = 0.001;
    int pareto_population_size; // = 100;
    int pareto_number_of_generations; // = 1000;
    float pareto_mutation_probability; // = 0.20;
    bool pareto_scalarize; // = 1;
    int pareto_nondominated_population_size; // = 10000;
    int pareto_clustering_level; // = 20000;
    float pareto_neighbour_distance; // = 0.50;
    float pareto_population_geometrical_factor; // = 0.50;
    int hga_initial_population_size; // = 0;
    int hga_population_size; // = 100;
    int hga_number_of_iterations; // = 10;
    TScalarizingFunctionType hga_scalarizing_function_type; // = _Linear;
    int hga_temporary_population_size; // = 20;
    int hga_ellite_size; // = 10;
    int momsls_number_of_iterations; // = 100;
    TScalarizingFunctionType momsls_scalarizing_function_type;// = _Chebycheff;

    AlgorithmPreferences(){
        this->sa_start_temperature = 1.00;
        this->sa_final_temperature = 0.05;
        this->sa_temperature_decrease_coefficient = 0.90;
        this->sa_moves_on_temperature_level = 20;
        this->sa_generating_population_size = 5;
        this->sa_weights_change_coefficient = 0.005;
        this->pareto_population_size = 2;
        this->pareto_number_of_generations = 100;
        this->pareto_mutation_probability = 0.20;
        this->pareto_scalarize = false;
        this->pareto_nondominated_population_size = 20;
        this->pareto_clustering_level = 20000;
        this->pareto_neighbour_distance = 0.50;
        this->pareto_population_geometrical_factor = 0.50;
        this->hga_initial_population_size = 0;
        this->hga_population_size = 20;
        this->hga_number_of_iterations = 10;
        this->hga_scalarizing_function_type = _Linear;
        this->hga_temporary_population_size = 20;
        this->hga_ellite_size = 10;
        this->momsls_number_of_iterations = 100;
        this->momsls_scalarizing_function_type = _Chebycheff;
    }

    bool load(const std::string &paramFile){
        std::ifstream infile(paramFile.c_str());
        if (!infile.good()){
            return false;
        }
        std::string reminder = "";
        while (infile.good()){
            std::string buffer_string;
            getline(infile, buffer_string);

            TDSEProblem::TrimSpaces(buffer_string);

            if(!buffer_string.empty() && buffer_string.find('#') != 0){
                std::size_t foundEqual = buffer_string.find('=');
                if(foundEqual == std::string::npos){
                    std::cerr << "Error in line \"" << buffer_string << "\" during parsing of Algorithm Parameters"  << std::endl;
                    return false;
                }
                if(buffer_string.size() <= foundEqual + 1){
                    std::cerr << "Error in line \"" << buffer_string << "\" during parsing of Algorithm Parameters"  << std::endl;
                    return false;
                }
                std::string option = buffer_string.substr(0, foundEqual);
                TDSEProblem::TrimSpaces(option);
                std::string optionValue = buffer_string.substr(foundEqual + 1);
                TDSEProblem::TrimSpaces(optionValue);
                if(TDSEProblem::ToLower(option) == "sa_start_temperature"){
                    this->sa_start_temperature = boost::lexical_cast<float>(optionValue);
                }
                else if(TDSEProblem::ToLower(option) == "sa_final_temperature"){
                    this->sa_final_temperature = boost::lexical_cast<float>(optionValue);
                }
                else if(TDSEProblem::ToLower(option) == "sa_temperature_decrease_coefficient"){
                    this->sa_temperature_decrease_coefficient = boost::lexical_cast<float>(optionValue);
                }
                else if(TDSEProblem::ToLower(option) == "sa_moves_on_temperature_level"){
                    this->sa_moves_on_temperature_level = boost::lexical_cast<int>(optionValue);
                }
                else if(TDSEProblem::ToLower(option) == "sa_generating_population_size"){
                    this->sa_generating_population_size = boost::lexical_cast<int>(optionValue);
                }
                else if(TDSEProblem::ToLower(option) == "sa_weights_change_coefficient"){
                    this->sa_weights_change_coefficient = boost::lexical_cast<float>(optionValue);
                }
                else if(TDSEProblem::ToLower(option) == "pareto_population_size"){
                    this->pareto_population_size = boost::lexical_cast<int>(optionValue);
                }
                else if(TDSEProblem::ToLower(option) == "pareto_number_of_generations"){
                    this->pareto_number_of_generations = boost::lexical_cast<int>(optionValue);
                }
                else if(TDSEProblem::ToLower(option) == "pareto_mutation_probability"){
                    this->pareto_mutation_probability = boost::lexical_cast<float>(optionValue);
                }
                else if(TDSEProblem::ToLower(option) == "pareto_scalarize"){
                    this->pareto_scalarize = boost::lexical_cast<bool>(optionValue);
                }
                else if(TDSEProblem::ToLower(option) == "pareto_nondominated_population_size"){
                    this->pareto_nondominated_population_size = boost::lexical_cast<int>(optionValue);
                }
                else if(TDSEProblem::ToLower(option) == "pareto_clustering_level"){
                    this->pareto_clustering_level = boost::lexical_cast<int>(optionValue);
                }
                else if(TDSEProblem::ToLower(option) == "pareto_neighbour_distance"){
                    this->pareto_neighbour_distance = boost::lexical_cast<float>(optionValue);
                }
                else if(TDSEProblem::ToLower(option) == "pareto_population_geometrical_factor"){
                    this->pareto_population_geometrical_factor = boost::lexical_cast<float>(optionValue);
                }
                else if(TDSEProblem::ToLower(option) == "hga_initial_population_size"){
                    this->hga_initial_population_size = boost::lexical_cast<int>(optionValue);
                }
                else if(TDSEProblem::ToLower(option) == "hga_population_size"){
                    this->hga_population_size = boost::lexical_cast<int>(optionValue);
                }
                else if(TDSEProblem::ToLower(option) == "hga_number_of_iterations"){
                    this->hga_number_of_iterations = boost::lexical_cast<int>(optionValue);
                }
                else if(TDSEProblem::ToLower(option) == "hga_temporary_population_size"){
                    this->hga_temporary_population_size = boost::lexical_cast<int>(optionValue);
                }
                else if(TDSEProblem::ToLower(option) == "hga_ellite_size"){
                    this->hga_ellite_size = boost::lexical_cast<int>(optionValue);
                }
                else if(TDSEProblem::ToLower(option) == "momsls_number_of_iterations"){
                    this->momsls_number_of_iterations = boost::lexical_cast<int>(optionValue);
                }
                else if(TDSEProblem::ToLower(option) == "hga_scalarizing_function_type"){
                    if(TDSEProblem::ToLower(optionValue) == "linear"){
                        this->hga_scalarizing_function_type = _Linear;
                    }
                    else if(TDSEProblem::ToLower(optionValue) == "chebycheff"){
                        this->hga_scalarizing_function_type = _Chebycheff;
                    }
                    else{
                        std::cerr << optionValue << " is not a valid scalarizing function type" << std::endl;
                        return false;
                    }
                }
                else if(TDSEProblem::ToLower(option) == "momsls_scalarizing_function_type"){
                    if(TDSEProblem::ToLower(optionValue) == "linear"){
                        this->momsls_scalarizing_function_type = _Linear;
                    }
                    else if(TDSEProblem::ToLower(optionValue) == "chebycheff"){
                        this->momsls_scalarizing_function_type = _Chebycheff;
                    }
                    else{
                        std::cerr << optionValue << " is not a valid scalarizing function type" << std::endl;
                        return false;
                    }
                }
            }
        }
        infile.close();

        // Now I print the loaded configuration
        std::cout << "The parameters driving the exploration are: " << std::endl;
        std::cout << "sa_start_temperature = " << this->sa_start_temperature << std::endl;
        std::cout << "sa_final_temperature = " << this->sa_final_temperature << std::endl;
        std::cout << "sa_temperature_decrease_coefficient = " << this->sa_temperature_decrease_coefficient << std::endl;
        std::cout << "sa_moves_on_temperature_level = " << this->sa_moves_on_temperature_level << std::endl;
        std::cout << "sa_generating_population_size = " << this->sa_generating_population_size << std::endl;
        std::cout << "sa_weights_change_coefficient = " << this->sa_weights_change_coefficient << std::endl;
        std::cout << "pareto_population_size = " << this->pareto_population_size << std::endl;
        std::cout << "pareto_number_of_generations = " << this->pareto_number_of_generations << std::endl;
        std::cout << "pareto_mutation_probability = " << this->pareto_mutation_probability << std::endl;
        std::cout << "pareto_scalarize = " << this->pareto_scalarize << std::endl;
        std::cout << "pareto_nondominated_population_size = " << this->pareto_nondominated_population_size << std::endl;
        std::cout << "pareto_clustering_level = " << this->pareto_clustering_level << std::endl;
        std::cout << "pareto_neighbour_distance = " << this->pareto_neighbour_distance << std::endl;
        std::cout << "pareto_population_geometrical_factor = " << this->pareto_population_geometrical_factor << std::endl;
        std::cout << "hga_initial_population_size = " << this->hga_initial_population_size << std::endl;
        std::cout << "hga_population_size = " << this->hga_population_size << std::endl;
        std::cout << "hga_number_of_iterations = " << this->hga_number_of_iterations << std::endl;
        std::cout << "hga_scalarizing_function_type = " << this->hga_scalarizing_function_type << std::endl;
        std::cout << "hga_temporary_population_size = " << this->hga_temporary_population_size << std::endl;
        std::cout << "hga_ellite_size = " << this->hga_ellite_size << std::endl;
        std::cout << "momsls_number_of_iterations = " << this->momsls_number_of_iterations << std::endl;
        std::cout << "momsls_scalarizing_function_type = " << this->momsls_scalarizing_function_type << std::endl;
        std::cout << std::endl;

        return true;
    }
};

// SA
TPSA < TDSESolution > *solutionPSA;
TMOSA < TDSESolution > *solutionMOSA;
TSMOSA < TDSESolution > *solutionSMOSA;
// HGA
TMOGLS < TDSESolution > *solutionMOGLS;
TIMMOGLS < TDSESolution > *solutionIMMOGLS;
TPMA < TDSESolution > *solutionPMA;
// MOMSLS
TMOMSLS < TDSESolution > *solutionMOMSLS;
// Pareto
TSPEA < TDSESolution > *solutionSPEA;
TNSGAII < TDSESolution > *solutionNSGAII;
TNSGA < TDSESolution > *solutionNSGA;

RespClient *client;

void sigproc(int)
{
    signal(SIGINT, sigproc);
    /* NOTE some versions of UNIX will reset signal to default
    after each call. So for portability reset signal each time */
    client->shutdown();
}

int main(int argc, char ** argv){
    boost::program_options::options_description desc("Driver for the Design Space Exploration algorithms in ReSP\n\nAvailable Options");
    desc.add_options()
        ("help,h", "produces the help message")
        ("param-file,p", boost::program_options::value<std::string>(), "path of the file holding the initial parameters for the problem")
        ("solution-file,s", boost::program_options::value<std::string>(), "path of the file where the computed solutions will be saved")
        ("algorithm,a", boost::program_options::value<std::string>(), "algorithm used to perform the exploration [PSA | MOSA | SMOSA | MOGLS | IMMOGLS | PMA | MOMSLS | SPEA | NSGAII | NSGA]")
        ("alg-file,f", boost::program_options::value<std::string>(), "parameters of the algorithms used to perform the exploration")
        ("cache-file,c", boost::program_options::value<std::string>(), "cache file for the simulated solutions")
        ("verbose,v", "Activates verbose execution")
    ;

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);

    if(vm.count("help") != 0){
        std::cerr << desc << std::endl;
        return 0;
    }

    if(vm.count("param-file") == 0){
        std::cerr << "Error, Path of the parameters file is required" << std::endl;
        std::cerr << desc << std::endl;
        return 1;
    }

    if(vm.count("solution-file") == 0){
        std::cerr << "Error, Path of the solution file is required" << std::endl;
        std::cerr << desc << std::endl;
        return 1;
    }

    if(vm.count("algorithm") == 0){
        std::cerr << "Error, The algorithm used to perform the exploration must be specified" << std::endl;
        std::cerr << desc << std::endl;
        return 1;
    }

    MOMHType iMomhType;
    std::string algorithm = vm["algorithm"].as<std::string>();
    if(algorithm == "PSA")
      iMomhType = MOMH_PSA;
    else if(algorithm == "MOSA")
      iMomhType = MOMH_MOSA;
    else if(algorithm == "SMOSA")
      iMomhType = MOMH_SMOSA;
    else if(algorithm == "IMMOGLS")
      iMomhType = MOMH_IMMOGLS;
    else if(algorithm == "MOGLS")
      iMomhType = MOMH_MOGLS;
    else if(algorithm == "PMA")
      iMomhType = MOMH_PMA;
    else if(algorithm == "MOMSLS")
      iMomhType = MOMH_MOMSLS;
    else if(algorithm == "SPEA")
      iMomhType = MOMH_SPEA;
    else if(algorithm == "NSGAII")
      iMomhType = MOMH_NSGAII;
    else if(algorithm == "NSGA")
      iMomhType = MOMH_NSGA;
    else{
        std::cerr << "Unknown Exploration Algorithm; please specify one in [PSA | MOSA | SMOSA | MOGLS | IMMOGLS | PMA | MOMSLS | SPEA | NSGAII | NSGA]" << std::endl;
        return 1;
    }

    srand ((unsigned)time (NULL));
    if(!Problem.Load((char *)vm["param-file"].as<std::string>().c_str())){
      std::cerr << "Error in loading the parameters from file " << vm["param-file"].as<std::string>() << std::endl;
      return 1;
    }

    AlgorithmPreferences preferences;
    if(vm.count("alg-file") != 0){
        if(!preferences.load(vm["alg-file"].as<std::string>())){
          std::cerr << "Error in loading the algorithm parameters from file " << vm["alg-file"].as<std::string>() << std::endl;
          return 1;
        }
    }
    Problem.verbose = (vm.count("verbose") != 0);
    if(vm.count("cache-file") != 0){
        Problem.simulationCache = new SimulationCache(vm["cache-file"].as<std::string>());
    }
    //int MovesOnTemperatureLevel;
    client = new RespClient(Problem.respPort, "localhost", Problem.respPath, Problem.graphic);
    signal(SIGINT, sigproc);
    TDSESolution::client = client;

    int nonDominatedSize = 0;
    TNondominatedSet *nonDomSol = NULL;
    time_t StartingTime = time(NULL);
    switch (iMomhType){
      case MOMH_PSA:
          solutionPSA = new TPSA < TDSESolution >;
          solutionPSA->SetParameters(preferences.sa_start_temperature, preferences.sa_final_temperature,
              preferences.sa_temperature_decrease_coefficient, preferences.sa_weights_change_coefficient,
              preferences.sa_moves_on_temperature_level, preferences.sa_generating_population_size);
          try {
            solutionPSA->Run();
          } catch( haltException e) {}
          solutionPSA->pNondominatedSet->Save((char *)vm["solution-file"].as<std::string>().c_str());
          nonDomSol = solutionPSA->pNondominatedSet;
          nonDominatedSize = solutionPSA->pNondominatedSet->iSetSize;
          break;
      case MOMH_MOSA:
          solutionMOSA = new TMOSA < TDSESolution >;
          solutionMOSA->SetParameters(preferences.sa_start_temperature, preferences.sa_final_temperature,
              preferences.sa_temperature_decrease_coefficient, preferences.sa_moves_on_temperature_level,
              preferences.sa_generating_population_size);
          try {
            solutionMOSA->Run();
          } catch( haltException e) {}
          solutionMOSA->pNondominatedSet->Save((char *)vm["solution-file"].as<std::string>().c_str());
          nonDomSol = solutionMOSA->pNondominatedSet;
          nonDominatedSize = solutionMOSA->pNondominatedSet->iSetSize;
          break;
      case MOMH_SMOSA:
          solutionSMOSA = new TSMOSA < TDSESolution >;
          solutionSMOSA->SetParameters(preferences.sa_start_temperature, preferences.sa_final_temperature,
              preferences.sa_temperature_decrease_coefficient, preferences.sa_moves_on_temperature_level);
          try {
            solutionSMOSA->Run();
          } catch( haltException e) {}
          solutionSMOSA->pNondominatedSet->Save((char *)vm["solution-file"].as<std::string>().c_str());
          nonDomSol = solutionSMOSA->pNondominatedSet;
          nonDominatedSize = solutionSMOSA->pNondominatedSet->iSetSize;
          break;
      case MOMH_MOGLS:
          solutionMOGLS = new TMOGLS < TDSESolution >;
          solutionMOGLS->SetParameters(preferences.hga_temporary_population_size,
              preferences.hga_initial_population_size, preferences.hga_number_of_iterations,
              preferences.hga_scalarizing_function_type);
          try {
            solutionMOGLS->Run();
          } catch( haltException e) {}
          solutionMOGLS->pNondominatedSet->Save((char *)vm["solution-file"].as<std::string>().c_str());
          nonDomSol = solutionMOGLS->pNondominatedSet;
          nonDominatedSize = solutionMOGLS->pNondominatedSet->iSetSize;
          break;
      case MOMH_IMMOGLS:
          solutionIMMOGLS = new TIMMOGLS < TDSESolution >;
          solutionIMMOGLS->SetParameters(preferences.hga_population_size,
              preferences.hga_number_of_iterations, preferences.hga_ellite_size,
              preferences.hga_scalarizing_function_type);
          try {
            solutionIMMOGLS->Run();
          } catch( haltException e) {}
          solutionIMMOGLS->pNondominatedSet->Save((char *)vm["solution-file"].as<std::string>().c_str());
          nonDomSol = solutionIMMOGLS->pNondominatedSet;
          nonDominatedSize = solutionIMMOGLS->pNondominatedSet->iSetSize;
          break;
      case MOMH_PMA:
          solutionPMA = new TPMA < TDSESolution >;
          solutionPMA->SetParameters(preferences.hga_temporary_population_size,
              preferences.hga_initial_population_size, preferences.hga_number_of_iterations,
              preferences.hga_scalarizing_function_type);
          try {
            solutionPMA->Run();
          } catch( haltException e) {}
          solutionPMA->pNondominatedSet->Save((char *)vm["solution-file"].as<std::string>().c_str());
          nonDomSol = solutionPMA->pNondominatedSet;
          nonDominatedSize = solutionPMA->pNondominatedSet->iSetSize;
          break;
      case MOMH_MOMSLS:
          solutionMOMSLS = new TMOMSLS < TDSESolution >;
          solutionMOMSLS->SetParameters(preferences.momsls_number_of_iterations,
              preferences.momsls_scalarizing_function_type);
          try {
            solutionMOMSLS->Run();
          } catch( haltException e) {}
          solutionMOMSLS->pNondominatedSet->Save((char *)vm["solution-file"].as<std::string>().c_str());
          nonDomSol = solutionMOMSLS->pNondominatedSet;
          nonDominatedSize = solutionMOMSLS->pNondominatedSet->iSetSize;
      case MOMH_SPEA:
          solutionSPEA = new TSPEA < TDSESolution >;
          solutionSPEA->SetParameters(preferences.pareto_population_size, preferences.pareto_number_of_generations,
              preferences.pareto_mutation_probability, preferences.pareto_nondominated_population_size,
              preferences.pareto_clustering_level, preferences.pareto_scalarize);
          try {
            solutionSPEA->Run();
          } catch( haltException e) {}
          solutionSPEA->pNondominatedSet->Save((char *)vm["solution-file"].as<std::string>().c_str());
          nonDomSol = solutionSPEA->pNondominatedSet;
          nonDominatedSize = solutionSPEA->pNondominatedSet->iSetSize;
          break;
      case MOMH_NSGAII:
          solutionNSGAII = new TNSGAII < TDSESolution >;
          solutionNSGAII->SetParameters(preferences.pareto_population_size,
              preferences.pareto_number_of_generations, preferences.pareto_mutation_probability,
              preferences.pareto_scalarize);
          try {
            solutionNSGAII->Run();
          } catch( haltException e) {}
          solutionNSGAII->pNondominatedSet->Save((char *)vm["solution-file"].as<std::string>().c_str());
          nonDomSol = solutionNSGAII->pNondominatedSet;
          nonDominatedSize = solutionNSGAII->pNondominatedSet->iSetSize;
          break;
      case MOMH_NSGA:
          solutionNSGA = new TNSGA < TDSESolution >;
          solutionNSGA->SetParameters(preferences.pareto_population_size,
              preferences.pareto_number_of_generations, preferences.pareto_mutation_probability,
              preferences.pareto_neighbour_distance, preferences.pareto_scalarize);
          try {
            solutionNSGA->Run();
          } catch( haltException e) {}
          solutionNSGA->pNondominatedSet->Save((char *)vm["solution-file"].as<std::string>().c_str());
          nonDomSol = solutionNSGA->pNondominatedSet;
          nonDominatedSize = solutionNSGA->pNondominatedSet->iSetSize;
          break;
      default:
          std::cerr << "INVALID ALGORITHM SPECIFIED" << std::endl;
          return -1;
          break;
    }
    time_t EndingTime = time(NULL);
    client->shutdown();

    // Here I have to print a csv file which is easily readable by the pyx-based scripts for the plot of the
    // diagrams of the pareto curve
    std::ofstream solfileCSV((vm["solution-file"].as<std::string>() + ".csv").c_str());
    solfileCSV << Problem.parameters.size() << ";" << Problem.objectives.size() << ";" << nonDominatedSize << std::endl;
    std::map<std::string, std::vector<std::string> >::const_iterator paramIter, paramEnd;
    for(paramIter = Problem.parameters.begin(), paramEnd = Problem.parameters.end(); paramIter != paramEnd; paramIter++){
        solfileCSV << paramIter->first << ";";
    }
    std::vector<std::string>::const_iterator metricIter, metricEnd;
    std::vector<std::string>::const_iterator metricIterTemp;
    for(metricIter = Problem.objectives.begin(), metricIterTemp = metricIter, metricIterTemp++, metricEnd = Problem.objectives.end(); metricIter != metricEnd; metricIter++, metricIterTemp++){
        solfileCSV << *metricIter;
        if(metricIterTemp != metricEnd)
            solfileCSV << ";";
    }
    solfileCSV << std::endl;
    // Now I can finally print all the solutions, the non-dominated first, then the others
    vector < TSolution * >::iterator nonDomIter, nonDomEnd;
    for(nonDomIter = nonDomSol->begin(), nonDomEnd = nonDomSol->end(); nonDomIter != nonDomEnd; nonDomIter++){
        dynamic_cast<TDSESolution *>(*nonDomIter)->printcsv(solfileCSV);
    }
    solfileCSV.close();
    // Ended printing the csv file

    std::cout << "Computing time: " << EndingTime - StartingTime << " seconds" << std::endl;
    std::cout << "Executed: " << Problem.simulations_num << " simulations" << std::endl;
    std::ofstream solfile(vm["solution-file"].as<std::string>().c_str(), std::ofstream::app);
    solfile << std::endl << "Executed " << Problem.simulations_num << " Simulations" << std::endl;
    solfile << "Computing time: " << EndingTime - StartingTime << " seconds" << std::endl;

    if(vm.count("cache-file") != 0){
        delete Problem.simulationCache;
    }

    return 0;
}
