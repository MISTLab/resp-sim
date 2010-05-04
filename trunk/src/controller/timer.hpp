#ifndef TIMER_HPP
#define TIMER_HPP

#include <ctime>
#include <sys/time.h>

class timer
{
 public:
    timer() { // postcondition: elapsed()==0
      struct timeval tv;
      gettimeofday(&tv, NULL); 
      _start_time=tv.tv_sec;
    } 
    void   restart() { // post: elapsed()==0
      struct timeval tv;
      gettimeofday(&tv, NULL); 
      _start_time=tv.tv_sec;
    }
    
    long int elapsed() const { // return elapsed time in seconds
      struct timeval tv;
      std::time_t now;
      gettimeofday(&tv, NULL); 
      now=tv.tv_sec;
      return  now - _start_time; 
    }

 private:
  std::time_t _start_time;
}; 

#endif
