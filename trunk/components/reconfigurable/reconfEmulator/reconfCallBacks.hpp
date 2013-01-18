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

#include "reconfEmulator.hpp"
#include "edgeCallBacks.hpp"
#include "sdrCallBacks.hpp"

namespace reconfEmu {

/*
 * template<class issueWidth> class __EXAMPLE__Call : public reconfCB<issueWidth>{
 * private:
 * 	configEngine* cE;
 * public:
 * 	__EXAMPLE__Call(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
 * 		 reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
 * 	bool operator()(ABIIf<issueWidth> &processorInstance){
 * 		processorInstance.preCall();
 * 		std::vector< issueWidth > callArgs = processorInstance.readArgs();
 * 
 * 		Arguments Manipulation Code
 * 
 * 		processorInstance.setRetVal(__RETURN_VALUE__);
 * 		processorInstance.returnFromCall();
 * 		processorInstance.postCall();
 * 		return true;
 * 	}
 * };
 */

template <typename issueWidth> class printValueCall : public reconfCB <issueWidth>{
private:
	configEngine* cE;
	map<unsigned int, bool> configured;
public:
	printValueCall(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
		reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
	bool operator()(ABIIf<issueWidth> &processorInstance){
		processorInstance.preCall();
		std::vector< issueWidth > callArgs = processorInstance.readArgs();

		(this->cE)->executeForce("printValue", this->latency, this->width, this->height, configured[processorInstance.getProcessorID()]);
		configured[processorInstance.getProcessorID()] = true;
//		unsigned int address = (this->cE)->configure("printValue", this->latency, this->width, this->height, true);
//		(this->cE)->manualRemove("printValue");
//		(this->cE)->execute(address);

		(this->cE)->printSystemStatus();

		unsigned int param1 = callArgs[0];
		cout << "(CppCall) Value: " << param1 << endl;
//		cout << "Width: " << this->width << "\tHeight: " << this->height << "\t Latency: " << this->latency.to_string() << endl;

		processorInstance.setRetVal(param1 + 1);
		processorInstance.returnFromCall();
		processorInstance.postCall();
		return true;
	}
};

template<typename issueWidth> class sumCall : public reconfCB<issueWidth>{
private:
	configEngine* cE;
public:
	sumCall(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
		reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
	bool operator()(ABIIf<issueWidth> &processorInstance){
		processorInstance.preCall();
		std::vector< issueWidth > callArgs = processorInstance.readArgs();

		(this->cE)->executeForce("sum", this->latency, this->width, this->height, false);
		(this->cE)->printSystemStatus();

		cout << "(CppCall) Summing values..." << endl;
//		cout << "Width: " << this->width << "\tHeight: " << this->height << "\t Latency: " << this->latency.to_string() << endl;
		unsigned int a = processorInstance.readMem(callArgs[0]);
		unsigned int b = processorInstance.readMem(callArgs[1]);
		unsigned int c = a + b;

		processorInstance.setRetVal(c);
		processorInstance.returnFromCall();
		processorInstance.postCall();
		return true;
	}
};

template<typename issueWidth> class generateCall : public reconfCB<issueWidth>{
private:
	configEngine* cE;
public:
	generateCall(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
		reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
	bool operator()(ABIIf<issueWidth> &processorInstance){
		processorInstance.preCall();
		std::vector< issueWidth > callArgs = processorInstance.readArgs();

		(this->cE)->executeForce("generate", this->latency, this->width, this->height, false);
		(this->cE)->printSystemStatus();

		cout << "(CppCall) Generating value..." << endl;
//		cout << "Width: " << this->width << "\tHeight: " << this->height << "\t Latency: " << this->latency.to_string() << endl;
		unsigned int tmp = processorInstance.readMem(callArgs[0]);
		tmp = 20;
		processorInstance.writeMem(callArgs[0],tmp);

		processorInstance.setRetVal(0);
		processorInstance.returnFromCall();
		processorInstance.postCall();
		return true;
	}
};

template<typename issueWidth> class putsCall : public reconfCB<issueWidth>{
private:
	configEngine* cE;
public:
	putsCall(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
		reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
	bool operator()(ABIIf<issueWidth> &processorInstance){
		processorInstance.preCall();
		std::vector< issueWidth > callArgs = processorInstance.readArgs();

		unsigned char* inputText = (unsigned char*) malloc(512*sizeof(unsigned char));
		int k;
		for (k=0; k<512; k++) {
			inputText[k] = processorInstance.readCharMem(callArgs[0]+k);
		}

		cout << inputText << endl;

		processorInstance.setRetVal(0);
		processorInstance.returnFromCall();
		processorInstance.postCall();
		return true;
	}
};

template<typename issueWidth> void reconfEmulator<issueWidth>::registerCppCall(string funName, sc_time latency, unsigned int w, unsigned int h) {

/*	if (funName=="__EXAMPLE__") {
 *		__EXAMPLE__Call<issueWidth> *rcb = NULL;
 *		rcb = new __EXAMPLE__Call<issueWidth>(cE,latency,w,h);
 *		if (!(this->register_call(funName, *rcb))) delete rcb;
 *	}
 */
	if (funName=="printValue") {
		printValueCall<issueWidth> *rcb = NULL;
		rcb = new printValueCall<issueWidth>(cE,latency,w,h);
		if (!(this->register_call(funName, *rcb))) delete rcb;
	}
	if (funName=="sum") {
		sumCall<issueWidth> *rcb = NULL;
		rcb = new sumCall<issueWidth>(cE,latency,w,h);
		if (!(this->register_call(funName, *rcb))) delete rcb;
	}
	if (funName=="generate") {
		generateCall<issueWidth> *rcb = NULL;
		rcb = new generateCall<issueWidth>(cE,latency,w,h);
		if (!(this->register_call(funName, *rcb))) delete rcb;
	}

	// Edge Detector
	if (funName=="read_bitmap") {
		read_bitmapCall<issueWidth> *rcb = NULL;
		rcb = new read_bitmapCall<issueWidth>(cE,latency,w,h);
		if (!(this->register_call(funName, *rcb))) delete rcb;
	}
	if (funName=="write_bitmap") {
		write_bitmapCall<issueWidth> *rcb = NULL;
		rcb = new write_bitmapCall<issueWidth>(cE,latency,w,h);
		if (!(this->register_call(funName, *rcb))) delete rcb;
	}
	if (funName=="puts") {
		putsCall<issueWidth> *rcb = NULL;
		rcb = new putsCall<issueWidth>(cE,latency,w,h);
		if (!(this->register_call(funName, *rcb))) delete rcb;
	}
	if (funName=="rgb2grey") {
		rgb2greyCall<issueWidth> *rcb = NULL;
		rcb = new rgb2greyCall<issueWidth>(cE,latency,w,h);
		if (!(this->register_call(funName, *rcb))) delete rcb;
	}
	if (funName=="edgeOverlapping") {
		edgeOverlappingCall<issueWidth> *rcb = NULL;
		rcb = new edgeOverlappingCall<issueWidth>(cE,latency,w,h);
		if (!(this->register_call(funName, *rcb))) delete rcb;
	}
	if (funName=="edgeDetectionSinglePixel") {
		edgeDetectionSinglePixelCall<issueWidth> *rcb = NULL;
		rcb = new edgeDetectionSinglePixelCall<issueWidth>(cE,latency,w,h);
		if (!(this->register_call(funName, *rcb))) delete rcb;
	}

	// SDR
	if (funName=="openWavToRead16") {
		openWavToRead16Call<issueWidth> *rcb = NULL;
		rcb = new openWavToRead16Call<issueWidth>(cE,latency,w,h);
		if (!(this->register_call(funName, *rcb))) delete rcb;
	}
	if (funName=="openWavToWrite16") {
		openWavToWrite16Call<issueWidth> *rcb = NULL;
		rcb = new openWavToWrite16Call<issueWidth>(cE,latency,w,h);
		if (!(this->register_call(funName, *rcb))) delete rcb;
	}
	if (funName=="readSample16") {
		readSample16Call<issueWidth> *rcb = NULL;
		rcb = new readSample16Call<issueWidth>(cE,latency,w,h);
		if (!(this->register_call(funName, *rcb))) delete rcb;
	}
	if (funName=="writeSample16") {
		writeSample16Call<issueWidth> *rcb = NULL;
		rcb = new writeSample16Call<issueWidth>(cE,latency,w,h);
		if (!(this->register_call(funName, *rcb))) delete rcb;
	}
	if (funName=="seekFirstSample16") {
		seekFirstSample16Call<issueWidth> *rcb = NULL;
		rcb = new seekFirstSample16Call<issueWidth>(cE,latency,w,h);
		if (!(this->register_call(funName, *rcb))) delete rcb;
	}
	if (funName=="closeWav16") {
		closeWav16Call<issueWidth> *rcb = NULL;
		rcb = new closeWav16Call<issueWidth>(cE,latency,w,h);
		if (!(this->register_call(funName, *rcb))) delete rcb;
	}
	if (funName=="printout") {
		printoutCall<issueWidth> *rcb = NULL;
		rcb = new printoutCall<issueWidth>(cE,latency,w,h);
		if (!(this->register_call(funName, *rcb))) delete rcb;
	}
	if (funName=="printresult") {
		printresultCall<issueWidth> *rcb = NULL;
		rcb = new printresultCall<issueWidth>(cE,latency,w,h);
		if (!(this->register_call(funName, *rcb))) delete rcb;
	}
	if (funName=="printStatus") {
		printStatusCall<issueWidth> *rcb = NULL;
		rcb = new printStatusCall<issueWidth>(cE,latency,w,h);
		if (!(this->register_call(funName, *rcb))) delete rcb;
	}
	if (funName=="hcMixer") {
		hcMixerCall<issueWidth> *rcb = NULL;
		rcb = new hcMixerCall<issueWidth>(cE,latency,w,h);
		if (!(this->register_call(funName, *rcb))) delete rcb;
	}
	if (funName=="decimatorFilter") {
		decimatorFilterCall<issueWidth> *rcb = NULL;
		rcb = new decimatorFilterCall<issueWidth>(cE,latency,w,h);
		if (!(this->register_call(funName, *rcb))) delete rcb;
	}
	if (funName=="CW300Filter") {
		CW300FilterCall<issueWidth> *rcb = NULL;
		rcb = new CW300FilterCall<issueWidth>(cE,latency,w,h);
		if (!(this->register_call(funName, *rcb))) delete rcb;
	}
	if (funName=="CW1KFilter") {
		CW1KFilterCall<issueWidth> *rcb = NULL;
		rcb = new CW1KFilterCall<issueWidth>(cE,latency,w,h);
		if (!(this->register_call(funName, *rcb))) delete rcb;
	}
	if (funName=="SSB2K4Filter") {
		SSB2K4FilterCall<issueWidth> *rcb = NULL;
		rcb = new SSB2K4FilterCall<issueWidth>(cE,latency,w,h);
		if (!(this->register_call(funName, *rcb))) delete rcb;
	}
	if (funName=="interpolatorFilter") {
		interpolatorFilterCall<issueWidth> *rcb = NULL;
		rcb = new interpolatorFilterCall<issueWidth>(cE,latency,w,h);
		if (!(this->register_call(funName, *rcb))) delete rcb;
	}
	if (funName=="rectify") {
		rectifyCall<issueWidth> *rcb = NULL;
		rcb = new rectifyCall<issueWidth>(cE,latency,w,h);
		if (!(this->register_call(funName, *rcb))) delete rcb;
	}
	if (funName=="filter") {
		filterCall<issueWidth> *rcb = NULL;
		rcb = new filterCall<issueWidth>(cE,latency,w,h);
		if (!(this->register_call(funName, *rcb))) delete rcb;
	}
	if (funName=="average") {
		averageCall<issueWidth> *rcb = NULL;
		rcb = new averageCall<issueWidth>(cE,latency,w,h);
		if (!(this->register_call(funName, *rcb))) delete rcb;
	}
	if (funName=="decode") {
		decodeCall<issueWidth> *rcb = NULL;
		rcb = new decodeCall<issueWidth>(cE,latency,w,h);
		if (!(this->register_call(funName, *rcb))) delete rcb;
	}
	if (funName=="posavg") {
		posavgCall<issueWidth> *rcb = NULL;
		rcb = new posavgCall<issueWidth>(cE,latency,w,h);
		if (!(this->register_call(funName, *rcb))) delete rcb;
	}
	if (funName=="negavg") {
		negavgCall<issueWidth> *rcb = NULL;
		rcb = new negavgCall<issueWidth>(cE,latency,w,h);
		if (!(this->register_call(funName, *rcb))) delete rcb;
	}
	if (funName=="decodeSymbol") {
		decodeSymbolCall<issueWidth> *rcb = NULL;
		rcb = new decodeSymbolCall<issueWidth>(cE,latency,w,h);
		if (!(this->register_call(funName, *rcb))) delete rcb;
	}
	if (funName=="normalize") {
		normalizeCall<issueWidth> *rcb = NULL;
		rcb = new normalizeCall<issueWidth>(cE,latency,w,h);
		if (!(this->register_call(funName, *rcb))) delete rcb;
	}
}

};
