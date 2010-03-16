//*****record_skeletons****
//Sauvegarder les squelettes
//****************************************************************************

#include <stdio.h>
#include <math.h>
#include "fonctions.h"

//==========================================================================================
//record_skeletons
//==========================================================================================

void record_skeletons(int edgenum, int skelpts[edgecount][maxlength][2], int *length_skelpts, int length_edgepoints[edgecount], float distevlp[edgecount][M][N],\
					  int evlp1[M][N], int evlp2[M][N],\
					  int min_i, int min_j, int max_i, int max_j,\
					  int skelim[M][N], int skellist[edgecount][maxlength][3], int skelregion[edgecount][M*N][2], int *q, int length_skellist[edgecount], int edgenum_list[edgecount])
{
	int skelind;
        int i,k,l;
	int r,c;
        /*#pragma omp parallel for ordered default(none) \
                shared(edgenum,evlp1,evlp2,skelregion,q,i,skelim,min_i,min_j,\
                skelpts,skellist,distevlp,length_skelpts,length_edgepoints)\
                private(k,l,r,c)*/
	for(k=1;k<=*length_skelpts;k++)
	{
		for(l=0;l<length_edgepoints[k];l++)
		{
				//printf("skelpts(%d,%d,%d,%d) = (%d,%d)\n",k,l,0,1,skelpts[k][l][0],skelpts[k][l][1]);
				// sub2ind( [M N], skelpts{k}(:,1), skelpts{k}(:,2) );
				//skelind = (skelpts[k][l][1]) * M + skelpts[k][l][0]; 
				skelim[skelpts[k][l][0]+min_i][skelpts[k][l][1]+min_j] = *q;
				
				skellist[*q][l][0] = skelpts[k][l][0]+min_i;
				skellist[*q][l][1] = skelpts[k][l][1]+min_j;
				skellist[*q][l][2] =  distevlp[edgenum][skelpts[k][l][0]+min_i][skelpts[k][l][1]+min_j] +\
									((distevlp[edgenum][skelpts[k][l][0]+min_i][skelpts[k][l][1]+min_j] - floor(distevlp[edgenum][skelpts[k][l][0]+min_i][skelpts[k][l][1]+min_j])) > 0.5);
				//printf("skellist(%d,%d) = (%d,%d,%d)\n",*q,l,skellist[*q][l][0],skellist[*q][l][1],skellist[*q][l][2]);
				//getchar();
		}
                length_skellist[*q] = length_edgepoints[k];
                edgenum_list[*q] = edgenum;
		i=0;
		//#pragma omp parallel for default(none)\
                //        shared(evlp1,evlp2,skelregion,q,i)\
                //        private(r,c)
                for(r=0;r<=max_i;r++)
		{
			for(c=0;c<=max_j;c++)
			{
				if((evlp1[min_i+r][min_j+c] * evlp2[min_i+r][min_j+c]) == 1)
				{
					skelregion[*q][i][0] = min_i + r;
					skelregion[*q][i++][1] = min_j + c;
				}
			}
		}
		//#pragma omp ordered
                (*q)++;
	}
	*length_skelpts = 0;
}
