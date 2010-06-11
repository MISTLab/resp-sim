#include "tipi.h"
#include <stdlib.h>
#include <string.h>

rows expand_image(rows image, int numrows, int numblocks,int *numrowstemp,int
*numblockstemp)
{
	rows temp;
	int i;

	if(numrows%2!=0) *numrowstemp=numrows+1;
	else *numrowstemp=numrows;
	if(numblocks%2!=0) *numblockstemp=numblocks+1;
	else *numblockstemp=numblocks;
	temp=(p_block *) malloc (*numrowstemp * sizeof(p_block));
	for (i=0;i<*numrowstemp;i++)
		*(temp+i)=(block *) malloc (*numblockstemp * sizeof(block));
	for(i=0;i<numrows;i++)
		memcpy(*(temp+i),*(image+i),numblocks * sizeof(block));
	if(numrows%2!=0)
	{
		memcpy(*(temp+(*numrowstemp)-1),*(temp+(*numrowstemp)-2),*numblockstemp *
sizeof(block));
	}

	if(numblocks%2!=0)
	{
		for (i=0;i<*numrowstemp;i++)
		{
			block *p_bfrom,*p_bto;
			p_bfrom=*(temp+i)+(*numblockstemp)-2;
			p_bto=*(temp+i)+(*numblockstemp)-1;
			memcpy(p_bto,p_bfrom,sizeof(block));
		}
	}
	return(temp);
}


void downsample (block *p_bl, block *crom_pbl, int offset)
{
	int i,pos[]={0,2,4,6,16,18,20,22,32,34,36,38,48,50,52,54};
	int crompos[]={0,1,2,3,8,9,10,11,16,17,18,19,24,25,26,27};

	for (i=0;i<16;i++)
	{

crom_pbl->comp2[crompos[i]+offset]=(p_bl->comp2[pos[i]]+p_bl->comp2[pos[i]+1]+p_bl->comp2[pos[i]+8]+p_bl->comp2[pos[i]+9])/4;

crom_pbl->comp3[crompos[i]+offset]=(p_bl->comp3[pos[i]]+p_bl->comp3[pos[i]+1]+p_bl->comp3[pos[i]+8]+p_bl->comp3[pos[i]+9])/4;
	}
}


