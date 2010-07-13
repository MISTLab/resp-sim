#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "utils.h"
#include "wavIO16.h"

#ifndef FILTER_ORDER
#define FILTER_ORDER 50
#endif
#ifndef BUFFER_SIZE
#define BUFFER_SIZE 512
#endif

void rectify(int np, float* x, float *y);

// y = filter(b,a,x), with |a|=|b|=FILTER_ORDER, coeff=a°b, and np=|x|=|y|
void filter(float coeff[2*FILTER_ORDER], int np, float *x, float *y);

void average(int np, float* x, float* avg);

int decode(int diffLen, int* tokens, int textLen, char* text);

void posavg(int np, int* x, float* avg);

void negavg(int np, int* x, float* avg);

int decodeSymbol(int seqLen, int* sequence, char* symbol);

// params:	1) lowthreshold	2) highthreshold
int morseDecoder(char *inputpath, float params[2]);

#ifndef MORSE_INCLUDED
#define MORSE_INCLUDED

extern int code[40][6];
extern char symbols[40];

#endif
