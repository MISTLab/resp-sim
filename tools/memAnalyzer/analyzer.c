#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>

#include "analyzer.h"

long long memLimit = 0;
unsigned char wordSize = 4;

void printHelp(){
    printf("Usage: memAnalyzer -f \"dumpFile\" -o \"outputFile\" -c \"startSimTime\" -e \"endSimTime\" -a \"address\" -d \"op\" -m \"memSize\" -w \"wordsize\" -v \"value\"\n");
    printf("Where operation can be: \n");
    printf("\t0: create memory image at simulation time... (if no simulation time is given it creates the memory image at the end of the simulation)\n");
    printf("\t1: get first modification of address after a given simulation time\n");
    printf("\t2: get last modification of an address\n");
    printf("\t3: get all memory modification in a simulation time interval to a specified address\n");
    printf("\t4: get all modifications to a specified address\n");
    printf("\t5: get all writes performed in memory of value v\n");
}

int main(int argc, char * argv[]){
  FILE * fileIn = NULL;
  FILE * fileOut = NULL;
  double simTime = -1;
  int addr = -1;
  int operation = -1;
  double endSimTime = -1;
  int value = -1;

  memAccess retVal;

  int nextOption = 0;
  const char* const shortOptions = "hf:o:c:a:d:e:v:m:w:";
  const struct option longOptions[] = {
    {"help", 0, NULL, 'h'},
    {"fileDump", 1, NULL, 'f'},
    {"fileOut", 1, NULL, 'o'},
    {"simTime", 1, NULL, 'c'},
    {"endSimTime", 1, NULL, 'e'},
    {"addr", 1, NULL, 'a'},
    {"operation", 1, NULL, 'd'},
    {"value", 1, NULL, 'v'},
    {"memLimit", 1, NULL, 'm'},
    {"wordsize", 1, NULL, 'w'},
    {NULL, 0, NULL, 0},
  };

  do{
    nextOption = getopt_long(argc, argv, shortOptions, longOptions, NULL);
    switch(nextOption){
      case 'h':
      case '?':
        printHelp();
        return 0;
      break;
      case 'f':
        fileIn = fopen(optarg, "r+b");
        if(fileIn == NULL){
          fprintf(stderr, "Unable to open input file %s\n", optarg);
          return 1;
        }
      break;
      case 'o':
        fileOut = fopen(optarg, "w+t");
        if(fileOut == NULL){
          fprintf(stderr, "Unable to open output file %s\n", optarg);
          return 1;
        }
      break;
      case 'c':
        simTime = atoi(optarg);
      break;
      case 'e':
        endSimTime = atoi(optarg);
      break;
      case 'a':
        addr = toIntNum(optarg);
      break;
      case 'd':
        operation = atoi(optarg);
      break;
      case 'v':
        value = toIntNum(optarg);
      break;
      case 'm':
        memLimit = toIntNum(optarg);
      break;
      case 'w':
        wordSize = (unsigned char)atoi(optarg);
      break;
      case -1:
      break;
      default:
        fprintf(stderr, "Unexpected  option value\n");
        printHelp();
        return 1;
      break;
    }
  }while(nextOption != -1);

  if(memLimit == 0){
     fprintf(stderr, "Dimesion of memory not specified; please use the -m option\n");
     printHelp();
     return 1;
  }

  //Now I have to choose the operation
  switch(operation){
    case 0: //create mem image
      createMemImage(fileIn, simTime, fileOut);
      return 0;
      break;
    case 1: //get first modification after the given cycle
      if(simTime <= 0 || addr < 0){
        fprintf(stderr, "You need to specify the processor cycle and the address\n");
        return 0;
      }
      retVal = getFirstModAfter(fileIn, simTime, addr);
      break;
    case 2: //get last modification
      if(addr < 0){
        fprintf(stderr, "You need to specify the address\n");
        return 0;
      }
      retVal = getLastMod(fileIn, addr);
      break;
    case 3: //all accesses to address in an interval
      if(addr < 0){
        fprintf(stderr, "You need to specify a valid address\n");
        return 0;
      }
      getAllAccesses(fileIn, addr, fileOut, simTime, endSimTime);
      return 0;
      break;
    case 4: //all write accesses to address
      if(addr < 0){
        fprintf(stderr, "You need to specify a valid address\n");
        return 0;
      }
      getAllModifications(fileIn, addr, fileOut);
      return 0;
      break;
    case 5: //all writes of value
      getAllValWrites(fileIn, value, fileOut);
      return 0;
      break;
    default:
      fprintf(stderr, "Unknown operation %d\n", operation);
      printHelp();
      return 0;
      break;
  }

  //Finally I can print the result of the call in case I didn't neet to write it to
  //file
  if(retVal.op == WRITE_DUMP)
    printf("address: %#x = %#x -- processor: %d at simulation time %le -- request: write\n",
                      retVal.address, retVal.val, retVal.procNum, retVal.simulationTime);
  else
    printf("address: %#x = %#x -- processor: %d at simulation time %le -- request: read\n",
                      retVal.address, retVal.val, retVal.procNum, retVal.simulationTime);

  return 0;
}

///Creates the image of the memory as it was at cycle procCycle
void createMemImage(FILE * dump, double simTime, FILE * outMem){
  int * tempMemImage = (int *)malloc(sizeof(int)*memLimit);
  memAccess readVal;
  int maxAddress = 0;

  bzero(tempMemImage, sizeof(tempMemImage));

  while(fread(&readVal, sizeof(memAccess), 1, dump) == 1){ //I read all the memory elements
    if(readVal.simulationTime > simTime && simTime > 0) //I've reached the desired cycle
      break;

    if(readVal.op == WRITE_DUMP && readVal.address < memLimit){
      if(readVal.address > memLimit || readVal.address < 0){
         fprintf(stderr, "ERROR -- addr:%d, op:%d, procNum:%d, simTime:%le\n", readVal.address, readVal.op, readVal.procNum, readVal.simulationTime);
         continue;
      }
      tempMemImage[readVal.address/wordSize] = readVal.val;
      if(readVal.address/wordSize > maxAddress)
        maxAddress = readVal.address/wordSize;
    }
  }

  //Now I print on the output file the memory image
  int i = 0;
  for(i = 0; i < maxAddress + 1; i++){
    fprintf(outMem, "%#x(%d) = %#x\n", i*wordSize, i*wordSize, tempMemImage[i]);
  }

  fclose(outMem);
  free(tempMemImage);
}

///Returns the first memory access that modifies the address addr after
///procCycle
memAccess getFirstModAfter(FILE * dump, double simTime, int addr){
  memAccess readVal;

  while(fread(&readVal, sizeof(memAccess), 1, dump) == 1){ //I read all the memory elements
    if(readVal.op == WRITE_DUMP && readVal.simulationTime >= simTime && readVal.address == addr)
      return readVal;
  }

  return readVal;
}

///Returns the last memory access that modified addr
memAccess getLastMod(FILE * dump, int addr){
  memAccess readVal;
  memAccess foundAccess = {0, 0, 0, 0, 0};

  while(fread(&readVal, sizeof(memAccess), 1, dump) == 1){ //I read all the memory elements
    if(readVal.op == WRITE_DUMP &&  readVal.address == addr)
      foundAccess = readVal;
  }

  return foundAccess;
}

///Prints all the modifications done to address addr
void getAllModifications(FILE * dump, int addr, FILE * outMem){
  memAccess readVal;

  while(fread(&readVal, sizeof(memAccess), 1, dump) == 1){ //I read all the memory elements
    if(readVal.op == WRITE_DUMP && readVal.address == addr)
      fprintf(outMem, "value = %#x -- processor: %d at simulation time %le\n",
                        readVal.val, readVal.procNum, readVal.simulationTime);
  }

  fclose(outMem);
}

///Prints all the accesses to a memory location executed in a time interval
void getAllAccesses(FILE * dump, int addr, FILE * outMem, double initSimTime, double endSimTime){
  memAccess readVal;

  while(fread(&readVal, sizeof(memAccess), 1, dump) == 1){ //I read all the memory elements
    if(readVal.address == addr && readVal.simulationTime >= initSimTime
                                && readVal.simulationTime <= endSimTime){
      if(readVal.op == READ_DUMP)
        fprintf(outMem, "value = %#x -- processor: %d at simulation time %le -- READ\n",
                      readVal.val, readVal.procNum, readVal.simulationTime);
      else
        fprintf(outMem, "value = %#x -- processor: %d at simulation time %le -- WRITE\n",
                      readVal.val, readVal.procNum, readVal.simulationTime);
    }
  }

  fclose(outMem);
}

///Prints all writes of the given value
void getAllValWrites(FILE * fileIn, int value, FILE *fileOut){
  memAccess readVal;

  while(fread(&readVal, sizeof(memAccess), 1, fileIn) == 1){ //I read all the memory elements
    if(readVal.val == value){
      if(readVal.op == WRITE_DUMP)
        fprintf(fileOut, "address = %#x -- processor: %d at simulation time %le\n",
                      readVal.address, readVal.procNum, readVal.simulationTime);
    }
  }

  fclose(fileOut);
}

///Given an array of chars (either in hex or decimal form) if converts it to the
///corresponding integer representation
int toIntNum(char * numStr){
   size_t len = strlen(numStr);

   if(len > 2 && numStr[0] == '0' && (numStr[1] == 'x' || numStr[1] == 'X')){
      return toHex(numStr + 2);
   }
   else{
      //It still might be the string does not start with 0x but it is still a hex number;
      //I also have to check that it is a valid string
      int ishex = 0;
      unsigned int i = 0;
      for(i = 0; i < len; i++){
         if(tolower(numStr[i]) >= 'a' && tolower(numStr[i]) <= 'f'){
           ishex++;
         }else if (tolower(numStr[i]) > 'f' || (tolower(numStr[i]) < 'a' && tolower(numStr[i]) > '9')  || tolower(numStr[i]) < '0'){
            fprintf(stderr,  "%s is not a valid number\n",  numStr);
            exit(0);
         }
      }
      if(ishex > 0){
         return toHex(numStr);
      }
      else
         return atoi(numStr);
   }
   return 0;
}

///Given an array of chars which represents a hexadecimal number,  it converts it
///to an integer
int toHex(char * hexStr){
    int retVal = 0;
    sscanf(hexStr, "%x", &retVal);
    return retVal;
}
