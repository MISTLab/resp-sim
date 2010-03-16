
//*****buildDistMap****
//Identifier les pixels se trouvant dans l'enveloppe a une distance predefinie
//****************************************************************************

#include <stdio.h>
#include "fonctions.h"

//==========================================================================================
//distMap_initialize
//==========================================================================================
//initialisation des distances

inline void distMap_initialize(int edgenum, float distMap[edgecount][M][N], int validMap[M][N])
{
	int i,j;
	#pragma omp parallel for default(none) \
                shared(distMap,validMap,edgenum) private(i,j)
        for(i=0;i<M;i++)
	{
		for(j=0;j<N;j++)
		{
			distMap[edgenum][i][j] = 1000;
			validMap[i][j] = 0;
		}
	}
}


//======================================================================================
//distMap_calcul
//======================================================================================
//Calcul des distances pour chaque pixel du contour edgenum

void buildDistMap(int edgenum, int length_edge[edgecount], int edgelist[edgecount][maxlength][2], float distGrid[maxscale+1][maxscale+1], int max_dist,\
				  int *min_is, int *min_js, int *max_is, int *max_js,\
				  float distMap[edgecount][M][N], int validMap[M][N])
{
	int i,j,k;
	int x_1,y_1;
	int limitj_inf,limitj_sup,limitk_inf,limitk_sup;
	int init = 1;
	int fore_n, back_n, fore_m, back_m;

	distMap_initialize(edgenum, distMap, validMap); //initialisation

	/*#pragma omp parallel for default(none) \
                shared(edgelist,length_edge,distGrid,distMap,validMap,edgenum)\
                private(i,j,k,fore_m,fore_n,max_dist,x_1,limitj_inf,limitj_sup,y_1,limitk_inf,limitk_sup)*/
        for(i=0;i<length_edge[edgenum];i++)
	{
		fore_n = edgelist[edgenum][i][1] - 1; // Column range

		fore_m = edgelist[edgenum][i][0] - 1; // Row range

		
		if(fore_m+1> max_dist)
		{
			x_1 = fore_m+1 - max_dist;
			limitj_inf = max_dist-1;
		}
		else
		{
			x_1 = 1;
			limitj_inf = fore_m-1;
		}
		
		if(M-1-fore_m> max_dist)
		{
			limitj_sup = max_dist;
		}
		else
		{
			limitj_sup = M-2-fore_m;
		}


		if(fore_n+1> max_dist)
		{
			y_1 = fore_n+1 - max_dist;
			limitk_inf = max_dist-1;
		}
		else
		{
			y_1 = 1;
			limitk_inf = fore_n-1;
		}

		if(N-1-fore_n> max_dist)
		{
			limitk_sup = max_dist;
		}
		else
		{
			limitk_sup = N-2-fore_n;
		}

		for (j=0;j<=limitj_inf;j++)
		{
			for (k=0;k<=limitk_inf;k++)
			{
				if (distGrid[limitj_inf+1-j][limitk_inf+1-k] < distMap[edgenum][x_1-1+j][y_1-1+k])
					distMap[edgenum][x_1-1+j][y_1-1+k] = distGrid[limitj_inf+1-j][limitk_inf+1-k];
			}
		}

		for (j=0;j<=limitj_inf;j++)
		{
			for (k=0;k<=limitk_sup;k++)
			{
				if (distGrid[limitj_inf+1-j][k] < distMap[edgenum][x_1-1+j][fore_n+k])
					distMap[edgenum][x_1-1+j][fore_n+k] = distGrid[limitj_inf+1-j][k];
			}
		}

		for (j=0;j<=limitj_sup;j++)
		{
			for (k=0;k<=limitk_inf;k++)
			{
				if (distGrid[j][limitk_inf+1-k] < distMap[edgenum][fore_m+j][y_1-1+k])
					distMap[edgenum][fore_m+j][y_1-1+k] = distGrid[j][limitk_inf+1-k];
			}
		}

		for (j=0;j<=limitj_sup;j++)
		{
			for (k=0;k<=limitk_sup;k++)
			{
				if (distGrid[j][k] < distMap[edgenum][fore_m+j][fore_n+k])
					distMap[edgenum][fore_m+j][fore_n+k] = distGrid[j][k];

			}
		}

	}

	*min_is = M;
	*min_js = N;
	*max_is = 0;
	*max_js = 0;
	/*#pragma omp parallel for default(none) \
                shared(edgenum,max_dist,distMap,validMap,min_is,min_js,max_is,max_js)\
                private(i,j)*/
        for (i=0;i<M;i++)
	{
		for (j=0;j<N;j++)
		{
			
			validMap[i][j] = (distMap[edgenum][i][j] <= max_dist);
			

			if(validMap[i][j] > 0)
			{
				if(i < *min_is)
					*min_is = i;
				if(j < *min_js)
					*min_js = j;
				if(i > *max_is)
					*max_is = i;
				if(j > *max_js)
					*max_js = j;

			}
		}
	}

}

