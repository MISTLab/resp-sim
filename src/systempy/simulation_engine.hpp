#ifndef SIMULATION_ENGINE_H
#define SIMULATION_ENGINE_H

/*
 *   The simulation engine
 *
 *   It implements the simulation start and stop mechanism
 *
 */

#include <systemc.h>
#include "callback.hpp"

namespace resp{

void register_EOS_callback(EOScallback &callBack);

class simulation_engine : public sc_module{
    friend class sc_simcontext;
  private:
    int pid;
    bool interactive;
    bool &controlPaused;
  public:
    bool isKilled;
    bool goingToPause;
    // Blocks the SystemC execution
    void pause();

    /// Executes the simulation cycle for a specified amount of time
//     void simulate( sc_time& duration );

    SC_HAS_PROCESS(simulation_engine);

    /// Simulation engine constructor
    simulation_engine(sc_module_name name, int pid,  bool interactive, bool &controlPaused);
    ///Overloading of the end_of_simulation method; it can be used to execute methods
    ///at the end of the simulation
    void end_of_simulation();
};

}

#endif /* SIMULATION_ENGINE_H */
