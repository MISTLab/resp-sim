#include "tipi.h"

void set_quantization_tbl(int lvl, int *tab)
{
	int i, cost, full;
	if (lvl>50)
	{
		cost=50/(100-lvl);
		for (i=0; i<64; i++)
		{
			if (*(tab+i)%cost!=0) full=1;
			else full=0;
			if ((*(tab+i)/=cost)<1) *(tab+i)=1;
			else *(tab+i)+=full;
		}
	}
	else
	{
		for (i=0; i<64; i++)
		{
			if ((*(tab+i)*= (float)50/(float)lvl )>255) *(tab+i)=255;
		}
	}
}

void correct_quantization_tbl(int *tab, int*tab_corr)
{
	int corr[]={8,	11,	11,	10,	15,	10,	9,	14,
				14,	9,	8,	13,	14,	13,	8,	6,
				11,	12,	12,	11,	6,	4,	9,	10,
				11,	10,	9,	4,	2,	6,	8,	9,
				9,	8,	6,	2,	3,	6,	7,	8,
				7,	6,	3,	3,	5,	6,	6,	5,
				3,	3,	4,	5,	4,	3,	2,	3,
				3,	2,	2,	2,	2,	1,	1,	1};
	int i;			
	
	for(i=0; i<64; i++)
		tab_corr[i] = tab[i]*corr[i];
}


void DCT_and_quantization (block *pblock, int lum_tbl[], int crom_tbl[],
			intblock *p_image, int crom_flag)
{
	int i,j,x[9],y[9],elem1[64], elem2[64], elem3[64];
	float temp;

	for (i=0; i<64; i++)
	{

		elem1[i]=(int) pblock->comp1[i]-128;
		if(crom_flag==1)
		{
			elem2[i]=(int) pblock->comp2[i]-128;
			elem3[i]=(int) pblock->comp3[i]-128;
		}

	}
	
	
	int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
	int tmp10, tmp11, tmp12, tmp13;
	int z1, z2, z3, z4, z5;
	int z11, z13;
	int *dataptr;
	int *resptr;
	int ctr;

	/* row 1-D DCT */
	dataptr = elem1;
	resptr = elem1;
	for (ctr=7; ctr>=0; ctr--) {
		tmp0 = dataptr[0] + dataptr[7];
		tmp0 = tmp0 << 8;
		tmp7 = dataptr[0] - dataptr[7];
		tmp7 = tmp7 << 8;
		tmp1 = dataptr[1] + dataptr[6];
		tmp1 = tmp1 << 8;
		tmp6 = dataptr[1] - dataptr[6];
		tmp6 = tmp6 << 8;
		tmp2 = dataptr[2] + dataptr[5];
		tmp2 = tmp2 << 8;
		tmp5 = dataptr[2] - dataptr[5];
		tmp5 = tmp5 << 8;
		tmp3 = dataptr[3] + dataptr[4];
		tmp3 = tmp3 << 8;
		tmp4 = dataptr[3] - dataptr[4];
		tmp4 = tmp4 << 8;
		/* even part */
		tmp10 = tmp0 + tmp3; /* step 2 */
		tmp13 = tmp0 - tmp3;
		tmp11 = tmp1 + tmp2;
		tmp12 = tmp1 - tmp2;
		resptr[0] = (tmp10 + tmp11); /* step 3 */
		resptr[4] = (tmp10 - tmp11);
		z1 = (int)((float)(tmp12 + tmp13) * 0.707106781);	/* step4 */
		resptr[2] = tmp13 + z1; /* step 5 */
		resptr[6] = tmp13 - z1;
		/* Odd part */
		tmp10 = tmp4 + tmp5; /* step 2 */
		tmp11 = tmp5 + tmp6;
		tmp12 = tmp6 + tmp7;
		z5 = (int)((float)(tmp10 - tmp12) * 0.382683433);  /* step 4 */
		z2 = (int)(0.541196100 * (float)tmp10) + z5;;
		z4 = (int)(1.306562965 * (float)tmp12) + z5;
		z3 = (int)((float)tmp11 * 0.707106781);
		z11 = tmp7 + z3; /* step 6 */
		z13 = tmp7 - z3;
		resptr[5] = z13 + z2; /* step 7 */
		resptr[3] = z13 - z2;
		resptr[1] = z11 + z4;
		resptr[7] = z11 - z4;
		dataptr += 8; /* next row */
		resptr += 8;
	}

	/* column 1-D DCT */
	dataptr = elem1;
	resptr = p_image->intcomp1;
	for (ctr = 7; ctr >= 0; ctr--) {
		tmp0 = (dataptr[8*0] + dataptr[8*7]); /* step 1*/
		tmp7 = (dataptr[8*0] - dataptr[8*7]);
		tmp1 = (dataptr[8*1] + dataptr[8*6]);
		tmp6 = (dataptr[8*1] - dataptr[8*6]);
		tmp2 = (dataptr[8*2] + dataptr[8*5]);
		tmp5 = (dataptr[8*2] - dataptr[8*5]);
		tmp3 = (dataptr[8*3] + dataptr[8*4]);
		tmp4 = (dataptr[8*3] - dataptr[8*4]);
		/* even part */
		tmp10 = (tmp0 + tmp3); /* step 2 */
		tmp13 = (tmp0 - tmp3);
		tmp11 = (tmp1 + tmp2);
		tmp12 = (tmp1 - tmp2);
		resptr[8*0] = (tmp10 + tmp11)>>8; /* step 3 */
		resptr[8*4] = (tmp10 - tmp11)>>8;
		z1 = (int)((float)(tmp12 + tmp13) * 0.707106781);;
		resptr[8*2] = (tmp13 + z1)>>8; /* step 5 */
		resptr[8*6] = (tmp13 - z1)>>8;

		/* odd part */
		tmp10 = (tmp4 + tmp5); /* step 2 */
		tmp11 = (tmp5 + tmp6);
		tmp12 = (tmp6 + tmp7);
		tmp7 = tmp7;
		z5 = (tmp10 - tmp12); /* step 3 */
		tmp10 = tmp10;
		tmp11 = tmp11;
		tmp12 = tmp12;
		tmp7 = tmp7;
		z5 = (int)((float)(tmp10 - tmp12) * 0.382683433f);
		z2 = (int)(0.541196100 * (float)tmp10) + z5; /* step 4 */
		z4 = (int)(1.306562965 * (float)tmp12) + z5;
		z3 = (int)((float)tmp11 * 0.707106781);
		z11 = (tmp7 + z3); /* step 6 */
		z13 = (tmp7 - z3);
		resptr[8*5] = (z13 + z2)>>8; /* step 7 */
		resptr[8*3] = (z13 - z2)>>8;
		resptr[8*1] = (z11 + z4)>>8;
		resptr[8*7] = (z11 - z4)>>8;
		resptr++; /* next column */
		dataptr++;
	}
	
	if (crom_flag == 1) {
		/* row 1-D DCT */
		dataptr = elem2;
		resptr = elem2;
		for (ctr=7; ctr>=0; ctr--) {
			tmp0 = dataptr[0] + dataptr[7];
			tmp0 = tmp0 << 8;
			tmp7 = dataptr[0] - dataptr[7];
			tmp7 = tmp7 << 8;
			tmp1 = dataptr[1] + dataptr[6];
			tmp1 = tmp1 << 8;
			tmp6 = dataptr[1] - dataptr[6];
			tmp6 = tmp6 << 8;
			tmp2 = dataptr[2] + dataptr[5];
			tmp2 = tmp2 << 8;
			tmp5 = dataptr[2] - dataptr[5];
			tmp5 = tmp5 << 8;
			tmp3 = dataptr[3] + dataptr[4];
			tmp3 = tmp3 << 8;
			tmp4 = dataptr[3] - dataptr[4];
			tmp4 = tmp4 << 8;
			/* even part */
			tmp10 = tmp0 + tmp3; /* step 2 */
			tmp13 = tmp0 - tmp3;
			tmp11 = tmp1 + tmp2;
			tmp12 = tmp1 - tmp2;
			resptr[0] = (tmp10 + tmp11); /* step 3 */
			resptr[4] = (tmp10 - tmp11);
			z1 = (int)((float)(tmp12 + tmp13) * 0.707106781);	/* step4 */
			resptr[2] = tmp13 + z1; /* step 5 */
			resptr[6] = tmp13 - z1;
			/* Odd part */
			tmp10 = tmp4 + tmp5; /* step 2 */
			tmp11 = tmp5 + tmp6;
			tmp12 = tmp6 + tmp7;
			z5 = (int)((float)(tmp10 - tmp12) * 0.382683433);  /* step 4 */
			z2 = (int)(0.541196100 * (float)tmp10) + z5;;
			z4 = (int)(1.306562965 * (float)tmp12) + z5;
			z3 = (int)((float)tmp11 * 0.707106781);
			z11 = tmp7 + z3; /* step 6 */
			z13 = tmp7 - z3;
			resptr[5] = z13 + z2; /* step 7 */
			resptr[3] = z13 - z2;
			resptr[1] = z11 + z4;
			resptr[7] = z11 - z4;
			dataptr += 8; /* next row */
			resptr += 8;
		}

		/* column 1-D DCT */
		dataptr = elem2;
		resptr = p_image->intcomp2;
		for (ctr = 7; ctr >= 0; ctr--) {
			tmp0 = (dataptr[8*0] + dataptr[8*7]); /* step 1*/
			tmp7 = (dataptr[8*0] - dataptr[8*7]);
			tmp1 = (dataptr[8*1] + dataptr[8*6]);
			tmp6 = (dataptr[8*1] - dataptr[8*6]);
			tmp2 = (dataptr[8*2] + dataptr[8*5]);
			tmp5 = (dataptr[8*2] - dataptr[8*5]);
			tmp3 = (dataptr[8*3] + dataptr[8*4]);
			tmp4 = (dataptr[8*3] - dataptr[8*4]);
			/* even part */
			tmp10 = (tmp0 + tmp3); /* step 2 */
			tmp13 = (tmp0 - tmp3);
			tmp11 = (tmp1 + tmp2);
			tmp12 = (tmp1 - tmp2);
			resptr[8*0] = (tmp10 + tmp11)>>8; /* step 3 */
			resptr[8*4] = (tmp10 - tmp11)>>8;
			z1 = (int)((float)(tmp12 + tmp13) * 0.707106781);;
			resptr[8*2] = (tmp13 + z1)>>8; /* step 5 */
			resptr[8*6] = (tmp13 - z1)>>8;
	
			/* odd part */
			tmp10 = (tmp4 + tmp5); /* step 2 */
			tmp11 = (tmp5 + tmp6);
			tmp12 = (tmp6 + tmp7);
			tmp7 = tmp7;
			z5 = (tmp10 - tmp12); /* step 3 */
			tmp10 = tmp10;
			tmp11 = tmp11;
			tmp12 = tmp12;
			tmp7 = tmp7;
			z5 = (int)((float)(tmp10 - tmp12) * 0.382683433f);
			z2 = (int)(0.541196100 * (float)tmp10) + z5; /* step 4 */
			z4 = (int)(1.306562965 * (float)tmp12) + z5;
			z3 = (int)((float)tmp11 * 0.707106781);
			z11 = (tmp7 + z3); /* step 6 */
			z13 = (tmp7 - z3);
			resptr[8*5] = (z13 + z2)>>8; /* step 7 */
			resptr[8*3] = (z13 - z2)>>8;
			resptr[8*1] = (z11 + z4)>>8;
			resptr[8*7] = (z11 - z4)>>8;
			resptr++; /* next column */
			dataptr++;
		}
		/* row 1-D DCT */
		dataptr = elem3;
		resptr = elem3;
		for (ctr=7; ctr>=0; ctr--) {
			tmp0 = dataptr[0] + dataptr[7];
			tmp0 = tmp0 << 8;
			tmp7 = dataptr[0] - dataptr[7];
			tmp7 = tmp7 << 8;
			tmp1 = dataptr[1] + dataptr[6];
			tmp1 = tmp1 << 8;
			tmp6 = dataptr[1] - dataptr[6];
			tmp6 = tmp6 << 8;
			tmp2 = dataptr[2] + dataptr[5];
			tmp2 = tmp2 << 8;
			tmp5 = dataptr[2] - dataptr[5];
			tmp5 = tmp5 << 8;
			tmp3 = dataptr[3] + dataptr[4];
			tmp3 = tmp3 << 8;
			tmp4 = dataptr[3] - dataptr[4];
			tmp4 = tmp4 << 8;
			/* even part */
			tmp10 = tmp0 + tmp3; /* step 2 */
			tmp13 = tmp0 - tmp3;
			tmp11 = tmp1 + tmp2;
			tmp12 = tmp1 - tmp2;
			resptr[0] = (tmp10 + tmp11); /* step 3 */
			resptr[4] = (tmp10 - tmp11);
			z1 = (int)((float)(tmp12 + tmp13) * 0.707106781);	/* step4 */
			resptr[2] = tmp13 + z1; /* step 5 */
			resptr[6] = tmp13 - z1;
			/* Odd part */
			tmp10 = tmp4 + tmp5; /* step 2 */
			tmp11 = tmp5 + tmp6;
			tmp12 = tmp6 + tmp7;
			z5 = (int)((float)(tmp10 - tmp12) * 0.382683433);  /* step 4 */
			z2 = (int)(0.541196100 * (float)tmp10) + z5;;
			z4 = (int)(1.306562965 * (float)tmp12) + z5;
			z3 = (int)((float)tmp11 * 0.707106781);
			z11 = tmp7 + z3; /* step 6 */
			z13 = tmp7 - z3;
			resptr[5] = z13 + z2; /* step 7 */
			resptr[3] = z13 - z2;
			resptr[1] = z11 + z4;
			resptr[7] = z11 - z4;
			dataptr += 8; /* next row */
			resptr += 8;
		}

		/* column 1-D DCT */
		dataptr = elem3;
		resptr = p_image->intcomp3;
		for (ctr = 7; ctr >= 0; ctr--) {
			tmp0 = (dataptr[8*0] + dataptr[8*7]); /* step 1*/
			tmp7 = (dataptr[8*0] - dataptr[8*7]);
			tmp1 = (dataptr[8*1] + dataptr[8*6]);
			tmp6 = (dataptr[8*1] - dataptr[8*6]);
			tmp2 = (dataptr[8*2] + dataptr[8*5]);
			tmp5 = (dataptr[8*2] - dataptr[8*5]);
			tmp3 = (dataptr[8*3] + dataptr[8*4]);
			tmp4 = (dataptr[8*3] - dataptr[8*4]);
			/* even part */
			tmp10 = (tmp0 + tmp3); /* step 2 */
			tmp13 = (tmp0 - tmp3);
			tmp11 = (tmp1 + tmp2);
			tmp12 = (tmp1 - tmp2);
			resptr[8*0] = (tmp10 + tmp11)>>8; /* step 3 */
			resptr[8*4] = (tmp10 - tmp11)>>8;
			z1 = (int)((float)(tmp12 + tmp13) * 0.707106781);;
			resptr[8*2] = (tmp13 + z1)>>8; /* step 5 */
			resptr[8*6] = (tmp13 - z1)>>8;
	
			/* odd part */
			tmp10 = (tmp4 + tmp5); /* step 2 */
			tmp11 = (tmp5 + tmp6);
			tmp12 = (tmp6 + tmp7);
			tmp7 = tmp7;
			z5 = (tmp10 - tmp12); /* step 3 */
			tmp10 = tmp10;
			tmp11 = tmp11;
			tmp12 = tmp12;
			tmp7 = tmp7;
			z5 = (int)((float)(tmp10 - tmp12) * 0.382683433f);
			z2 = (int)(0.541196100 * (float)tmp10) + z5; /* step 4 */
			z4 = (int)(1.306562965 * (float)tmp12) + z5;
			z3 = (int)((float)tmp11 * 0.707106781);
			z11 = (tmp7 + z3); /* step 6 */
			z13 = (tmp7 - z3);
			resptr[8*5] = (z13 + z2)>>8; /* step 7 */
			resptr[8*3] = (z13 - z2)>>8;
			resptr[8*1] = (z11 + z4)>>8;
			resptr[8*7] = (z11 - z4)>>8;
			resptr++; /* next column */
			dataptr++;
		}
	
	}
	
	//inizio quantizzazione
	
	for(i=0;i<64;i++)
		{
		temp=(float) (p_image)->intcomp1[i]/lum_tbl[i];
		if (temp>=0) temp+=0.5;
			else temp-=0.5;
		(p_image)->intcomp1[i]=(int)temp;
		}

	if(crom_flag==1)
	{
		for(i=0;i<64;i++)
		{
		temp=(float) (p_image)->intcomp2[i]/crom_tbl[i];
		if (temp>=0) temp+=0.5;
			else temp-=0.5;
		(p_image)->intcomp2[i]=(int)temp;
		temp=(float) (p_image)->intcomp3[i]/crom_tbl[i];
		if (temp>=0) temp+=0.5;
			else temp-=0.5;
		(p_image)->intcomp3[i]=(int)temp;
		}
	}

	else
	{
		for(i=0;i<64;i++)
		{
			(p_image)->intcomp2 [i]=0;
			(p_image)->intcomp3 [i]=0;
		}

	}
}
