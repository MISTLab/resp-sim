#include <stdio.h>
#include <math.h>
#include "utils.h"
#include "wavIO16.h"
#include "coeffs.h"

#define FS		44100.0
#ifndef  C_PI
 #define C_PI  (3.1415926535897932384626)
#endif

void hcMixer(float *input, float *rLocalOscPhAcc, float output[2]);

void decimatorFilter(int iDecimatorIdxIn, float rDecimatorQueueI[64], float rDecimatorQueueQ[64], float output[2]);

void CW300Filter(int iMainFilterIdxIn, float rMainFilterQueueI[428], float rMainFilterQueueQ[428], float output[2]);

void CW1KFilter(int iMainFilterIdxIn, float rMainFilterQueueI[428], float rMainFilterQueueQ[428], float output[2]);

void SSB2K4Filter(int iMainFilterIdxIn, float rMainFilterQueueI[428], float rMainFilterQueueQ[428], float output[2]);

void interpolatorFilter(int iInterpolatorParams[2], float rInterpolatorQueueI[16], float rInterpolatorQueueQ[16], float output[2]);

// params:	1) start_freq	2) filter	3) gain
int frequencyFilter(char inputpath[512], char outputpath[512], int params[3]);

