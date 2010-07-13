#include "normalize.h"

int normalize(char *inputpath, char *outputpath) {

	int i;

	int samples;
	int wavfd = openWavToRead16(inputpath,&samples);
	if (wavfd == -1) return 1;
	#ifdef QUICK
	samples=Q_SAMPLES;
	#endif

	int wavout = openWavToWrite16(outputpath,samples);
	if (wavout == -1) { closeWav16(wavfd); return 1; }

	#ifdef NORMIN
	int wavin = openWavToWrite16((char*)"in.wav",samples);
	if (wavin == -1) { closeWav16(wavfd); closeWav16(wavout); return 1; }
	#endif

	printout("\nNormalization: starting peak search...\n\n",0);
	int max = readSample16(wavfd),tmp;
	for (i=0; i<samples; i++) {
		tmp = abs( readSample16(wavfd) );
		if ( tmp > max ) max = tmp;
	}

	printout("\nNormalization: ...peak found, equal to %d; process begins...\n\n",max);
	seekFirstSample16(wavfd);
	for (i=0; i<samples; i++) {
		tmp = readSample16(wavfd);
		#ifdef NORMIN
		writeSample16(wavin,tmp);
		#endif
		tmp = 32768 * tmp / max;
		writeSample16(wavout,tmp);
	}
	printout("\nNormalization: ...process terminated!\n\n",0);

	closeWav16(wavfd);
	closeWav16(wavout);
	#ifdef NORMIN
	closeWav16(wavin);
	#endif

	return 0;

}
