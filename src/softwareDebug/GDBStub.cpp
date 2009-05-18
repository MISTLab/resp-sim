#include <vector>
#include "controller.hpp"
#include "GDBStub.hpp"


//namespace resp {
std::vector<GDBStubBase *> resp::stubs;
unsigned int resp::numStopped = 0;
boost::mutex resp::stop_mutex;
boost::mutex resp::cont_mutex;
double resp::curSimTime = 0;
double resp::timeToGo = 0;
double resp::timeToJump = 0;
double resp::simStartTime = 0;
#ifdef ENABLE_MYSQL
bool resp::realtime = true;
#endif
GDBStubBase::DirectionType resp::direction = GDBStubBase::FORWARD;
//}

using namespace resp;

///Callback called by SystemC to signal to all stubs that the simulation
///has ended
void GDBSimEndCallback::operator()(){
    #ifndef NDEBUG
    std::cerr << __PRETTY_FUNCTION__ << " GDB EOS callback" << std::endl;
    #endif
    #ifdef ENABLE_MYSQL
    if(!realtime)
        return;
    #endif
    std::vector<GDBStubBase *>::iterator stubsIter, stubsEnd;
    for(stubsIter = stubs.begin(), stubsEnd = stubs.end();
                            stubsIter != stubsEnd; stubsIter++){
        (*stubsIter)->signalProgramEnd();
    }
}

///Callback called by the controller to signal that the simulation
///ha been paused
void GDBPauseCallback::operator()(){
    #ifdef ENABLE_MYSQL
    if(!realtime)
        return;
    #endif
    #ifndef NDEBUG
    std::cerr << __PRETTY_FUNCTION__ << " Calling GDB pause callback" << std::endl;
    #endif
    if(stubs.size() > 0)
        stubs[0]->setStopped(GDBStubBase::PAUSED);
}

///Callback called by the controller to signal that the simulation
///timeout has expired
void GDBTimeoutCallback::operator()(){
    #ifdef ENABLE_MYSQL
    if(!realtime)
        return;
    #endif
    #ifndef NDEBUG
    std::cerr << __PRETTY_FUNCTION__ << " Calling GDB timeout callback" << std::endl;
    #endif
    if(stubs.size() > 0)
        stubs[0]->setStopped(GDBStubBase::TIMEOUT);
}

///Callback called by SystemC to signal to all stubs that the simulation
///has ended
void GDBErrorCallback::operator()(){
    #ifndef NDEBUG
    std::cerr << __PRETTY_FUNCTION__ << " GDB Error callback" << std::endl;
    #endif
    std::vector<GDBStubBase *>::iterator stubsIter, stubsEnd;
    for(stubsIter = stubs.begin(), stubsEnd = stubs.end();
                            stubsIter != stubsEnd; stubsIter++){
        (*stubsIter)->signalProgramEnd(true);
    }
}
