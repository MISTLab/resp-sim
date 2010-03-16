//*****bwmorph****
//bwmorph(BW,'fcn',Inf)
//****************************************************************************
//operation morphologique sur les images

#include "fonctions.h"

//==========================================================================================
//test_egalite_double
//==========================================================================================
int test_egalite_double(int min_i, int min_j, int max_i, int max_j,double lastc[M][N], int c[M][N])
{
	int i,j;
	for(i=0;i<=max_i;i++)
	{
		for(j=0;j<=max_j;j++)
		{
			if((lastc[i][j] != c[i][j]))
			{
				return(0);
			}
		}
	}
	return(1);
}


//==========================================================================================
//test_egalite_int
//==========================================================================================
int test_egalite_int(int min_i, int min_j, int max_i, int max_j,int lastc[M][N], int c[M][N])
{
	int i,j;
	for(i=0;i<=max_i;i++)
	{
		for(j=0;j<=max_j;j++)
		{
			if(lastc[i][j] != c[i][j])
			{
				return(0);
			}
		}
	}
	return(1);
}
//==========================================================================================
//Nhood3Offset_logic
//==========================================================================================

int Nhood3Offset(int min_i, int min_j, int max_i, int max_j,int BWin[M][N], int r, int c)
{

    int minR, maxR, minC, maxC;
    int rr, cc;
    int result = 0;
    int weights3[3][3] = {{1, 8, 64},{2, 16, 128},{4, 32, 256}};

    /* Figure out the neighborhood extent that does not go past */
    /* image boundaries */
    if (r == 0) {
        minR = 1;
    } else {
        minR = 0;
    }
    if (r == max_i) {
        maxR = 1;
    } else {
        maxR = 2;
    }
    if (c == 0) {
        minC = 1;
    } else {
        minC = 0;
    }
    if (c == max_j) {
        maxC = 1;
    } else {
        maxC = 2;
    }

    for (rr = minR; rr <= maxR; rr++)
	{
        for (cc = minC; cc <= maxC; cc++)
		{
            result += weights3[rr][cc] * (BWin[r+rr-1][c+cc-1] != 0);
        }
    }

    return(result);
}

//==========================================================================================
//applylutc
//==========================================================================================

void applylut(int min_i, int min_j, int max_i, int max_j,int BWin[M][N], int lut[512], int BWout[M][N])
{
	int r,c;
	/*#pragma omp parallel for default(none)\
                shared(min_i,min_j,max_i,max_j,lut,BWout,BWin)\
                private(r,c)*/
        for (c = 0; c <=max_j; c++)
	{
            for (r = 0; r <=max_i; r++)
		{
                    BWout[r][c] = lut[Nhood3Offset(min_i,min_j,max_i,max_j,BWin, r, c)];
                }
        }

}