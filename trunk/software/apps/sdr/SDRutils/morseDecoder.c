#include "morseDecoder.h"

int morseDecoder(char *inputpath, float params[2]) {

	float lowthreshold = params[0];
	float highthreshold = params[1];

	int i,j,remainder,inTmp;
	int interval=BUFFER_SIZE/10;

	float *in = (float*)malloc(BUFFER_SIZE*sizeof(float));
	float *abs = (float*)malloc(BUFFER_SIZE*sizeof(float));
	float *abs2 = (float*)malloc((BUFFER_SIZE+interval)*sizeof(float));
	float avg;

	if (FILTER_ORDER<1) return 2;
	float *a = (float*)malloc(FILTER_ORDER*sizeof(float));
	float *b = (float*)malloc(FILTER_ORDER*sizeof(float));
	a[0]=1;
	b[0]=1/(float)FILTER_ORDER;
	for (i=1; i<FILTER_ORDER; i++) {
		a[i]=0;
		b[i]=1/(float)FILTER_ORDER;
	}
	float *coeff = (float*)malloc(2*FILTER_ORDER*sizeof(float));
	for (i=0; i<FILTER_ORDER; i++) {
		coeff[i] = a[i];
		coeff[i+FILTER_ORDER] = b[i];
	}
	float *flt2 = (float*)malloc((BUFFER_SIZE+interval)*sizeof(float));
	float *flt = (float*)malloc(BUFFER_SIZE*sizeof(float));

	int *discrete = (int*)malloc(BUFFER_SIZE*sizeof(int));
	int discreteOld = 0;
	int last = 0, diffCnt = 0, frameCnt = 0;
	int *diff = (int*)malloc(BUFFER_SIZE*sizeof(int));
	int *frames = (int*)malloc(BUFFER_SIZE*sizeof(int));

	int samples;
	int wavfd = openWavToRead16(inputpath,&samples);
	if (wavfd == -1) return 1;
	#ifdef QUICK
	samples=Q_SAMPLES;
	#endif

	#ifdef DISCROUT
	int wavout = openWavToWrite16((char*)"out.wav",samples);
	if (wavout == -1) { closeWav16(wavfd);	return 1;}
	#endif

	printout("\nDiscretization: process begins...\n\n",0);
	for (i=0; i<samples; i+=BUFFER_SIZE) {
		for (j=0; j<BUFFER_SIZE; j++) {
			inTmp = readSample16(wavfd);
			in[j] = (float) inTmp / 32768;
		}

		rectify(BUFFER_SIZE,in,abs);

		average(interval,abs,&avg);
		for (j=0; j<BUFFER_SIZE+interval; j++) {
			if (j<interval) abs2[j]=avg;
			else abs2[j] = abs[j-interval];
		}
		filter(coeff,BUFFER_SIZE+interval,abs2,flt2);
		for (j=0; j<BUFFER_SIZE; j++) {
			flt[j] = flt2[j+interval];
		}

		for (j=0; j<BUFFER_SIZE; j++) {
			if ( flt[j] > highthreshold ) discrete[j] = 1;
			else {
				if ( flt[j] < lowthreshold ) discrete[j] = 0;
				else discrete[j] = discreteOld;
			}
			discreteOld = discrete[j];

			#ifdef DISCROUT
				#ifndef LOWPASSOUT
				writeSample16(wavout,discrete[j]*32760);
				#endif
				#ifdef LOWPASSOUT
				inTmp = flt[j] * 32768;
				writeSample16(wavout,inTmp);
				#endif
			#endif
		}

		for (j=0; j<BUFFER_SIZE; j++) {
			if (last == 0 && discrete[j] == 1) {
				diff[diffCnt] = -1;
				frames[diffCnt] = frameCnt;
				diffCnt++;
				last = 1;
				frameCnt=0;
			}
			else if (last == 1 && discrete[j] == 0) {
				diff[diffCnt] = 1;
				frames[diffCnt] = frameCnt;
				diffCnt++;
				last = 0;
				frameCnt=0;
			}
			else frameCnt++;
		}

		#ifdef VERBOSE
		printout("\nDiscretization: checkpoint %d reached!\n\n",i);
		#endif
	}

	remainder = i - samples;
	for (j=0; j<remainder; j++) {
		inTmp = readSample16(wavfd);
		in[j] = (float) inTmp / 32768;
	}

	rectify(remainder,in,abs);

	average(interval,abs,&avg);
	for (j=0; j<remainder+interval; j++) {
		if (j<interval) abs2[j]=avg;
		else abs2[j] = abs[j-interval];
	}
	filter(coeff,remainder+interval,abs2,flt2);
	for (j=0; j<remainder; j++) {
		flt[j] = flt2[j+interval];
	}

	for (j=0; j<remainder; j++) {
		if ( flt[j] > highthreshold ) discrete[j] = 1;
		else {
			if ( flt[j] < lowthreshold ) discrete[j] = 0;
			else discrete[j] = discreteOld;
		}
		discreteOld = discrete[j];

		#ifdef DISCROUT
			#ifndef LOWPASSOUT
			writeSample16(wavout,discrete[j]*32760);
			#endif
			#ifdef LOWPASSOUT
			inTmp = flt[j] * 32768;
			writeSample16(wavout,inTmp);
			#endif
		#endif
	}

	for (j=0; j<remainder; j++) {
		if (last == 0 && discrete[j] == 1) {
			diff[diffCnt] = -1;
			frames[diffCnt] = frameCnt;
			diffCnt++;
			last = 1;
			frameCnt=0;
		}
		else if (last == 1 && discrete[j] == 0) {
			diff[diffCnt] = 1;
			frames[diffCnt] = frameCnt;
			diffCnt++;
			last = 0;
			frameCnt=0;
		}
		else frameCnt++;
	}

	frames[0]=0;

	printout("\nDiscretization: ...process terminated!\n\n",0);

	closeWav16(wavfd);
	#ifdef DISCROUT
	closeWav16(wavout);
	#endif

	printout("\nDecoding: process begins...\n\n",0);

	int *tokens = (int*)malloc(diffCnt*sizeof(int));
	for (i=0; i<diffCnt; i++) {
		tokens[i] = diff[i] * frames[i];
	}
	char *text = (char*)malloc(5*sizeof(char));
	int result = decode(diffCnt, tokens, 5, text);

	printout("\nDecoding: ...process terminated!\n\n",0);

	printresult("\nThe result is: %s \n\n",text);

	//free(in);
	//free(abs);
	//free(abs2);
	//free(a);
	//free(b);
	//free(coeff);
	//free(flt2);
	//free(flt);
	//free(discrete);
	//free(diff);
	//free(frames);
	//free(tokens);
	return result;

}

void rectify(int np, float* x, float *y) {
	int i;
	for (i=0; i<np; i++) {
		y[i] = fabs ( x[i] );
	}
}

void filter(float coeff[2*FILTER_ORDER], int np, float *x, float *y) {
	int i,j;
//	float a[FILTER_ORDER];
//	float b[FILTER_ORDER];
	float *a = (float*)malloc(FILTER_ORDER*sizeof(float));
	float *b = (float*)malloc(FILTER_ORDER*sizeof(float));
	for (i=0; i<FILTER_ORDER; i++) {
		a[i] = coeff[i];
		b[i] = coeff[FILTER_ORDER+i];
	}

	y[0]=b[0]*x[0];
	for (i=1;i<FILTER_ORDER;i++) {
		y[i]=0.0;
		for (j=0;j<i+1;j++)
			y[i]=y[i]+b[j]*x[i-j];
		for (j=0;j<i;j++)
			y[i]=y[i]-a[j+1]*y[i-j-1];
	}	// end of initial part

	for (i=FILTER_ORDER;i<np;i++) {
		y[i]=0.0;
		for (j=0;j<FILTER_ORDER;j++)
			y[i]=y[i]+b[j]*x[i-j];
		for (j=0;j<FILTER_ORDER-1;j++)
			y[i]=y[i]-a[j+1]*y[i-j-1];
	}
	//free(a);
	//free(b);
}	// end of filter

void average(int np, float* x, float* avg) {
	int i;
	float sum=0;
	for (i=0; i<np; i++) {
		sum += x[i];
	}
	(*avg)= sum/(float)np;
}

int decode(int diffLen, int* tokens, int textLen, char* text) {
	int i;

	int *tokens2 = (int*)malloc(diffLen*sizeof(int));
	int *buff = (int*)malloc(diffLen*sizeof(int));
	int buffLen = 0;
	int textCnt = 0;

	float cut_t,cut_s;
	posavg(diffLen,tokens,&cut_t);
	negavg(diffLen,tokens,&cut_s);
	//printf("Pos: %f, Neg: %f\n",cut_t,cut_s);
	for (i=1; i<diffLen; i++) {
		if (tokens[i] > 0 && tokens[i] < cut_t) tokens2[i-1] = 1;
		else if (tokens[i] > 0 && tokens[i] >= cut_t) tokens2[i-1] = 2;
		else if (tokens[i] < 0 && tokens[i] <= cut_s) tokens2[i-1] = -2;
		else if (tokens[i] < 0 && tokens[i] > cut_s) tokens2[i-1] = -1;
	}
	tokens2[diffLen-1]=-2;

	for (i=0; i<textLen-1; i++) text[i]='_';
	text[textLen-1]='\0';
	for (i=0; i<diffLen; i++) {
		switch(tokens2[i]) {
		case 1:
		case 2:
			buff[buffLen] = tokens2[i];
			buffLen++;
		case -1:
			break;
		case -2:
			if ( textCnt >= textLen-1) return 2;
			if ( decodeSymbol(buffLen,buff,&text[textCnt]) != 0 ) return 1;
			buffLen=0;
			textCnt++;
		}
	}

	//free(tokens2);
	//free(buff);
	return 0;
}

void posavg(int np, int* x, float* avg) {
	int i;
	int sum=0,cnt=0;
	for (i=0; i<np; i++) {
		if (x[i]>0) {sum += x[i]; cnt++;}
	}
	(*avg) = (float)sum/(float)cnt;
}

void negavg(int np, int* x, float* avg) {
	int i;
	int sum=0,cnt=0;
	for (i=0; i<np; i++) {
		if (x[i]<0) {sum += x[i]; cnt++;}
	}
	(*avg) = (float)sum/(float)cnt;
}

int decodeSymbol(int seqLen, int* sequence, char* symbol) {
	int i,j;
	int equal = 0;
	int *newSeq = (int*)malloc(6*sizeof(int));

	for (i=0; i<6; i++) {
		if (i<seqLen)
			newSeq[i]=sequence[i];
		else newSeq[i]=0;
	}

	for (i=0; (i<40 && equal == 0); i++) {
		equal = 1;
		for (j=0; (j<6 && equal == 1); j++)
			if (newSeq[j] != code[i][j]) equal = 0;
	}
	i--;

	if (equal == 1) {
		(*symbol)=symbols[i];
		return 0;
	}
	else {
		(*symbol)='*';
		return 1;
	}
}

int code[40][6] = {	1,2,0,0,0,0,	2,1,1,1,0,0,	2,1,2,1,0,0,	2,1,1,0,0,0,	1,0,0,0,0,0,	1,1,2,1,0,0,	2,2,1,0,0,0,	1,1,1,1,0,0,	1,1,0,0,0,0,
			1,2,2,2,0,0,	2,1,2,0,0,0,	1,2,1,1,0,0,	2,2,0,0,0,0,	2,1,0,0,0,0,	2,2,2,0,0,0,	1,2,2,1,0,0,	1,2,1,2,0,0,	1,2,1,0,0,0,
			1,1,1,0,0,0,	2,0,0,0,0,0,	1,1,2,0,0,0,	1,1,1,2,0,0,	1,2,2,0,0,0,	2,1,1,2,0,0,	2,1,2,2,0,0,	2,2,1,1,0,0,
			1,2,1,2,1,2,	2,2,1,1,2,2,	1,1,2,2,1,1,	2,1,1,2,1,0,
			1,2,2,2,2,0,	1,1,2,2,2,0,	1,1,1,2,2,0,	1,1,1,1,2,0,	1,1,1,1,1,0,	2,1,1,1,1,0,	2,2,1,1,1,0,	2,2,2,1,1,0,	2,2,2,2,1,0,
			2,2,2,2,2,0	};

char symbols[40] = {	'a',		'b',		'c',		'd',		'e',		'f',		'g',		'h',		'i',
			'j',		'k',		'l',		'm',		'n',		'o',		'p',		'q',		'r',
			's',		't',		'u',		'v',		'w',		'x',		'y',		'z',
			'.',		',',		'?',		'/',
			'1',		'2',		'3',		'4',		'5',		'6',		'7',		'8',		'9',
			'0',	};

