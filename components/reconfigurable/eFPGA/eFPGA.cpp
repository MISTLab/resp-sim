#include "eFPGA.hpp"

eFPGA::eFPGA(sc_module_name name, unsigned int w, unsigned int h, sc_time lw, float wa, float ca) :
				sc_module(name), name(name), latword(lw), wordarea(wa), cellarea(ca), tab(string(name),w,h),
				targetSocket((boost::lexical_cast<std::string>(name) + "_port").c_str()) {

	//I bind the port to the class (this) which contains the transport methods
	this->targetSocket.register_b_transport(this, &eFPGA::b_transport);
	end_module();

	//cerr << "eFPGA created with name " << name << ";"  << endl;
	//cerr << " - " << name << " has " << w << " X " << h << " cells of dimension;" << endl;
	//cerr << " - " << name << " has " << latword << " ns of latency per word; " << endl;
	//cerr << " - " << name << " has " << wordarea << " words per square mm;" << endl;
	//cerr << " - " << name << " has " << cellarea << " cells per square mm." << endl;
}

void eFPGA::printStatus() {tab.printStatus();}

void eFPGA::b_transport(tlm_generic_payload& message, sc_time& delay) {
	/*
	 * Data in the request is a string that concatenates 4 values:
	 * - funcAddr, the function address;
	 * - latency, the latency of the function, expressed in nanoseconds;
	 * - width, the function width, expressed in terms of cells;
	 * - height, the function height, expressed in terms of cells.
	 */
	unsigned char* received = message.get_data_ptr();
	payloadData* messageData = (payloadData*) received;
	unsigned int* retVal = (unsigned int*) received;
	unsigned int address = messageData->address;
	sc_time latency = messageData->latency;
	unsigned int funWidth = messageData->width;
	unsigned int funHeight = messageData->height;
	unsigned int answer = messageData->answer;

	unsigned int numWords, heur;
	sc_time howMuch;

	//cerr << name << " received: " << address << "\t" << latency << "\t" << funWidth << "\t" << funHeight << endl;
	message.set_response_status(TLM_COMMAND_ERROR_RESPONSE);

	/* Command W0	Allocates funcAddr, waiting for the appropriate latency, and sets the response with:
	 *		- (unsigned int)-1 if an error happened
	 *		- the number of words of the bitstream otherwise
	 */
	if ( message.get_command() == TLM_WRITE_COMMAND	&&	message.get_address() == 0 ) {
		if ( !tab.add(address, latency, funWidth, funHeight) ) message.set_response_status(TLM_GENERIC_ERROR_RESPONSE);
		else {
			numWords = (unsigned int) funWidth*funHeight * wordarea / cellarea;
			howMuch = latword * numWords;
			//cerr << name << ": using " << numWords << " words; configuration latency is " << howMuch << endl;
			messageData->answer = numWords;
			message.set_response_status(TLM_OK_RESPONSE);
			wait(howMuch);
		}
	}

	/* Command W1	Deletes funcAddr, sets the response with:
	 *		- (unsigned int)-1 if an error happened
	 *		- 1 otherwise
	 */
	if ( message.get_command() == TLM_WRITE_COMMAND	&&	message.get_address() == 1 ) {
		if ( !tab.remove(address) ) message.set_response_status(TLM_GENERIC_ERROR_RESPONSE);
		else message.set_response_status(TLM_OK_RESPONSE);
	}

	/* Command R0	Executes funcAddr, waiting for the appropriate latency, and sets the response with:
	 *		- (unsigned int)-1 if an error happened
	 *		- 1 otherwise
	 */
	if ( message.get_command() == TLM_READ_COMMAND	&&	message.get_address() == 0 ) {
		if ( !tab.execute(address,&howMuch) ) message.set_response_status(TLM_GENERIC_ERROR_RESPONSE);
		else {
			message.set_response_status(TLM_OK_RESPONSE);
			wait(howMuch);
		}
	}

	/* Command R1	Sets the response with:
	 *		- an heuristic (the lower the better) measuring the quality of a 'width' X 'height' allocation
	 *		- (unsigned int)-1 means that there is not enough space
	 */
	if ( message.get_command() == TLM_READ_COMMAND	&&	message.get_address() == 1 ) {
		heur = tab.searchForSpace(funWidth,funHeight);
		//cerr << name << ": the heuristic is: " << (int)heur << endl;
		messageData->answer = heur;
		message.set_response_status(TLM_OK_RESPONSE);
	}

	// Command R2:	Prints the fabric status
	if ( message.get_command() == TLM_READ_COMMAND	&&	message.get_address() == 2 ) {
		tab.printStatus();
		message.set_response_status(TLM_OK_RESPONSE);
	}
}
