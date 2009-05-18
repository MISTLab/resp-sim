#include <boost/lexical_cast.hpp>
#include <vector>

#include "utils.hpp"

#include "leon2If.hpp"

#define NUMBER_WINDOWS 8

using namespace resp;

void leon2If::clearStatus(){
    for(int i = 0; i < procInst.RB.size(); i++){
        procInst.RB[i] = 0;
    }
    for(int i = 0; i < procInst.REGS.size(); i++){
        procInst.REGS[i] = 0;
    }
    procInst.PSR = 0x80 + (NUMBER_WINDOWS - 1);
    procInst.Y = 0;
    procInst.ASR = 0;
    procInst.TBR = 0;
    procInst.WIM= 0;
    procInst.npc = 0;
    procInst.ac_pc = 0;
    procInst.decode_pc = 0;
}

std::vector<unsigned char> leon2If::getStatus(){
    std::vector<unsigned char> status;
    //I have to take all the registers (well,  the ones currently used) and
    //add them to the list
    for(int i = 0; i < procInst.RB.size(); i++){
        for(unsigned int j = 0; j < sizeof(procInst.RB[i]); j++)
            status.push_back((procInst.RB[i] & (0x0FF << 8*j)) >> 8*j);
    }
    for(int i = 0; i < procInst.REGS.size(); i++){
        for(unsigned int j = 0; j < sizeof(procInst.REGS[i]); j++)
            status.push_back((procInst.REGS[i] & (0x0FF << 8*j)) >> 8*j);
    }
    //I also have to save the special processor registers
    for(unsigned int j = 0; j < sizeof(procInst.npc); j++)
        status.push_back((procInst.npc & (0x0FF << 8*j)) >> 8*j);
    for(unsigned int j = 0; j < sizeof(procInst.pc); j++)
        status.push_back((procInst.pc & (0x0FF << 8*j)) >> 8*j);
    for(unsigned int j = 0; j < sizeof(procInst.PSR); j++)
        status.push_back((procInst.PSR & (0x0FF << 8*j)) >> 8*j);
    for(unsigned int j = 0; j < sizeof(procInst.Y); j++)
        status.push_back((procInst.Y & (0x0FF << 8*j)) >> 8*j);
    for(unsigned int j = 0; j < sizeof(procInst.TBR); j++)
        status.push_back((procInst.TBR & (0x0FF << 8*j)) >> 8*j);
    for(unsigned int j = 0; j < sizeof(procInst.ASR); j++)
        status.push_back((procInst.ASR & (0x0FF << 8*j)) >> 8*j);
    for(unsigned int j = 0; j < sizeof(procInst.WIM); j++)
        status.push_back((procInst.WIM & (0x0FF << 8*j)) >> 8*j);
    for(unsigned int j = 0; j < sizeof(procInst.ac_pc); j++)
        status.push_back((procInst.ac_pc & (0x0FF << 8*j)) >> 8*j);
    #ifndef NDEBUG
    if(status.size() != sizeof(procInst.RB[0])*procInst.RB.size() + sizeof(procInst.REGS[0])*procInst.REGS.size() + sizeof(procInst.npc) + sizeof(procInst.pc) + sizeof(procInst.PSR) + 
        sizeof(procInst.Y) + sizeof(procInst.TBR) + sizeof(procInst.ASR) + sizeof(procInst.WIM) + sizeof(procInst.ac_pc)){
        THROW_EXCEPTION("Wrong dimension of the computed status vector: expected " << sizeof(procInst.RB[0])*procInst.RB.size() << " got: " << status.size());
    }
    #endif
    return status;
}

void leon2If::setStatus(std::vector<unsigned char> &status){
    #ifndef NDEBUG
    if(status.size() != sizeof(procInst.RB[0])*procInst.RB.size() + sizeof(procInst.REGS[0])*procInst.REGS.size() + sizeof(procInst.npc) + sizeof(procInst.pc) + sizeof(procInst.PSR) + 
        sizeof(procInst.Y) + sizeof(procInst.TBR) + sizeof(procInst.ASR) + sizeof(procInst.WIM) + sizeof(procInst.ac_pc)){
        THROW_EXCEPTION("Wrong dimension of the saved status vector: expected " << sizeof(procInst.RB[0])*procInst.RB.size() << " got: " << status.size());
    }
    #endif
    int curIndexBase = 0;
    for(int i = 0; i < procInst.RB.size(); i++){
        procInst.RB[i] = 0;
        for(unsigned int j = 0; j < sizeof(procInst.RB[i]); j++)
            procInst.RB[i] |= (status[curIndexBase + j] << j*8);
        curIndexBase += sizeof(procInst.RB[i]);
    }
    for(int i = 0; i < procInst.REGS.size(); i++){
        procInst.REGS[i] = 0;
        for(unsigned int j = 0; j < sizeof(procInst.REGS[i]); j++)
            procInst.REGS[i] |= (status[curIndexBase + j] << j*8);
        curIndexBase += sizeof(procInst.REGS[i]);
    }
    procInst.npc.write(0);
    for(unsigned int j = 0; j < sizeof(procInst.npc); j++)
        procInst.npc.write(procInst.npc.read() | (status[curIndexBase + j] << j*8));
    curIndexBase += sizeof(procInst.npc);
    procInst.pc.write(0);
    for(unsigned int j = 0; j < sizeof(procInst.pc); j++)
        procInst.pc.write(procInst.pc.read() | (status[curIndexBase + j] << j*8));
    curIndexBase += sizeof(procInst.pc);
    procInst.PSR.write(0);
    for(unsigned int j = 0; j < sizeof(procInst.PSR); j++)
        procInst.PSR.write(procInst.PSR.read() | (status[curIndexBase + j] << j*8));
    curIndexBase += sizeof(procInst.PSR);
    procInst.Y.write(0);
    for(unsigned int j = 0; j < sizeof(procInst.Y); j++)
        procInst.Y.write(procInst.Y.read() | (status[curIndexBase + j] << j*8));
    curIndexBase += sizeof(procInst.Y);
    procInst.TBR.write(0);
    for(unsigned int j = 0; j < sizeof(procInst.TBR); j++)
        procInst.TBR.write(procInst.TBR.read() | (status[curIndexBase + j] << j*8));
    curIndexBase += sizeof(procInst.TBR);
    procInst.ASR.write(0);
    for(unsigned int j = 0; j < sizeof(procInst.ASR); j++)
        procInst.ASR.write(procInst.ASR.read() | (status[curIndexBase + j] << j*8));
    curIndexBase += sizeof(procInst.ASR);
    procInst.WIM.write(0);
    for(unsigned int j = 0; j < sizeof(procInst.WIM); j++)
        procInst.WIM.write(procInst.WIM.read() | (status[curIndexBase + j] << j*8));
    curIndexBase += sizeof(procInst.WIM);
    procInst.ac_pc.write(0);
    for(unsigned int j = 0; j < sizeof(procInst.ac_pc); j++)
        procInst.ac_pc.write(procInst.ac_pc.read() | (status[curIndexBase + j] << j*8));
    procInst.decode_pc = (unsigned int)procInst.ac_pc.read();
}

void leon2If::setStack(unsigned long long stackAddr){
    //procInst.REGS[14] = (unsigned int)stackAddr - 1152;
    procInst.REGS[14] = (unsigned int)stackAddr - 1024;
    procInst.REGS[30] = (unsigned int)stackAddr - 1024;
    #ifndef NDEBUG
    if((procInst.REGS[14] & 0x7) != 0){
        THROW_EXCEPTION("Stack address " << std::showbase << std::hex << procInst.REGS[14] << std::dec << " is not double word aligned");
    }
    #endif
}

void leon2If::setReturnAddr(unsigned long long retAddr){
    procInst.REGS[15] = (unsigned int)retAddr - 8;
}

void leon2If::setRetVal(int retVal){
    procInst.REGS[8] = retVal;
}

unsigned long long leon2If::getReturnAddr(){
    return procInst.REGS[15] + 8;
}

unsigned long long leon2If::getStack(){
    return procInst.REGS[14];
}

void leon2If::setFunctionArg(unsigned int position, void *argument){
    #ifndef NDEBUG
    if(position > 3){
        THROW_EXCEPTION("Only up to 4 arguments can be supplied to a routine call for the leon processor");
    }
    #endif
    procInst.REGS[8 + position] = (unsigned long)argument;
}

void leon2If::setPC(unsigned long long pcAddr){
    procInst.decode_pc = (unsigned int)pcAddr;
    procInst.ac_pc = (unsigned int)pcAddr;
    procInst.npc = (unsigned int)pcAddr + 4;
}

unsigned long long leon2If::getPC(){
    return procInst.ac_pc;
}

unsigned int leon2If::getId(){
    return procInst.procId;
}

void leon2If::setTLS(std::pair<void *, void *> tls){
    procInst.REGS[7] = (unsigned long)tls.first;
}
