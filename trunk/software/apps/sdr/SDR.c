#include <stdio.h>
#include "utils.h"
#include "frequencyFilter.h"
#include "normalize.h"
#include "morseDecoder.h"

void printStatus() {printf("\n\n");}

int main(void) {

	int result=0;

	// fish
	// Filter Params:	1) start_freq	2) filter	3) gain
	int filterParams[3] = {0,0,1};

	// Morse Params:	1) lowthreshold	2) highthreshold
	float morseParams[2] = {0.4,0.6};

	result = frequencyFilter(
		(char*)"software/apps/sdr/SDRutils/60wpm_mix.wav",
		(char*)"software/apps/sdr/SDRutils/60wpm_filtered.wav",
		filterParams);

	if (result !=0 ) return result;

	printStatus();

	result = normalize(
		(char*)"software/apps/sdr/SDRutils/60wpm_filtered.wav",
		(char*)"software/apps/sdr/SDRutils/60wpm_normalized.wav");

	if (result !=0 ) return result;

	printStatus();

	result = morseDecoder(
		(char*)"software/apps/sdr/SDRutils/60wpm_normalized.wav",
		morseParams);

//	if (result !=0 ) return result;

	printStatus();

	// chip
	filterParams[0]=300;

	result = frequencyFilter(
		(char*)"software/apps/sdr/SDRutils/60wpm_mix.wav",
		(char*)"software/apps/sdr/SDRutils/60wpm_filtered.wav",
		filterParams);

	if (result !=0 ) return result;

	printStatus();

	result = normalize(
		(char*)"software/apps/sdr/SDRutils/60wpm_filtered.wav",
		(char*)"software/apps/sdr/SDRutils/60wpm_normalized.wav");

	if (result !=0 ) return result;

	printStatus();

	result = morseDecoder(
		(char*)"software/apps/sdr/SDRutils/60wpm_normalized.wav",
		morseParams);

//	if (result !=0 ) return result;

	printStatus();

	// ab45
	filterParams[0]=1300;
	filterParams[1]=1;

	result = frequencyFilter(
		(char*)"software/apps/sdr/SDRutils/60wpm_mix.wav",
		(char*)"software/apps/sdr/SDRutils/60wpm_filtered.wav",
		filterParams);

	if (result !=0 ) return result;

	printStatus();

	result = normalize(
		(char*)"software/apps/sdr/SDRutils/60wpm_filtered.wav",
		(char*)"software/apps/sdr/SDRutils/60wpm_normalized.wav");

	if (result !=0 ) return result;

	printStatus();

	result = morseDecoder(
		(char*)"software/apps/sdr/SDRutils/60wpm_normalized.wav",
		morseParams);

//	if (result !=0 ) return result;

	printStatus();

	// tune
	filterParams[0]=3500;
	filterParams[1]=2;

	result = frequencyFilter(
		(char*)"software/apps/sdr/SDRutils/60wpm_mix.wav",
		(char*)"software/apps/sdr/SDRutils/60wpm_filtered.wav",
		filterParams);

	if (result !=0 ) return result;

	printStatus();

	result = normalize(
		(char*)"software/apps/sdr/SDRutils/60wpm_filtered.wav",
		(char*)"software/apps/sdr/SDRutils/60wpm_normalized.wav");

	if (result !=0 ) return result;

	printStatus();

	result = morseDecoder(
		(char*)"software/apps/sdr/SDRutils/60wpm_normalized.wav",
		morseParams);

//	if (result !=0 ) return result;

	printStatus();

	// moon
	filterParams[0]=8500;

	result = frequencyFilter(
		(char*)"software/apps/sdr/SDRutils/60wpm_mix.wav",
		(char*)"software/apps/sdr/SDRutils/60wpm_filtered.wav",
		filterParams);

	if (result !=0 ) return result;

	printStatus();

	result = normalize(
		(char*)"software/apps/sdr/SDRutils/60wpm_filtered.wav",
		(char*)"software/apps/sdr/SDRutils/60wpm_normalized.wav");

	if (result !=0 ) return result;

	printStatus();

	result = morseDecoder(
		(char*)"software/apps/sdr/SDRutils/60wpm_normalized.wav",
		morseParams);

//	if (result !=0 ) return result;

	printStatus();

/*	float coeff[100];
	float x[512];
	float y[512];
	filter(coeff,512,x,y);

	printout("\ndone\n",0);
*/
	return result;

}

