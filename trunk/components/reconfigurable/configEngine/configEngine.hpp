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

/***************************************************************************\
 *   Configuration Engine, performs reconfiguraion on the eFpgas attached
 *   using TLM ports. Also used to catch calls to reconfigured functions.
 *   Can be also done using Python. See /architectures/test/test_reconfig.py
 *   for an example.
\***************************************************************************/

#ifndef  _CONFENG_H
#define  _CONFENG_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <list>

#include "../payloadData.hpp"
#include "cEAllocationTable.hpp"

#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/multi_passthrough_initiator_socket.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/tlm_quantumkeeper.h>
#include <boost/lexical_cast.hpp>
#include <trap_utils.hpp>

using namespace std;
using namespace trap;
using namespace tlm;
using namespace tlm_utils;

class configEngine: public sc_module {

private:
	sc_dt::uint64 bitstream_source_address;
	map<unsigned int, sc_dt::uint64> bitstream_dest_address;
	unsigned int lastBinding;
	sc_time requestDelay, execDelay, configDelay, removeDelay;

	// The centralized map of the configured functions
	cEAllocationTable tab;

	// Dealing with multiple parallel configuration accesses
	unsigned int curConfigEvent;
	map<unsigned int,sc_event *> wakeConfigEvents;
	queue<unsigned int> nextConfigWake;
	bool configBusy;
	void configLock();
	void configUnlock();

	// Dealing with multiple parallel module executions
	unsigned int curExecEvent;
	map<unsigned int,sc_event *> wakeExecEvents;
	map<unsigned int, queue<unsigned int> > nextExecWake;
	list<unsigned int> executing;
	void execLock(unsigned int address);
	void execUnlock(unsigned int address);

	// Internal utility function for module configuration. No configuration locks are taken inside this routine
	unsigned int config(string funcName, sc_time latency, unsigned int width, unsigned int height, bool reUse);

	// Internal utility function for module execution. No execution locks are taken inside this routine
	bool exec(unsigned int funcAddr);

//	int status;

public:
//	void printStatus() {cout << "Configuration Engine Status: " << status << endl;}
//	void changeStatus(int nw) {status = nw;}

	// This is the port communicating with the devices
	multi_passthrough_initiator_socket< configEngine, 32 > initiatorSocket;

	// This is the port attached to the system bitstream RAM (which can be attached to an interconnection such as a bus)
	simple_initiator_socket < configEngine, 32 > ramSocket;

	// This is the port attached to bitstream sinks of the devices (which can be attached to an interconnection such as a bus)
	multi_passthrough_initiator_socket< configEngine, 32 > destSocket;

	/*
	 * Constructor:
	 * - 'bitstreamSource' specifies the source address for the bitstreams.
	 * - 'delAlg' specifies the type of deletion algorithm to be used if a
	 * configuration is required and there is no more space on the fabrics.
	 / 
	 */
	configEngine(sc_module_name name, sc_dt::uint64 bitstreamSource, deletionAlgorithm delAlg = LRU);

	/*
	 * Binds an address to a new device. This function is incremental, it starts from device 1 and each time automatically
	 * refers to a new eFPGA. It returns the ID number of the device bound to the given address.
	 */
	unsigned int bindFPGA(sc_dt::uint64 dest_addr);

	/* 
	 * Configures a fabric with function 'funcName', which has the given 'latency' and occupies 'width' X 'height' logic cells;
	 * furthermore, latency and bus occupation required by the (eventual) configuration are managed by this method.
	 * Returns the address at which the function is mapped. If some error happens, reserved address 0 is returned.
	 * If 'reUse' is activated, the address of a previously allocated function with the same name (if exists) is
	 * returned and no new functions are configured.
	 * This function is thread-safe, and can be accessed only by one thread at a time.
	 */
	unsigned int configure(string funcName, sc_time latency, unsigned int width, unsigned int height, bool reUse = true);

	/*
	 * Executes the function at the address 'funcAddr' on the fabric and waits for the appropriate latency. Returns FALSE if
	 * the address is empty or some error happens. TRUE elsewhere.
	 * This function is thread-safe, and the function at a given address can be executed only by one thread at a time.
	 */
	bool execute(unsigned int funcAddr);

	/*
	 * Executes the first free function with name 'funcName' on the fabric and waits for the appropriate latency. Returns FALSE
	 * if the required function doesn't exists or some error happens. TRUE elsewhere.
	 * This function is thread-safe, and every instance of the required function can be executed only by one thread at a time.
	 */
	bool execute(string funcName);

	/*
	 * Configures and executes function 'funcName', which has the given 'latency' and occupies 'width' X 'height' logic cells;
	 * furthermore, latency and bus occupation required by the (eventual) configuration are managed by this method.
	 * If 'reUse' is activated, a previously allocated function with the same name (if exists) is executed and no new functions
	 * are configured.
	 * This function is thread-safe, and can be accessed only by one thread at a time. Furthermore, it is guaranteed that the
	 * (potentially) configured function is executed at least one time before it is removed from the fabric.
	 */
	void executeForce(string funcName, sc_time latency, unsigned int width, unsigned int height, bool reUse = true);

	/*
	 * Removes the function at the address 'funcAddr' from the fabric. Returns FALSE if the address is empty or some error
	 * happens. TRUE elsewhere. This function is thread-safe: the function is not removed until its execution is terminated.
	 */
	bool manualRemove(unsigned int funcAddr);
	/*
	 * Removes a function with name 'funcName' from the fabric. Returns FALSE if the name doesn't exists is empty or some error
	 * happens. TRUE elsewhere. This function is thread-safe: a function is not removed until its execution is terminated.
	 */
	bool manualRemove(string funcName);

	/*
	 * Prints on the standard output the internal status of the entire system.
	 */
	void printSystemStatus();

	/*
	 * Setters for the internal fixed latencies (initialized to 0)
	 */
	void setRequestDelay(sc_time new_time);
	void setExecDelay(sc_time new_time);
	void setConfigDelay(sc_time new_time);
	void setRemoveDelay(sc_time new_time);

};

#endif  //_CONFENG_H
