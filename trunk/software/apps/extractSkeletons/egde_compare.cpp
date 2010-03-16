//*****edge_compare****
//Comparer les contours par paires pour identifier ceux qui sont parallels
//****************************************************************************

#include "fonctions.h"

#include <math.h>
#include <stdio.h>


//====================================================================================================
//id_initialize
//====================================================================================================
//Initialisation de l'ensemble de contours parallel a un certain contour edgenum

void id_initialize(int edgenum, int id[edgecount][edgecount], int length_id[edgecount])
{
	int i;
	/*#pragma omp parallel for default(none)\
                shared(edgenum,id,length_id)\
                private(i)*/
        for(i=0;i<edgecount;i++)
	{
		id[edgenum][i] = 0;
	}
	length_id[edgenum] = 0;
}

//==================================================================================
void id_affiche(int edgenum, int length_id[edgecount],int id[edgecount][edgecount])
{
	int i;
	printf("id= [");
	for(i=0;i<length_id[edgenum];i++)
		printf(" %d ",id[edgenum][i]);
	printf("]\n");
}

//====================================================================================================
//edge_compare
//====================================================================================================
//Comparer les contours par paires pour identifier ceux qui sont parallels

void edge_compare(int edgenum, float projinfo[edgecount][5], float angThresh, int id[edgecount][edgecount], int length_id[edgecount])
{
	// Non-intuitive projective geometry to determine distance between
    // endpoint i and other lines. '0' distance point is intersection
    // of a line extended from origin in direction of edge i and the
    // normal through endpoint of edge i. Distance of a line is from
    // '0' point along normal through endpoint i, to point where line
    // intersects this normal.
	float d_top[edgecount], d_bot[edgecount], delta_top[edgecount], delta_bot[edgecount], interAng[edgecount],\
			projOff[edgecount], proj_top_rt[edgecount], proj_top_lf[edgecount], proj_bot_rt[edgecount], proj_bot_lf[edgecount];

	int ang_id[edgecount], top_id[edgecount], bot_id[edgecount];

	int k=0;
	int i;

	d_top[edgenum] = sqrt( pow(projinfo[edgenum][1],2) + pow(projinfo[edgenum][2],2) ) *\
			sin( projinfo[edgenum][0] - atan2( projinfo[edgenum][2], projinfo[edgenum][1]) );

	d_bot[edgenum] = sqrt( pow(projinfo[edgenum][3],2) + pow(projinfo[edgenum][4],2) )*\
			sin( projinfo[edgenum][0] - atan2( projinfo[edgenum][4], projinfo[edgenum][3]) );
	

	for (i=0;i<=(edgenum-1);i++)
	{
		d_top[i] = sqrt( pow(projinfo[i][1],2) + pow(projinfo[i][2],2) ) *\
			sin( projinfo[edgenum][0] - atan2( projinfo[i][2], projinfo[i][1]) );

		d_bot[i] = sqrt( pow(projinfo[i][3],2) + pow(projinfo[i][4],2) )*\
			sin( projinfo[edgenum][0] - atan2( projinfo[i][4], projinfo[i][3]) );

		delta_top[i] = d_top[i] - d_top[edgenum];
		delta_bot[i] = d_bot[i] - d_bot[edgenum];

		// Parallelism test
		interAng[i] = fabs(cos( projinfo[edgenum][0] - projinfo[i][0]));

		projOff[i] = maxscale * interAng[i];
		proj_top_rt[i] = projOff[i] + delta_top[i];
		proj_top_lf[i] = -projOff[i] + delta_top[i];
		proj_bot_rt[i] = projOff[i] + delta_bot[i];
		proj_bot_lf[i] = -projOff[i] + delta_bot[i];
		// Pair those edges that are near parallel and close to one another

		ang_id[i] = (interAng[i] > cos(angThresh));

		top_id[i] = (proj_top_lf[i]- maxscale <=0 ) & (proj_top_rt[i] + maxscale >= 0);


		bot_id[i] = (proj_bot_lf[i] - maxscale <=0 ) & (proj_bot_rt[i] + maxscale>= 0);

		if((ang_id[i]*(top_id[i]+bot_id[i]))!=0)
			id[edgenum][k++] = i;
	}
	
	length_id[edgenum] = k ;
	//id_affiche(k,id);
}