
//*****Conversion des pixels de l'image en coordonnees cartesiennes****
//*********************************************************************

#include "fonctions.h"
#include <math.h>
#include <stdio.h>


//===================================================================================================

//====================================================================================================
//cartezian_initialize_op
//====================================================================================================
//Calcul des distances cartesiennes entre pixels

void cartesian_initialize(int max_dist, float distGrid[maxscale+1][maxscale+1], float XY_mag[M][N], float XY_ang[M][N])
{
	int i,j;

	#pragma omp parallel for default(none)\
                shared(distGrid) private(i,j)
        for(i=0;i<=maxscale;i++)
	{
		for(j=0;j<=i;j++)
		{
			distGrid[i][j] = sqrt((float)(j*j + i*i));
			distGrid[j][i] = distGrid[i][j];
		}
	}

	#pragma omp parallel for default(none)\
                shared(XY_mag,XY_ang) private(i,j)
	for(i=0;i<M;i++)
	{
		for(j=0;j<N;j++)
		{

			XY_mag[i][j] = sqrt((float)(j*j + i*i));
			XY_ang[i][j] = atan2((float)-i,(float)j);
		}
	}




}