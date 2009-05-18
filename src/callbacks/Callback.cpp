#include <Python.h>
#include <iostream>
#include <string>
#include <list>
#include "Callback.hpp"

using namespace resp;

///Given a name of the instance of a C++ object instantiated into
///Python it returns the PyObject containing it; baseObj is the
///namespace into which we are going to find for the symbol
///NULL if the object is not found
PyObject * Callback::getObjectInstance(const char* instanceName, PyObject *baseObj){
   PyObject* instance_string = PyString_FromString(instanceName);
   PyObject* instance = PyObject_GetAttr(baseObj, instance_string);
   
   return instance;
}

///Given a Python object it returns the address of the
///C++ object contained into it
unsigned long Callback::getAddress(PyObject * obj){
   return 0;
}

///Given the name of the instance of a C++ object instantiated into
///Python it sets a watch on this instance, so that every time
///it is modified or it is accessed actions are executed
///It returns true if the operation was successful, false
///otherwise
bool Callback::setWatch(std::string instanceName){
   //First of all I have to get a handle to the dictionary
   //containing the desired objects
   PyObject* main_module = PyImport_AddModule("__main__");
   PyObject* main_dict =  PyModule_GetDict(main_module);
   PyObject* ifaceInstrance = PyDict_GetItemString(main_dict , "iface");
   PyObject* locals_string = PyString_FromString("locals");
   PyObject* consoleLocals = PyObject_GetAttr(ifaceInstrance, locals_string);
   
   //Now I have to parse the instanceName string, dividing the different
   //namespaces contained in it
   std::list<std::string> namespaces;
   std::string::size_type dotPos = instanceName.find('.', 0);
   while(dotPos != std::string::npos){
      namespaces.push_back(instanceName.substr(0, dotPos));
      instanceName = instanceName.substr(dotPos + 1);
      dotPos = instanceName.find('.', 0);
   }
   namespaces.push_back(instanceName);

   //Now it is time to iterate on the namespaces and get their instances;
   //First of all I have to separately get the first name of the namespace from
   //the dictionary
   PyObject* curNameSpace = PyDict_GetItemString(consoleLocals, namespaces.front().c_str());
   if(curNameSpace == NULL){
      std::cerr << namespaces.front() << " namespace not correctly resolved" << std::endl;
      return false;
   }
   namespaces.pop_front();
   std::list<std::string>::iterator listBeg, listEnd;
   for(listBeg = namespaces.begin(), listEnd = namespaces.end(); listBeg != listEnd; listBeg++){
      curNameSpace = this->getObjectInstance(listBeg->c_str(), curNameSpace);
      if(curNameSpace == NULL){
         std::cerr << *listBeg << " namespace not correctly resolved" << std::endl;
         return false;
      }
   }
   
   //Now I get the address of the C++ object represented by the found Python Object
   std::cerr << " determined address " << std::hex << this->getAddress(curNameSpace) << std::dec <<std::endl;
   
   //TODO: now connection with GDB ....
   
   return true;
}
