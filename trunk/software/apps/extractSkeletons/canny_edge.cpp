/*#include "fonctions.h"
#include <math.h>


#define taille 8
#define Pi 3.14159265
#define GaussianDieOff 0.0001


float ax[M][N], ay[M][N], mag[M][N];
//==========================================================================================
//imfilter
//==========================================================================================

void imfilter(int im[M][N],int width,\
			  float ax[M][N], float ay[M][N], float mag[M][N])
{
	int i,j,k,l;
	int magmax = 0;
	float gau[taille];
	float dgau2D[taille][taille];
	float ssq;
	float	im_rep1[M][N+2*taille-1], aSmooth[M][N],\
			im_rep2[M+2*taille-1][N],\
			im_rep3[M+2*taille-1][N+2*taille-1];
	//===============================================================================
	//linear gaussian filter
	for(i=0;i<=width;i++)
	{
		gau[i] = exp(-pow(i,2)/(2*ssq))/(2*Pi*ssq); //the gaussian 1D filter
	}

	//===============================================================================	
	//smooth the image out
	//replicate image at row level
	for(i=0;i<M;i++)
	{
		for(j=0;j<width;j++)
		{
			im_rep1[i][j] = im[i][0];
			im_rep1[i][N+width+j] = im[i][N-1];
		}
	}

	for(i=0;i<M;i++)
	{
		for(j=0;j<N;j++)
		{
			im_rep1[i][j+width] = im[i][j];
		}
	}

	//aSmooth=imfilter(a,gau,'conv','replicate');   % run the filter accross rows
	for(i=0;i<M;i++)
	{
		for(j=0;j<N;j++)
		{
			aSmooth[i][j] = im_rep1[i][width+j] * gau[0];
			for(k=1;k<=width;k++)
			{
				aSmooth[i][j] = aSmooth[i][j] + (im_rep1[i][width+k] - im_rep1[i][width-k]) * gau[k];
			}
			im_rep2[i+width][j] = aSmooth[i][j];
		}
	}
	
	//===============================================================================
	//replicate image at column level
	for(j=0;j<N;j++)
	{
		for(i=0;i<width;i++)
		{
			im_rep2[i][j] = aSmooth[0][j];
			im_rep2[M+width+i][j] = aSmooth[M-1][j];
		}
	}
	
	//aSmooth=imfilter(aSmooth,gau','conv','replicate'); % and then accross columns
	for(i=0;i<M;i++)
	{
		for(j=0;j<N;j++)
		{
			aSmooth[i][j] = im_rep2[width+i][j] * gau[0];
			for(k=1;k<=width;k++)
			{
				aSmooth[i][j] = aSmooth[i][j] + (im_rep2[width+k][j] - im_rep2[width-k][j]) * gau[k];
			}
			im_rep3[i+width][j+width] = aSmooth[i][j];
		}
	}

	//===============================================================================
	// 2D gaussian filter
	for(i=0;i<=width;i++)
	{
		for(j=0;j<=width;j++)
		{
			dgau2D[i][j]= -j * (exp(-(pow(j,2) + pow(i,2))/(2*ssq))) / (Pi*ssq);
		}
	}
	
	//===============================================================================
	//replicate image at 2D level
	for(j=0;j<N;j++)
	{
		for(i=0;i<width;i++)
		{
			im_rep3[i][j+width] = aSmooth[0][j];
			im_rep3[i+M+width][j+width] = aSmooth[M-1][j];
		}
	}

	for(i=0;i<M;i++)
	{
		for(j=0;j<width;j++)
		{
			im_rep3[i+width][j] = aSmooth[i][0];
			im_rep3[i+width][j+N+width] = aSmooth[i][N-1];
		}
	}
	
	for(i=0;i<width;i++)
	{
		for(j=0;j<width;j++)
		{
			im_rep3[i][j] = aSmooth[0][0];
			im_rep3[i][j+N+width] = aSmooth[0][N-1];
			im_rep3[i+M+width][j] = aSmooth[M-1][0];
			im_rep3[i+M+width][j+N+width] = aSmooth[M-1][N-1];
		}
	}
	
	//apply directional derivative
	//ax = imfilter(aSmooth, dgau2D, 'conv','replicate');
	//ay = imfilter(aSmooth, dgau2D', 'conv','replicate');

	for(i=0;i<M;i++)
	{
		for(j=0;j<N;j++)
		{
			ax[i][j] = dgau2D[0][0] * im_rep3[i+width][j+width];
			ay[i][j] = dgau2D[0][0] * im_rep3[i+width][j+width];

			for(l=1;l<=width;l++)
			{
				ax[i][j] = ax[i][j] + dgau2D[0][l] * im_rep3[i+width][j+l+width];
				ay[i][j] = ay[i][j] + dgau2D[l][0] * im_rep3[i+width][j+l+width];
				ax[i][j] = ax[i][j] + dgau2D[l][0] * im_rep3[i+l+width][j+width];
				ay[i][j] = ay[i][j] + dgau2D[0][l] * im_rep3[i+l+width][j+width];
			}

			for(k=1;k<=width;k++)
			{
				for(l=1;l<=width;l++)
				{
					ax[i][j] = ax[i][j] + dgau2D[k][l] * (im_rep3[i+k+width][j+l+width] + im_rep3[i-k+width][j+l+width]\
															- im_rep3[i+k+width][j-l+width] - im_rep3[i-k+width][j-l+width]);
					ay[i][j] = ay[i][j] + dgau2D[l][k] * (im_rep3[i+k+width][j+l+width] + im_rep3[i-k+width][j+l+width]\
															- im_rep3[i+k+width][j-l+width] - im_rep3[i-k+width][j-l+width]);
				}
			}
			mag[i][j] = sqrt(pow(ax[i][j],2) + pow(ay[i][j],2));
			if(mag[i][j] > magmax)
				magmax = mag[i][j];
		}
	}

	// normalize
	if(magmax > 0)
	{
		for(i=0;i<M;i++)
		{
			for(j=0;j<N;j++)
			{
				mag[i][j] = mag[i][j] / magmax;
			}
		}
	}

}

//==========================================================================================
//canny_edge
//==========================================================================================
void canny_edge(int im[M][N], float sigma)
{
	int i,j,k,l,m,n,p;
	int hit = 0;
	int width;
	float ssq;


	ssq = pow(sigma,2);

	//width = find(exp(-(pw.*pw)/(2*ssq))>GaussianDieOff,1,'last');
	i = 30;
	while((i>0) && (hit == 0))
	{
		if(exp(-pow(i,2)/(2*ssq)) > GaussianDieOff)
		{
			hit = 1;
			width = i;
		}
		i--;
	}

	imfilter(im,width,ax,ay,mag);

	//===============================================================================
	//idxLocalMax = cannyFindLocalMaxima(dir,ax,ay,mag); 
	//dir=1
	k = 0;
	l = 0;
	m = 0;
	n = 0;
	p = 0;
	for(i=0;i<M;i++)
	{
		for(j=0;j<N;j++)
		{
			if((ay[i][j]<=0 && ax[i][j]>-ay[i][j])||(ay[i][j]>=0 && ax[i][j]<-ay[i][j]))
			{
				idx[k] = i;
				idy[k] = j;
				//v[k] = (idx[k]+1) % M;
				if!(idx[k]==0 || idx[k]==M-1 || idy[k]==0 || idy[k]==N-1)
				{
					ixv[l] = ax[idx[k]][idy[k]];
					iyv[l] = ay[idx[k]][idy[k]];
					gradmag[l] = mag[idx[k]][idy[k]];
					
					//Do the linear interpolations for the interior pixels
					d[l] = fabs(iyv[l]/ivx[l]);
					gradmag1[l] = mag[idx[k]][idy[k]+1]*(1-d[l]) + mag[idx[k]-1][idy[k]+1]*d[l];
					gradmag2[l] = mag[idx[k]][idy[k]-1]*(1-d[l]) + mag[idx[k]+1][idy[k]-1]*d[l];

					if(gradmag[l]>=gradmagl1[l] && gradmag[l]>=gradmagl2[l])
					{
						idxLocalMax[m] = idx[l];
						idyLocalMax[m] = idy[l];
						if(mag[idxLocalMax[m]][idyLocalMax[m]]>lowThresh)
						{
							idxWeak[n] = idxLocalMax[m];
							idyWeak[n] = idyLocalMax[m];
							e[idxWeak[n]][idyWeak[n]]=1;
							if(mag[idxWeak[n]][idyWeak[n]]>highThresg)
							{
								rstrong[p]=idxWeak[n];
								cstrong[p]=idyWeak[n];
								p++;
							}
							n++;
						}
						m++;
					}
					l++;
				}
				k++;
			}
		}
	}

	//dir=2
	
}*/