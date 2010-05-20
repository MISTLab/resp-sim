/***************************************************************************\
 *
 *
 *         ___           ___           ___           ___
 *        /  /\         /  /\         /  /\         /  /\
 *       /  /::\       /  /:/_       /  /:/_       /  /::\
 *      /  /:/\:\     /  /:/ /\     /  /:/ /\     /  /:/\:\
 *     /  /:/~/:/    /  /:/ /:/_   /  /:/ /::\   /  /:/~/:/
 *    /__/:/ /:/___ /__/:/ /:/ /\ /__/:/ /:/\:\ /__/:/ /:/
 *    \  \:\/:::::/ \  \:\/:/ /:/ \  \:\/:/~/:/ \  \:\/:/
 *     \  \::/~~~~   \  \::/ /:/   \  \::/ /:/   \  \::/
 *      \  \:\        \  \:\/:/     \__\/ /:/     \  \:\
 *       \  \:\        \  \::/        /__/:/       \  \:\
 *        \__\/         \__\/         \__\/         \__\/
 *
 *
 *
 *
 *   This file is part of ReSP.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   The following code is derived, directly or indirectly, from the SystemC
 *   source code Copyright (c) 1996-2004 by all Contributors.
 *   All Rights reserved.
 *
 *   The contents of this file are subject to the restrictions and limitations
 *   set forth in the SystemC Open Source License Version 2.4 (the "License");
 *   You may not use this file except in compliance with such restrictions and
 *   limitations. You may obtain instructions on how to receive a copy of the
 *   License at http://www.systemc.org/. Software distributed by Contributors
 *   under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 *   ANY KIND, either express or implied. See the License for the specific
 *   language governing rights and limitations under the License.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *   or see <http://www.gnu.org/licenses/>.
 *
 *
 *
 *   (c) Giovanni Beltrame, Luca Fossati
 *       Giovanni.Beltrame@esa.int fossati@elet.polimi.it
 *
 *   Component developed by: Fabio Arlati arlati.fabio@gmail.com
 *
\***************************************************************************/

#include "eFPGA.hpp"

eFPGA::eFPGA(sc_module_name name, unsigned int w, unsigned int h, sc_time lw, float wa, float ca) :
				sc_module(name), latword(lw), wordarea(wa), cellarea(ca), tab(string(name),w,h),
				bS((boost::lexical_cast<std::string>(name) + "_bS").c_str(),lw),
				targetSocket((boost::lexical_cast<std::string>(name) + "_targSock").c_str()) {

	//I bind the port to the class (this) which contains the transport methods
	this->targetSocket.register_b_transport(this, &eFPGA::b_transport);
	end_module();

	#ifdef DEBUGMODE
	cerr << "eFPGA created with name " << this->name() << ";"  << endl;
	cerr << " - " << this->name() << " has " << w << " X " << h << " cells of dimension;" << endl;
	cerr << " - " << this->name() << " has " << latword << " ns of latency per word; " << endl;
	cerr << " - " << this->name() << " has " << wordarea << " words per square mm;" << endl;
	cerr << " - " << this->name() << " has " << cellarea << " cells per square mm." << endl;
	#endif
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
	sc_dt::uint64 address = messageData->address;
	sc_time latency = messageData->latency;
	unsigned int funWidth = messageData->width;
	unsigned int funHeight = messageData->height;
	unsigned int answer = messageData->answer;

	unsigned int numWords, heur;
	sc_time howMuch;

	#ifdef DEBUGMODE
	cerr << sc_time_stamp() << ": " << name() << " - Received " << address << "\t" << latency << "\t" << funWidth << "\t" << funHeight << endl;
	#endif
	message.set_response_status(TLM_COMMAND_ERROR_RESPONSE);

	/* Command W0	Allocates funcAddr, waiting for the appropriate latency, and sets the response with:
	 *		- (unsigned int)-1 if an error happened
	 *		- the number of words of the bitstream otherwise
	 */
	if ( message.get_command() == TLM_WRITE_COMMAND	&&	message.get_address() == 0 ) {
		if ( !tab.add(address, latency, funWidth, funHeight) ) message.set_response_status(TLM_GENERIC_ERROR_RESPONSE);
		else {
			numWords = (unsigned int) funWidth*funHeight * wordarea / cellarea;
			#ifdef DEBUGMODE
			cerr << sc_time_stamp() << ": " << name() << " - Configuring " << address << " using " << numWords << " words" << endl;
			#endif
			messageData->answer = numWords;
			message.set_response_status(TLM_OK_RESPONSE);
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
		#ifdef DEBUGMODE
		cerr << sc_time_stamp() << ": " << name() << " - The heuristic is: " << (int)heur << endl;
		#endif
		messageData->answer = heur;
		message.set_response_status(TLM_OK_RESPONSE);
	}

	// Command R2:	Prints the fabric status
	if ( message.get_command() == TLM_READ_COMMAND	&&	message.get_address() == 2 ) {
		tab.printStatus();
		message.set_response_status(TLM_OK_RESPONSE);
	}
}
