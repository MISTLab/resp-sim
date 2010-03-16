
//*****bwmorph_skel****
//bwmorph(slel,'skel',Inf)
//****************************************************************************
#include <stdio.h>
#include "fonctions.h"
#include "lut_skel.h"

//==========================================================================================
//bwmorph_skel
//==========================================================================================



void bwmorph_skel(int min_i, int min_j, int max_i, int max_j,double skel[M][N])
{

	int r,c;
	int i;
        int skel_logic[M][N], skel_out[M][N];

	for(r=0;r<=max_i;r++)
	{
		for(c=0;c<=max_j;c++)
		{
			skel_logic[r][c] = (skel[r][c] != 0);
		}
	}
	
	int egal =0;
	while(egal==0)
	{
		for(r=0;r<=max_i;r++)
		{
			for(c=0;c<=max_j;c++)
			{
				skel[r][c] = skel_logic[r][c];
			}
		}

		for(i=0;i<8;i++)
		{
			applylut(min_i,min_j,max_i,max_j,skel_logic,lutskel[i],skel_out);
			for(c=0;c<=max_j;c++)
			{
				for(r=0;r<=max_i;r++)
				{
					skel_logic[r][c] = skel_logic[r][c] & !skel_out[r][c];
				}
			}
		}
		egal =test_egalite_double(min_i,min_j,max_i,max_j,skel,skel_logic);
	}

}
