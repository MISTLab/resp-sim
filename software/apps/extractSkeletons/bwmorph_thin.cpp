
//*****bwmorph_thin****
//bwmorph(slel,'thin',Inf)
//****************************************************************************
#include <stdio.h>
#include "fonctions.h"
#include "lut_thin.h"

//==========================================================================================
//bwmorph_thin
//==========================================================================================



void bwmorph_thin(int min_i, int min_j, int max_i, int max_j,int lutlut[512], int c[M][N], int lookup[M][N])
{
	int i,j;
        int lastc[M][N];

	int done = 0;

	for(i=0;i<512;i++)
	{
		lutlut[i] = i;
	}

	while(done==0)
	{
		for(i=0;i<=max_i;i++)
		{
			for(j=0;j<=max_j;j++)
			{
				lastc[i][j] = c[i][j];
			}
		}

		applylut(min_i,min_j,max_i,max_j,c,lutlut,lookup);
		

		//First subiteration

		for(i=0;i<=max_i;i++)
		{
			for(j=0;j<=max_j;j++)
			{
				c[i][j] = c[i][j] & (!((lutthin1[lookup[i][j]] & lutthin2[lookup[i][j]]) & lutthin3[lookup[i][j]]));
			}
		}

		//Second subiteration
		
		applylut(min_i,min_j,max_i,max_j,c,lutlut,lookup);


		for(i=0;i<=max_i;i++)
		{
			for(j=0;j<=max_j;j++)
			{
				c[i][j] = c[i][j] & (!((lutthin1[lookup[i][j]] & lutthin2[lookup[i][j]]) & lutthin4[lookup[i][j]]));
			}
		}

		done =test_egalite_int(min_i,min_j,max_i,max_j,lastc,c);
	}

}