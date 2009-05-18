#ifndef BFDFRONTEND_H
#define BFDFRONTEND_H

//Part of the code of this class is taken from the binutils sources

extern "C" {
#include <bfd.h>
#include <dis-asm.h>
}

#include <map>
#include <string>
#include <list>
#include <vector>
#include <boost/regex.hpp>

struct Section{
    struct bfd_section *descriptor;
    bfd_byte * data;
    bfd_vma startAddr;
    bfd_size_type datasize;
    std::string name;
};

class BFDFrontend{
  private:
    //Contains the list of all the symbols in the binary file
    asymbol **sy;
    ///Size of each assembly instruction in bytes
    unsigned int wordsize;
    ///Descriptor of the binary file
    bfd * execImage;
    std::string execName;

    ///Variables holding what read from the file
    std::map<unsigned int, std::list<std::string> > addrToSym;
    std::map<unsigned int, std::string> addrToFunction;
    std::map<std::string, unsigned int> symToAddr;
    std::map<unsigned int, std::pair<std::string, unsigned int> > addrToSrc;
    std::map<unsigned int, std::string> addrToAssembly;

    //end address and start address (not necessarily the entry point) of the loadable part of the binary file
    std::pair<unsigned int, unsigned int> codeSize;

    ///Contains a list of the sections which contain executable code
    std::vector<Section> secList;

    ///Accesses the BFD internal structures in order to get the dissassbly of the instructions
    void readDisass();
    ///Accesses the BFD internal structures in order to get correspondence among machine code and
    ///the source code
    void readSrc();
    ///Accesses the BFD internal structures in order to get the dissassbly of the symbols
    void readSyms();
    ///Is case it is not possible to open the BFD because it is not possible to determine
    ///it target, this function extracts the list of possible targets
    std::string getMatchingFormats (char **p);
    /// sprintf wrapper with append features
    static void sprintf_wrapper(char *str, const char *format, ...);
  public:
    BFDFrontend(std::string binaryName, bool readSrc = false);
    ~BFDFrontend();
    ///Given an address, it returns the symbol found there,
    ///"" if no symbol is found at the specified address; note
    ///That if address is in the middle of a function, the symbol
    ///returned refers to the function itself (but this only if
    ///the object was created with parameter readSrc = true)
    std::list<std::string> symbolAt(unsigned int address);
    ///Given the name of a symbol it returns its value
    ///(which usually is its address);
    ///valid is set to false if no symbol with the specified
    ///name is found
    unsigned int getSymAddr(std::string symbol, bool &valid);
    bool exists(std::string symbol);
    ///Given an address, it sets fileName to the name of the source file
    ///which contains the code and line to the line in that file. Returns
    ///false if the address is not valid
    bool getSrcFile(unsigned int address, std::string &fileName, unsigned int &line);
    ///Given an address it returns the assembly code at that
    ///address, "" if there is none or address is not valid
    std::string getAssembly(unsigned int address);
    ///Returns the name of the executable file
    std::string getExecName();
    ///Returns the end address of the loadable code
    unsigned int getBinaryEnd();
    ///It returns all the symbols that match the given regular expression
    std::map<std::string,  unsigned int> findFunction(boost::regex &regEx);
};

#endif
