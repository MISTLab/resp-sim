
#include <stdio.h>
#include <math.h>
#include "tipi.h"
#define IDTABLUM 0;
#define IDTABCROM 1;


void arrange (intblock *pint)
{
	int
i,zig_zag[]={0,1,8,16,9,2,3,10,17,24,32,25,18,11,4,5,12,19,26,33,40,48,41,34,27,20,13,6,7,14,21,28,35,42,49,56,57,50,43,36,29,22,15,23,30,37,44,51,58,59,52,45,38,31,39,46,53,60,61,54,47,55,62,63};
	intblock temp;

	for (i=0;i<64;i++)
		{
			temp.intcomp1[i]=pint->intcomp1[zig_zag[i]];
			temp.intcomp2[i]=pint->intcomp2[zig_zag[i]];
			temp.intcomp3[i]=pint->intcomp3[zig_zag[i]];
		}
	for (i=0;i<64;i++)
		{
			pint->intcomp1[i]=temp.intcomp1[i];
			pint->intcomp2[i]=temp.intcomp2[i];
			pint->intcomp3[i]=temp.intcomp3[i];
		}


}


void initialize_huff_tbl(huffman_tab *p_tab,int id)
{
	int i,j;

	p_tab->id=id;
	switch (id)
	{
		case 0:	{	//dc luminance
				int bits[]={0,0,1,5,1,1,1,1,1,1,0,0,0,0,0,0,0};
				int symb[]={0,1,2,3,4,5,6,7,8,9,10,11};
				int code[]={0,2,3,4,5,6,14,30,62,126,254,510};
				p_tab->number=12;
				p_tab->maxsize=9;
				for (i=0;i<17;i++)
					p_tab->code_size[i]=bits[i];
				for (i=0;i<p_tab->number;i++)
					p_tab->symbols[i]=symb[i];
				for (i=0;i<p_tab->number;i++)
					p_tab->codes[i]=code[i];
			}
			break;
		case 1:	{	//dc crominance
				int bits[]={0,0,3,1,1,1,1,1,1,1,1,1,0,0,0,0,0};
				int symb[]={0,1,2,3,4,5,6,7,8,9,10,11};
				int code[]={0,1,2,6,14,30,62,126,254,510,1022,2046};
				p_tab->number=12;
				p_tab->maxsize=11;
				for (i=0;i<17;i++)
					p_tab->code_size[i]=bits[i];
				for (i=0;i<p_tab->number;i++)
					p_tab->symbols[i]=symb[i];
				for (i=0;i<p_tab->number;i++)
					p_tab->codes[i]=code[i];
			}
			break;
		case 16:{	//ac luminance
				int bits[]={0,0,2,1,3,3,2,4,3,5,5,4,4,0,0,1,125};
				int symb[]={
					0x01,0x02,0x03,0x00,0x04,0x11,0x05,0x12,0x21,0x31,0x41,0x06,0x13,0x51,0x61,0x07,0x22,0x71,

					0x14,0x32,0x81,0x91,0xa1,0x08,0x23,0x42,0xb1,0xc1,0x15,0x52,0xd1,0xf0,0x24,0x33,0x62,0x72,

					0x82,0x09,0x0a,0x16,0x17,0x18,0x19,0x1a,0x25,0x26,0x27,0x28,0x29,0x2a,0x34,0x35,0x36,0x37,

					0x38,0x39,0x3a,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x53,0x54,0x55,0x56,0x57,0x58,0x59,

					0x5a,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x83,

					0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0xa2,0xa3,

					0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xc2,0xc3,

					0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xe1,0xe2,

					0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa};
				int code[]={
					0,1,4,10,11,12,26,27,28,58,59,120,121,122,123,248,249,250,502,503,504,505,506,1014,1015,1016,

					1017,1018,2038,2039,2040,2041,4084,4085,4086,4087,32704,65410,65411,65412,65413,65414,65415,

					65416,65417,65418,65419,65420,65421,65422,65423,65424,65425,65426,65427,65428,65429,65430,

					65431,65432,65433,65434,65435,65436,65437,65438,65439,65440,65441,65442,65443,65444,65445,

					65446,65447,65448,65449,65450,65451,65452,65453,65454,65455,65456,65457,65458,65459,65460,

					65461,65462,65463,65464,65465,65466,65467,65468,65469,65470,65471,65472,65473,65474,65475,

					65476,65477,65478,65479,65480,65481,65482,65483,65484,65485,65486,65487,65488,65489,65490,

					65491,65492,65493,65494,65495,65496,65497,65498,65499,65500,65501,65502,65503,65504,65505,

					65506,65507,65508,65509,65510,65511,65512,65513,65514,65515,65516,65517,65518,65519,65520,

					65521,65522,65523,65524,65525,65526,65527,65528,65529,65530,65531,65532,65533,65534};
				p_tab->number=162;
				p_tab->maxsize=16;
				for (i=0;i<17;i++)
					p_tab->code_size[i]=bits[i];
				for (i=0;i<p_tab->number;i++)
					p_tab->symbols[i]=symb[i];
				for (i=0;i<p_tab->number;i++)
					p_tab->codes[i]=code[i];
			}
			break;
		case 17:{	//ac crominance
				int bits[]={0,0,2,1,2,4,4,3,4,7,5,4,4,0,1,2,119};
				int symb[]={
					0x00,0x01,0x02,0x03,0x11,0x04,0x05,0x21,0x31,0x06,0x12,0x41,0x51,0x07,0x61,0x71,0x13,0x22,

					0x32,0x81,0x08,0x14,0x42,0x91,0xa1,0xb1,0xc1,0x09,0x23,0x33,0x52,0xf0,0x15,0x62,0x72,0xd1,

					0x0a,0x16,0x24,0x34,0xe1,0x25,0xf1,0x17,0x18,0x19,0x1a,0x26,0x27,0x28,0x29,0x2a,0x35,0x36,

					0x37,0x38,0x39,0x3a,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x53,0x54,0x55,0x56,0x57,0x58,

					0x59,0x5a,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,

					0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,

					0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,

					0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,

					0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa};
				int code[]={
					0,1,4,10,11,24,25,26,27,56,57,58,59,120,121,122,246,247,248,249,500,501,502,503,504,505,

					506,1014,1015,1016,1017,1018,2038,2039,2040,2041,4084,4085,4086,4087,16352,32706,32707,

					65416,65417,65418,65419,65420,65421,65422,65423,65424,65425,65426,65427,65428,65429,65430,

					65431,65432,65433,65434,65435,65436,65437,65438,65439,65440,65441,65442,65443,65444,65445,

					65446,65447,65448,65449,65450,65451,65452,65453,65454,65455,65456,65457,65458,65459,65460,

					65461,65462,65463,65464,65465,65466,65467,65468,65469,65470,65471,65472,65473,65474,65475,

					65476,65477,65478,65479,65480,65481,65482,65483,65484,65485,65486,65487,65488,65489,65490,

					65491,65492,65493,65494,65495,65496,65497,65498,65499,65500,65501,65502,65503,65504,65505,

					65506,65507,65508,65509,65510,65511,65512,65513,65514,65515,65516,65517,65518,65519,65520,

					65521,65522,65523,65524,65525,65526,65527,65528,65529,65530,65531,65532,65533,65534};
				p_tab->number=162;
				p_tab->maxsize=16;
				for (i=0;i<17;i++)
					p_tab->code_size[i]=bits[i];
				for (i=0;i<p_tab->number;i++)
					p_tab->symbols[i]=symb[i];
				for (i=0;i<p_tab->number;i++)
					p_tab->codes[i]=code[i];
			}
			break;
	}
}


void wrbuffer (FILE *fileout, int symb, unsigned char *free, unsigned char
*buff, int symb_size, int value, int value_size)
{
	unsigned char mask_t=0;
	int empty=0x00;

		if((*free<symb_size)&&(symb_size<=(*free+8)))
			{
				mask_t= symb >> (symb_size - *free);
				*buff=*buff | mask_t;
				fwrite(buff,1,1,fileout);
				if (*buff==0xff) fwrite(&empty,1,1,fileout);
				*buff=0x00;
				mask_t=(unsigned char) symb << (8 -  (symb_size - *free));
				*buff=*buff | mask_t;
				*free=8 - (symb_size - *free);
				if(*free==0)
				{
					fwrite(buff,1,1,fileout);
					if (*buff==0xff) fwrite(&empty,1,1,fileout);
					*buff=0x00;
					*free=8;
				}
			}
		else if (symb_size>(*free+8))
			{
				mask_t=  symb >> (symb_size - *free);
				*buff=*buff | mask_t;
				fwrite(buff,1,1,fileout);
				if (*buff==0xff) fwrite(&empty,1,1,fileout);
				*buff= symb >> (symb_size -  (8 + *free));
				fwrite(buff,1,1,fileout);
				if (*buff==0xff) fwrite(&empty,1,1,fileout);
				*buff=0x00;
				mask_t=(unsigned char) symb << (16 + *free - symb_size);
				*buff=*buff | mask_t;
				*free=16 + *free - symb_size;
			}
		else
		{
			mask_t=(unsigned char) symb <<  (*free - symb_size);
			*buff=mask_t | *buff;
			*free=*free - symb_size;
			if(*free==0)
			{
				fwrite(buff,1,1,fileout);
				if (*buff==0xff) fwrite(&empty,1,1,fileout);
				*buff=0x00;
				*free=8;
			}
		}
		if(value!=0)
		{
			if(value<0)
			{
				value--;
				value=value<<(sizeof(int)*8- value_size);
				value=value>>(sizeof(int)*8- value_size);
			}
			if((*free<value_size)&&(value_size<=(*free+8)))
			{
				mask_t= value >> (value_size - *free);
				*buff=*buff | mask_t;
				fwrite(buff,1,1,fileout);
				if (*buff==0xff) fwrite(&empty,1,1,fileout);
				*buff=0x00;
				mask_t= value << (8 -  (value_size - *free));
				*buff=*buff | mask_t;
				*free=8 - (value_size - *free);
				if(*free==0)
				{
					fwrite(buff,1,1,fileout);
					if (*buff==0xff) fwrite(&empty,1,1,fileout);
					*buff=0x00;
					*free=8;
				}
			}
			else if (value_size>(*free+8))
			{
				mask_t= value >> (value_size - *free);
				*buff=*buff | mask_t;
				fwrite(buff,1,1,fileout);
				if (*buff==0xff) fwrite(&empty,1,1,fileout);
				*buff= value >> (value_size -  (8 + *free));
				fwrite(buff,1,1,fileout);
				if (*buff==0xff) fwrite(&empty,1,1,fileout);
				*buff=0x00;
				mask_t= value << (16 + *free - value_size);
				*buff=*buff | mask_t;
				*free=16 + *free - value_size;
			}
			else
			{
				mask_t= value <<  (*free - value_size);
				*buff=mask_t | *buff;
				*free=*free - value_size;
				if(*free==0)
				{
					fwrite(buff,1,1,fileout);
					if (*buff==0xff) fwrite(&empty,1,1,fileout);
					*buff=0x00;
					*free=8;
				}
			}
		}
}

void arrange_table (int *ptab)
{
	int
i,temp[64],zig_zag[]={0,1,8,16,9,2,3,10,17,24,32,25,18,11,4,5,12,19,26,33,40,48,41,34,27,20,13,6,7,14,21,28,35,42,49,56,57,50,43,36,29,22,15,23,30,37,44,51,58,59,52,45,38,31,39,46,53,60,61,54,47,55,62,63};

	for (i=0;i<64;i++)
		temp[i]=*(ptab+zig_zag[i]);
	for (i=0;i<64;i++)
		*(ptab+i)=temp[i];
}


void dhtsegment (huffman_tab tab, FILE *fileout)
{
	int i;

	unsigned char write;
	
	write=tab.id;
	
	fwrite(&write,1,1,fileout);

//	integer=tab.number;
//	i=1;

	for(i=1;i<17;i++)
	{  
	   write=tab.code_size[i];
		fwrite(&write,1,1,fileout);
	}
	for (i=0;i<tab.number;i++)
	{
	   write=tab.symbols[i];
		fwrite(&write,1,1,fileout);	
	}
}

// int findcode (int symbol, huffman_tab table, int *dim)
// {
// 	int i,k,y;
// 	for (i=0; i<256; i++)
// 		if(symbol==table.symbols[i])
// 		{
// 			y=0;
// 			for(k=0;k<17;k++)
// 			{
// 				y+=table.code_size[k];
// 				if (y>i)
// 				{
// 					*dim=k;
// 					return(table.codes[i]);
// 				}
// 			}
// 		}
// }

int findcode (int symbol, huffman_tab table, int *dim)
{
	int ret = -1;
	int i,k,y;
	*dim = 0;
	for (i=0; i<256 && ret == -1; i++)
		if(symbol==table.symbols[i])
		{
			y=0;
			for(k=0;k<17 && ret == -1; k++)
			{
				y+=table.code_size[k];
				if (y>i)
				{
					*dim=k;
					ret = table.codes[i];
				}
			}
		}
	return ret;
}

void writeheaders (FILE *fileout, info infoimm, int *lumtab, int *cromtab,
huffman_tab taclum,huffman_tab taccrom,huffman_tab tdclum,huffman_tab tdccrom)
{
	unsigned char header;
	unsigned char length;
	int i;
	unsigned char empty=0x00;
	
	unsigned char full=0xff;
	char integer;
	
	unsigned char app0segment[]={0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00}; //{1,1,0,0,1,0,1,0,0};
	unsigned char SOFsegment[]=	{0x08,0x03,0x01,0x22,0x00,0x02,0x011,0x01,0x03,0x011,0x01};	//{8,3,1,34,0,2,17,1,3,17,1};
	unsigned char SOFsegmentGREY[]= {0x01,0x01,0x22,0x00};	//{1,1,34,0};
	unsigned char SOSsegment[]= {0x03,0x01,0x02,0x03,0x00,0x3f,0x00};		//{3,1,2,3,0,63,0};
	unsigned char SOSsegmentGREY[]= {0x01,0x01,0x00,0x3f,0x00};	//{1,1,0,63,0};
	
	
	
	// int app0segment[]={1,1,0,0,1,0,1,0,0};
	// int SOFsegment[]={8,3,1,34,0,2,17,1,3,17,1};
	// int SOFsegmentGREY[]={1,1,34,0};
	// int SOSsegment[]={3,1,2,3,0,63,0};
	// int SOSsegmentGREY[]={1,1,0,63,0};
	char String[5]="JFIF";

	header=0xd8; //SOI segment
	
	fwrite(&full,1,1,fileout);
	fwrite(&header,1,1,fileout);

	header=0xe0; //APP0 segment
		fwrite(&full,1,1,fileout);
		fwrite(&header,1,1,fileout);
		length=0x10;
		fwrite(&empty,1,1,fileout);
		fwrite(&length,1,1,fileout);
		fwrite(&String,5,1,fileout);
		for (i=0;i<9;i++)
			fwrite(&app0segment[i],1,1,fileout);


	header=0xdb; //DQT segment
		fwrite(&full,1,1,fileout);
		fwrite(&header,1,1,fileout);
		length=0x43;
		fwrite(&empty,1,1,fileout);
		fwrite(&length,1,1,fileout);
		integer=IDTABLUM;
		fwrite(&integer,1,1,fileout);
		for (i=0;i<64;i++)
		   {
			unsigned char write;
			write=lumtab[i];
			fwrite(&write,1,1,fileout);
			}
	if(infoimm.color==1)
	{
		header=0xdb; //DQT segment
			fwrite(&full,1,1,fileout);
			fwrite(&header,1,1,fileout);
			length=0x43;
			fwrite(&empty,1,1,fileout);
			fwrite(&length,1,1,fileout);
			integer=IDTABCROM;
			fwrite(&integer,1,1,fileout);
			for (i=0;i<64;i++)
			{
				unsigned char write;
			   write=cromtab[i];
	  			fwrite(&write,1,1,fileout);
        	}
	}
	header=0xc0; //SOF segment
		fwrite(&full,1,1,fileout);
		fwrite(&header,1,1,fileout);
		if(infoimm.color==1) length=0x11;
		else length=0x0b;
		fwrite(&empty,1,1,fileout);
		fwrite(&length,1,1,fileout);
		fwrite(&SOFsegment[0],1,1,fileout);
		integer=(infoimm.H>>8);
		fwrite(&integer,1,1,fileout);
		integer=infoimm.H;
		fwrite(&integer,1,1,fileout);
		integer=(infoimm.L>>8);
		fwrite(&integer,1,1,fileout);
		integer=infoimm.L;
		fwrite(&integer,1,1,fileout);
		if(infoimm.color==1)
			for (i=1;i<11;i++)
				fwrite(&SOFsegment[i],1,1,fileout);
		else
			for (i=0;i<4;i++)
				fwrite(&SOFsegmentGREY[i],1,1,fileout);

	header=0xc4; //DHT segment
		fwrite(&full,1,1,fileout);
		fwrite(&header,1,1,fileout);
		length=2+1+16+12;
		fwrite(&empty,1,1,fileout);
		fwrite(&length,1,1,fileout);
		dhtsegment(tdclum, fileout);

	header=0xc4; //DHT segment
		fwrite(&full,1,1,fileout);
		fwrite(&header,1,1,fileout);
		length=2+1+16+162;
		fwrite(&empty,1,1,fileout);
		fwrite(&length,1,1,fileout);
		dhtsegment(taclum, fileout);
	if(infoimm.color==1)
	{
		header=0xc4; //DHT segment
			fwrite(&full,1,1,fileout);
			fwrite(&header,1,1,fileout);
			length=2+1+16+tdccrom.number;
			fwrite(&empty,1,1,fileout);
			fwrite(&length,1,1,fileout);
			dhtsegment(tdccrom, fileout);
		header=0xc4; //DHT segment
			fwrite(&full,1,1,fileout);
			fwrite(&header,1,1,fileout);
			length=2+1+16+taccrom.number;
			fwrite(&empty,1,1,fileout);
			fwrite(&length,1,1,fileout);
			dhtsegment(taccrom, fileout);
	}

	header=0xda; //SOS segment
		fwrite(&full,1,1,fileout);
		fwrite(&header,1,1,fileout);
		if(infoimm.color==1) length=0x0c;
		else length=0x08;
		fwrite(&empty,1,1,fileout);
		fwrite(&length,1,1,fileout);
		if(infoimm.color==1)
		{
		for(i=0;i<2;i++)
			fwrite(&SOSsegment[i],1,1,fileout);
		integer=(tdclum.id<<4)+(taclum.id-16);
		fwrite(&integer,1,1,fileout);
		fwrite(&SOSsegment[2],1,1,fileout);
		integer=(tdccrom.id<<4)+(taccrom.id-16);
		fwrite(&integer,1,1,fileout);
		fwrite(&SOSsegment[3],1,1,fileout);
		integer=(tdccrom.id<<4)+(taccrom.id-16);
		fwrite(&integer,1,1,fileout);
		for (i=4;i<7;i++)
			fwrite(&SOSsegment[i],1,1,fileout);
		}
		else
		{
		for(i=0;i<2;i++)
			fwrite(&SOSsegmentGREY[i],1,1,fileout);
		integer=(tdclum.id<<4)+(taclum.id-16);
		fwrite(&integer,1,1,fileout);
		for (i=2;i<5;i++)
			fwrite(&SOSsegmentGREY[i],1,1,fileout);
		}
}

void writescan (int old, int block[], unsigned char *free, unsigned char *buff,
FILE *fileout, huffman_tab tac,huffman_tab tdc, info infoimm)
{
	int i,run=0,size,value,ssss,symbol,rs;

	for (i=0;i<64;i++)
	{
		if (i==0)
		{
			value=block[0]-old;
			if (value==0) ssss=0;
			else ssss=1+(float)(log(fabs(value))/log(2));
			symbol=findcode(ssss,tdc,&size);
			wrbuffer(fileout,symbol,free,buff,size,value,ssss);
		}
		else if (i==63)
		{
			symbol=findcode(0,tac,&size);
			wrbuffer(fileout,symbol,free,buff,size,0,0);
		}
		else
		{
			if (block[i]==0)
			{
				run++;
			}
			else
			{
				while(run>15)
				{
					symbol=findcode(240,tac,&size);
					wrbuffer(fileout,symbol,free,buff,size,0,0);
					run-=16;
				}
				value=block[i];
				ssss=(1+(float)(log(fabs(value))/log(2)));
				rs=(run<<4)+ssss;
				symbol=findcode(rs,tac,&size);
				wrbuffer(fileout,symbol,free,buff,size,value,ssss);
				run=0;
			}

		}
	}
}

void write_end_of_image (FILE *fileout)
{
	unsigned char header,full=0xff;
	header=0xd9; //EOI segment
	fwrite(&full,1,1,fileout);
	fwrite(&header,1,1,fileout);

}


