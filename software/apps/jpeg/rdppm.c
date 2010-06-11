
/*DI QUESTO MODULO, LE FUNZIONI PBM_GETC E READ_PBM_INTEGER SONO STATE COPIATE
//QUASI INTEGRALMENTE DALLE LIBRERIE IJG. TUTTO IL RESTO E' MIO MA HO LASCIATO
IL
//COPYRIGHT INIZIALE (NON SAPEVO ESATTAMENTE COME COMPORTARMI)*/


/*
 * rdppm.c
 *
 * Copyright (C) 1991-1997, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains routines to read input images in PPM/PGM format.
 * The extended 2-byte-per-sample raw PPM/PGM formats are supported.
 * The PBMPLUS library is NOT required to compile this software
 * (but it is highly useful as a set of PPM image manipulation programs).
 *
 * These routines may need modification for non-Unix environments or
 * specialized applications.  As they stand, they assume input from
 * an ordinary stdio stream.  They further assume that reading begins
 * at the start of the file; start_input may need work if the
 * user interface has already read some data (e.g., to determine that
 * the file is indeed PPM format).
 */

/* Portions of this code are based on the PBMPLUS library, which is:
**
** Copyright (C) 1988 by Jef Poskanzer.
**
** Permission to use, copy, modify, and distribute this software and its
** documentation for any purpose and without fee is hereby granted, provided
** that the above copyright notice appear in all copies and that both that
** copyright notice and this permission noticetest2.c:13: error: `righe'
undeclared (first use in this function)
 appear in supporting
** documentation.  This software is provided "as is" without express or
** implied warranty.
*/



#include <stdio.h>
#include <stdlib.h>
#include "tipi.h"

int pbm_getc (FILE *infile)
/* Read next char, skipping over any comments */
/* A comment/newline sequence is returned as a newline */
{
  unsigned char ch;
  // register int ch;
  
  fread(&ch,1,1,infile);
  
  if (ch == '#') {
    do {
     
	  fread(&ch,1,1,infile);
  
	  // ch = getc(infile);
    } while (ch != '\n' /* && feof(infile)==0*/);
  }
  return ch;
}

unsigned int read_pbm_integer (FILE * infile)
/* Read an unsigned decimal integer from the PPM file */
/* Swallows one trailing character after the integer */
{
	char ch;
  	int val;

  	/* Skip any leading whitespace */
  	do
  		{
    		ch=pbm_getc(infile);
  		} while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r');

  	val=(ch - '0');

  	while(((ch=pbm_getc(infile))!=' ')&&(ch != '\t') && (ch != '\n') && (ch !=
'\r')/*&&(feof(infile)==0)*/)
		{
		  val*=10;
		  val+=ch - '0';
		}
  return val;
}


rows rdppm (info *pinf, FILE *in) //deve ritornare rdppm puntatore righe e info mediante puntatore
{
	FILE *source = in;
  	int file,real_columns,real_rows,horiz_border,vertical_border,i,j,z,scale;
	rows p_icture;
	unsigned char value[3];
   unsigned char ch;
	
	fread(&ch,1,1,source);
	
	if (ch != 'P')
	{
		printf("Input file is not a portable pixel/grey map \n");
		return NULL;
	}
	
	fread(&ch,1,1,source);
	
	file = ch -'0';

	if ((file<2)||(file>6)||(file==4))
	{
		printf("PBM is not supported\n");
		return NULL;
	}
	real_columns = read_pbm_integer(source);
  	real_rows = read_pbm_integer(source);
	pinf->maxsample = read_pbm_integer(source);
	if (real_columns <= 0 || real_rows <= 0 || pinf->maxsample <= 0)
	{
		printf("Reading error\n");
		return NULL;
	}

	horiz_border=8-real_columns%8;
	if (horiz_border>7) horiz_border=0;
	pinf->L=real_columns+horiz_border;

	vertical_border=8-real_rows%8;
	if (vertical_border>7) vertical_border=0;
	pinf->H=real_rows+vertical_border;

	pinf->numrows=pinf->H/8;
	pinf->numblocks=pinf->L/8;

	if ((file==3)||(file==6)) pinf->color=1;
		else pinf->color=0;
	scale=255/pinf->maxsample;
	pinf->maxsample=255;

	p_icture=(p_block *) malloc (pinf->numrows*sizeof(p_block));
	for (i=0;i<pinf->numrows;i++)
		*(p_icture+i)=(block *) malloc (pinf->numblocks*sizeof(block));
	switch (file)
	{
		case 2:	//grayscale ascii
		{
			for (i=0;i<pinf->H;i++)
				for (j=0;j<pinf->L;j++)
				{
					int position=(i%8)*8+j%8;
					if (i>=real_rows)
					{
						int last_position=((real_rows-1)%8)*8+j%8;
						value[0]=(*(p_icture+i/8)+j/8)->comp1[last_position];
					}
					else if (j>=real_columns)
					{
						int last_position=(i%8)*8+(real_columns-1)%8;
						value[0]=(*(p_icture+i/8)+j/8)->comp1[last_position];
					}
					else value[0]=read_pbm_integer(source)*scale;
					(*(p_icture+i/8)+j/8)->comp1[position]=value[0];
					(*(p_icture+i/8)+j/8)->comp2[position]=0;
					(*(p_icture+i/8)+j/8)->comp3[position]=0;
				}
		}
		break;
		case 3: //color ascii
		{
			for (i=0;i<pinf->H;i++)
				for (j=0;j<pinf->L;j++)
				{
					int position=(i%8)*8+j%8;
					if (i>=real_rows)
					{
						int last_position=((real_rows-1)%8)*8+j%8;
						value[0]=(*(p_icture+i/8)+j/8)->comp1[last_position];
						value[1]=(*(p_icture+i/8)+j/8)->comp2[last_position];
						value[2]=(*(p_icture+i/8)+j/8)->comp3[last_position];
					}
					else if (j>=real_columns)
					{
						int last_position=(i%8)*8+(real_columns-1)%8;
						value[0]=(*(p_icture+i/8)+j/8)->comp1[last_position];
						value[1]=(*(p_icture+i/8)+j/8)->comp2[last_position];
						value[2]=(*(p_icture+i/8)+j/8)->comp3[last_position];
					}
					else
						for (z=0;z<3;z++)
							value[z]=read_pbm_integer(source)*scale;
						(*(p_icture+i/8)+j/8)->comp1[position]=value[0];
						(*(p_icture+i/8)+j/8)->comp2[position]=value[1];
						(*(p_icture+i/8)+j/8)->comp3[position]=value[2];
				}
		}
		break;
		case 5: //grayscale rawbit
		{
			for (i=0;i<pinf->H;i++)
				for (j=0;j<pinf->L;j++)
				{
					int position=(i%8)*8+j%8;
					if (i>=real_rows)
					{
						int last_position=((real_rows-1)%8)*8+j%8;
						value[0]=(*(p_icture+i/8)+j/8)->comp1[last_position];
					}
					else if (j>=real_columns)
					{
						int last_position=(i%8)*8+(real_columns-1)%8;
						value[0]=(*(p_icture+i/8)+j/8)->comp1[last_position];
					}
					else
					{
						fread(&value[0],sizeof(char),1,source);
						value[0]*=scale;
					}
					(*(p_icture+i/8)+j/8)->comp1[position]=value[0];
					(*(p_icture+i/8)+j/8)->comp2[position]=0;
					(*(p_icture+i/8)+j/8)->comp3[position]=0;
				}
		}
		break;
		case 6: //color rawbit
		{
			for (i=0;i<pinf->H;i++)
			{
				for (j=0;j<pinf->L;j++)
				{
					int position=(i%8)*8+j%8;
					if (i>=real_rows)
					{
						int last_position=((real_rows-1)%8)*8+j%8;
						value[0]=(*(p_icture+i/8)+j/8)->comp1[last_position];
						value[1]=(*(p_icture+i/8)+j/8)->comp2[last_position];
						value[2]=(*(p_icture+i/8)+j/8)->comp3[last_position];
					}
					else if (j>=real_columns)
					{
						int last_position=(i%8)*8+(real_columns-1)%8;
						value[0]=(*(p_icture+i/8)+j/8)->comp1[last_position];
						value[1]=(*(p_icture+i/8)+j/8)->comp2[last_position];
						value[2]=(*(p_icture+i/8)+j/8)->comp3[last_position];
					}
					else
					{
						fread(&value[0],sizeof(char),1,source);
						fread(&value[1],sizeof(char),1,source);
						fread(&value[2],sizeof(char),1,source);
						value[0]*=scale;
						value[1]*=scale;
						value[2]*=scale;
					}
					(*(p_icture+i/8)+j/8)->comp1[position]=value[0];
					(*(p_icture+i/8)+j/8)->comp2[position]=value[1];
					(*(p_icture+i/8)+j/8)->comp3[position]=value[2];
				}
			}
		}
		break;
	}
	pinf->H=real_rows;
	pinf->L=real_columns;

	return(p_icture);
}


