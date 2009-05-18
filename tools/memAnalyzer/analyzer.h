#ifndef _ANALYZER
#define _ANALYZER

#define WRITE_DUMP 0
#define READ_DUMP 1

///This structure represents a single memory access operation
typedef struct _memAccess{
  int address;
  int op;
  int procNum;
  int val;
  double simulationTime;
} memAccess;

///Creates the image of the memory as it was at cycle procCycle
void createMemImage(FILE * dump, double simTime, FILE * outMem);
///Returns the first memory access that modifies the address addr after
///procCycle
memAccess getFirstModAfter(FILE * dump, double simTime, int addr);
///Returns the last memory access that modified addr
memAccess getLastMod(FILE * dump, int addr);
///Prints all the modifications done to address addr
void getAllModifications(FILE * dump, int addr, FILE * outMem);
///Prints all the accesses to a memory location executed in a time interval
void getAllAccesses(FILE * dump, int addr, FILE * outMem, double initSimTime, double endSimTime);
///Prints all writes of the given value
void getAllValWrites(FILE * fileIn, int value, FILE *fileOut);
///Given an array of chars (either in hex or decimal form) if converts it to the
///corresponding integer representation
int toIntNum(char * numStr);
///Given an array of chars which represents a hexadecimal number,  it converts it
///to an integer
int toHex(char * hexStr);

#endif
