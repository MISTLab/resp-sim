//*****envelope*****
//*****Chercher les pixels equidistants entre 2 contours paralleles****
//*********************************************************************

#include "fonctions.h"

#include <math.h>
#include <stdio.h>

//============================================================================
//envelope_edgenum
//============================================================================

void envelope_edgenum(int edgenum, float projdist[edgecount][M][N],\
					  int min_is[edgecount], int min_js[edgecount], int max_is[edgecount], int max_js[edgecount],\
					  int evlp1[edgecount][M][N])
{
	int i,j;


	for(i=0;i<=max_is[edgenum];i++)
	{
		for(j=0;j<=max_js[edgenum];j++)
		{
			evlp1[edgenum][min_is[edgenum]+i][min_js[edgenum]+j] = (projdist[edgenum][min_is[edgenum]+i][min_js[edgenum]+j] >= 0); // Keep good parts of projected
		}
	}
}

//============================================================================
//envelope
//============================================================================



void envelope (int edgenum, int k, float projdist[edgecount][M][N], int id[edgecount][edgecount],\
			   int min_is[edgecount], int min_js[edgecount], int max_is[edgecount], int max_js[edgecount],\
			   int *min_i, int *min_j, int *max_i, int *max_j,\
			   int evlp1[edgecount][M][N], int evlp2[M][N], double skel[M][N], int *notnull)
{	
        int i,j;
        double diffMap[M][N], overlap[M][N];
	//int min_i,min_j,max_i,max_j;

	//printf("(min_i,min_j);(max_i,max_j) = (%d,%d);(%d,%d)\n", min_is[id[k]],min_js[id[k]],max_is[id[k]],max_js[id[k]]);
	//getchar();
	//printf("(min_i,min_j);(max_i,max_j) = (%d,%d);(%d,%d)\n", min_is[edgenum],min_js[edgenum],max_is[edgenum],max_js[edgenum]);
	//getchar();
	if(min_is[edgenum] < min_is[id[edgenum][k]])
		*min_i = min_is[id[edgenum][k]];
	else
		*min_i = min_is[edgenum];

	if(min_js[edgenum] < min_js[id[edgenum][k]])
		*min_j = min_js[id[edgenum][k]];
	else
		*min_j = min_js[edgenum];

	if(max_is[edgenum] + min_is[edgenum] < max_is[id[edgenum][k]] + min_is[id[edgenum][k]])
		*max_i = max_is[edgenum] + min_is[edgenum] - *min_i;
	else
		*max_i = max_is[id[edgenum][k]] + min_is[id[edgenum][k]] - *min_i;

	if(max_js[edgenum] + min_js[edgenum]< max_js[id[edgenum][k]] + min_js[id[edgenum][k]])
		*max_j = max_js[edgenum] + min_js[edgenum] - *min_j;
	else
		*max_j = max_js[id[edgenum][k]] + min_js[id[edgenum][k]] - *min_j;


	if((*max_i > 0) && (*max_j > 0))
	{
		for(i=0;i<=*max_i;i++)
		{
			for(j=0;j<=*max_j;j++)
			{

				//evlp1[i][j] = (projdist[edgenum][i][j] >= 0); // Keep good parts of projected

				evlp2[*min_i+i][*min_j+j] = (projdist[id[edgenum][k]][*min_i+i][*min_j+j] >= 0);  // dist map around both edges


				diffMap[*min_i+i][*min_j+j] =  fabs(projdist[edgenum][*min_i+i][*min_j+j] - projdist[id[edgenum][k]][*min_i+i][*min_j+j]);

				overlap[*min_i+i][*min_j+j] = ((diffMap[*min_i+i][*min_j+j] - diffthresh) <= 0);

				overlap[*min_i+i][*min_j+j] = overlap[*min_i+i][*min_j+j] * evlp1[edgenum][*min_i+i][*min_j+j] * evlp2[*min_i+i][*min_j+j] * (diffMap[*min_i+i][*min_j+j]+ 0.00000000000000022204);


				if(overlap[*min_i+i][*min_j+j] != 0)
				{
					skel[i][j] = (fabs(overlap[*min_i+i][*min_j+j])/overlap[*min_i+i][*min_j+j])* (diffthresh - overlap[*min_i+i][*min_j+j]);
					(*notnull)= 1;
				}

				else
					skel[i][j] = 0;

			}
		}
	}

}