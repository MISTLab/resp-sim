#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <ostream>
#include <sstream>
#include <map>
#include <exception>
#include <stdexcept>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include "bfdFrontend.hpp"

#include "backtracer.hpp"

using namespace resp;

///Prints the details regarding this function call
std::string FunCall::print(){
    std::ostringstream stream;
    stream << "prototype: " << prototype << std::endl;
    stream << "file: " << definitionPoint << std::endl;
    stream << "called at time: " << callingTime << std::endl;
    return stream.str();
}

///Parses the trace and goes to the specified time; it returns the PC
///and the LR at that time
std::pair<unsigned int,  unsigned int> Backtracer::gotoCycleTime(double time,  std::fstream::pos_type &nextPos){
    std::string line;
    bool beginFound = false;
    double possibleTime;
    std::pair<unsigned int,  unsigned int> retVal((unsigned int)-1, (unsigned int)-1);
    while (!this->traceFile.eof()){
        nextPos = this->traceFile.tellg();
        std::getline(this->traceFile,  line);
        if(beginFound){
            //Ok,  this line is a timestamp: lets see if it is the right timestamp
            std::istringstream strLine(line);
            strLine >> possibleTime;
            if(possibleTime == time){
                //Now I have to parse the rest of the trace,  looking for PC and LR
                while (!this->traceFile.eof() && line != "##END##"){
                    std::getline(this->traceFile,  line);
                    std::string::size_type regPos = line.find(this->linkReg);
                    if(regPos != std::string::npos){
                        //Found the link register
                        std::string tempStr = line.substr(regPos + this->linkReg.size());
                        std::string::size_type spacePos = tempStr.find(' ');
                        if(spacePos != std::string::npos)
                            tempStr = tempStr.substr(0,  spacePos);
                        retVal.second = toIntNum(tempStr);
                    }
                    regPos = line.find(this->pcReg);
                    if(regPos != std::string::npos){
                        //Found the program counter
                        std::string tempStr = line.substr(regPos + this->pcReg.size());
                        std::string::size_type spacePos = tempStr.find(' ');
                        tempStr = tempStr.substr(0,  spacePos);
                        retVal.first = toIntNum(tempStr);
                    }
                }
                return retVal;
            }
        }
        beginFound = (line == "##BEGIN##");
    }
    return retVal;
}

///Parses back the trace until it finds a trace at the given progCounter; it then
///returns the current simulation time and the link registerIf there are more than one traces
///with the same program counter,  the one closest (and previous) to the file position stopPos is
///chosen
std::pair<unsigned int,  double> Backtracer::gotoPC(unsigned int progCount,  std::fstream::pos_type stopPos,
                                                                                                                                        std::fstream::pos_type &nextPos){
    std::string line;
    bool beginFound = false;
    double possibleTime;
    std::fstream::pos_type curPos;
    std::pair<unsigned int,  double> retVal((unsigned int)-1, (unsigned int)-1);
    while(this->traceFile.tellg() < stopPos && !this->traceFile.eof()){
        curPos = this->traceFile.tellg();
        std::getline(this->traceFile,  line);
        if(beginFound){
            std::istringstream strLine(line);
            strLine >> possibleTime;
            //Now I have to parse the rest of the trace,  looking for PC and LR
            while (!this->traceFile.eof() && line != "##END##"){
                std::getline(this->traceFile,  line);
                std::string::size_type regPos = line.find(this->pcReg);
                if(regPos != std::string::npos){
                    //Found the program counter
                    std::string tempStr = line.substr(regPos + this->pcReg.size());
                    std::string::size_type spacePos = tempStr.find(' ');
                    tempStr = tempStr.substr(0,  spacePos );
                    unsigned int pc = toIntNum(tempStr);
                    if(pc == progCount){
                        nextPos = curPos;
                        while (!this->traceFile.eof() && line != "##END##"){
                            std::getline(this->traceFile,  line);
                            regPos = line.find(this->linkReg);
                            if(regPos != std::string::npos){
                                //Found the link register
                                tempStr = line.substr(regPos + this->linkReg.size());
                                spacePos = tempStr.find(' ');
                                if(spacePos != std::string::npos)
                                    tempStr = tempStr.substr(0,  spacePos );
                                retVal.first = toIntNum(tempStr);
                            }
                        }
                        retVal.second = possibleTime;
                    }
                    break;
                }
            }
        }
        beginFound = (line == "##BEGIN##");
    }
    return retVal;
}

///Creates an instance of the backtracer;
///@param objdumpPath the path of the objdump command for the architecture
///we are simulating (this command should come with the cross-compiler for the
///architecture under analysis)
///@param objFile the executable file which contains the software that we are
///simulating; note that it must be compiled with the -g flag
///@param traceFile the file containing the trace of the execution
///@param linkReg the string identifying the link register in the backtrace
///@param pcReg the string identifying the program counter register in the backtrace
Backtracer::Backtracer(std::string objFile,  std::string traceFile,
               std::string linkReg, std::string pcReg): traceFile(traceFile.c_str()),  linkReg(linkReg), pcReg(pcReg){
    this->HexMap['0'] = 0;
    this->HexMap['1'] = 1;
    this->HexMap['2'] = 2;
    this->HexMap['3'] = 3;
    this->HexMap['4'] = 4;
    this->HexMap['5'] = 5;
    this->HexMap['6'] = 6;
    this->HexMap['7'] = 7;
    this->HexMap['8'] = 8;
    this->HexMap['9'] = 9;
    this->HexMap['A'] = 10;
    this->HexMap['B'] = 11;
    this->HexMap['C'] = 12;
    this->HexMap['D'] = 13;
    this->HexMap['E'] = 14;
    this->HexMap['F'] = 15;
    this->HexMap['a'] = 10;
    this->HexMap['b'] = 11;
    this->HexMap['c'] = 12;
    this->HexMap['d'] = 13;
    this->HexMap['e'] = 14;
    this->HexMap['f'] = 15;
    
    #ifndef NDEBUG
    //Checking that the file is actually open
    if (!this->traceFile.is_open())
        throw std::runtime_error("Unable to open trace file " + traceFile);
    #endif
    
    //Now I can create the symbol manager
    this->symbolManager = new BFDFrontend(objFile);
}

Backtracer::~Backtracer(){
    delete this->symbolManager;
}

///Computes the backtrace
///@param callTime the simulation time from which we want to create the backtrace
void Backtracer::computeBackTrace(double callTime){
    //First of all I have to find the starting point of the back-trace we want,  which is at callTime
    //Then,  starting at this time,  we go back and trace the callers
    std::fstream::pos_type nextPos;
    std::pair<unsigned int,  unsigned int> initialPC = this->gotoCycleTime(callTime,  nextPos);
    #ifndef NDEBUG
    if(initialPC.first == (unsigned int)-1 || initialPC.second == (unsigned int)-1){
        throw std::runtime_error("Error, specified call time " + boost::lexical_cast<std::string>(callTime) + " not found in the trace file");
    }
    #endif
    FunCall funTemp;
    funTemp.prototype = this->symbolManager->symbolAt(initialPC.first).front();
    std::string file;
    unsigned int line = 0;
    this->symbolManager->getSrcFile(initialPC.first, file, line);
    funTemp.definitionPoint = file + " line: " + boost::lexical_cast<std::string>(line);
    funTemp.callingTime = callTime;
    this->backTrace.push_back(funTemp);
    
    //Now I iteratively procede until there are no more function calls to examine
    unsigned int nextPC = this->gotoFunctionStart(initialPC.second - sizeof(int));
    std::fstream::pos_type tempPos;
    while(nextPC != (unsigned int)-1){
        this->traceFile.seekg(0);
        std::pair<unsigned int,  double> nextStack = this->gotoPC(nextPC,  nextPos,  tempPos);
        if(nextStack.second != (unsigned int)-1){
            nextPos = tempPos;
            funTemp.prototype = this->symbolManager->symbolAt(nextPC).front();
            this->symbolManager->getSrcFile(nextPC, file, line);
            funTemp.definitionPoint = file + " line: " + boost::lexical_cast<std::string>(line);
            funTemp.callingTime = nextStack.second;
            this->backTrace.push_back(funTemp);
        }
        if(nextStack.first > sizeof(int) && nextStack.first != (unsigned int)-1){
            nextPC = this->gotoFunctionStart(nextStack.first - sizeof(int));
        }
        else
            nextPC = (unsigned int)-1;
    }
}

///Creates a string which represent the current backtrace
std::string Backtracer::print(){
    std::ostringstream stream;
    std::list<FunCall>::iterator funIter,  funEnd;
    for(funIter = this->backTrace.begin(),  funEnd = this->backTrace.end(); funIter !=  funEnd; funIter++){
        stream << funIter->print() << std::endl;
    }
    
    return stream.str();
}

///Converts an hexadecimal number expressed with a string
///into its correspondent integer number
///each hex number of the string is in the same order of the endianess
///of the processor linked to this stub 
unsigned int Backtracer::toIntNum(std::string &toConvert){
   std::string toConvTemp = toConvert;
   if(toConvTemp.size() >= 2 && toConvTemp[0] == '0' && (toConvTemp[1] == 'X' || toConvTemp[1] == 'x')) 
      toConvTemp = toConvTemp.substr(2);

   unsigned int result = 0;
   unsigned int pos = 0;
   std::string::reverse_iterator hexIter, hexIterEnd;
   for(hexIter = toConvTemp.rbegin(), hexIterEnd = toConvTemp.rend();
                  hexIter != hexIterEnd; hexIter++){
      std::map<char, unsigned int>::iterator mapIter = this->HexMap.find(*hexIter);
      #ifndef NDEBUG
      if(mapIter == this->HexMap.end())
         throw std::runtime_error("bad hex convertion; trying to cast 0x" + toConvTemp);
      #endif
      result |= (mapIter->second << pos);
      pos += 4;
   }
   
   return result;
}

unsigned int Backtracer::gotoFunctionStart(unsigned int address){
    std::string funName = this->symbolManager->symbolAt(address).front();
    std::string tempName = funName;
    unsigned int curAddr = address;
    do{
        curAddr -= sizeof(int);
        tempName = this->symbolManager->symbolAt(curAddr).front();
    }while(tempName == funName && curAddr > 0);
    return (curAddr + sizeof(int));
}

int main(int argc,  char * argv[]){
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produces the help message")
        ("exec-file,e", boost::program_options::value<std::string>(), "name of the executabe file we simulated")
        ("simTime,s", boost::program_options::value<double>(), "simulation time from which we want to start the backtrace")
        ("trace-file,t", boost::program_options::value<std::string>(), "file containing the trace")
        ("lr,l", boost::program_options::value<std::string>(), "string identifying the link register in the trace")
        ("pc,p", boost::program_options::value<std::string>(), "string identifying the program counter in the trace")
    ;

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);

    //Now the program option parsing
    if (vm.count("help")  != 0) {
        std::cout << desc << "\n";
        return 0;
    }

    if (vm.count("exec-file") == 0) {
        std::cerr << "Path of the executable file is required\n";
        std::cerr << desc << "\n";
        return 1;
    }
    if (vm.count("simTime") == 0) {
        std::cerr << "Initial simulation time required\n";
        std::cerr << desc << "\n";
        return 1;
    }
    if (vm.count("trace-file") == 0) {
        std::cerr << "Path to the trace file required\n";
        std::cerr << desc << "\n";
        return 1;
    }
    if (vm.count("lr") == 0) {
        std::cerr << "Identifier of the link register required\n";
        std::cerr << desc << "\n";
        return 1;
    }
    if (vm.count("pc") == 0) {
        std::cerr << "Identifier of the program counter required\n";
        std::cerr << desc << "\n";
        return 1;
    }
    
    Backtracer bt(vm["exec-file"].as<std::string>(),  vm["trace-file"].as<std::string>(),
                  vm["lr"].as<std::string>(), vm["pc"].as<std::string>());
    bt.computeBackTrace(vm["simTime"].as<double>());
    std::cout << bt.print();
    
    return 0;
}
