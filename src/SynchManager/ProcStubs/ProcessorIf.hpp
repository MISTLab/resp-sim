#ifndef PROCESSORIF_HPP
#define PROCESSORIF_HPP

#include <vector>

namespace resp{

class ProcessorIf{
    public:
        virtual ~ProcessorIf(){}
        virtual std::vector<unsigned char> getStatus() = 0;
        virtual void setStatus(std::vector<unsigned char> &status) = 0;
        virtual void setStack(unsigned long long stackAddr) = 0;
        virtual unsigned long long getStack() = 0;
        virtual void setPC(unsigned long long pcAddr) = 0;
        virtual unsigned long long getPC() = 0;
        virtual void setReturnAddr(unsigned long long retAddr) = 0;
        virtual unsigned long long getReturnAddr() = 0;
        virtual void setRetVal(int retVal) = 0;
        virtual void clearStatus() = 0;
        virtual unsigned int getId() = 0;
        virtual void setFunctionArg(unsigned int position, void *argument) = 0;
        virtual void setTLS(std::pair<void *, void *> tls) = 0;
};

}
#endif
