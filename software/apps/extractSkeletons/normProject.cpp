//*****normProject****
//Identifier les pixels se trouvant dans l'enveloppe entre les deux normales 
//aux extrimites du contour				
//****************************************************************************

#include "fonctions.h"
#include <math.h>
#include <stdio.h>


//=======================================================================================================
//reverseOrder
//=======================================================================================================
//changer l'odre des pixels formant un contour edgenum

inline void reverseOrder_edge(int edgenum, int length_edge[edgecount], int x[maxlength],int y[maxlength])
{
	int temp;
	int i;
	/*#pragma omp parallel for default(none)\
                shared(length_edge,edgenum,x,y)\
                private(i,temp)*/
        for(i=0;i<(length_edge[edgenum]/2);i++)
	{
		temp = x[length_edge[edgenum]-(i+1)];
		x[length_edge[edgenum]-(i+1)] = x[i];
		x[i] = temp;
		temp = y[length_edge[edgenum]-(i+1)];
		y[length_edge[edgenum]-(i+1)] = y[i];
		y[i] = temp;
	}
}

inline void reverseOrder(int edgenum, int length_edge[edgecount], int x[maxlength],int y[maxlength], int *x_1, int *x_2, int *y_1, int *y_2)
{

	reverseOrder_edge(edgenum,length_edge,x,y);
	
	*x_1 = x[0];
	*x_2 = x[length_edge[edgenum]-1];
	*y_1 = y[0];
	*y_2 = y[length_edge[edgenum]-1];
}

inline void convert_edge(int edgenum, int length_edge[edgecount], int edgelist[edgecount][maxlength][2], int* x, int* y)
{
	int i;
	/*#pragma omp parallel for default(none)\
                shared(length_edge,edgelist,x,y,edgenum)\
                private(i)*/
        for (i=0;i<length_edge[edgenum];i++)
	{
		y[i] = -( edgelist[edgenum][i][0] - 1 ); // Convert line coordinates to Cartesian
		x[i] = ( edgelist[edgenum][i][1] - 1 );
	}
}

inline void remap_edge(int edgenum, int length_edge[edgecount], int xycoord[maxlength][2], int x_1, int x_2, int y_1, int y_2)
{
		xycoord[0][0] = x_1;
		xycoord[0][1] = y_1;
		xycoord[length_edge[edgenum]-1][0] = x_2;
		xycoord[length_edge[edgenum]-1][1] = y_2;
}

//=========================================================================================================
//normProject
//=========================================================================================================
//Calcul des projection de chaque pixel sur le contour edgenum

void normProject(int edgenum, int length_edge[edgecount], int edgelist[edgecount][maxlength][2],\
				 float XY_mag[M][N], float XY_ang[M][N],\
				 int min_ie, int min_je, int max_ie, int max_je,\
				 int *min_is, int *min_js, int *max_is, int *max_js,\
				 float *ang_t, int xycoord[maxlength][2], int validMap[M][N])
{
	int i,j;
    int x_1,x_2,y_1,y_2;
    int x[maxlength], y[maxlength];
    float rho1,rho2, rho1_XY[M][N];//,rho2_XY[M][N];

	// Convert line coordinates to Cartesian
	convert_edge(edgenum, length_edge, edgelist, x, y);

	x_1 = x[0]; // Isolate endpoints
	y_1 = y[0];
	x_2 = x[length_edge[edgenum]-1];
	y_2 = y[length_edge[edgenum]-1];

	if (y_1 != y_2) // Ensure first point is the (cartesian) top of line
	{    
		if (y_1 < y_2) // Have to reverse the chain order
			reverseOrder(edgenum,length_edge,x,y,&x_1,&x_2,&y_1,&y_2);
	}    
	else
	{
		if (x_2 < x_1) // Have to reverse the chain order
			reverseOrder(edgenum,length_edge,x,y,&x_1,&x_2,&y_1,&y_2);

	}
	// Define tangent direction at top and bottom of line
	*ang_t = atan2( (float)( y_2 - y_1 ), (float)( x_2 - x_1 ) );

	
	// Define line endpoint distances in Cartesian frame
	rho1 = sqrt( (float)(x_1*x_1 + y_1*y_1) ) * cos( *ang_t - atan2( (float)y_1, (float)x_1 ) );
	rho2 = sqrt( (float)(x_2*x_2 + y_2*y_2) ) * cos( *ang_t - atan2( (float)y_2, (float)x_2 ) );

	// Define distances to all points in Cartesian frame

	*min_is = max_ie - min_ie;
	*min_js = max_je - min_je;
	*max_is = 0;
	*max_js = 0;
	
	/*#pragma omp parallel for default(none)\
                shared(min_ie,max_ie,min_je,max_je,min_is,min_js,max_is,max_js,\
                        ang_t,XY_ang,XY_mag,validMap,rho1,rho2,rho1_XY)\
                private(i,j)*/
        for(i=0;i<=(max_ie - min_ie);i++)
	{
		for(j=0;j<=(max_je - min_je);j++)
		{
			rho1_XY[i][j] = XY_mag[min_ie + i][min_je + j] * cos( *ang_t - XY_ang[min_ie + i][min_je + j] );
	
			validMap[i][j] = (rho1_XY[i][j] >= rho1) & (rho1_XY[i][j] <= rho2);

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
	
	// Remap the line into image coordinates
	remap_edge(edgenum, length_edge, xycoord, x_1, x_2, y_1, y_2);

}

