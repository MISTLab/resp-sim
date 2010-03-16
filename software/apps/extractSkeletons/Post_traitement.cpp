//*****Post_traitement****
//Definir l'envloppe entourant un contour edgenum
//****************************************************************************

#include "fonctions.h"
#include <stdio.h>


//=======================================================================================================
//Post_traitement
//=======================================================================================================

//int valunion[M][N];

void Post_traitement(int edgenum, int length_edge[edgecount],int valdist[M][N], int valproj[M][N], float distevlp[edgecount][M][N],\
					 float projtan, int xycoord[maxlength][2],\
					 int min_ie1, int min_je1, int max_ie1, int max_je1,\
					 int min_ie2, int min_je2, int max_ie2, int max_je2,\
					 int min_is[edgecount], int min_js[edgecount], int max_is[edgecount], int max_js[edgecount],\
					 float projdist[edgecount][M][N], float projinfo[edgecount][5])
{	
	int i,j,k=0;
	

	min_is[edgenum] = min_ie1 + min_ie2;
	min_js[edgenum] = min_je1 + min_je2;
	max_is[edgenum] = max_ie2 - min_ie2;
	max_js[edgenum] = max_je2 - min_je2;
	
        /*#pragma omp parallel for default(none)\
                shared(min_ie2,min_je2,max_ie2,max_je2,min_ie1,min_je1,max_ie1,max_je1,\
                valdist,valproj,distevlp,edgenum,projdist)\
                private(i,j)*/
	for(i=0;i<=(max_ie2 - min_ie2);i++)
	{
		for(j=0;j<=(max_je2 - min_je2);j++)
		{
			if((valdist[min_ie2 + min_ie1 + i][min_je2 + min_je1 +j] > 0) && (valproj[i+min_ie2][j+min_je2] > 0))
				projdist[edgenum][min_ie2 + min_ie1 + i][min_je2 + min_je1 +j] = distevlp[edgenum][min_ie2 + min_ie1 + i][min_je2 + min_je1 +j];
			else
				projdist[edgenum][min_ie2 + min_ie1 + i][min_je2 + min_je1 +j] = -100;
		}
	}

	projinfo[edgenum][0] = projtan;
	projinfo[edgenum][1] = xycoord[0][0];
	projinfo[edgenum][2] = xycoord[0][1];
	projinfo[edgenum][3] = xycoord[length_edge[edgenum]-1][0];
	projinfo[edgenum][4] = xycoord[length_edge[edgenum]-1][1];
}

