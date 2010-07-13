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

template<typename issueWidth> class read_bitmapCall : public reconfCB<issueWidth>{
private:
	configEngine* cE;
public:
	read_bitmapCall(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
		reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
	bool operator()(ABIIf<issueWidth> &processorInstance){
		processorInstance.preCall();
		std::vector< issueWidth > callArgs = processorInstance.readArgs();

//		(this->cE)->executeForce("read_bitmap", this->latency, this->width, this->height, false);
//		(this->cE)->printSystemStatus();

		int k;
		char * input_file_name = (char*) malloc(512*sizeof(char));
		k=0;
		do{
			input_file_name[k] = processorInstance.readCharMem(callArgs[0]+k);
			k++;
		} while(input_file_name[k-1] != '\0' && k < 512);
		
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
		FILE 			*fp; // file pointer
		BITMAP_HEADER		BH; // bitmap header
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
        
		int imageLength=0;
		unsigned char * bitmap_values = NULL;

		// open the file
		fp = fopen(input_file_name, "rb");
		// check: does the file exist?
		if (fp != NULL) {
		  output_flag++;
		  // Check: is the opened file a valid bitmap file?
		  if (((fgetc(fp)) == 'B') && ((fgetc(fp)) == 'M')) {
		    output_flag++;
		    // read the bitmap header, without palette
		    if (fread(&BH, ((sizeof(BITMAP_HEADER)) - (256 * 3)), 1, fp) != 1) THROW_EXCEPTION("error while opening image file");
		    // get the bitmap properties
		    bitmap_depth	= ((long int)	((BH.BitPerPixel) / 8));
		    bitmap_width 	= ((long int)	(BH.Larghezza));
		    bitmap_height 	= ((long int)	(BH.Altezza));
		    /*---Read palette if the bitmap is 8-bits grey level---*/
		    if (bitmap_depth < 2) {		
		      for(j = 0; j < ((long int)(BH.ColoriUsati)); j++) {
		        // (R,G,B) mapping for index_2 grey level
		        if (fread((BH.palette) + (j * 3), 1, 3, fp) != 3) THROW_EXCEPTION("error while opening image file");
		        ((void)(fgetc(fp))); // filler
		      }
		    }

		    imageLength = bitmap_width * bitmap_height * bitmap_depth;
		    bitmap_values = (unsigned char*) malloc(imageLength*sizeof(unsigned char));
		  	
		    /*---Find row-bytes filler (in a bitmap the number of bytes per row is a multiple of 4)---*/
		    row_bytes_filler = ((long int)((bitmap_width * bitmap_depth)%(4))); // row_filler = 0, 1, 2 or 3;
		    if (row_bytes_filler) {
		      row_bytes_filler = (4 - row_bytes_filler); // row_bytes_filler = 0, 3, 2 or 1
		      /*---Read bitmap values---*/
		      if (bitmap_depth < 2) { // grey level
		        for ((j = (bitmap_height - 1)); (j > (-1)); (j--)) {
		          /*---Read a row---*/
		          coord_row = (bitmap_width * j);
		          for ((i = 0); (i < bitmap_width); (i++)) {
		            (*(bitmap_values + coord_row + i)) = ((unsigned char)(fgetc(fp))); // grey level	
		          }
		          /*---Read row-bytes filler---*/
		          for (i = 0; i < row_bytes_filler; i++) {
		            ((void)(fgetc(fp)));
		          }
		        }
		      } else { // RGB
		        for ((j = (bitmap_height - 1)); (j > (-1)); (j--)) {
		          /*---Read a row---*/
		          coord_row = (bitmap_width * j);
		          for ((i = 0); (i < bitmap_width); (i++)) {
		            coord_pixel = (3 * (coord_row + i));
		            (*(bitmap_values + coord_pixel + 2)) = ((unsigned char)(fgetc(fp))); // B
		            (*(bitmap_values + coord_pixel + 1)) = ((unsigned char)(fgetc(fp))); // G
		            (*(bitmap_values + coord_pixel)) = ((unsigned char)(fgetc(fp))); // R
		          }
		          /*---Read row-bytes filler---*/
		          for (i = 0; i < row_bytes_filler; i++) {
		            ((void)(fgetc(fp)));
		          }
		        }
		      }
		    } else { /*---Read bitmap values---*/
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
		            (*(bitmap_values + coord_pixel + 2)) = ((unsigned char)(fgetc(fp))); // B
		            (*(bitmap_values + coord_pixel + 1)) = ((unsigned char)(fgetc(fp))); // G
		            (*(bitmap_values + coord_pixel)) = ((unsigned char)(fgetc(fp))); // R
		          }
		        }
		      }
		    }
		    // close the file
		    fclose(fp);
		    // set the output flag
		    output_flag = 1;

      		    //transfer the image in the processor memory
      		    for (k=0; k<imageLength; k++) {
        		    processorInstance.writeCharMem(callArgs[1]+k,*(bitmap_values+k));
      		    }
		  } else {fclose(fp);}
		} else {THROW_EXCEPTION("error while opening image file");}

		processorInstance.setRetVal(output_flag);
		processorInstance.returnFromCall();
		processorInstance.postCall();
		return true;
	}
};

template<typename issueWidth> class write_bitmapCall : public reconfCB<issueWidth>{
private:
	configEngine* cE;
public:
	write_bitmapCall(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
		reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
	bool operator()(ABIIf<issueWidth> &processorInstance){
		processorInstance.preCall();
		//the function has 6 parameters: the first four ones are retrieved by using the readArgs; the other ones are retrieved in the stack
		std::vector< issueWidth > callArgs = processorInstance.readArgs();
		callArgs.push_back(processorInstance.readMem(processorInstance.readSP()));
		callArgs.push_back(processorInstance.readMem(processorInstance.readSP() + sizeof(issueWidth)));

//		(this->cE)->executeForce("write_bitmap", this->latency, this->width, this->height, false);
//		(this->cE)->printSystemStatus();


		//get actual parameters from the processor
 		int k;
		char * output_file_name = (char*) malloc(512*sizeof(char)); // name of the output bitmap file to write
		k=0;
		do{
			output_file_name[k] = processorInstance.readCharMem(callArgs[0]+k);		
			k++;
		} while(output_file_name[k-1] != '\0' && k < 512);
		       
		unsigned char bitmap_type = callArgs[1];// input bitmap type: 0 --> GS; 1 --> RGB
		unsigned short int bitmap_width = callArgs[2]; // input bitmap width (pixels)
		unsigned short int bitmap_height = callArgs[3]; // input bitmap height (pixels)		
		unsigned char * bitmap_values = NULL; // input (pre-allocated) array of bitmap intensities
		unsigned int imageLength = 0;
		if(bitmap_type==0){
			imageLength = bitmap_width*bitmap_height;
		} else { imageLength = bitmap_width*bitmap_height*3; }
		bitmap_values = (unsigned char*) malloc(imageLength*sizeof(unsigned char));
		for(k = 0; k < imageLength; k++){
			bitmap_values[k] = processorInstance.readCharMem(callArgs[4]+k);
		}
		
		unsigned char * bitmap_palette = NULL; // input bitmap palette (bitmap type = 0 --> 256 * 3 array; bitmap type = 1 --> NULL)
		if(bitmap_type==0){
			bitmap_palette = (unsigned char*) malloc(256*3*sizeof(unsigned char));
			for(k = 0; k < 256*3; k++){
				bitmap_palette[k] = processorInstance.readCharMem(callArgs[5]+k);
			}
		} else {bitmap_palette = NULL;}

		// file pointer
		FILE * fp;
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

		// bitmap header
		BITMAP_HEADER		BH;

		// bitmap properties
		long int		bitmap_depth,
					bitmap_width_LI,
					bitmap_height_LI;
	
		// bitmap palette and values
		long int		row_bytes_filler;

		// scanning
		long int		j,
					i;
		long int		coord_row,
					coord_pixel;

		// output flag
		char			output_flag = 0;
	
		/*---Open the file---*/
		fp = fopen(output_file_name, "wb");
		if (fp != NULL) {
		  /*---Set the bitmap properties---*/
		  if (bitmap_type < 2) {
		    if (bitmap_type < 1) {
		      bitmap_depth = 1;
		    } else {
		      bitmap_depth = 3;
		    }
		    if (bitmap_width && bitmap_height) {
		      bitmap_width_LI	= ((long int)(bitmap_width));
		      bitmap_height_LI	= ((long int)(bitmap_height));

		      /*---Create a valid bitmap file: write 'BM' as the first two letters---*/
		      fputc('B', fp);
		      fputc('M', fp);

		      /*---Create the header of the bitmap, without palette---*/
		      if (bitmap_depth < 2) // grey level
		      { 
		        BH.LunghezzaFile = 1078 + bitmap_width * bitmap_height;
		        BH.RiservatoPre = 0;
		        BH.Offset = 1078;
		        BH.DimensioneHeader = 40;
		        BH.Larghezza = ((unsigned int)(bitmap_width));
		        BH.Altezza = ((unsigned int)(bitmap_height));
		        BH.Piani = 1;
		        BH.BitPerPixel = 8;
		        BH.Compressione = 0;
		        BH.DimensioneImmagine = 0;
		        BH.RisoluzioneOrizzontale = 0;
		        BH.RisoluzioneVerticale = 0;
		        BH.ColoriUsati = 256;
		        BH.ColoriImportanti = 0;
		      } else // RGB
		      { 
		        BH.LunghezzaFile = 54 + bitmap_width * bitmap_height;
		        BH.RiservatoPre = 0;
		        BH.Offset = 54;
		        BH.DimensioneHeader = 40;
		        BH.Larghezza = ((unsigned int)(bitmap_width));
		        BH.Altezza = ((unsigned int)(bitmap_height));
		        BH.Piani = 1;
		        BH.BitPerPixel = 24;
		        BH.Compressione = 0;
		        BH.DimensioneImmagine = 0;
		        BH.RisoluzioneOrizzontale = 0;
		        BH.RisoluzioneVerticale = 0;
		        BH.ColoriUsati = 0;
		        BH.ColoriImportanti = 0;
		      }

		      /*---Write the header of the bitmap, without palette---*/
		      fwrite((&BH), ((sizeof(BITMAP_HEADER)) - (256 * 3)), 1, fp);

		      /*---Write palette if the bitmap is 8-bits grey level---*/
		      if (bitmap_depth < 2) {		
		        for(j = 0; j < ((long int)(BH.ColoriUsati)); j++) {
		          fwrite(bitmap_palette + (j * 3), 1, 3, fp);  // (R,G,B) mapping for index_2 grey level
		          fputc(0, fp); // filler
		        }	
		      }

		      /*---Find row-bytes filler (in a bitmap the number of bytes per row is a multiple of 4)---*/
		      row_bytes_filler = ((long int)((bitmap_width_LI * bitmap_depth)%(4))); // row_filler = 0, 1, 2 or 3;
		      if (row_bytes_filler) {
		        row_bytes_filler = (4 - row_bytes_filler); // row_bytes_filler = 0, 3, 2 or 1

		        /*---Write bitmap values---*/
		        if (bitmap_depth < 2) // grey level
		        {			
		          for ((j = (bitmap_height_LI - 1)); (j > (-1)); (j--)) {
		            /*---Write a row---*/
		            coord_row = (bitmap_width_LI * j);
		            for ((i = 0); (i < bitmap_width_LI); (i++)) {
		              fputc((*(bitmap_values + coord_row + i)), fp); // grey level
		            }

		            /*---Write row-bytes filler---*/
		            for (i = 0; i < row_bytes_filler; i++) {
		              fputc(0, fp);
		            }
		          }
		        } else // RGB
		        {			
		          for ((j = (bitmap_height_LI - 1)); (j > (-1)); (j--)) {
		            /*---Write a row---*/
		            coord_row = (bitmap_width_LI * j);
		            for ((i = 0); (i < bitmap_width_LI); (i++)) {
		              coord_pixel = (3 * (coord_row + i));
		              fputc((*(bitmap_values + coord_pixel + 2)), fp); // B
		              fputc((*(bitmap_values + coord_pixel + 1)), fp); // G
		              fputc((*(bitmap_values + coord_pixel)), fp); // R
		            }

		            /*---Write row-bytes filler---*/
		            for (i = 0; i < row_bytes_filler; i++) {
		              fputc(0, fp);
		            }
		          }
		        }
		      } else {
		        /*---Write bitmap values---*/
		        if (bitmap_depth < 2) // grey level
		        {			
		          for ((j = (bitmap_height_LI - 1)); (j > (-1)); (j--)) {
		            /*---Write a row---*/
		            coord_row = (bitmap_width_LI * j);
		            for ((i = 0); (i < bitmap_width_LI); (i++)) {
		              fputc((*(bitmap_values + coord_row + i)), fp); // grey level
		            }
		          }
		        } else // RGB
		        {			
		          for ((j = (bitmap_height_LI - 1)); (j > (-1)); (j--)) {
		            /*---Write a row---*/
		            coord_row = (bitmap_width_LI * j);
		            for ((i = 0); (i < bitmap_width_LI); (i++)) {
		              coord_pixel = (3 * (coord_row + i));
		              fputc((*(bitmap_values + coord_pixel + 2)), fp); // B
		              fputc((*(bitmap_values + coord_pixel + 1)), fp); // G
		              fputc((*(bitmap_values + coord_pixel)), fp); // R
		            }
		          }
		        }
		      }

		      // Close the file
		      fclose(fp);

		      // Set the output flag
		      output_flag = 1;
		    } else {fclose (fp);}
		  } else {fclose (fp);}
		} else {THROW_EXCEPTION("error while opening image file");}
	
		processorInstance.setRetVal(output_flag);
		processorInstance.returnFromCall();
		processorInstance.postCall();
		return true;
	}
};


template <typename issueWidth> class rgb2greyCall : public reconfCB <issueWidth>{
private:
	configEngine* cE;
	map<unsigned int, bool> configured;
public:
	rgb2greyCall(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
		reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
	bool operator()(ABIIf<issueWidth> &processorInstance){
		processorInstance.preCall();
		//the function has 5 parameters: the first four ones are retrieved by using the readArgs; the other ones are retrieved in the stack
		std::vector< issueWidth > callArgs = processorInstance.readArgs();
		callArgs.push_back(processorInstance.readMem(processorInstance.readSP()));

		(this->cE)->executeForce("rgb2grey", this->latency, this->width, this->height, configured[processorInstance.getProcessorID()]);
		if ( !configured[processorInstance.getProcessorID()] ) { (this->cE)->printSystemStatus(); cout << endl << endl;}
		configured[processorInstance.getProcessorID()] = true;

		unsigned short int width = callArgs[2];
		unsigned short int height = callArgs[3];
		unsigned short int depth = callArgs[4];
		unsigned short int size = width*height;
		unsigned short int rgbSize = size*depth;

		int  i = 0;
		for(i = 0; i < width*height; i++){
			processorInstance.writeCharMem(callArgs[1] + i, processorInstance.readCharMem(callArgs[0]+ i*3));
			processorInstance.writeCharMem(callArgs[1] + i, 255 - processorInstance.readCharMem(callArgs[1] + i));
		}

		processorInstance.returnFromCall();
		processorInstance.postCall();
		return true;
	}
};

template <typename issueWidth> class edgeOverlappingCall : public reconfCB <issueWidth>{
private:
	configEngine* cE;
	map<unsigned int, bool> configured;
public:
	edgeOverlappingCall(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
		reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
	bool operator()(ABIIf<issueWidth> &processorInstance){
		processorInstance.preCall();
		//the function has 6 parameters: the first four ones are retrieved by using the readArgs; the other ones are retrieved in the stack
		std::vector< issueWidth > callArgs = processorInstance.readArgs();
		callArgs.push_back(processorInstance.readMem(processorInstance.readSP()));
		callArgs.push_back(processorInstance.readMem(processorInstance.readSP() + sizeof(issueWidth)));

		(this->cE)->executeForce("edgeOverlapping", this->latency, this->width, this->height, configured[processorInstance.getProcessorID()]);
		if ( !configured[processorInstance.getProcessorID()] ) { (this->cE)->printSystemStatus(); cout << endl << endl;}
		configured[processorInstance.getProcessorID()] = true;

		unsigned short int width = callArgs[3];
		unsigned short int height = callArgs[4];
		unsigned short int depth = callArgs[5];
		unsigned short int size = width*height;
		unsigned short int rgbSize = size*depth;
		unsigned char* inputImage = (unsigned char*) malloc(rgbSize*sizeof(unsigned char));
		unsigned char* edgeImage = (unsigned char*) malloc(size*sizeof(unsigned char));
		unsigned char* outputImage = (unsigned char*) malloc(rgbSize*sizeof(unsigned char));
		int k;

		int i = 0;
		for(i = 0; i < width*height; i++){
		  if(processorInstance.readCharMem(callArgs[1] + i) < 100) {
		    processorInstance.writeCharMem(callArgs[2] + i*3, 255);
		    processorInstance.writeCharMem(callArgs[2] + i*3 + 1, 255);
		    processorInstance.writeCharMem(callArgs[2] + i*3 + 2, processorInstance.readCharMem(callArgs[1] + i));
		  }
		  else
		    for(k = 0; k <3; k++)
		      processorInstance.writeCharMem(callArgs[2] + i*3 +k, processorInstance.readCharMem(callArgs[0] + i*3+k));
		}

		processorInstance.returnFromCall();
		processorInstance.postCall();
		return true;
	}
};

template <typename issueWidth> class edgeDetectionSinglePixelCall : public reconfCB <issueWidth>{
private:
	configEngine* cE;
	map<unsigned int, bool> configured;
public:
	edgeDetectionSinglePixelCall(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
		reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
	bool operator()(ABIIf<issueWidth> &processorInstance){
		processorInstance.preCall();
		//void edgeDetectionSinglePixel(unsigned char* inputImage, unsigned char* outputImage, int X, int Y, unsigned short int width, unsigned short int height, unsigned short int depth){
		//the function has 7 parameters: the first four ones are retrieved by using the readArgs; the other ones are retrieved in the stack
		std::vector< issueWidth > callArgs = processorInstance.readArgs();
		callArgs.push_back(processorInstance.readMem(processorInstance.readSP()));
		callArgs.push_back(processorInstance.readMem(processorInstance.readSP() + sizeof(issueWidth)));
		callArgs.push_back(processorInstance.readMem(processorInstance.readSP() + 2*sizeof(issueWidth)));
		callArgs.push_back(processorInstance.readMem(processorInstance.readSP() + 3*sizeof(issueWidth)));
		callArgs.push_back(processorInstance.readMem(processorInstance.readSP() + 4*sizeof(issueWidth)));

		(this->cE)->executeForce("edgeDetectionSinglePixel", this->latency, this->width, this->height, configured[processorInstance.getProcessorID()]);
		if ( !configured[processorInstance.getProcessorID()] ) { (this->cE)->printSystemStatus(); cout << endl << endl;}
		configured[processorInstance.getProcessorID()] = true;

		int X = callArgs[2];
		int Y = callArgs[3];
		unsigned short int width = callArgs[4];
		unsigned short int height = callArgs[5];
		unsigned short int depth = callArgs[6];
		unsigned short int size = width*height;
		unsigned short int rgbSize = size*depth;

		long sumX = 0;
		long sumY = 0;
		int I, J;
		int SUM;

		int GX[3][3];
		int GY[3][3];

		/* 3x3 GX Sobel mask.  Ref: www.cee.hw.ac.uk/hipr/html/sobel.html */
		GX[0][0] = -1; GX[0][1] = 0; GX[0][2] = 1;
		GX[1][0] = -2; GX[1][1] = 0; GX[1][2] = 2;
		GX[2][0] = -1; GX[2][1] = 0; GX[2][2] = 1;

		/* 3x3 GY Sobel mask.  Ref: www.cee.hw.ac.uk/hipr/html/sobel.html */
		GY[0][0] =  1; GY[0][1] =  2; GY[0][2] =  1;
		GY[1][0] =  0; GY[1][1] =  0; GY[1][2] =  0;
		GY[2][0] = -1; GY[2][1] = -2; GY[2][2] = -1;

		if(Y==0 || Y==height-1)
			SUM = 0;
		else if(X==0 || X==width-1)
			SUM = 0;
		else {
			for(I=-1; I<=1; I++)  {
				for(J=-1; J<=1; J++)  {
					sumX = sumX + (int) (processorInstance.readCharMem(callArgs[0]+ X+I + (Y+J)*width)) * GX[I+1][J+1];
				}
			}
			for(I=-1; I<=1; I++)  {
				for(J=-1; J<=1; J++)  {
					sumY = sumY + (int) (processorInstance.readCharMem(callArgs[0]+ X+I + (Y+J)*width)) * GY[I+1][J+1];
				}
			}
			SUM = abs(sumX) + abs(sumY) -150;
		}
		if(SUM>255) SUM=255;
		if(SUM<0) SUM=0;
		processorInstance.writeCharMem(callArgs[1] + X + Y*width, 255 - (unsigned char)(SUM));

		processorInstance.returnFromCall();
		processorInstance.postCall();
		return true;
	}
};

};
