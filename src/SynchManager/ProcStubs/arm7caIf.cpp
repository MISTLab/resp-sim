#include <boost/lexical_cast.hpp>
#include <vector>

#include "utils.hpp"

#include "arm7caIf.hpp"
#include "ac_syscall.H"

using namespace resp;

void arm7caIf::clearStatus(){
    for(int i = 0; i < procInst.RB.size(); i++){
        procInst.RB[i] = 0;
    }
    procInst.PSR[0] = 0;
    procInst.ac_pc.write_immediate(0);
    procInst.decode_pc = 0;
}

std::vector<unsigned char> arm7caIf::getStatus(){
    std::vector<unsigned char> status;
    //I have to take all the registers (well,  the ones currently used) and
    //add them to the list
//     cerr << "###" << procInst.RB[15] << " --- " << procInst.ac_pc<< endl;
    if (procInst.RB[15] - procInst.ac_pc == 4 ) {
        procInst.RB[15] = procInst.ac_pc -4;
    }
    for(int i = 0; i < procInst.RB.size(); i++){
        for(unsigned int j = 0; j < sizeof(procInst.RB[i]); j++)
            status.push_back((procInst.RB[i] & (0x0FF << 8*j)) >> 8*j);
    }
    //I also have to save the program status register
    for(unsigned int j = 0; j < sizeof(procInst.PSR[0]); j++)
        status.push_back((procInst.PSR[0] & (0x0FF << 8*j)) >> 8*j);
    // and the program counter
    for(unsigned int j = 0; j < sizeof(procInst.ac_pc); j++)
        status.push_back((procInst.ac_pc & (0x0FF << 8*j)) >> 8*j);
    #ifndef NDEBUG
    if(status.size() != sizeof(procInst.RB[0])*procInst.RB.size() + sizeof(procInst.PSR[0]) + sizeof(procInst.ac_pc)){
        THROW_EXCEPTION("Wrong dimension of the computed status vector: expected " << sizeof(procInst.RB[0])*procInst.RB.size() << " got: " << status.size());
    }
    #endif

    return status;
}

void arm7caIf::setStatus(std::vector<unsigned char> &status){
    #ifndef NDEBUG
    if(status.size() != sizeof(procInst.RB[0])*procInst.RB.size() +  sizeof(procInst.PSR[0]) + sizeof(procInst.ac_pc)){
        THROW_EXCEPTION("Wrong dimension of the saved status vector: expected " << sizeof(procInst.RB[0])*procInst.RB.size() << " got: " << status.size());
    }
    #endif
    for(int i = 0; i < procInst.RB.size(); i++){
        procInst.RB[i] = 0;
        for(unsigned int j = 0; j < sizeof(procInst.RB[i]); j++)
            procInst.RB[i] |= (status[i*sizeof(procInst.RB[i]) + j] << j*8);
    }
    procInst.PSR[0] = 0;
    for(unsigned int j = 0; j < sizeof(procInst.PSR[0]); j++)
        procInst.PSR[0] |= (status[procInst.RB.size()*sizeof(procInst.RB[0]) + j] << j*8);
    procInst.ac_pc = 0;
    for(unsigned int j = 0; j < sizeof(procInst.ac_pc); j++)
        procInst.ac_pc |= (status[procInst.RB.size()*sizeof(procInst.RB[0]) + sizeof(procInst.PSR[0]) + j] << j*8);

    procInst.ac_pc.write_immediate(procInst.RB[15]+ 4);
    procInst.decode_pc = procInst.ac_pc;
    procInst.RB[15] = procInst.RB[15]+8;
    procInst.isa.branchAddr = procInst.ac_pc;

    procInst.MAINPIPE_EX.flush();
    procInst.MAINPIPE_ID.flush();
    procInst.MAINPIPE_IF.flush();

    procInst.MAINPIPE_EX.skip_instruction = true;
    procInst.MAINPIPE_ID.skip_instruction = true;
    procInst.MAINPIPE_IF.skip_instruction = true;
}

void arm7caIf::setStack(unsigned long long stackAddr){
    procInst.RB[13] = (unsigned int)stackAddr;
}

void arm7caIf::setRetVal(int retVal){
    procInst.RB[0] = retVal;
}

void arm7caIf::setReturnAddr(unsigned long long retAddr){
    procInst.RB[14] = (unsigned int)retAddr;
}

unsigned long long arm7caIf::getReturnAddr(){
    return procInst.RB[14];
}

unsigned long long arm7caIf::getStack(){
    return procInst.RB[13];
}

void arm7caIf::setFunctionArg(unsigned int position, void *argument){
    #ifndef NDEBUG
    if(position > 3){
        THROW_EXCEPTION("Only up to 4 arguments can be supplied to a routine call for the arm processor");
    }
    #endif
    procInst.RB[position] = (unsigned long)argument;
}

void arm7caIf::setPC(unsigned long long pcAddr){

    procInst.ac_pc.write_immediate((unsigned int)pcAddr);
    procInst.decode_pc = (unsigned int)pcAddr;
    procInst.RB.write(15, (unsigned int)pcAddr);
    procInst.isa.branchAddr = pcAddr;

    procInst.MAINPIPE_EX.flush();
    procInst.MAINPIPE_ID.flush();
    procInst.MAINPIPE_IF.flush();

    procInst.MAINPIPE_EX.skip_instruction = true;
    procInst.MAINPIPE_ID.skip_instruction = true;
    procInst.MAINPIPE_IF.skip_instruction = true;
}

unsigned long long arm7caIf::getPC(){
    return procInst.ac_pc;
}

unsigned int arm7caIf::getId(){
    return procInst.procId;
}

void arm7caIf::setTLS(std::pair<void *, void *> tls){
}
