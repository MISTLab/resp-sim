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

/// Driver for the design exploration using MDPs. This program is fully
/// configurable and flexible. The only assumptions and restrictions are:
/// -- we want to minimize the metric values
/// -- the scalarizing objective function hasn't been implemented in a
///    plugin style, but it is hardcoded into files ObjectiveFun.cpp/.hpp
/// Note how both the objective function and the plugins must a-priori
/// have knowledge of which are our objectives: this means that if, instead
/// of only power and speed for also used the area consumption, the
/// parameter transformation plugins and the objective function needs
/// changing

#include <cstdlib>
#include <cstdio>

#include <dlfcn.h>
#include <iostream>
#include <fstream>
#include <vector>

#include <boost/program_options.hpp>
#include <unistd.h>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/fstream.hpp"
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/path.hpp>
#include <exception>

#include "simulationCache.hpp"
#include "MDPSolver.hpp"
#include "configuration.hpp"
#include "pluginIf.hpp"
#include "utils.hpp"

void resp_close_terminate_handler(){
    std::string mex = "";
    try{
        throw;
    }
    catch(std::runtime_error &exc){
        mex = exc.what();
    }
    catch(std::string &exc){
        mex = exc;
    }
    catch(...){}
    try{
        MDPSolver::client->shutdown();
        if(mex != "")
            std::cerr << mex << std::endl;
    }
    catch(...){
        if(mex != "")
            std::cerr << mex << std::endl;
        else
            throw;
    }
}

void sigproc(int signal){
    try{
        MDPSolver::client->shutdown();
    }
    catch(...){}
    exit(0);
}

void clientCloseExit(){
    try{
        MDPSolver::client->shutdown();
    }
    catch(...){}
}

int main(int argc, char * argv[]){
    boost::program_options::options_description desc("Driver for the Design Space Exploration algorithms in ReSP\n\nAvailable Options");
    desc.add_options()
        ("help,h", "produces the help message")
        ("param-file,p", boost::program_options::value<std::string>(), "path of the file holding the initial parameters for the problem")
        ("solution-file,s", boost::program_options::value<std::string>(), "path of the file where the computed solutions will be saved")
        ("cache-file,c", boost::program_options::value<std::string>(), "cache file for the simulated solutions")
        ("verbose,v", "Activates verbose execution")
    ;

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);

    // Checking that the parameters are correctly specified
    if(vm.count("help") != 0){
        std::cerr << desc << std::endl;
        return 0;
    }
    if(vm.count("param-file") == 0){
        std::cerr << "Path of the parameters file is required" << std::endl;
        std::cerr << desc << std::endl;
        return 1;
    }
    if(vm.count("solution-file") == 0){
        std::cerr << "Path of the solution file is required" << std::endl;
        std::cerr << desc << std::endl;
        return 1;
    }
    config.verbose = (vm.count("verbose") != 0);

    // Loading the configuration
    if(!config.load(vm["param-file"].as<std::string>())){
        std::cerr << "Error in loading the configuration parameters from file " << vm["param-file"].as<std::string>() << std::endl;
        return 1;
    }

    if(vm.count("cache-file") != 0){
        MDPSolver::simulationCache = new SimulationCache(vm["cache-file"].as<std::string>());
    }

    // loading the plugins
    boost::filesystem::path fullPluginPath = boost::filesystem::system_complete(boost::filesystem::path(config.pluginFolder, boost::filesystem::native));
    if ( !boost::filesystem::exists( fullPluginPath ) )
        THROW_EXCEPTION("Plugin folder " << fullPluginPath.string() << " does not exists");
    if (!boost::filesystem::is_directory(fullPluginPath))
        THROW_EXCEPTION("Path " << fullPluginPath.string() << " specified for the plugin folder is not a directory");
    boost::filesystem::directory_iterator end_itr;
    for(boost::filesystem::directory_iterator itr( fullPluginPath ); itr != end_itr; itr++ ){
        // I load all the shared libraries that I find in this folder; note
        // that this is highly unportable
        if(boost::filesystem::extension(itr->path()) == ".so"){
            void *hndl = dlopen(itr->path().native_file_string().c_str(), RTLD_NOW);
            if(hndl == NULL){
               std::cerr << "Error while opening shared library " << itr->leaf() << " --> " << dlerror() << std::endl;
               return 1;
            }
        }
    }
    // Ok, I have completed the loading; I can initialize the plugins; first of all I check that
    // there is a plugin for each of the parameters
    std::map<std::string, std::vector<std::string> >::const_iterator paramIter, paramEnd;
    for(paramIter = config.parameters.begin(), paramEnd = config.parameters.end(); paramIter != paramEnd; paramIter++){
        if(plugin_creator_handler.find(paramIter->first) == plugin_creator_handler.end()){
            std::cerr << "There is not plugin to handle parameter " << paramIter->first << std::endl;
            return 1;
        }
    }
    std::map<std::string, PluginCreator* >::const_iterator  creatorIter, creatorEnd;
    for(creatorIter = plugin_creator_handler.begin(), creatorEnd = plugin_creator_handler.end(); creatorIter != creatorEnd; creatorIter++){
        if(config.parameters.find(creatorIter->first) != config.parameters.end()){
            if(config.verbose){
                std::cout << "Initializing plugin " << creatorIter->first << std::endl;
            }
            plugin_handler[creatorIter->first] = creatorIter->second->create(config.parameters[creatorIter->first], creatorIter->first);
            rev_plugin_handler[plugin_handler[creatorIter->first]] = creatorIter->first;
        }
    }
    if(config.verbose){
        std::cout << std::endl;
    }

    //Creating the client for communication with ReSP
    MDPSolver::client = new RespClient(config.respPort, "localhost", config.respPath, config.graphic);
    std::set_terminate(resp_close_terminate_handler);
    signal(SIGINT, sigproc);
    atexit (clientCloseExit);
    // Starting the algorithm
    MDPSolver solver;
    solver.run();
    //closing connection with ReSP
    MDPSolver::client->shutdown();
    // Finally I have to print the results; for more easily interpreting the results I print
    // both a human readable file and a csv file. The csv will contain all the simulated
    // solutions. The first line will indicate the number of parameters and the number
    // of objectives; the third number shall the the number of non-dominated solutions.
    // Then we shall start with the real csv file: first I will print a line with the parameters
    // and objectives names, then the nn-dominated solutions (one per line), then the
    // other ones
    std::ofstream solfile(vm["solution-file"].as<std::string>().c_str());
    std::ofstream solfileCSV((vm["solution-file"].as<std::string>() + ".csv").c_str());
    if(config.verbose)
        std::cout << "Printing " << MDPSolver::nonDominated.size() << " non-dominated solutions" << std::endl;
    std::vector<SystemConfig>::iterator nonDomIter, nonDomEnd;
    for(nonDomIter = MDPSolver::nonDominated.begin(), nonDomEnd = MDPSolver::nonDominated.end(); nonDomIter != nonDomEnd; nonDomIter++){
        if(config.verbose)
            std::cout << *nonDomIter << std::endl;
        solfile << *nonDomIter << std::endl;
    }
    if(config.verbose)
        std::cout << std::endl << "Executed " << MDPSolver::numSimulations << " Simulations" << std::endl;
    solfile << std::endl << "Executed " << MDPSolver::numSimulations << " Simulations" << std::endl;
    if(config.verbose)
        std::cout << std::endl << "Number of I kind uncertainties " << MDPSolver::numIKindUncertainty << std::endl;
    solfile << std::endl << "Number of I kind uncertainties " << MDPSolver::numIKindUncertainty << std::endl;
    if(config.verbose)
        std::cout << std::endl << "Number of II kind uncertainties " << MDPSolver::numIIKindUncertainty << std::endl;
    solfile << std::endl << "Number of II kind uncertainties " << MDPSolver::numIIKindUncertainty << std::endl;
    if(config.verbose)
        std::cout << std::endl << "Number of uncertainty explosions " << MDPSolver::numExplosions << std::endl;
    solfile << std::endl << "Number of uncertainty explosions " << MDPSolver::numExplosions << std::endl;
    solfile.close();
    // Now I print the csv file
    // header
    solfileCSV << config.parameters.size()+1 << ";" << config.objectives.size() << ";" << MDPSolver::nonDominated.size() << std::endl;
    solfileCSV << "alpha;";
    for(paramIter = config.parameters.begin(), paramEnd = config.parameters.end(); paramIter != paramEnd; paramIter++){
        solfileCSV << paramIter->first << ";";
    }

    std::vector<std::string>::const_iterator metricIter, metricEnd;
    std::vector<std::string>::const_iterator metricIterTemp;
    for(metricIter = config.objectives.begin(), metricIterTemp = metricIter, metricIterTemp++, metricEnd = config.objectives.end(); metricIter != metricEnd; metricIter++, metricIterTemp++){
        solfileCSV << *metricIter;
        if(metricIterTemp != metricEnd)
            solfileCSV << ";";
    }
    solfileCSV << std::endl;
    // Now I can finally print all the solutions, the non-dominated first, then the others
    for(nonDomIter = MDPSolver::nonDominated.begin(), nonDomEnd = MDPSolver::nonDominated.end(); nonDomIter != nonDomEnd; nonDomIter++){
        nonDomIter->printcsv(solfileCSV);
    }
    std::vector<SystemConfig>::const_iterator simulIter, simulEnd;
    for(simulIter = MDPSolver::simulatedConfigs.begin(), simulEnd = MDPSolver::simulatedConfigs.end(); simulIter != simulEnd; simulIter++){
        if(!simulIter->dominated)
            simulIter->printcsv(solfileCSV);
    }
    solfileCSV.close();

    std::cout << "Correctly ended the exploration" << std::endl;

    if(vm.count("cache-file") != 0){
        delete MDPSolver::simulationCache;
    }

    return 0;
}
