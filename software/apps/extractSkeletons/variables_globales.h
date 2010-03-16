#define M 480
#define N 640
#define maxlength 161
#define edgecount 55
#define maxscale 48
#define Pi 3.14159265
#define diffthresh  1
#define list_length 200

float angThresh = Pi/15;
float distGrid[maxscale+1][maxscale+1];
float XY_mag[M][N], XY_ang[M][N];

float projtan;
int xychain[maxlength][2];

int valproj[M][N];

float distevlp[edgecount][M][N];
int valdist[M][N];

float projdist[edgecount][M][N], projinfo[edgecount][5];

int id[edgecount][edgecount], length_id[edgecount];

int evlp1[edgecount][M][N];//,evlp2[edgecount][M][N];

double skel[M][N];

int skelpts[edgecount][maxlength][2];
int skelmap[M][N];
int length_skelpts;
int length_edgepoints[edgecount];

int skelim[M][N];
int skellist[edgecount][maxlength][3];

int length_skellist[edgecount];
int edgenum_list[edgecount];

int q;
int skelregion[edgecount][M*N][2];

int lutlut[512];
int lookup[M][N];


/*int tabx[M*N];
int taby[M*N];
int length_tab;*/


int min_id,min_jd,max_id,max_jd;
int min_in,min_jn,max_in,max_jn;
int min_ip[edgecount], min_jp[edgecount], max_ip[edgecount], max_jp[edgecount];
int min_i,min_j,max_i,max_j;

int notnull;

struct {
    int edgenum;
    int id;
    int min_i;
    int min_j;
    int max_i;
    int max_j;
    double skel[M][N];
    int evlp2[M][N];
} parallel_data[list_length];
