//*****im_edge2param****
//im_edge2param
//****************************************************************************

#include <stdio.h>
#include "fonctions.h"


//=========================================================================================
//im_edge2param
//=========================================================================================



void im_edge2param(double im[M][N], int minlength, int lutlut[512],int lookup[M][N],\
				  int min_i, int min_j, int max_i, int max_j,\
				  int edgelist[edgecount][maxlength][2], int EDGEIM[M][N], int *length_edgelist, int length_edgepoints[edgecount])
{
	int i;
	int r,c;
	int edgeNo = 1;
        int edgepoints[maxlength][2];
	
	//EDGEIM = im ~= 0
	for(r=0;r<=max_i;r++)
	{
		for(c=0;c<=max_j;c++)
		{
			EDGEIM[r][c] = (int)(im[r][c]!=0);
		}
	}
	
	bwmorph_thin(min_i,min_j,max_i,max_j,lutlut,EDGEIM,lookup);
	
	for(r=0;r<=max_i;r++)
	{
		for(c=0;c<=max_j;c++)
		{
			if (EDGEIM[r][c] == 1)
			{
				pk_trackEdge(r,c,edgeNo,minlength,EDGEIM,edgepoints,length_edgepoints);

				if (length_edgepoints>0)
				{
					for(i=0;i<length_edgepoints[edgeNo];i++)
					{
						edgelist[edgeNo][i][0] = edgepoints[i][0];
						edgelist[edgeNo][i][1] = edgepoints[i][1];
						//printf("skelpts (%d,%d) = (%d , %d)\n", edgeNo, i, skelpts[edgenum][j][edgeNo][i][0], skelpts[edgenum][j][edgeNo][i][1]);
						//getchar();
					}
					edgeNo++;
				}
				*length_edgelist = edgeNo - 1;
			}
		}
	}
	
	//EDGEIM = -EDGEIM
	for(r=0;r<=max_i;r++)
	{
		for(c=0;c<=max_j;c++)
		{
			EDGEIM[r][c] = -EDGEIM[r][c];
		}
	}

}
