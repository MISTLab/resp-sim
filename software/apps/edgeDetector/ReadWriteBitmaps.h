

/*---------------------------------------COMMENT---------------------------------------*/

/*
 * ReadWriteBitmaps.h
 * written by Alessandro Lanza
 * 10.03.10
 */

/*-------------------------------------------------------------------------------------*/


// include this header file only
// if it was not included before
#ifndef _READ_WRITE_BITMAPS_H

	#define _READ_WRITE_BITMAPS_H


	/*-------------------------------------PRECOMPILER---------------------------------*/

	/*-----CONSTANTS-----*/

	/*------MACROS-------*/
	
	/*-------TYPES-------*/
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

	
	/*--------------------------------------PROTOTYPES-------------------------------------*/

	
	char get_bitmap_properties(
		char * input_file_name, // name of the input bitmap file to get the properties of
		unsigned char * bitmap_type, // pointer to the returned bitmap type (0->GS; 1->RGB)
		unsigned short int * bitmap_width, // pointer to the returned bitmap width (pixels)
		unsigned short int * bitmap_height // pointer to the returned bitmap height (pixels)		
		);

	char read_bitmap(
		char * input_file_name, // name of the input bitmap file to read
		unsigned char * bitmap_values // pointer to the returned (pre-allocated) array of bitmap intensities
		);

	char write_bitmap(
		char * output_file_name, // name of the output bitmap file to write
		unsigned char bitmap_type, // input bitmap type: 0 --> GS; 1 --> RGB
		unsigned short int bitmap_width, // input bitmap width (pixels)
		unsigned short int bitmap_height, // input bitmap height (pixels)		
		unsigned char * bitmap_values, // input (pre-allocated) array of bitmap intensities
		unsigned char * bitmap_palette // input bitmap palette (bitmap type = 0 --> 256 * 3 array; bitmap type = 1 --> NULL)
		);

	void create_bitmap_palette_grey_levels(
		unsigned char * palette // output (pre-allocated) palette (256 * 3 elements)
		);

	void create_bitmap_palette_binary(
		unsigned char * palette, // output (pre-allocated) palette (256 * 3 elements)
		unsigned char black_value, // black value
		unsigned char white_value // white value
		);

	void modify_bitmap_palette(
		unsigned char * palette, // input (pre-allocated) palette (256 * 3 elements)
		unsigned char invert_channels, // invert channels? 0->no; else->yes
		unsigned char value_to_modify, // value to modify
		unsigned char R, unsigned char G, unsigned char B // R, G, B
		);

	void create_bitmap_palette_blobs(
		unsigned char * palette // input (pre-allocated) palette (256 * 3 elements)
		);


#endif	



