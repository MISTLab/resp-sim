#ifndef RESPCLIENT_HPP
#define RESPCLIENT_HPP

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <vector>
#include <map>

#include "utils.hpp"

///This class implementes some facilities for the communication with ReSP
///through the socket server; in particular it redefines the API contained
///in server_api.py
class RespClient{
    private:
        ///Represents the currently open connection
        boost::asio::ip::tcp::socket * socket;
        boost::asio::io_service io_service;
        std::string hostname;
        ///Kepps track of the last response message
        std::string lastResponse;
        ///the PID of the process holding ReSP in case
        ///it has been started at the creation of the client
        int respPid;
        /// Indicates whether the simulation has been manually
        /// stopped using the APIs of ReSP Client
        bool stopped;
        /// Startup options: the path, whether to use or not the graphic
        /// configuration, the port and the hostname
        std::string respPath;
        bool graphic;
        int port;

        ///Sends a generic message to ReSP server
        void send(const std::string &message);
        ///Receives a generic message from ReSP server
        std::string receive();

        void connect();
        unsigned int getPid();

        /** Helper methods and variables used to encode and decode the messages in a format compatible with
            ReSP server**/
        std::string encodeMessage(std::string message);
        std::string decodeMessage(std::string message);
        unsigned int toIntNum(std::string toConvert);
        std::map<const char, unsigned int> HexMap;
        void initHexMap();

        template<class T> void parseDecodedMessage(const std::string &lastResponsePayload, T &readValue){
            switch(lastResponsePayload[0]){
                case 'S':
                case 'I':
                case 'G':
                case 'F':
                case 'B':
                    try{
                        readValue = boost::lexical_cast<T>(lastResponsePayload.substr(1,lastResponsePayload.size()-2));
                    }
                    catch(...){
                        THROW_EXCEPTION("Error in converting value -->" << lastResponsePayload << "<--");
                    }
                break;
                default:
                    try{
                        readValue = boost::lexical_cast<T>(lastResponsePayload);
                    }
                    catch(...){
                        THROW_EXCEPTION("Error in converting value --> " << lastResponsePayload << "<--");
                    }
                break;
            }
        }
    public:
        ///Initializes the client and performs the connection with the server; in case the
        ///server hasn't started yet, it is possible to specify the path of the ReSP startSim
        ///script: as a consequence the server will be started before attempting to establish
        ///the connection
        RespClient(unsigned int port, std::string hostname = "localhost", std::string respPath = "", bool graphic = true);
        ///When the client is killed, the connection with the server is also closed and
        ///the server terminated
        ~RespClient();

        void reconnect();

        /** Now here we have the methods necessary to actually communicate with the ReSP server**/
        ///Terminates the connection with ReSP
        void shutdown();

        ///Returns the ammount of memory used by ReSP
        unsigned long getUsedMemory();

        ///sends the reset message to ReSP
        ///returns the response
        bool reset();

        ///sends the quit message to ReSP
        ///returns the response
        bool quit();

        ///sends the list component message to ReSP
        ///returns the response
        bool list_components();

        ///sends the dir message to ReSP. The dir command is
        ///applied on item.
        ///returns the response
        bool dir_element(std::string item);

        ///creates an instance of an architectural component;
        ///the parameters of the constructor must be specified
        ///returns the response
        bool create_component(const std::string& compType, const std::string &name, const std::vector<std::string> & params);

        ///connects two components together using a TLM port
        ///returns the response
        bool connect_tlm(const std::string& initiator, const std::string& initiator_port, const std::string& target, const std::string& target_port);

        ///Connects two components together using systemc ports
        ///returns the response
        bool connect_sysc(const std::string& initiator, const std::string& initiator_port, const std::string& target, const std::string& target_port);

        ///Connects a systemc signal with a systemc port
        ///returns the response
        bool connect_sig(const std::string& signal_component, const std::string& signal, const std::string& target, const std::string& target_port);

        ///Loads an application into memory and initializes the processors
        ///returns the response
        bool load_application(const std::string & application);

        ///Loads a file describing the hardware architecture to be simulated; note that actually
        ///this file can contain any valid python directive
        ///returns the response
        bool load_architecture(const std::string & archFile);

        ///starts the simulation for the specified ammount of time
        ///in nanoseconds, returns the response
        bool run_simulation(double &time);

        ///undefinitely starts the simulation
        ///returns the response
        bool run_simulation();

        ///stops the simulation
        ///returns the response
        bool stop_simulation();

        ///returns SystemC time
        unsigned long get_systemc_time();

        ///returns real host time
        unsigned long  get_host_time();

        ///returns system energy use
        float get_energy();

        ///halts execution until simulation hasn't terminated
        void wait_sim_end();

        ///Calls a method in ReSP and returns the response; note that methodName
        ///must be the path of the method with respect to the global namespace
        template<class T> std::string callMethod(const std::string &methodName, T &returnValue,
                                const std::vector<std::string> &params = std::vector<std::string>()){
            std::string parameters = "";
            std::vector<std::string>::const_iterator paramIter, paramEnd;
            for(paramIter = params.begin(), paramEnd = params.end(); paramIter != paramEnd;){
                parameters += "'" + *paramIter + "'";
                paramIter++;
                if(paramIter != paramEnd)
                    parameters += ",";
            }
            std::string message = this->encodeMessage("EXEC-S" + methodName + "(" + parameters + "))");
            this->send(message);
            this->lastResponse = this->decodeMessage(this->receive());
            std::string lastResponsePayload = this->lastResponse.substr(2);
            if(this->lastResponse.find("OK") == 0){
                this->parseDecodedMessage(lastResponsePayload, returnValue);
                return "";
            }
            else{
                return lastResponsePayload;
            }
        }

        ///Registres a functor which is called when simulation ends
        template <class callback> void end_of_sim_callback(callback & cb){
            ///TODO: still to implement, see boost for a generic container
            ///in which register the callbacks
        }

        ///returns the value of the specifyed probe; note that probeName
        ///is the full path of the probe with respect to the global namespace
        template<class T> void get_probe_value(const std::string &probeName, T &readValue){
            std::string message = this->encodeMessage("EXEC-S" + probeName + ')');
            this->send(message);
            this->lastResponse = this->decodeMessage(this->receive());
            std::string lastResponsePayload = this->lastResponse.substr(2);
            if(this->lastResponse.find("OK") == 0){
                this->parseDecodedMessage(lastResponsePayload, readValue);
            }
            else
                THROW_EXCEPTION("Error occurred during read of probe " << probeName << ". Error: " << lastResponsePayload);
        }

        ///Sets the value of a variable; note that the variable name should include
        ///the path of the variable with respect to the global namespace.
        ///returns true if the operation was successful
        bool set_variable_value(const std::string &varName, const std::string &varValue);

        ///Executes a custom python statement
        ///returns the response
        bool execute_command(const std::string &command);

        ///Returns the response message of the last issued command
        std::string getResponseMessage();

        ///Returns the response message of the last issued command as
        ///a vector, a C++ representation of a python list
        std::vector<std::string> getMexAsVector();
};

#endif
