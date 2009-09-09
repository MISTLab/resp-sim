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


/*
 *   Class representing a possible solution to the DSE
 *   problem. It is simply formed by the values given
 *   to the objective for this particular problem
 *   instance.
*/


#include <momh/nondominatedset.h>
#include <momh/tlistset.h>

#include "solution.hpp"
#include "problem.hpp"

#include "utils.hpp"


#include <unistd.h>

#include <boost/lexical_cast.hpp>

//Includes necessary for boost random
#include <ctime>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#ifdef BOOST_NO_STDC_NAMESPACE
namespace std {
  using ::time;
}
#endif

RespClient* TDSESolution::client = NULL;

void TDSESolution::updateObjectives(){
    Problem.simulations_num++;

    if( Problem.simulations_max != 0 && Problem.simulations_num >= Problem.simulations_max )
        throw std::exception();

    // Here I determine if the current configuration has already been cached on file;
    // in case I do not execute simulation, but I simply get it; otherwise I need to simulated
    // it and I add it to the cached simulations
    if(Problem.simulationCache != NULL){
        std::map<std::string, double> resultingMetrics = Problem.simulationCache->find(Problem.objectives, this->param2Value, Problem.application);
        if(resultingMetrics.size() > 0){
            // I found the solution: I simply update the objective values and return
            for (int iobj = 0; iobj < NumberOfObjectives; iobj++){
                std::map<std::string, double>::const_iterator foundMetric = resultingMetrics.find(Problem.objectives[iobj]);
                if(foundMetric == resultingMetrics.end())
                    THROW_EXCEPTION("No value for metric " << Problem.objectives[iobj] << " found in the simulation cache");
                this->ObjectiveValues[iobj] = foundMetric->second;
            }
            return;
        }
    }

    if(!TDSESolution::client->load_architecture(Problem.archFile)){
        std::string respMex = TDSESolution::client->getResponseMessage();
        TDSESolution::client->quit();
        THROW_EXCEPTION("Error during loading of the architectural file " << Problem.archFile << " error: " << respMex);
    }
    if(Problem.verbose){
            std::cerr << "Parameters: " << std::endl;
    }
    std::map<std::string, std::string>::iterator solIter, solEnd;
    for(solIter = this->param2Value.begin(), solEnd = this->param2Value.end(); solIter != solEnd; solIter++){
        if(Problem.verbose){
            std::cerr << "    " << solIter->first << " = " << solIter->second << std::endl;
        }
        if(!TDSESolution::client->set_variable_value(solIter->first, solIter->second)){
            std::string respMex = TDSESolution::client->getResponseMessage();
            TDSESolution::client->quit();
            THROW_EXCEPTION("Error during setting variable " << solIter->first << " - " << respMex);
        }
    }
    std::vector<std::string> setUpParams;
    setUpParams.push_back(Problem.application);
    std::string respMex;
    if(TDSESolution::client->callMethod("setUp", respMex, setUpParams) != ""){
        respMex = TDSESolution::client->getResponseMessage();
        TDSESolution::client->quit();
        THROW_EXCEPTION("Error during call of SetUp method - " << respMex);
    }
    if(!TDSESolution::client->run_simulation()){
        respMex = TDSESolution::client->getResponseMessage();
        TDSESolution::client->quit();
        THROW_EXCEPTION("Error during simulation start - " << respMex);
    }
    TDSESolution::client->wait_sim_end();
    //Finally I update the value of the objectives;
    //the objectives are simply variables in the global namespace
    //of which I have to read the values
    if(TDSESolution::client->callMethod("getStats", respMex) != ""){
        respMex = TDSESolution::client->getResponseMessage();
        TDSESolution::client->quit();
        THROW_EXCEPTION("Error during call of getStats method - " << respMex);
    }
    if(Problem.verbose){
        std::cerr << "Resulting metrics:" << std::endl;
    }
    for (int iobj = 0; iobj < NumberOfObjectives; iobj++){
        TDSESolution::client->get_probe_value(Problem.objectives[iobj], this->ObjectiveValues[iobj]);
        if(Problem.verbose){
            std::cerr << "    " << Problem.objectives[iobj] << " = " << this->ObjectiveValues[iobj] << std::endl;
        }
    }
    if(Problem.verbose){
        std::cerr << std::endl;
    }

    // Ok, since I am here, this is a new, never simulated before solution: I add it to the solution cache
    // and to the solution file
    if(Problem.simulationCache != NULL){
        Problem.simulationCache->add(Problem.objectives, this->ObjectiveValues, this->param2Value, Problem.application);
    }

    if(!TDSESolution::client->reset()){
        std::string resetError;
        try{
            resetError = TDSESolution::client->getResponseMessage();
            TDSESolution::client->quit();
        }
        catch(...){}
        std::cerr << "Error in reseting ReSP client: " << resetError << " ... restarting ReSP" << std::endl;
        TDSESolution::client->reconnect();
    }

    if(TDSESolution::client->getUsedMemory() > 1024*1024*1024){
        std::cerr << "Reconnecting since ReSP was using too much memory" << std::endl;
        TDSESolution::client->reconnect();
    }
}

void TDSESolution::createRandom(TDSESolution &solution){
    std::map<std::string, std::vector<std::string> >::iterator paramsIter, paramsEnd;
    for(paramsIter = Problem.parameters.begin(), paramsEnd = Problem.parameters.end(); paramsIter != paramsEnd; paramsIter++){
        boost::uniform_int<> degen_dist(0, paramsIter->second.size() - 1);
        boost::variate_generator<boost::minstd_rand&, boost::uniform_int<> > deg(Problem.generator, degen_dist);
        solution.param2Value[paramsIter->first] = paramsIter->second[deg()];
    }
}


/// Constructs new random solution; used by PSA, MOSA, SMOSA, NSGA and SPEA
TDSESolution::TDSESolution() : TMOMHSolution() {
    this->WeightVector = GetRandomWeightVector();
    //A solution is formed by filling the map
    //param2Value.
    createRandom(*this);
    // Now I have to set the objective value for the execution time and the power consumption;
    // in order to do this I have to resort to simulation
    this->updateObjectives();
}

///Copy constructor
TDSESolution::TDSESolution(TDSESolution & DSESolution) : TMOMHSolution(DSESolution){
    this->param2Value = DSESolution.param2Value;
}

/// Constructs new solution; if there is knowledge on the problem it is possible
/// to use heuristics trying to make the generated solution as good as possible.
/// it might be the case of performing a local search at then end (but this is
/// a hibridization of the algorithm)
TDSESolution::TDSESolution(TScalarizingFunctionType ScalarizingFunctionType,
    TPoint & ReferencePoint, TWeightVector WeightVector, TNondominatedSet & NondominatedSet):
        TMOMHSolution(ScalarizingFunctionType, ReferencePoint, WeightVector, (TNondominatedSet &)NondominatedSet){
    // well, I can simply create a random solution and then modify it through a local
    // search
    createRandom(*this);
    this->updateObjectives();
    //I now perform local search in order to improve the found solution
    this->LocalSearch(ReferencePoint, NondominatedSet);
}

/// Constructs new solution by recombination of Parent1 and Parent 2.
/// The important features of the parents should be considered.
/// Mutation should/can also be applied to the newly generated solution
/// Again, the local search method might be called at the end to improve the
/// solution. used by PMA, IMMOGLS and MOGLS.
TDSESolution::TDSESolution(TDSESolution & Parent1, TDSESolution & Parent2,
    TScalarizingFunctionType ScalarizingFunctionType,
    TPoint & ReferencePoint, TWeightVector WeightVector, TNondominatedSet & NondominatedSet):
        TMOMHSolution(Parent1, Parent2, ScalarizingFunctionType, ReferencePoint, WeightVector, (TNondominatedSet &) NondominatedSet){
    this->crossSolution(*this, Parent1, Parent2);
    this->updateObjectives();
    this->LocalSearch(ReferencePoint, NondominatedSet);
}

/// Recombination constructor used by NSGA and SPEA; there is not local heuristic to
/// be called at the end of the recombination operator
TDSESolution::TDSESolution(TDSESolution & Parent1, TDSESolution & Parent2) :
                                                TMOMHSolution(Parent1, Parent2){
    this->WeightVector = GetRandomWeightVector();
    this->crossSolution(*this, Parent1, Parent2);
    this->updateObjectives();
}

/// Given two parents it composed them into solution
void TDSESolution::crossSolution(TDSESolution & solution, TDSESolution & Parent1, TDSESolution & Parent2){
    // I simply take a random element among the two solutions and create the new solution
    // which is a combination of the two
    boost::uniform_int<> degen_dist(0, 1);
    boost::variate_generator<boost::minstd_rand&, boost::uniform_int<> > deg(Problem.generator, degen_dist);
    std::map<std::string, std::string>::iterator solIter, solEnd;
    for(solIter = Parent1.param2Value.begin(), solEnd = Parent1.param2Value.end(); solIter != solEnd; solIter++){
        //I now randomly choose whether to take the value from the first or the second parent
        if(deg() == 0)
            solution.param2Value[solIter->first] = solIter->second;
        else
            solution.param2Value[solIter->first] = Parent2.param2Value[solIter->first];
    }
}

/// Local search method: heuristic to locally improve a solution; usually this is not
/// part of the base version of the algorithms, but it is used to improve the algorithms
/// as hibridize them. Note that during local search we might decide to add some non-dominated
/// solutions to the external non-dominated set.
// Note that it might be useful to specify
/// the number of solutions examined in the local search, otherwise we might end up spending
/// too much time in the local search function. The k neighbour solutions to be examined are
/// randomly chosen. We used the local search as described in Murata's paper.
/// In case a linear scalarizing function is used, the direction of the local search is given
/// by the weights of the function.
/// TODO: set max number of moves that I want to perform durin local search
void TDSESolution::LocalSearch(TPoint & ReferencePoint, TNondominatedSet & NondominatedSet){
    /// I do not want to spend too much time in local search, otherwise I
    /// loose the effectiveness of the rest of the algorithm;
    /// lets say that for each parameter I try to move forward or backward
    /// with respect to the current position; for each intermediate solution found
    /// I also check if it is non-dominated and, in case, I add it to the non-dominated set
    double currentOptimalValue = TSolution::ScalarizingFunction(ReferencePoint);
    boost::uniform_int<> degen_dist(0, 1);
    boost::variate_generator<boost::minstd_rand&, boost::uniform_int<> > deg(Problem.generator, degen_dist);
    std::map<std::string, std::string>::iterator solIter, solEnd;
    for(solIter = this->param2Value.begin(), solEnd = this->param2Value.end(); solIter != solEnd; solIter++){
        // First I save the old solution
        std::map<std::string, std::string> oldSolution = this->param2Value;
        SaveObjectiveValues();
        // I now randomly choose whether to take the value from the first or the second parent
        if(deg() == 0)
            solIter->second = TDSESolution::findPrevious(solIter->first, solIter->second);
        else
            solIter->second = TDSESolution::findSuccessive(solIter->first, solIter->second);
        if(this->param2Value[solIter->first] != this->oldSolution[solIter->first])
            this->updateObjectives();
        double newObjValue = TSolution::ScalarizingFunction(ReferencePoint);
        // Now I check to see if the current solution is a non dominated one
        this->addToNonDominated(NondominatedSet);
        if(newObjValue < currentOptimalValue)
            break;
        else{
            this->param2Value = oldSolution;
            RestoreObjectiveValues();
        }
    }
}

/// Given a point, it checks if it is the case of adding it to the non-dominated set or not
void TDSESolution::addToNonDominated(TNondominatedSet & NondominatedSet){
    NondominatedSet.Update(*this);
}

/// We perform a single random local move; method used by PSA, SMOSA, MOSA.
/// This method actually performs the move; in case the move has to be undone,
/// the RejectLocalMove method is called.
void TDSESolution::FindLocalMove(){
    /// I simply choose randomly one parameter and the direction (whether to
    /// increment or decrement the value)
    boost::uniform_int<> params_dist(0, this->param2Value.size() - 1);
    boost::variate_generator<boost::minstd_rand&, boost::uniform_int<> > params_generator(Problem.generator, params_dist);
    std::map<std::string, std::string>::iterator solIter = this->param2Value.begin();
    unsigned int randomParam = params_generator();
    for(unsigned int i = 0; i < randomParam; solIter++, i++);
    // First I save the old solution
    this->oldSolution = this->param2Value;
    SaveObjectiveValues();
    // I now randomly choose whether to take the value from the first or the second parent
    boost::uniform_int<> degen_dist(0, 1);
    boost::variate_generator<boost::minstd_rand&, boost::uniform_int<> > deg(Problem.generator, degen_dist);
    if(deg() == 0)
        solIter->second = TDSESolution::findPrevious(solIter->first, solIter->second);
    else
        solIter->second = TDSESolution::findSuccessive(solIter->first, solIter->second);
    if(this->param2Value[solIter->first] != this->oldSolution[solIter->first])
        this->updateObjectives();
}

/// Rejects the local move found by FindLocalMove () method; we call RestoreObjectiveValues
/// in order to restore the objective values of the previous solution. Instead the solution
/// itself has to be manually undone using information saved
void TDSESolution::RejectLocalMove(){
    this->param2Value = this->oldSolution;
    RestoreObjectiveValues();
}

/// Mutates the solution; this method is used by the NSGA and SPEA algorithms. The mutation
/// consists in a random change of a random parameter of the solution. Note that mutation is not
/// necessary; we might configure the probability of performing it.
/// TODO: set probability of mutation
void TDSESolution::Mutate(){
    /// I randomly choose one parameter and randomly choose the value for this parameter;
    /// then I change the value
    boost::uniform_int<> params_dist(0, this->param2Value.size() - 1);
    boost::variate_generator<boost::minstd_rand&, boost::uniform_int<> > params_generator(Problem.generator, params_dist);
    std::map<std::string, std::string>::iterator solIter = this->param2Value.begin();
    unsigned int randomParam = params_generator();
    for(unsigned int i = 0; i < randomParam; solIter++, i++);
    // I now randomly choose whether to take the value from the first or the second parent
    boost::uniform_int<> degen_dist(0, Problem.parameters[solIter->first].size() - 1);
    boost::variate_generator<boost::minstd_rand&, boost::uniform_int<> > deg(Problem.generator, degen_dist);
    solIter->second = Problem.parameters[solIter->first][deg()];
    this->updateObjectives();
}

/// Given a parameter and its current value it moves either to the previous or the next value
std::string TDSESolution::findPrevious(const std::string & paramName, const std::string & paramVal){
    for(unsigned int i = 0; i < Problem.parameters[paramName].size(); i++){
        if(Problem.parameters[paramName][i] == paramVal){
            if(i == 0)
                return paramVal;
            else
                return Problem.parameters[paramName][i - 1];
        }
    }
    return "";
}

std::string TDSESolution::findSuccessive(const std::string & paramName, const std::string & paramVal){
    for(unsigned int i = 0; i < Problem.parameters[paramName].size(); i++){
        if(Problem.parameters[paramName][i] == paramVal){
            if(i == Problem.parameters[paramName].size() - 1)
                return paramVal;
            else
                return Problem.parameters[paramName][i + 1];
        }
    }
    return "";
}

/// Redefinition of the save method for saving the solutions to file
ostream & TDSESolution::Save(ostream & Stream){
    Stream << "Parameters:" << std::endl;
    std::map<std::string, std::string>::iterator solIter, solEnd;
    for(solIter = this->param2Value.begin(), solEnd = this->param2Value.end(); solIter != solEnd; solIter++){
        Stream << '\x09' << solIter->first << " = " << solIter->second << std::endl;
    }
    Stream << "Metrics:" << std::endl;
    Stream << std::endl;
    for (int i = 0; i < NumberOfObjectives; i++){
        Stream << '\x09' << Problem.objectives[i] << "=" << ObjectiveValues[i] << std::endl;
    }
    return Stream;
}

///Print CSV file containin the solution
void TDSESolution::printcsv(std::ostream & stream) const{
    std::map<std::string, std::string>::const_iterator solIter, solEnd;
    for(solIter = this->param2Value.begin(), solEnd = this->param2Value.end(); solIter != solEnd; solIter++){
        stream << solIter->second << ";";
    }
    for (int i = 0; i < NumberOfObjectives; i++){
        stream << ObjectiveValues[i];
        if(i < NumberOfObjectives - 1)
            stream << ";";
    }
    stream << std::endl;
}
