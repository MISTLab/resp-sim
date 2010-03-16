#define M 480
#define N 640
#define maxlength 161
#define edgecount 55
#define maxscale 48
#define diffthresh  1
//#include "contours.h"


void canny_edge (float sigma);

void cartesian_initialize(int max_dist, float distGrid[maxscale+1][maxscale+1], float XY_mag[M][N], float XY_ang[M][N]);

void buildDistMap(int edgenum, int length_edge[edgecount], int edgelist[edgecount][maxlength][2], float distGrid[maxscale+1][maxscale+1], int max_dist,\
				  int *min_is, int *min_js, int *max_is, int *max_js,\
				  float distMap[edgecount][M][N], int validMap[M][N]);

void normProject(int edgenum, int length_edge[edgecount], int edgelist[edgecount][maxlength][2],\
				 float XY_mag[M][N], float XY_ang[M][N],\
				 int min_ie, int min_je, int max_ie, int max_je,\
				 int *min_is, int *min_js, int *max_is, int *max_js,\
				 float *ang_t, int xycoord[maxlength][2], int validMap[M][N]);

void Post_traitement(int edgenum, int length_edge[edgecount],int valdist[M][N], int valproj[M][N], float distevlp[edgecount][M][N],\
					 float projtan, int xycoord[maxlength][2],\
					 int min_ie1, int min_je1, int max_ie1, int max_je1,\
					 int min_ie2, int min_je2, int max_ie2, int max_je2,\
					 int min_is[edgecount], int min_js[edgecount], int max_is[edgecount], int max_js[edgecount],\
					 float projdist[edgecount][M][N], float projinfo[edgecount][5]);

void id_initialize(int edgenum, int id[edgecount][edgecount], int length_id[edgecount]);

void edge_compare(int edgenum, float projinfo[edgecount][5], float angThresh, int id[edgecount][edgecount], int length_id[edgecount]);

void envelope_edgenum(int edgenum, float projdist[edgecount][M][N],\
					  int min_is[edgecount], int min_js[edgecount], int max_is[edgecount], int max_js[edgecount],\
					  int evlp1[edgecount][M][N]);

void envelope (int edgenum, int k, float projdist[edgecount][M][N], int id[edgecount][edgecount],\
			   int min_is[edgecount], int min_js[edgecount], int max_is[edgecount], int max_js[edgecount],\
			   int *min_i, int *min_j, int *max_i, int *max_j,\
			   int evlp1[edgecount][M][N], int evlp2[M][N], double skel[M][N], int *notnull);

int test_egalite_double(int min_i, int min_j, int max_i, int max_j,double lastc[M][N], int c[M][N]);

int test_egalite_int(int min_i, int min_j, int max_i, int max_j,int lastc[M][N], int c[M][N]);

int Nhood3Offset(int min_i, int min_j, int max_i, int max_j,int BWin[M][N], int r, int c);

void applylut(int min_i, int min_j, int max_i, int max_j,int BWin[M][N], int lut[512], int BWout[M][N]);

void bwmorph_skel(int min_i, int min_j, int max_i, int max_j,double skel[M][N]);

void bwmorph_spur(int min_i, int min_j, int max_i, int max_j,double skel[M][N]);

void bwmorph_thin(int min_i, int min_j, int max_i, int max_j,int lutlut[512], int lookup[M][N], int c[M][N]);

int pk_nextPoint(int rp, int cp, int EDGEIM[M][N], int *nextr, int *nextc);

void pk_trackEdge(int rstart, int cstart, int edgeNo, int minlength,\
				  int EDGEIM[M][N], int edgepoints[maxlength][2], int length_edgepoints[edgecount]);

void im_edge2param(double im[M][N], int minlength, int lutlut[512],int lookup[M][N],\
					int min_i, int min_j, int max_i, int max_j,\
					int edgelist[edgecount][maxlength][2], int EDGEIM[M][N], int *length_edgelist, int length_edgepoints[edgecount]);

void record_skeletons(int edgenum, int skelpts[edgecount][maxlength][2], int *length_skelpts, int length_edgepoints[edgecount], float distevlp[edgecount][M][N],\
					  int evlp1[M][N], int evlp2[M][N],\
					  int min_i, int min_j, int max_i, int max_j,\
					  int skelim[M][N], int skellist[edgecount][maxlength][3], int skelregion[edgecount][M*N][2], int *q, int length_skellist[edgecount], int edgenum_list[edgecount]);
