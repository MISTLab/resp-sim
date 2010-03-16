//*****bwmorph_spur****
//bwmorph(slel,'spur')
//****************************************************************************

#include "fonctions.h"
#include "lut_spur.h"


//==========================================================================================
//bwmorph_spur
//==========================================================================================


void bwmorph_spur(int min_i, int min_j, int max_i, int max_j,double skel[M][N])
{
	
	int r,c;
	int endPoints[M][N], newEndPoints[M][N], skel_init[M][N];
	//complement
	for(r=0;r<=max_i;r++)
	{
		for(c=0;c<=max_j;c++)
		{
			skel_init[r][c] = (!(int)skel[r][c]);
		}
	}
	
	//endPoints = applylut(skel_init,lutspur);
	
	applylut(min_i,min_j,max_i,max_j,skel_init,lutspur,endPoints);

	for(r=0;r<=max_i;r=r+2)
	{
		for(c=0;c<=max_j;c=c+2)
		{
			skel_init[r][c] = (skel_init[r][c] ^ endPoints[r][c]);
		}
	}

	//1
	//newEndPoints = applylut(skel_init,lutspur);	
	for(r=0;r<=max_i;r++)
	{
		for(c=0;c<=max_j;c++)
		{
			newEndPoints[r][c] = (endPoints[r][c] & lutspur[Nhood3Offset(min_i,min_j,max_i,max_j,skel_init, r, c)]);
		}
	}

	for(r=0;r<=max_i;r=r+2)
	{
		for(c=1;c<=max_j;c=c+2)
		{
			skel_init[r][c] = (skel_init[r][c] ^ newEndPoints[r][c]);
		}
	}

	//2
	//newEndPoints = applylut(skel_init,lutspur);
	
	for(r=0;r<=max_i;r++)
	{
		for(c=0;c<=max_j;c++)
		{
			newEndPoints[r][c] = (endPoints[r][c] & lutspur[Nhood3Offset(min_i,min_j,max_i,max_j,skel_init, r, c)]);
		}
	}

	for(r=1;r<=max_i;r=r+2)
	{
		for(c=0;c<=max_j;c=c+2)
		{
			skel_init[r][c] = (skel_init[r][c] ^ newEndPoints[r][c]);
		}
	}

	//3

	//newEndPoints = applylut(skel_init,lutspur);
	
	for(r=0;r<=max_i;r++)
	{
		for(c=0;c<=max_j;c++)
		{
			newEndPoints[r][c] = (endPoints[r][c] & lutspur[Nhood3Offset(min_i,min_j,max_i,max_j,skel_init, r, c)]);
		}
	}

	for(r=1;r<=max_i;r=r+2)
	{
		for(c=1;c<=max_j;c=c+2)
		{
			skel_init[r][c] = (skel_init[r][c] ^ newEndPoints[r][c]);
		}
	}

	//complement
	for(r=0;r<=max_i;r++)
	{
		for(c=0;c<=max_j;c++)
		{
			skel[r][c] = !skel_init[r][c];
		}
	}

}