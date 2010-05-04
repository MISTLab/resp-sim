#ifndef TIMER_HPP
#define TIMER_HPP

#include <ctime>

class timer
{
 public:
    timer() { _start_time = std::time(NULL); } // postcondition: elapsed()==0
    void   restart() { _start_time = std::time(NULL); } // post: elapsed()==0
    long int elapsed() const                  // return elapsed time in seconds
    { return  std::time(NULL) - _start_time; }

 private:
  std::time_t _start_time;
}; 

#endif
