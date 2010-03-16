//*****tracking****
//tracking
//****************************************************************************

#include "fonctions.h"


//================================================================================
//pk_nextPoint
//================================================================================

int pk_nextPoint(int rp, int cp, int EDGEIM[M][N], int *nextr, int *nextc)
{
	// row and column offsets for the eight neighbours of a point
	// starting with those that are 4-connected.
	int roff[8] = {1,0,-1,0,1,1,-1,-1};
	int coff[8] = {0,1,0,-1,1,-1,-1,1};
	int row,col;
	int thereIsAPoint;
	int i;
	
	//Search through neighbours and return first connected edge point
	for(i=0;i<8;i++)
	{
		row = rp+roff[i];
		col = cp+coff[i];
		if ((row >= 0) && (row < M) && (col >= 0) && (col < N))
		{
			if (EDGEIM[row][col] == 1)
			{
				*nextr = row;
				*nextc = col;
				thereIsAPoint = 1;
				return(thereIsAPoint); // break out and return with the data
			}
		}
	}
	// If we get here there was no connecting next point.
	*nextr = 0;
	*nextc = 0;
	thereIsAPoint = 0;
	return(thereIsAPoint);
}

//======================================================================================
//pk_trackEdge
//======================================================================================
int nextr,nextc;

void pk_trackEdge(int rstart, int cstart, int edgeNo, int minlength,\
				  int EDGEIM[M][N], int edgepoints[maxlength][2], int length_edgepoints[edgecount])
{
	int k = 0;
	int thereIsAPoint;
	int i;

	int temp;

	edgepoints[0][0] = rstart;
	edgepoints[0][1] = cstart; // Start a new list for this edge.
	
	
	EDGEIM[rstart][cstart] = -edgeNo; // Edge points in the image are encoded by -ve of their edgeNo.

	thereIsAPoint= pk_nextPoint(rstart,cstart,EDGEIM,&nextr,&nextc); // Find next connected edge point.
	
	while (thereIsAPoint)
	{
		edgepoints[++k][0] = nextr; // Add point to point list
		edgepoints[k][1] = nextc;
		EDGEIM[nextr][nextc] = -edgeNo; // Update edge image
		thereIsAPoint = pk_nextPoint(nextr,nextc,EDGEIM,&nextr,&nextc);
	}

	//edgepoints = flipud(edgepoints); // Reverse order of points

	
	for(i=0;i<(k+1)/2;i++)
	{
		temp = edgepoints[k-i][0];
		edgepoints[k-i][0] = edgepoints[i][0];
		edgepoints[i][0] = temp;
		temp = edgepoints[k-i][1];
		edgepoints[k-i][1] = edgepoints[i][1];
		edgepoints[i][1] = temp;
	}


	// Now track from original point in the opposite direction
	
	thereIsAPoint= pk_nextPoint(rstart,cstart,EDGEIM,&nextr,&nextc);

	while (thereIsAPoint)
	{
		edgepoints[++k][0] = nextr; // Add point to point list
		edgepoints[k][1] = nextc;
		EDGEIM[nextr][nextc] = -edgeNo; // Update edge image
		thereIsAPoint = pk_nextPoint(nextr,nextc,EDGEIM,&nextr,&nextc);
	}
	
	length_edgepoints[edgeNo] = k+1;
}