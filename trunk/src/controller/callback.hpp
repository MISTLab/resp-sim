#ifndef EOSCALLBACK_HPP
#define EOSCALLBACK_HPP

namespace resp {

///superclass of all the callbacks which are called by SystemC when the simulation
///has ended
class EOScallback{
  public:
   EOScallback(){}
   virtual void operator()() = 0;
   virtual ~EOScallback(){}
};

class PauseCallback{
  public:
   PauseCallback(){}
   virtual void operator()() = 0;
   virtual ~PauseCallback(){}
};

class TimeoutCallback{
  public:
   TimeoutCallback(){}
   virtual void operator()() = 0;
   virtual ~TimeoutCallback(){}
};

class ErrorCallback{
  public:
   ErrorCallback(){}
   virtual void operator()() = 0;
   virtual ~ErrorCallback(){}
};

class DeltaCallback {
public:
	DeltaCallback(){}
    virtual void operator()() = 0;
    virtual ~DeltaCallback() {}
};

}

#endif
