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

template<typename issueWidth> class read_bitmapCall : public reconfCB<issueWidth>{
private:
	configEngine* cE;
public:
	read_bitmapCall(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
		reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
	bool operator()(ABIIf<issueWidth> &processorInstance){
		processorInstance.preCall();
		std::vector< issueWidth > callArgs = processorInstance.readArgs();

		(this->cE)->executeForce("read_bitmap", this->latency, this->width, this->height, false);
//		(this->cE)->printSystemStatus();

		int k;
		char * input_file_name = (char*) malloc(512*sizeof(char));
		unsigned char * bitmap_values = (unsigned char*) malloc(76800*sizeof(unsigned char));
		for (k=0; k<500; k++) {
			input_file_name[k] = processorInstance.readMem(callArgs[0]+k);
		}
		cout << input_file_name << endl;
		typedef struct {
			unsigned int LunghezzaFile;
			unsigned int RiservatoPre;
			unsigned int Offset;
			unsigned int DimensioneHeader;
			unsigned int Larghezza;
			unsigned int Altezza;
			unsigned short Piani;
			unsigned short BitPerPixel;
			unsigned int Compressione;
			unsigned int DimensioneImmagine;
			unsigned int RisoluzioneOrizzontale;
			unsigned int RisoluzioneVerticale;
			unsigned int ColoriUsati;
			unsigned int ColoriImportanti;
			unsigned char palette[256 * 3];
		} BITMAP_HEADER;
		FILE 			* fp; // file pointer
		BITMAP_HEADER 	BH; // bitmap header
		char			output_flag = -1; // output flag
		// bitmap properties
		long int		bitmap_depth,
					bitmap_width,
					bitmap_height;
		// bitmap palette and values
		long int 		row_bytes_filler;
		// scanning
		long int		j,
					i;
		long int		coord_row,
					coord_pixel;
		// open the file
		fp = fopen(input_file_name, "rb");
		// check: does the file exist?
		if (fp != NULL) {
		    output_flag++;
		    // Check: is the opened file a valid bitmap file?
		    if (((fgetc(fp)) == 'B') && ((fgetc(fp)) == 'M')) {
			output_flag++;
		    	// read the bitmap header, without palette
			fread(&BH, ((sizeof(BITMAP_HEADER)) - (256 * 3)), 1, fp);
			// get the bitmap properties
			bitmap_depth	= ((long int)	((BH.BitPerPixel) / 8));
			bitmap_width 	= ((long int)	(BH.Larghezza));
			bitmap_height 	= ((long int)	(BH.Altezza));
			/*---Read palette if the bitmap is 8-bits grey level---*/
			if (bitmap_depth < 2) {		
				for(j = 0; j < ((long int)(BH.ColoriUsati)); j++) {
					fread((BH.palette) + (j * 3), 1, 3, fp);  // (R,G,B) mapping for index_2 grey level
					((void)(fgetc(fp))); // filler
				}
			}
			/*---Find row-bytes filler (in a bitmap the number of bytes per row is a multiple of 4)---*/
			row_bytes_filler = ((long int)((bitmap_width * bitmap_depth)%(4))); // row_filler = 0, 1, 2 or 3;
			if (row_bytes_filler) {
				row_bytes_filler = (4 - row_bytes_filler); // row_bytes_filler = 0, 3, 2 or 1
				/*---Read bitmap values---*/
				if (bitmap_depth < 2) { // grey level
					for ((j = (bitmap_height - 1)); (j > (-1)); (j--)) {
						/*---Read a row---*/
						coord_row = (bitmap_width * j);
						for ((i = 0); (i < bitmap_width); (i++))
						{
							(*(bitmap_values + coord_row + i)) = ((unsigned char)(fgetc(fp))); // grey level	
						}
						/*---Read row-bytes filler---*/
						for (i = 0; i < row_bytes_filler; i++)
						{
							((void)(fgetc(fp)));
						}
					}
				}
				else  { // RGB
					for ((j = (bitmap_height - 1)); (j > (-1)); (j--)) {
						/*---Read a row---*/
						coord_row = (bitmap_width * j);
						for ((i = 0); (i < bitmap_width); (i++)) {
							coord_pixel = (3 * (coord_row + i));
							(*(bitmap_values + coord_pixel + 2)) 	= ((unsigned char)(fgetc(fp))); // B
							(*(bitmap_values + coord_pixel + 1))	= ((unsigned char)(fgetc(fp))); // G
							(*(bitmap_values + coord_pixel)) 		= ((unsigned char)(fgetc(fp))); // R
						}
						/*---Read row-bytes filler---*/
						for (i = 0; i < row_bytes_filler; i++) {
							((void)(fgetc(fp)));
						}
					}
				}
			}
			else { /*---Read bitmap values---*/
				if (bitmap_depth < 2) { // grey level
					for ((j = (bitmap_height - 1)); (j > (-1)); (j--)) {
						/*---Read a row---*/
						coord_row = (bitmap_width * j);
						for ((i = 0); (i < bitmap_width); (i++)) {
							(*(bitmap_values + coord_row + i)) = ((unsigned char)(fgetc(fp))); // grey level	
						}
					}
				}
				else { // RGB
					for ((j = (bitmap_height - 1)); (j > (-1)); (j--)) {
						/*---Read a row---*/
						coord_row = (bitmap_width * j);
						for ((i = 0); (i < bitmap_width); (i++)) {
							coord_pixel = (3 * (coord_row + i));
							(*(bitmap_values + coord_pixel + 2))	= ((unsigned char)(fgetc(fp))); // B
							(*(bitmap_values + coord_pixel + 1))	= ((unsigned char)(fgetc(fp))); // G
							(*(bitmap_values + coord_pixel))		= ((unsigned char)(fgetc(fp))); // R
						}
					}
				}
			}
			// close the file
			fclose(fp);
			// set the output flag
			output_flag = 1;
		    }
		    else {
			fclose(fp);
		    }
		}
		for (k=0; k<76800; k++) {
			processorInstance.writeMem(callArgs[1]+k,*(bitmap_values+k));
		}

		processorInstance.setRetVal(output_flag);
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

	if (funName=="read_bitmap") {
		read_bitmapCall<issueWidth> *rcb = NULL;
		rcb = new read_bitmapCall<issueWidth>(cE,latency,w,h);
		if (!(this->register_call(funName, *rcb))) delete rcb;
	}
}

};
