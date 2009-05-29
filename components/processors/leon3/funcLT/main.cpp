/***************************************************************************\
 *
 *
 *            ___        ___           ___           ___
 *           /  /\      /  /\         /  /\         /  /\
 *          /  /:/     /  /::\       /  /::\       /  /::\
 *         /  /:/     /  /:/\:\     /  /:/\:\     /  /:/\:\
 *        /  /:/     /  /:/~/:/    /  /:/~/::\   /  /:/~/:/
 *       /  /::\    /__/:/ /:/___ /__/:/ /:/\:\ /__/:/ /:/
 *      /__/:/\:\   \  \:\/:::::/ \  \:\/:/__\/ \  \:\/:/
 *      \__\/  \:\   \  \::/~~~~   \  \::/       \  \::/
 *           \  \:\   \  \:\        \  \:\        \  \:\
 *            \  \ \   \  \:\        \  \:\        \  \:\
 *             \__\/    \__\/         \__\/         \__\/
 *
 *
 *
 *
 *   This file is part of TRAP.
 *
 *   TRAP is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *   or see <http://www.gnu.org/licenses/>.
 *
 *
 *
 *   (c) Luca Fossati, fossati@elet.polimi.it
 *
\***************************************************************************/



#ifdef _WIN32
#pragma warning( disable : 4101 )
#endif
#define WIN32_LEAN_AND_MEAN
#include <MemoryLT.hpp>
#include <processor.hpp>
#include <instructions.hpp>
#include <trap_utils.hpp>
#include <systemc.h>
#include <execLoader.hpp>
#include <GDBStub.hpp>
#include <osEmulator.hpp>
#include <boost/program_options.hpp>
#include <boost/timer.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/path.hpp>
#include <string>
#include <vector>

int sc_main( int argc, char * * argv ){
    using namespace leon3_funcat_trap;
    using namespace trap;


    boost::program_options::options_description desc("Processor simulator for LEON3");
    desc.add_options()
    ("help,h", "produces the help message")
    ("debugger,d", "activates the use of the software debugger")
    ("frequency,f", boost::program_options::value<double>(), "processor clock frequency \
        specified in MHz [Default 1MHz]")
    ("application,a", boost::program_options::value<std::string>(), "application to be \
        executed on the simulator")
    ("disassembler,i", "prints the disassembly of the application")
    ("arguments,r", boost::program_options::value<std::string>(), "command line arguments \
        (if any) of the application being simulated")
    ("environment,e", boost::program_options::value<std::string>(), "environmental variables \
        (if any) which can be accesses by the application being simulated")
    ("sysconf,s", boost::program_options::value<std::string>(), "configuration information \
        (if any) which can be accesses by the application being simulated")
    ;

    std::cerr << std::endl;

    boost::program_options::variables_map vm;
    try{
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, \
            desc), vm);
    }
    catch(...){
        std::cerr << "ERROR in parsing the command line parametrs" << std::endl << std::endl;
        std::cerr << desc << std::endl;
        return -1;
    }
    boost::program_options::notify(vm);

    // Checking that the parameters are correctly specified
    if(vm.count("help") != 0){
        std::cout << desc << std::endl;
        return 0;
    }
    if(vm.count("application") == 0){
        std::cerr << "It is necessary to specify the application which has to be simulated \
            using the --application command line option" << std::endl << std::endl;
        std::cerr << desc << std::endl;
        return -1;
    }double latency = 10e-6; // 1us
    if(vm.count("frequency") != 0){
        latency = 1/(vm["frequency"].as<double>());
    }
    //Now we can procede with the actual instantiation of the processor
    Processor procInst("LEON3", sc_time(latency*10e9, SC_NS));
    //Here we instantiate the memory and connect it
    //wtih the processor
    MemoryLT<2, 4> mem("procMem", 1024*1024*10, sc_time(latency*10e9*2, SC_NS));
    procInst.instrMem.initSocket.bind(*(mem.socket[0]));
    procInst.dataMem.initSocket.bind(*(mem.socket[1]));

    //And with the loading of the executable code
    boost::filesystem::path applicationPath = boost::filesystem::system_complete(boost::filesystem::path(vm["application"].as<std::string>(), \
        boost::filesystem::native));
    if ( !boost::filesystem::exists( applicationPath ) ){
        std::cerr << "ERROR: specified application " << vm["application"].as<std::string>() \
            << " does not exist" << std::endl;
        return -1;
    }
    ExecLoader loader(vm["application"].as<std::string>());
    //Lets copy the binary code into memory
    unsigned char * programData = loader.getProgData();
    for(unsigned int i = 0; i < loader.getProgDim(); i++){
        mem.write_byte_dbg(loader.getDataStart() + i, programData[i]);
    }
    if(vm.count("disassembler") != 0){
        std:cout << "Entry Point: " << std::hex << std::showbase << loader.getProgStart() \
            << std::endl << std::endl;
        for(unsigned int i = 0; i < loader.getProgDim(); i+= 4){
            Instruction * curInstr = procInst.decode(procInst.instrMem.read_word_dbg(loader.getDataStart() \
                + i));
            std::cout << std::hex << std::showbase << loader.getDataStart() + i << ":    " << \
                procInst.instrMem.read_word_dbg(loader.getDataStart() + i);
            if(curInstr != NULL){
                std::cout << "    " << curInstr->getMnemonic();
            }
            std::cout << std::endl;
        }
        return 0;
    }
    //Finally I can set the processor variables
    procInst.ENTRY_POINT = loader.getProgStart();
    procInst.PROGRAM_LIMIT = loader.getProgDim() + loader.getDataStart();
    procInst.PROGRAM_START = loader.getDataStart();

    //Now I initialize the tools (i.e. debugger, os emulator, ...)
    OSEmulator< unsigned int, 0 > osEmu(*(procInst.abiIf), 0);
    GDBStub< unsigned int > gdbStub(*(procInst.abiIf));
    osEmu.initSysCalls(vm["application"].as<std::string>());
    std::vector<std::string> options;
    options.push_back(vm["application"].as<std::string>());
    if(vm.count("arguments") > 0){
        //Here we have to parse the command line program arguments; they are
        //in the form option,option,option ...
        std::string packedOpts = vm["arguments"].as<std::string>();
        while(packedOpts.size() > 0){
            std::size_t foundComma = packedOpts.find(',');
            if(foundComma != std::string::npos){
                options.push_back(packedOpts.substr(0, foundComma));
                packedOpts = packedOpts.substr(foundComma + 1);
            }
            else{
                options.push_back(packedOpts);
                break;
            }
        }
    }
    OSEmulatorBase::set_program_args(options);
    if(vm.count("environment") > 0){
        //Here we have to parse the environment; they are
        //in the form option=value,option=value .....
        std::string packedEnv = vm["environment"].as<std::string>();
        while(packedEnv.size() > 0){
            std::size_t foundComma = packedEnv.find(',');
            std::string curEnv;
            if(foundComma != std::string::npos){
                curEnv = packedEnv.substr(0, foundComma);
                packedEnv = packedEnv.substr(foundComma + 1);
            }
            else{
                curEnv = packedEnv;
                packedEnv = "";
            }
            // Now I have to split the current environment
            std::size_t equalPos = curEnv.find('=');
            if(equalPos == std::string::npos){
                std::cerr << "Error in the command line environmental options: = not found in option \
                    " << curEnv << std::endl;
                return -1;
            }
            OSEmulatorBase::set_environ(curEnv.substr(0, equalPos), curEnv.substr(equalPos + 1));
        }
    }
    if(vm.count("sysconf") > 0){
        //Here we have to parse the environment; they are
        //in the form option=value,option=value .....
        std::string packedEnv = vm["sysconf"].as<std::string>();
        while(packedEnv.size() > 0){
            std::size_t foundComma = packedEnv.find(',');
            std::string curEnv;
            if(foundComma != std::string::npos){
                curEnv = packedEnv.substr(0, foundComma);
                packedEnv = packedEnv.substr(foundComma + 1);
            }
            else{
                curEnv = packedEnv;
                packedEnv = "";
            }
            // Now I have to split the current environment
            std::size_t equalPos = curEnv.find('=');
            if(equalPos == std::string::npos){
                std::cerr << "Error in the command line sysconf options: = not found in option " \
                    << curEnv << std::endl;
                return -1;
            }
            try{
                OSEmulatorBase::set_sysconf(curEnv.substr(0, equalPos), boost::lexical_cast<int>(curEnv.substr(equalPos \
                    + 1)));
            }
            catch(...){
                std::cerr << "Error in the command line sysconf options: error in option " << curEnv \
                    << std::endl;
                return -1;
            }
        }
    }
    procInst.toolManager.addTool(osEmu);
    if(vm.count("debugger") != 0){
        procInst.toolManager.addTool(gdbStub);
        gdbStub.initialize();
        procInst.instrMem.setDebugger(&gdbStub);
        procInst.dataMem.setDebugger(&gdbStub);
    }

    //Now we can start the execution
    boost::timer t;
    sc_start();
    double elapsedSec = t.elapsed();
    std::cout << "Elapsed " << elapsedSec << " sec." << std::endl;
    std::cout << "Executed " << procInst.numInstructions << " instructions" << std::endl;
    std::cout << "Execution Speed " << (double)procInst.numInstructions/(elapsedSec*1e6) \
        << " MIPS" << std::endl;
    std::cout << "Simulated time " << sc_time_stamp()/10e3 << std::endl;

    return 0;
}

