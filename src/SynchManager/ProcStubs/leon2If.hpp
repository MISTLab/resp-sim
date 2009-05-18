#ifndef LEON2IF_HPP
#define LEON2IF_HPP

#include "ProcessorIf.hpp"
#include "leon2.H"

#include <vector>

namespace resp{

class leon2If : public ProcessorIf{
    private:
        leon2 &procInst;
    public:
        leon2If(leon2 &procInst) : procInst(procInst){}
        std::vector<unsigned char> getStatus();
        void setStatus(std::vector<unsigned char> &status);
        void setStack(unsigned long long stackAddr);
        void clearStatus();
        void setReturnAddr(unsigned long long retAddr);
        unsigned long long getReturnAddr();
        unsigned long long getStack();
        void setPC(unsigned long long pcAddr);
        unsigned long long getPC();
        unsigned int getId();
        void setRetVal(int retVal);
        void setFunctionArg(unsigned int position, void *argument);
        void setTLS(std::pair<void *, void *> tls);
};

}
#endif
