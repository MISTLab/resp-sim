
#include "RespClient.hpp"
#include "utils.hpp"

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

void RespClient::initHexMap(){
    this->HexMap.insert(std::pair<const char, unsigned int>('0', 0));
    this->HexMap.insert(std::pair<const char, unsigned int>('1', 1));
    this->HexMap.insert(std::pair<const char, unsigned int>('2', 2));
    this->HexMap.insert(std::pair<const char, unsigned int>('3', 3));
    this->HexMap.insert(std::pair<const char, unsigned int>('4', 4));
    this->HexMap.insert(std::pair<const char, unsigned int>('5', 5));
    this->HexMap.insert(std::pair<const char, unsigned int>('6', 6));
    this->HexMap.insert(std::pair<const char, unsigned int>('7', 7));
    this->HexMap.insert(std::pair<const char, unsigned int>('8', 8));
    this->HexMap.insert(std::pair<const char, unsigned int>('9', 9));
    this->HexMap.insert(std::pair<const char, unsigned int>('a', 10));
    this->HexMap.insert(std::pair<const char, unsigned int>('A', 10));
    this->HexMap.insert(std::pair<const char, unsigned int>('b', 11));
    this->HexMap.insert(std::pair<const char, unsigned int>('B', 11));
    this->HexMap.insert(std::pair<const char, unsigned int>('c', 12));
    this->HexMap.insert(std::pair<const char, unsigned int>('C', 12));
    this->HexMap.insert(std::pair<const char, unsigned int>('d', 13));
    this->HexMap.insert(std::pair<const char, unsigned int>('D', 13));
    this->HexMap.insert(std::pair<const char, unsigned int>('e', 14));
    this->HexMap.insert(std::pair<const char, unsigned int>('E', 14));
    this->HexMap.insert(std::pair<const char, unsigned int>('f', 15));
    this->HexMap.insert(std::pair<const char, unsigned int>('F', 15));
}


///Initializes the client and performs the connection with the server; in case the
///server hasn't started yet, it is possible to specify the path of the ReSP startSim
///script: as a consequence the server will be started before attempting to establish
///the connection
RespClient::RespClient(unsigned int port, std::string hostname, std::string respPath, bool graphic) : socket(NULL), hostname(hostname), stopped(false){
    this->initHexMap();
    this->respPid = -1;
    this->respPath = respPath;
    this->graphic = graphic;
    this->port = port;
    this->connect();
}

///When the client is killed, the connection with the server is also closed and
///the server terminated
RespClient::~RespClient(){
    this->shutdown();
}

///Terminates the connection with ReSP
void RespClient::shutdown(){
    this->stopped = true;
    if(this->socket != NULL){
        if(this->socket->is_open()){
            //std::cerr << "Killing socket" << std::endl;
            try{
                this->quit();
            }
            catch(...){}
            //std::cerr << "called quit" << std::endl;
            this->socket->close();
        }
        //std::cerr << "deleting socket" << std::endl;
        delete this->socket;
        this->socket = NULL;
        //std::cerr << "deleted" << std::endl;
    }
    if(this->respPid > 0){
        //std::cerr << "Killing ReSP itself" << std::endl;
        ::kill(this->respPid, SIGTERM);
        int status;
        ::wait(&status);
        this->respPid = 0;
        //std::cerr << "Killed ReSP itself" << std::endl;
    }
}

///Sends a generic message to ReSP server; notes that the message
///has to be encoded before being able to send it
void RespClient::send(const std::string &message){
    if(this->socket != NULL){
        boost::system::error_code error;
        boost::asio::write(*(this->socket), boost::asio::buffer(message.c_str(), message.size()), boost::asio::transfer_all(), error);
        if (error == boost::asio::error::eof)
            THROW_EXCEPTION("Connecetion closed by host \"" << this->hostname << "\" during message reception");
        else if(error)
            THROW_EXCEPTION("Unknown error during message reception with host --> " << this->hostname << ". Error: " << error);
    }
}

///Receives a generic message from ReSP server; note that the message
///has to be decoded before being able to read it
std::string RespClient::receive(){
    if(this->socket != NULL){
        boost::system::error_code error;
        boost::asio::streambuf response;
        boost::asio::read_until(*(this->socket), response, '#', error);
        if (error == boost::asio::error::eof)
            THROW_EXCEPTION("Connecetion closed by host \"" << this->hostname << "\" during message reception");
        else if(error)
            THROW_EXCEPTION("Unknown error during message reception with host --> " << this->hostname << ". Error: " << error);
        std::istream response_stream(&response);
        std::string response_string;
        response_stream >> response_string;
        return response_string;
    }
    else{
        return "";
    }
}

///sends the quit message to ReSP
///returns the response
bool RespClient::quit(){
    std::string message = this->encodeMessage("QUIT");
    this->send(message);
    this->lastResponse = this->decodeMessage(this->receive());
    this->stopped = true;
    return this->lastResponse == "OK";
}

///sends the quit message to ReSP
///returns the response
bool RespClient::reset(){
    std::string message = this->encodeMessage("RESET");
    this->send(message);
    this->lastResponse = this->decodeMessage(this->receive());
    this->stopped = false;
    return this->lastResponse == "OK";
}

///sends the list component message to ReSP
///returns the response
bool RespClient::list_components(){
    // TODO: implement this. I'm too lazy to do it now
    return false;
}

///sends the dir message to ReSP. The dir command is
///applied on item.
///returns the response
bool RespClient::dir_element(std::string item){
    // TODO: implement this. I'm too lazy to do it now
    return false;
}

///creates an instance of an architectural component;
///the parameters of the constructor must be specified
///returns the response
bool RespClient::create_component(const std::string& compType, const std::string &name, const std::vector<std::string> & params){
    std::string parameters;
    std::vector<std::string>::const_iterator paramIter, paramEnd;
    for(paramIter = params.begin(), paramEnd = params.end(); paramIter != paramEnd; paramIter++){
        parameters += 'S' + *paramIter + ");";
    }
    std::string message = this->encodeMessage("CREATE-LS" + compType + ");S" + name + ");" + parameters);
    this->send(message);
    this->lastResponse = this->decodeMessage(this->receive());
    return this->lastResponse.find("OK") == 0;
}

///connects two components together using a TLM port
///returns the response
bool RespClient::connect_tlm(const std::string& initiator, const std::string& initiator_port, const std::string& target, const std::string& target_port){
    std::string message = this->encodeMessage("CONNECTTLM-LS" + initiator + ");S" + initiator_port + ");S" + target + ");S" + target_port + ");");
    this->send(message);
    this->lastResponse = this->decodeMessage(this->receive());
    return this->lastResponse.find("OK") == 0;
}

///Connects two components together using systemc ports
///returns the response
bool RespClient::connect_sysc(const std::string& initiator, const std::string& initiator_port, const std::string& target, const std::string& target_port){
    std::string message = this->encodeMessage("CONNECTSYSC-LS" + initiator + ");S" + initiator_port + ");S" + target + ");S" + target_port + ");");
    this->send(message);
    this->lastResponse = this->decodeMessage(this->receive());
    return this->lastResponse.find("OK") == 0;
}

///Connects a systemc signal with a systemc port
///returns the response
bool RespClient::connect_sig(const std::string& signal_component, const std::string& signal, const std::string& target, const std::string& target_port){
    std::string message = this->encodeMessage("CONNECTSIG-LS" + signal_component + ");S" + signal + ");S" + target + ");S" + target_port + ");");
    this->send(message);
    this->lastResponse = this->decodeMessage(this->receive());
    return this->lastResponse.find("OK") == 0;
}

///Loads an application into memory and initializes the processors
///returns the response
bool RespClient::load_application(const std::string & application){
    // TODO: implement when we have decided how to make it work with
    //multi-processor systems
    return false;
}

///Loads a file describing the hardware architecture to be simulated; note that actually
///this file can contain any valid python directive
///returns the response
bool RespClient::load_architecture(const std::string & archFile){
    boost::filesystem::path fullPath = boost::filesystem::system_complete(boost::filesystem::path(archFile, boost::filesystem::native));
    if ( !boost::filesystem::exists( fullPath ) )
        THROW_EXCEPTION("Path " << fullPath.string() << " does not exists");
    std::string message = this->encodeMessage("LOADARCH-S" + archFile + ')');
    this->send(message);
    this->lastResponse = this->decodeMessage(this->receive());
    return this->lastResponse == "OK";
}

///starts the simulation for the specified ammount of time
///in nanoseconds
bool RespClient::run_simulation(double &time){
    std::string message = this->encodeMessage("RUN-S" + boost::lexical_cast<std::string>(time) + ')');
    this->send(message);
    this->lastResponse = this->decodeMessage(this->receive());
    return this->lastResponse == "OK";
}

///undefinitely starts the simulation
///returns the response
bool RespClient::run_simulation(){
    std::string message = this->encodeMessage("RUN-S-1)");
    this->send(message);
    this->lastResponse = this->decodeMessage(this->receive());
    return this->lastResponse == "OK";
}

///stops the simulation
///returns the response
bool RespClient::stop_simulation(){
    std::string message = this->encodeMessage("STOP");
    this->send(message);
    this->lastResponse = this->decodeMessage(this->receive());
    this->stopped = true;
    return this->lastResponse == "OK";
}

///returns SystemC time
unsigned long RespClient::get_systemc_time(){
    std::string message = this->encodeMessage("GETTIMES");
    this->send(message);
    this->lastResponse = this->decodeMessage(this->receive());
    std::string lastResponsePayload = this->lastResponse.substr(2);
    if(this->lastResponse.find("OK") == 0){
        return atol(lastResponsePayload.substr(1,lastResponsePayload.size()-2).c_str());
    } else THROW_EXCEPTION("Error occurred during get_energy. Error: " << lastResponsePayload);
}

///returns real host time
unsigned long RespClient::get_host_time(){
    std::string message = this->encodeMessage("GETTIMER");
    this->send(message);
    this->lastResponse = this->decodeMessage(this->receive());
    std::string lastResponsePayload = this->lastResponse.substr(2);
    if(this->lastResponse.find("OK") == 0){
        return atol(lastResponsePayload.substr(1,lastResponsePayload.size()-2).c_str());
    } else THROW_EXCEPTION("Error occurred during get_energy. Error: " << lastResponsePayload);
}

///returns system energy use
float RespClient::get_energy(){
    std::string message = this->encodeMessage("GETENERGY");
    this->send(message);
    this->lastResponse = this->decodeMessage(this->receive());
    std::string lastResponsePayload = this->lastResponse.substr(2);
    if(this->lastResponse.find("OK") == 0){
        return atof(lastResponsePayload.substr(1,lastResponsePayload.size()-2).c_str());
    } else THROW_EXCEPTION("Error occurred during get_energy. Error: " << lastResponsePayload);
}

///Executes a custom python statement
///returns the response
bool RespClient::execute_command(const std::string &command){
    std::string message = this->encodeMessage("EXEC-S" + command + ')');
    this->send(message);
    this->lastResponse = this->decodeMessage(this->receive());
    return this->lastResponse == "OK";
}

///Sets the value of a variable; note that the variable name should include
///the path of the variable with respect to the global namespace.
///returns true if the operation was successful
bool RespClient::set_variable_value(const std::string &varName, const std::string &varValue){
    std::string message = this->encodeMessage("EXEC-S" + varName + "=" + varValue + ')');
    this->send(message);
    this->lastResponse = this->decodeMessage(this->receive());
    return this->lastResponse == "OK";
}

///halts execution until simulation hasn't terminated
void RespClient::wait_sim_end(){
    std::string message = this->encodeMessage("WAITEND");
    this->send(message);
    this->lastResponse = this->decodeMessage(this->receive());
    this->stopped = true;
}

///Returns the response message of the last issued command
std::string RespClient::getResponseMessage(){
    return this->lastResponse;
}

///Returns the response message of the last issued command as
///a vector, a C++ representation of a python list
std::vector<std::string> RespClient::getMexAsVector(){
    // TODO: implement this. I'm too lazy to do it now
    std::vector<std::string> message;
    return message;
}

///Given a message string it encodes it into hex numbers according to ReSP
///communication protocol
std::string RespClient::encodeMessage(std::string message){
    std::ostringstream buffer;
    buffer << '$';
    std::string::const_iterator mexIter, mexEnd;
    for(mexIter = message.begin(), mexEnd = message.end(); mexIter != mexEnd; mexIter++){
        buffer << std::hex << std::setw(2) << std::setfill('0') << (unsigned int)*mexIter;
    }
    buffer << '#';
    return buffer.str();
}

///Given a string expressed with hex numbers according to ReSP protocol, it decodes it
std::string RespClient::decodeMessage(std::string message){
    std::ostringstream buffer;
    #ifndef NDEBUG
    if(message.size() % 2 != 0)
        THROW_EXCEPTION("Error, in message " << message << " we expected the message length to be a multiple of 2");
    #endif
    for(unsigned int i = 0; i < (message.size() - 2)/2; i++){
         std::string temp = message.substr(i*2 + 1,  2);
         buffer << (char)(this->toIntNum(temp));
    }
    return buffer.str();
}

///Converts an hexadecimal number expressed with a string
///into its correspondent integer number
///each hex number of the string is in the same order of the endianess
///of the processor linked to this stub
unsigned int RespClient::toIntNum(std::string toConvert){
   if(toConvert.size() >= 2 && toConvert[0] == '0' && (toConvert[1] == 'X' || toConvert[1] == 'x'))
      toConvert = toConvert.substr(2);

   unsigned int result = 0;
   unsigned int pos = 0;
   std::string::reverse_iterator hexIter, hexIterEnd;
   for(hexIter = toConvert.rbegin(), hexIterEnd = toConvert.rend();
                  hexIter != hexIterEnd; hexIter++){
      std::map<char, unsigned int>::iterator mapIter = this->HexMap.find(*hexIter);
      #ifndef NDEBUG
      if(mapIter == this->HexMap.end()){
         std::cerr << __PRETTY_FUNCTION__ << ": bad hex convertion; trying to cast 0x" << toConvert << " current character -" << *hexIter << "-" << std::endl;
         return 0;
      }
      #endif
      result |= (mapIter->second << pos);
      pos += 4;
   }

   return result;
}

void RespClient::reconnect(){
    this->shutdown();
    ::sleep(1);
    this->connect();
}

unsigned int RespClient::getPid(){
    this->execute_command("os.getpid()");
    unsigned int pid;
    this->parseDecodedMessage(this->lastResponse.substr(2), pid);
    return pid;
}

unsigned long RespClient::getUsedMemory(){
    unsigned int respPythonPID = this->getPid();
    boost::filesystem::path fullPath = boost::filesystem::system_complete(boost::filesystem::path("/proc/" + boost::lexical_cast<std::string>(respPythonPID) + "/stat", boost::filesystem::native));
    if ( !boost::filesystem::exists( fullPath ) )
        THROW_EXCEPTION("Error in detrmining ReSP memory usage: /proc/" << respPythonPID << "/stat not found");
    std::ifstream infile(fullPath.string().c_str());
    //Now I have to read 22 dummy values; the 23rd is the one I need
    std::string dummyVal;
    unsigned long usedMem = 0;
    for(int i = 0; i < 22 && infile.good(); i++){
        infile >> dummyVal;
    }
    if(infile.good())
        infile >> usedMem;
    infile.close();
    return usedMem;
}

void RespClient::connect(){
    //std::cerr << "Reconnecting to ReSP" << std::endl;
    if(this->respPath != ""){
        //First of all I have to start ReSP; lets check it respPath is a valid path
        boost::filesystem::path fullPath = boost::filesystem::system_complete(boost::filesystem::path(this->respPath, boost::filesystem::native));
        if ( !boost::filesystem::exists( fullPath ) )
            THROW_EXCEPTION("Path " << fullPath.string() << " does not exists");
        if (!boost::filesystem::is_directory(fullPath))
            THROW_EXCEPTION(fullPath.string() << " is not a directory");
        if ( !boost::filesystem::exists( fullPath / "startSim.sh") )
            THROW_EXCEPTION("File " << fullPath / "startSim.sh" << " does not exists");

        // Now I make sure that ReSP can listen on the specified port
        //std::cerr << "Testing integrity of the connection" << std::endl;
        try{
            boost::asio::io_service temp_io_service;
            boost::asio::ip::tcp::acceptor acceptor(temp_io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));
            boost::asio::ip::tcp::socket tempSocket(temp_io_service);
            acceptor.listen(0);//(tempSocket);
        }
        catch(...){
            //std::cerr << "Connection integrity failed" << std::endl;
            THROW_EXCEPTION("Cannot create a ReSP instance listening on port " << port << " error in the TCP/IP socket");
        }

        //std::cerr << "Forking" << std::endl;
        this->respPid = ::fork();
        if ( this->respPid < 0 ) {
            THROW_EXCEPTION("fork() failed");
        }

        if ( this->respPid == 0 ) {
            //std::cerr << "Child: creating new console" << std::endl;
            if(::chdir("/") != 0)
                THROW_EXCEPTION("Errore while changing to the root folder");
            ::setsid();
            ::umask(0);

            if(this->graphic){
                ::execlp("xterm", "xterm",
                       "-bg", "black",
                       "-fg", "green",
                       "-geometry", "100x50",
                       "-e", ((fullPath / "startSim.sh").string() + " --silent -s " + boost::lexical_cast<std::string>(this->port)).c_str(),
                       NULL);
            }
            else{
                ::execlp((fullPath / "startSim.sh").string().c_str(), "resp.py",
                       "--silent",
                       "--no-color",
                       "-s", boost::lexical_cast<std::string>(this->port).c_str(),
                       NULL);
            }
            ::perror("execlp");
            ::kill(getppid(), SIGTERM);
            THROW_EXCEPTION("Error, execlp should have never returned");
        }
    }
    // Ok, now one way or another ReSP server is up: I perform the connection
    // note that, to take into account delays in firing up ReSP server
    // I try for 10 seconds to perform the connection; I signal a failure only
    // if the connection cannot be established after this time
    boost::asio::ip::tcp::resolver resolver(this->io_service);
    boost::system::error_code error = boost::asio::error::host_not_found;
    unsigned int numTries = 0;
    while(error && numTries < 10){
        boost::asio::ip::tcp::resolver::query query(hostname, boost::lexical_cast<std::string>(this->port));
        boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query, error);
        if (!error){
            this->socket = new boost::asio::ip::tcp::socket(io_service);
            boost::asio::ip::tcp::endpoint endpoint = *iterator;
            error = boost::asio::error::host_not_found;
            this->socket->connect(endpoint, error);
        }
        if(error)
            ::sleep(1);
        numTries++;
    }
    if (error){
        if(this->respPid > 0)
            ::kill(this->respPid, SIGTERM);
        THROW_EXCEPTION("Error in the connection with host " << this->hostname << " on port " << this->port << ". Error: " << error);
    }
}
