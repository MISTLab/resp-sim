#ifndef BACKTRACER_HPP
#define BACKTRACER_HPP

/**
 * The idea of this program is to use information of the trace file together with information of the original
 * executable file in order to produce a backtrace of the calls. The user has to provide a simulation time:
 * and the tool produces the function which were being executed at that time and the list
 * of the functions which called that one
 */

#include <list>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include "bfdFrontend.hpp"

namespace resp{

/**
 * Represents a single function call in the backtrace
 */
struct FunCall{
    ///The prototype of the function
    std::string prototype;
    ///The source file in which the function is defined
    std::string definitionPoint;
    ///The time at which the call was performed
    double callingTime;
    ///The processor trace at the time the call was performed
    std::string trace;
    ///Prints the details regarding this function call
    std::string print();
};

/**
 * Represents a backtrace: a list of function calls
 */
class Backtracer{
  private:
    ///Keeps track of the different function calls in the backtrace that we are examining
    std::list<FunCall> backTrace;
    ///File containing the execution trace
    std::ifstream traceFile;
    ///Contain the string indentifying the link register in the trace
    std::string linkReg;
    ///Contain the string indentifying the program counter in the trace
    std::string pcReg;
    ///Helper used to query and manage the symbols in the object files
    BFDFrontend *symbolManager;
    ///Map used to convert hex strings in integers
    std::map<char, unsigned int> HexMap;
    ///Parses the trace and goes to the specified time; it returns the PC
    ///and the LR at that time
    std::pair<unsigned int,  unsigned int> gotoCycleTime(double time,  std::fstream::pos_type &nextPos);
    ///Parses trace until it finds a trace at the given progCount; it then
    ///returns the current simulation time and the link register. If there are more than one traces
    ///with the same program counter,  the one closest (and previous) to the file position stopPos is
    ///chosen
    std::pair<unsigned int,  double> gotoPC(unsigned int progCount,  std::fstream::pos_type stopPos,  std::fstream::pos_type &nextPos);
    ///Converts an hexadecimal number expressed with a string
    ///into its correspondent integer number
    ///each hex number of the string is in the same order of the endianess
    ///of the processor linked to this stub 
    unsigned int toIntNum(std::string &toConvert);
    ///Given an address in the body of a function,  it determines the start address of that function
    unsigned int gotoFunctionStart(unsigned int address);
  public:
    ///Creates an instance of the backtracer;
    ///@param objdumpPath the path of the objdump command for the architecture
    ///we are simulating (this command should come with the cross-compiler for the
    ///architecture under analysis)
    ///@param objFile the executable file which contains the software that we are
    ///simulating; note that it must be compiled with the -g flag
    ///@param traceFile the file containing the trace of the execution
    ///@param linkReg the string identifying the link register in the backtrace
    ///@param pcReg the string identifying the program counter register in the backtrace
    Backtracer(std::string objFile,  std::string traceFile, std::string linkReg, std::string pcReg);
    ~Backtracer();
    ///Computes the backtrace
    ///@param callTime the simulation time from which we want to create the backtrace
    void computeBackTrace(double callTime);
    ///Creates a string which represent the current backtrace
    std::string print();
};

}

#endif
