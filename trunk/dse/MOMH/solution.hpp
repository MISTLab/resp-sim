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
 *   Class representing a possible solution to the DSE
 *   problem. It is simply formed by the values given
 *   to the objective for this particular problem
 *   instance.
*/

#ifndef TDSESOLUTION_HPP
#define TDSESOLUTION_HPP

#include <momh/momhsolution.h>
#include <map>
#include <string>
#include "RespClient.hpp"

#include <cstdlib>
#include <string>
#include <exception>
#include <stdexcept>
#include <iostream>

class haltException: public std::exception{
};

class RespClient;

/// A solution for the design exploration problem simply consists of
/// the values chosen for each parameter of the architecture
class TDSESolution : public TMOMHSolution{
  protected:
    //Map storing, for each parameter, the value that the parameter has
    std::map<std::string, std::string> param2Value;
    //Map used to temporarily hold the solution during local search
    std::map<std::string, std::string> oldSolution;

  public:
    // ReSP Client connector
    static RespClient* client;

    /********************* CONSTRUCTORS *****************************/

    /// Constructs new random solution; used by PSA, MOSA, SMOSA, NSGA and SPEA
    TDSESolution();
    ///Copy constructor
    TDSESolution(TDSESolution & DSESolution);
    /// Constructs new solution; if there is knowledge on the problem it is possible
    /// to use heuristics trying to make the generated solution as good as possible.
    /// it might be the case of performing a local search at then end (but this is
    /// a hibridization of the algorithm)
    TDSESolution(TScalarizingFunctionType ScalarizingFunctionType,
        TPoint & ReferencePoint, TWeightVector WeightVector, TNondominatedSet & NondominatedSet);
    /// Constructs new solution by recombination of Parent1 and Parent 2.
    /// The important features of the parents should be considered.
    /// Mutation should/can also be applied to the newly generated solution
    /// Again, the local search method might be called at the end to improve the
    /// solution. used by PMA, IMMOGLS and MOGLS.
    TDSESolution(TDSESolution & Parent1, TDSESolution & Parent2,
        TScalarizingFunctionType ScalarizingFunctionType,
        TPoint & ReferencePoint, TWeightVector WeightVector, TNondominatedSet & NondominatedSet);
    /// Recombination constructor used by NSGA and SPEA; there is not local heuristic to
    /// be called at the end of the recombination operator
    TDSESolution(TDSESolution & Parent1, TDSESolution & Parent2);

    /// initializes solution to a random point
    static void createRandom(TDSESolution &solution);

    /// Given two parents it composed them into solution
    void crossSolution(TDSESolution & solution, TDSESolution & Parent1, TDSESolution & Parent2);
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
    void LocalSearch(TPoint & ReferencePoint, TNondominatedSet & NondominatedSet);
    /// We perform a single random local move; method used by PSA, SMOSA, MOSA.
    /// This method actually performs the move; in case the move has to be undone,
    /// the RejectLocalMove method is called.
    void FindLocalMove();
    /// Rejects the local move found by FindLocalMove () method; we call RestoreObjectiveValues
    /// in order to restore the objective values of the previous solution. Instead the solution
    /// itself has to be manually undone using information saved
    void RejectLocalMove();
    /// Mutates the solution; this method is used by the NSGA and SPEA algorithms. The mutation
    /// consists in a random change of a random parameter of the solution. Note that mutation is not
    /// necessary; we might configure the probability of performing it.
    void Mutate();
    ///Update the values of the objectives through a simulation run
    void updateObjectives();
    /// Given a point, it checks if it is the case of adding it to the non-dominated set or not
    void addToNonDominated(TNondominatedSet & NondominatedSet);
    /// Given a parameter and its current value it moves either to the previous or the next value
    static std::string findPrevious(const std::string & paramName, const std::string & paramVal);
    static std::string findSuccessive(const std::string & paramName, const std::string & paramVal);
    /// Redefinition of the save method for saving the solutions to file
    ostream & Save(ostream & Stream);
    void printcsv(std::ostream & stream) const;
};

#endif
