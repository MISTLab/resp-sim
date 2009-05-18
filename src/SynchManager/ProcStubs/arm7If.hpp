#ifndef ARM7IF_HPP
#define ARM7IF_HPP

#include "ProcessorIf.hpp"
#include "arm7.H"

#include <vector>

namespace resp{

class arm7If : public ProcessorIf{
    private:
        arm7 &procInst;
    public:
        arm7If(arm7 &procInst) : procInst(procInst){}
        std::vector<unsigned char> getStatus();
        void setStatus(std::vector<unsigned char> &status);
        void setStack(unsigned long long stackAddr);
        void setReturnAddr(unsigned long long retAddr);
        unsigned long long getReturnAddr();
        unsigned long long getStack();
        void clearStatus();
        void setPC(unsigned long long pcAddr);
        unsigned long long getPC();
        unsigned int getId();
        void setRetVal(int retVal);
        void setFunctionArg(unsigned int position, void *argument);
        void setTLS(std::pair<void *, void *> tls);
};

}
#endif
