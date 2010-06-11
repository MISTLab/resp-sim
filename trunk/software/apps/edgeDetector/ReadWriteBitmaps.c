

/*---------------------------------------COMMENT---------------------------------------*/

/*
 * ReadWriteBitmaps.c
 * written by Alessandro Lanza
 * 10.03.10
 */

/*-------------------------------------------------------------------------------------*/


/*---------------------------------------HEADERS---------------------------------------*/
 
// C standard headers
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

// user-defined headers
#include "ReadWriteBitmaps.h"

/*-------------------------------------------------------------------------------------*/


/*--------------------------------------FUNCTIONS--------------------------------------*/


char get_bitmap_properties(
	char * input_file_name, // name of the input bitmap file to get the properties of
	unsigned char * bitmap_type, // pointer to the returned bitmap type (0->GS; 1->RGB)
	unsigned short int * bitmap_width, // pointer to the returned bitmap width (pixels)
	unsigned short int * bitmap_height // pointer to the returned bitmap height (pixels)		
	)
{
	FILE 			* fp; // file pointer
	BITMAP_HEADER 	BH; // bitmap header
	char			output_flag = -1; // output flag

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
			(*(bitmap_type))	= ((unsigned char)		(!((BH.BitPerPixel) == 8)));
			(*(bitmap_width)) 	= ((unsigned short int)	(BH.Larghezza));
			(*(bitmap_height)) 	= ((unsigned short int)	(BH.Altezza));

			// close the file
			fclose(fp);
		}
		else {
			fclose(fp);	    		
		}
	}

	return (output_flag);
}

char read_bitmap(
	char * input_file_name, // name of the input bitmap file to read
	unsigned char * bitmap_values // pointer to the returned (pre-allocated) array of bitmap intensities
	)
{
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
			if (bitmap_depth < 2)
			{		
				for(j = 0; j < ((long int)(BH.ColoriUsati)); j++)
				{
					fread((BH.palette) + (j * 3), 1, 3, fp);  // (R,G,B) mapping for index_2 grey level
					((void)(fgetc(fp))); // filler
				}	
			}

			/*---Find row-bytes filler (in a bitmap the number of bytes per row is a multiple of 4)---*/
			row_bytes_filler = ((long int)((bitmap_width * bitmap_depth)%(4))); // row_filler = 0, 1, 2 or 3;
			if (row_bytes_filler)
			{
				row_bytes_filler 	= (4 - row_bytes_filler); // row_bytes_filler = 0, 3, 2 or 1
			
				/*---Read bitmap values---*/
				if (bitmap_depth < 2) // grey level
				{			
					for ((j = (bitmap_height - 1)); (j > (-1)); (j--))
					{
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
				else // RGB
				{			
					for ((j = (bitmap_height - 1)); (j > (-1)); (j--))
					{
						/*---Read a row---*/
						coord_row = (bitmap_width * j);
						for ((i = 0); (i < bitmap_width); (i++))
						{
							coord_pixel = (3 * (coord_row + i));
							(*(bitmap_values + coord_pixel + 2)) 	= ((unsigned char)(fgetc(fp))); // B
							(*(bitmap_values + coord_pixel + 1))	= ((unsigned char)(fgetc(fp))); // G
							(*(bitmap_values + coord_pixel)) 		= ((unsigned char)(fgetc(fp))); // R
						}

						/*---Read row-bytes filler---*/
						for (i = 0; i < row_bytes_filler; i++)
						{
							((void)(fgetc(fp)));
						}
					}
				}
			}
			else
			{
				/*---Read bitmap values---*/
				if (bitmap_depth < 2) // grey level
				{			
					for ((j = (bitmap_height - 1)); (j > (-1)); (j--))
					{
						/*---Read a row---*/
						coord_row = (bitmap_width * j);
						for ((i = 0); (i < bitmap_width); (i++))
						{
							(*(bitmap_values + coord_row + i)) = ((unsigned char)(fgetc(fp))); // grey level	
						}
					}
				}
				else // RGB
				{			
					for ((j = (bitmap_height - 1)); (j > (-1)); (j--))
					{
						/*---Read a row---*/
						coord_row = (bitmap_width * j);
						for ((i = 0); (i < bitmap_width); (i++))
						{
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

	return (output_flag);
}

char write_bitmap(
	char * output_file_name, // name of the output bitmap file to write
	unsigned char bitmap_type, // input bitmap type: 0 --> GS; 1 --> RGB
	unsigned short int bitmap_width, // input bitmap width (pixels)
	unsigned short int bitmap_height, // input bitmap height (pixels)		
	unsigned char * bitmap_values, // input (pre-allocated) array of bitmap intensities
	unsigned char * bitmap_palette // input bitmap palette (bitmap type = 0 --> 256 * 3 array; bitmap type = 1 --> NULL)
	)
{
	/*---DECLARATIONS---*/
  output_file_name = "output.bmp";
	// file pointer
	FILE 			* fp;

	// bitmap header
	BITMAP_HEADER 	BH;

	// bitmap properties
	long int		bitmap_depth,
					bitmap_width_LI,
					bitmap_height_LI;
	
	// bitmap palette and values
	long int 		row_bytes_filler;

	// scanning
	long int		j,
					i;
	long int		coord_row,
					coord_pixel;

	// output flag
	char			output_flag = 0;
	
	/*---END DECLARATIONS---*/
	
	

	/*---INSTRUCTIONS---*/

	/*---Open the file---*/
	fp = fopen(output_file_name, "wb");
	if (fp != NULL)
	{
		/*---Set the bitmap properties---*/
		if (bitmap_type < 2)
		{
			if (bitmap_type < 1)
			{
				bitmap_depth	= 1;
			}
			else
			{
				bitmap_depth	= 3;
			}
			if (bitmap_width && bitmap_height)
			{
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
					BH.Larghezza	= ((unsigned int)(bitmap_width));
					BH.Altezza	= ((unsigned int)(bitmap_height));
					BH.Piani = 1;
					BH.BitPerPixel = 8;
					BH.Compressione = 0;
					BH.DimensioneImmagine = 0;
					BH.RisoluzioneOrizzontale = 0;
					BH.RisoluzioneVerticale = 0;
					BH.ColoriUsati = 256;
					BH.ColoriImportanti = 0;
				}
			  	else // RGB
			  	{ 
					BH.LunghezzaFile = 54 + bitmap_width * bitmap_height;
					BH.RiservatoPre = 0;
					BH.Offset = 54;
					BH.DimensioneHeader = 40;
					BH.Larghezza	= ((unsigned int)(bitmap_width));
					BH.Altezza	= ((unsigned int)(bitmap_height));
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
				if (bitmap_depth < 2)
				{		
					for(j = 0; j < ((long int)(BH.ColoriUsati)); j++)
					{
						fwrite(bitmap_palette + (j * 3), 1, 3, fp);  // (R,G,B) mapping for index_2 grey level
						fputc(0, fp); // filler
					}	
				}

				/*---Find row-bytes filler (in a bitmap the number of bytes per row is a multiple of 4)---*/
				row_bytes_filler = ((long int)((bitmap_width_LI * bitmap_depth)%(4))); // row_filler = 0, 1, 2 or 3;
				if (row_bytes_filler)
				{
					row_bytes_filler 	= (4 - row_bytes_filler); // row_bytes_filler = 0, 3, 2 or 1
				
					/*---Write bitmap values---*/
					if (bitmap_depth < 2) // grey level
					{			
						for ((j = (bitmap_height_LI - 1)); (j > (-1)); (j--))
						{
							/*---Write a row---*/
							coord_row = (bitmap_width_LI * j);
							for ((i = 0); (i < bitmap_width_LI); (i++))
							{
								fputc((*(bitmap_values + coord_row + i)), fp); // grey level
							}

							/*---Write row-bytes filler---*/
							for (i = 0; i < row_bytes_filler; i++)
							{
								fputc(0, fp);
							}
						}
					}
					else // RGB
					{			
						for ((j = (bitmap_height_LI - 1)); (j > (-1)); (j--))
						{
							/*---Write a row---*/
							coord_row = (bitmap_width_LI * j);
							for ((i = 0); (i < bitmap_width_LI); (i++))
							{
								coord_pixel = (3 * (coord_row + i));
								fputc((*(bitmap_values + coord_pixel + 2)), fp); // B
								fputc((*(bitmap_values + coord_pixel + 1)), fp); // G
								fputc((*(bitmap_values + coord_pixel)), fp); // R
							}

							/*---Write row-bytes filler---*/
							for (i = 0; i < row_bytes_filler; i++)
							{
								fputc(0, fp);
							}
						}
					}
				}
				else
				{
					/*---Write bitmap values---*/
					if (bitmap_depth < 2) // grey level
					{			
						for ((j = (bitmap_height_LI - 1)); (j > (-1)); (j--))
						{
							/*---Write a row---*/
							coord_row = (bitmap_width_LI * j);
							for ((i = 0); (i < bitmap_width_LI); (i++))
							{
								fputc((*(bitmap_values + coord_row + i)), fp); // grey level
							}
						}
					}
					else // RGB
					{			
						for ((j = (bitmap_height_LI - 1)); (j > (-1)); (j--))
						{
							/*---Write a row---*/
							coord_row = (bitmap_width_LI * j);
							for ((i = 0); (i < bitmap_width_LI); (i++))
							{
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
			}
			else
			{
				fclose (fp);
				fprintf(stderr, "\n\n\nNULL (= 0) input bitmap dimensions");
			}
		}
		else
		{
			fclose (fp);
			fprintf(stderr, "\n\n\nnot recognized input bitmap type");
		}
	}
	else
	{
		fprintf(stderr, "\n\n\noutput bitmap file can not be opened");
	}
	
	/*---END INSTRUCTIONS---*/



	/*---RETURN---*/
	return (output_flag);
	/*---END RETURN---*/
}

void create_bitmap_palette_grey_levels(
	unsigned char * palette // output (pre-allocated) palette (256 * 3 elements)
	)
{
	unsigned short int i;	
	for(i = 0; i < 256; i++)
		palette[3 * i] = palette[3 * i + 1] = palette[3 * i + 2] = ((unsigned char)(i));
}

void create_bitmap_palette_binary(
	unsigned char * palette, // output (pre-allocated) palette (256 * 3 elements)
	unsigned char black_value, // black value
	unsigned char white_value // white value
	)
{
	palette[3 * ((int)(black_value))] = palette[3 * ((int)(black_value)) + 1] = palette[3 * ((int)(black_value)) + 2] = 0;
	palette[3 * ((int)(white_value))] = palette[3 * ((int)(white_value)) + 1] = palette[3 * ((int)(white_value)) + 2] = 255;
}

void modify_bitmap_palette(
	unsigned char * palette, // input (pre-allocated) palette (256 * 3 elements)
	unsigned char invert_channels, // invert channels? 0->no; else->yes
	unsigned char value_to_modify, // value to modify
	unsigned char R, unsigned char G, unsigned char B // R, G, B
	)
{
	unsigned short int v = ((unsigned short int)(value_to_modify));
	if (invert_channels)
	{
		palette[3 * v + 2]	= R;
		palette[3 * v + 1] 	= G;
		palette[3 * v + 0] 	= B;	
	}
	else
	{
		palette[3 * v + 0]	= R;
		palette[3 * v + 1] 	= G;
		palette[3 * v + 2] 	= B;	
	}	
}

void create_bitmap_palette_blobs(
	unsigned char * palette // input (pre-allocated) palette (256 * 3 elements)
	)
{
	unsigned short int	R	= 56,
						G	= 102,
						B	= 70,
						j;
	for(j = 1; j < 256; j++)
	{
		palette[(3 * j) + 2]	= (unsigned char)B;
		palette[(3 * j) + 1]	= (unsigned char)G;
		palette[(3 * j) + 0]	= (unsigned char)R;

		R	= (R + 150)%(256);
		G	= (G + 82)%(256);
		B	= (B + 200)%(256);			
	}
}

