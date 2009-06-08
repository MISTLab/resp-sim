#include "utils.hpp"

#include <iostream>
#include <string>

#include <cstdlib>

#ifdef STATIC_PLATFORM
void resp::killAll(std::string errorMsg){
    std::cerr << errorMsg << std::endl;
    ::exit(-1);
}
#else
#include <exception>
#include <stdexcept>
#include <execinfo.h>
#include <signal.h>
void resp::RaiseTraceException(std::string message){
    void * array[25];
    int nSize = backtrace(array, 25);
    char ** symbols = backtrace_symbols(array, nSize);
    std::ostringstream traceMex;

    for (int i = 0; i < nSize; i++){
        traceMex << symbols[i] << std::endl;
    }
    traceMex << std::endl;
    traceMex << message;

    throw std::runtime_error(traceMex.str());

    free(symbols);
}
#endif
