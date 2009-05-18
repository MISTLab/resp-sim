#ifndef ARM7CAIF_HPP
#define ARM7CAIF_HPP

#include "ProcessorIf.hpp"
#include "arm7ca.H"

#include <vector>

namespace resp{

class arm7caIf : public ProcessorIf{
    private:
        arm7ca &procInst;
    public:
        arm7caIf(arm7ca &procInst) : procInst(procInst){}
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
