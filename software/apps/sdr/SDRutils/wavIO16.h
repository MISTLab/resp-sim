#include <stdio.h>
#include <stdint.h>
#include "utils.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int openWavToRead16(char *path, int *samples);		// 7 op, 12 nano + (mem+bus)*(8+516)
int openWavToWrite16(char *path, int samples);		// 7 op, 14 nano + (mem+bus)*(9+512)
int readSample16(int wavfile);				// 5 op, 10 nano + (mem+bus)*6
void writeSample16(int wavfile, int sample);		// 4 op, 10 nano + (mem+bus)*5
void seekFirstSample16(int wavfile);			// 10 op, 26 nano + (mem+bus)*??
void closeWav16(int wavfile);				// 3 op, 7 nano + (mem+bus)*3

/*
	int samples;					// 1 op, 0 nano
	if (wavread == -1) return 1;			// 3 op, 7 nano + (mem+bus)*4
	if (wavwrite == -1) return 1;			// 5 op, 13 nano + (mem+bus)*(7+516)
*/
