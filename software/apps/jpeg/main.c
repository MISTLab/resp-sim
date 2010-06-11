#include <math.h>


#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "tipi.h"
/*
#include "xtmrctr_l.h" 		// Per l'uso dei timer
#include "xutil.h"
*/

//Prototipi
void RGBtoYUV(unsigned char *component1, unsigned char *component2, unsigned char *component3);
rows rdppm (info *pinf, FILE *in);
rows expand_image(rows image, int numrows, int numblocks,int *numrowstemp,int *numblockstemp);
void downsample (block *p_bl, block *crom_pbl, int offset);
void set_quantization_tbl(int lvl, int *tab);
void correct_quantization_tbl(int *tab, int*tab_corr);
void DCT_and_quantization (block *pblock, int lum_tbl[], int crom_tbl[], intblock *p_image, int crom_flag);
void arrange (intblock *pint);
void initialize_huff_tbl(huffman_tab *p_tab,int id);
void arrange_table (int *ptab);
void writeheaders (FILE *fileout, info infoimm, int *lumtab, int *cromtab,huffman_tab taclum,
		huffman_tab taccrom,huffman_tab tdclum,huffman_tab tdccrom);
void writescan (int old, int block[], unsigned char *free, unsigned char *buff,
		FILE *fileout, huffman_tab tac,huffman_tab tdc, info infoimm);
int findcode (int symbol, huffman_tab table, int *dim);
void wrbuffer (FILE *fileout, int symb, unsigned char *free, unsigned char *buff, int symb_size,
		int value, int value_size);
void write_end_of_image (FILE *fileout);

int main(int argc, char *argv[])
{

#if XPAR_MICROBLAZE_0_USE_ICACHE
      microblaze_init_icache_range(0, XPAR_MICROBLAZE_0_CACHE_BYTE_SIZE);
      microblaze_enable_icache();
   #endif

   #if XPAR_MICROBLAZE_0_USE_DCACHE
      microblaze_init_dcache_range(0, XPAR_MICROBLAZE_0_DCACHE_BYTE_SIZE);
      microblaze_enable_dcache();
   #endif
	
	rows imago;
	int crom_flag,i,j,lvl,temprows,tempblocks,offset[]={0,4,32,36},simb,size;
	int  luminance_table[]={16, 11, 10, 16,  24,  40,  51,  61,
				12, 12, 14, 19,  26,  58,  60,  55,
				14, 13, 16, 24,  40,  57,  69,  56,
				14, 17, 22, 29,  51,  87,  80,  62,
				18, 22, 37, 56,  68, 109, 103,  77,
				24, 35, 55, 64,  81, 104, 113,  92,
				49, 64, 78, 87, 103, 121, 120, 101,
				72, 92, 95, 98, 112, 100, 103,  99};

	int crominance_table[]={17, 18, 24, 47, 99, 99, 99, 99,
				18, 21, 26, 66, 99, 99, 99, 99,
				24, 26, 56, 99, 99, 99, 99, 99,
				47, 66, 99, 99, 99, 99, 99, 99,
				99, 99, 99, 99, 99, 99, 99, 99,
				99, 99, 99, 99, 99, 99, 99, 99,
				99, 99, 99, 99, 99, 99, 99, 99,
				99, 99, 99, 99, 99, 99, 99, 99,};
	int lum_tab_corr[64], crom_tab_corr[64];
				
	info infoimago;
	int_rows intimago;
	huffman_tab tbl_dclum, tbl_aclum, tbl_dccrom, tbl_accrom;
	FILE *file_in, *file_out;
	unsigned  char buffer,field_free_space=8;
	buffer = 0x0;		//UCCIDI UCCIDI UCCIDI

/*
   xil_printf("\n\rINIZIO COMPRESSIONE!!!");
*/
	if (argc > 1)
        {
           file_in=fopen(argv[1],"r");
        }
        else 
           file_in=fopen("./software/apps/jpeg/img.ppm","r");			//binario?

        if (!file_in) return(0);
	
	//Avvio del timer
/*
	XTmrCtr_mLoadTimerCounterReg( XPAR_OPB_TIMER_1_BASEADDR, 0);
	XTmrCtr_mSetControlStatusReg( XPAR_OPB_TIMER_1_BASEADDR, 0, XTC_CSR_ENABLE_TMR_MASK );
*/
	imago=rdppm(&infoimago, file_in);
	// Stop timer e stampa dei cicli di computazione
/*
	XTmrCtr_mDisable(XPAR_OPB_TIMER_1_BASEADDR,0);
	xil_printf("\n\rlettura file: %d\n\r",XTmrCtr_mGetTimerCounterReg(XPAR_OPB_TIMER_1_BASEADDR,0));
*/
	
	if (imago==NULL) return(0);

	fclose(file_in);



	intimago=(p_intblock *) malloc (infoimago.numrows*sizeof(p_intblock));
	for (i=0;i<infoimago.numrows;i++)
		*(intimago+i)=(intblock *) malloc (infoimago.numblocks*sizeof(intblock));

	if (infoimago.color==1)
	{	
		//Avvio del timer
/*
		XTmrCtr_mLoadTimerCounterReg( XPAR_OPB_TIMER_1_BASEADDR, 0);
		XTmrCtr_mSetControlStatusReg( XPAR_OPB_TIMER_1_BASEADDR, 0, XTC_CSR_ENABLE_TMR_MASK );
*/
		for (i=0;i<infoimago.numrows;i++)
			for (j=0;j<infoimago.numblocks;j++)
				RGBtoYUV(&(*(imago+i)+j)->comp1[0],&(*(imago+i)+j)->comp2[0],&(*(imago+i)+j)->comp3[0]);
		// Stop timer e stampa dei cicli di computazione
/*
		XTmrCtr_mDisable(XPAR_OPB_TIMER_1_BASEADDR,0);
		xil_printf("\n\rrgb to yuv: %d\n\r",XTmrCtr_mGetTimerCounterReg(XPAR_OPB_TIMER_1_BASEADDR,0));
*/
	}

	if (infoimago.color==1)
	{
		rows tempimago;
		
		//Avvio del timer
/*
		XTmrCtr_mLoadTimerCounterReg( XPAR_OPB_TIMER_1_BASEADDR, 0);
		XTmrCtr_mSetControlStatusReg( XPAR_OPB_TIMER_1_BASEADDR, 0, XTC_CSR_ENABLE_TMR_MASK );
*/
		tempimago=expand_image(imago,infoimago.numrows,infoimago.numblocks,&temprows,&tempblocks);

		for (i=0;i<temprows;i++)
			for (j=0;j<tempblocks;j++)
				downsample((*(tempimago+i)+j),(*(imago+i/2)+j/2),offset[j%2+(i%2)*2]);
		for (i=0;i<temprows;i++)
		free(*(tempimago+i));
		free(tempimago);
		
		// Stop timer e stampa dei cicli di computazione
/*
		XTmrCtr_mDisable(XPAR_OPB_TIMER_1_BASEADDR,0);
		xil_printf("\n\rdownsample e espandsione: %d\n\r",XTmrCtr_mGetTimerCounterReg(XPAR_OPB_TIMER_1_BASEADDR,0));
*/
	}

	lvl= 80;
	
	//Avvio del timer
/*
	XTmrCtr_mLoadTimerCounterReg( XPAR_OPB_TIMER_1_BASEADDR, 0);
	XTmrCtr_mSetControlStatusReg( XPAR_OPB_TIMER_1_BASEADDR, 0, XTC_CSR_ENABLE_TMR_MASK );
*/
	set_quantization_tbl(lvl, &luminance_table[0]);
	set_quantization_tbl(lvl, &crominance_table[0]);
	correct_quantization_tbl(luminance_table, lum_tab_corr);
	correct_quantization_tbl(crominance_table, crom_tab_corr);
	// Stop timer e stampa dei cicli di computazione
/*
	XTmrCtr_mDisable(XPAR_OPB_TIMER_1_BASEADDR,0);
	xil_printf("\n\rrset quantization table: %d\n\r",XTmrCtr_mGetTimerCounterReg(XPAR_OPB_TIMER_1_BASEADDR,0));
*/

	//Avvio del timer
/*
	XTmrCtr_mLoadTimerCounterReg( XPAR_OPB_TIMER_1_BASEADDR, 0);
	XTmrCtr_mSetControlStatusReg( XPAR_OPB_TIMER_1_BASEADDR, 0, XTC_CSR_ENABLE_TMR_MASK );
*/
	for (i=0;i<infoimago.numrows;i++)
		for (j=0;j<infoimago.numblocks;j++)
		{
			if ((infoimago.color==1)&&(i<temprows/2)&&(j<tempblocks/2)) crom_flag=1;
			else crom_flag=0;

			DCT_and_quantization(*(imago+i)+j,lum_tab_corr,crom_tab_corr,*(intimago+i)+j,crom_flag);
		}
	
	for (i=0;i<infoimago.numrows;i++)
		free(*(imago+i));
	free(imago);

	for (i=0;i<infoimago.numrows;i++)
		for (j=0;j<infoimago.numblocks;j++)
			arrange(*(intimago+i)+j);
	// Stop timer e stampa dei cicli di computazione
/*
	XTmrCtr_mDisable(XPAR_OPB_TIMER_1_BASEADDR,0);
	xil_printf("\n\rdct and quantization: %d\n\r",XTmrCtr_mGetTimerCounterReg(XPAR_OPB_TIMER_1_BASEADDR,0));
*/
//Avvio del timer
/*XTmrCtr_mLoadTimerCounterReg( XPAR_OPB_TIMER_1_BASEADDR, 0);
XTmrCtr_mSetControlStatusReg( XPAR_OPB_TIMER_1_BASEADDR, 0, XTC_CSR_ENABLE_TMR_MASK );
*/
initialize_huff_tbl(&tbl_dclum,0);
initialize_huff_tbl(&tbl_aclum,16);
initialize_huff_tbl(&tbl_dccrom,1);
initialize_huff_tbl(&tbl_accrom,17);

arrange_table(&luminance_table[0]);
arrange_table(&crominance_table[0]);
if (argc > 2)
   file_out=fopen(argv[2],"w");
else
   file_out=fopen("img_out.jpg","w");			//binario??
writeheaders (file_out, infoimago, &luminance_table[0], &crominance_table[0],
tbl_aclum,tbl_accrom,tbl_dclum,tbl_dccrom);

if (infoimago.color==1)
{
	int oldlum=0, oldcrom1=0, oldcrom2=0;
	for (i=0;i<infoimago.numrows;i+=2)
		for (j=0;j<infoimago.numblocks;j+=2)
		{

			writescan(oldlum,(*(intimago+i)+j)->intcomp1,&field_free_space,&buffer,file_out,tbl_aclum,tbl_dclum,infoimago);
			if ((j+1)<infoimago.numblocks)
			{

				writescan((*(intimago+i)+j)->intcomp1[0],(*(intimago+i)+j+1)->intcomp1,&field_free_space,&buffer,file_out,tbl_aclum,tbl_dclum,infoimago);
				if ((i+1)<infoimago.numrows)
				{

					writescan((*(intimago+i)+j+1)->intcomp1[0],(*(intimago+i+1)+j)->intcomp1,&field_free_space,&buffer,file_out,tbl_aclum,tbl_dclum,infoimago);

					writescan((*(intimago+i+1)+j)->intcomp1[0],(*(intimago+i+1)+j+1)->intcomp1,&field_free_space,&buffer,file_out,tbl_aclum,tbl_dclum,infoimago);
					oldlum=(*(intimago+i+1)+j+1)->intcomp1[0];
				}
				else
				{
					simb=findcode(0,tbl_dclum,&size);
					wrbuffer(file_out,simb,&field_free_space,&buffer,size,0,0);
					simb=findcode(0,tbl_aclum,&size);
					wrbuffer(file_out,simb,&field_free_space,&buffer,size,0,0);
					simb=findcode(0,tbl_dclum,&size);
					wrbuffer(file_out,simb,&field_free_space,&buffer,size,0,0);
					simb=findcode(0,tbl_aclum,&size);
					wrbuffer(file_out,simb,&field_free_space,&buffer,size,0,0);
					oldlum=(*(intimago+i)+j+1)->intcomp1[0];
				}
			}
			else
			{
				simb=findcode(0,tbl_dclum,&size);
				wrbuffer(file_out,simb,&field_free_space,&buffer,size,0,0);
				simb=findcode(0,tbl_aclum,&size);
				wrbuffer(file_out,simb,&field_free_space,&buffer,size,0,0);
				if ((i+1)<infoimago.numrows)
				{

				writescan((*(intimago+i)+j)->intcomp1[0],(*(intimago+i+1)+j)->intcomp1,&field_free_space,&buffer,file_out,tbl_aclum,tbl_dclum,infoimago);
				simb=findcode(0,tbl_dclum,&size);
				wrbuffer(file_out,simb,&field_free_space,&buffer,size,0,0);
				simb=findcode(0,tbl_aclum,&size);
				wrbuffer(file_out,simb,&field_free_space,&buffer,size,0,0);
				oldlum=(*(intimago+i+1)+j)->intcomp1[0];
				}
				else
				{
					simb=findcode(0,tbl_dclum,&size);
					wrbuffer(file_out,simb,&field_free_space,&buffer,size,0,0);
					simb=findcode(0,tbl_aclum,&size);
					wrbuffer(file_out,simb,&field_free_space,&buffer,size,0,0);
					simb=findcode(0,tbl_dclum,&size);
					wrbuffer(file_out,simb,&field_free_space,&buffer,size,0,0);
					simb=findcode(0,tbl_aclum,&size);
					wrbuffer(file_out,simb,&field_free_space,&buffer,size,0,0);
				}
			}


			writescan(oldcrom1,(*(intimago+i/2)+j/2)->intcomp2,&field_free_space,&buffer,file_out,tbl_accrom,tbl_dccrom,infoimago);

			writescan(oldcrom2,(*(intimago+i/2)+j/2)->intcomp3,&field_free_space,&buffer,file_out,tbl_accrom,tbl_dccrom,infoimago);
			oldcrom1=(*(intimago+i/2)+j/2)->intcomp2[0];
			oldcrom2=(*(intimago+i/2)+j/2)->intcomp3[0];

		}
	}
	else
	{
	int oldlum=0;
	for (i=0;i<infoimago.numrows;i++)
		for (j=0;j<infoimago.numblocks;j++)
		{

			writescan(oldlum,(*(intimago+i)+j)->intcomp1,&field_free_space,&buffer,file_out,tbl_aclum,tbl_dclum,infoimago);
			oldlum=(*(intimago+i)+j)->intcomp1[0];
		}
	}
write_end_of_image(file_out);
fclose(file_out);

// Stop timer e stampa dei cicli di computazione
/*
XTmrCtr_mDisable(XPAR_OPB_TIMER_1_BASEADDR,0);
xil_printf("\n\rcodifiche entropiche: %d\n\r",XTmrCtr_mGetTimerCounterReg(XPAR_OPB_TIMER_1_BASEADDR,0));

xil_printf("\n\rFINE COMPRESSIONE!!!");
*/
 return 0;
}

