#ifndef CALLBACK_HPP
#define CALLBACK_HPP

#include <Python.h>
#include <string>

namespace resp{

class Callback{
private:
   ///Given a name of the instance of a C++ object instantiated into
   ///Python it returns the PyObject containing it; baseObj is the
   ///namespace into which we are going to find for the symbol
   ///NULL if the object is not found
   PyObject * getObjectInstance(const char* instanceName, PyObject *baseObj);
   ///Given a Python object it returns the address of the
   ///C++ object contained into it
   unsigned long getAddress(PyObject * obj);
  public:
   ///Given the name of the instance of a C++ object instantiated into
   ///Python it sets a watch on this instance, so that every time
   ///it is modified or it is accessed actions are executed
   ///It returns true if the operation was successful, false
   ///otherwise
   bool setWatch(std::string instanceName);
};

}

#endif
