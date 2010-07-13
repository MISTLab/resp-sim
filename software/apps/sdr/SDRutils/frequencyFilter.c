#include "frequencyFilter.h"

int frequencyFilter(char inputpath[512], char outputpath[512], int params[3]) {

	int start_freq = params[0];
	int filter = params[1];
	int gain = params[2];

	float g_rLoPhInc = (start_freq * 2.0 * C_PI / FS);
	float g_rOutputGainFactor = gain;
	int g_iFilterBW = filter;			// 0-1-2 => NARROW-MEDIUM-WIDE

	int samples;
	int wavfd = openWavToRead16(inputpath,&samples);
	if (wavfd == -1) return 1;
	#ifdef QUICK
	samples=Q_SAMPLES;
	#endif

	int wavout = openWavToWrite16(outputpath,samples);
	if (wavout == -1) { closeWav16(wavfd); return 1; }

	#ifdef FFLTIN
	int wavin = openWavToWrite16((char*)"in.wav",samples);
	if (wavin == -1) { closeWav16(wavfd); closeWav16(wavout); return 1; }
	#endif

	int input,output;
	float leftInput, rightInput=0;
	float leftOutput, rightOutput=0;
	//float rTmp;
	float rLocalOscPhAcc;	float r44k[2];

	static int iDecimatorIdxIn = 0;
	static float rDecimatorQueueI[64];
	static float rDecimatorQueueQ[64];

	static unsigned int ui44kSampleCounter = 0;
	float r11k[2];

	static int iMainFilterIdxIn = 0;
	static float rMainFilterQueueI[FIR_LARGEST_LEN];
	static float rMainFilterQueueQ[FIR_LARGEST_LEN];

	static int iInterpolatorIdxIn = 0;
	static int iInterpolatorParams[2];
	static float rInterpolatorQueueI[16];
	static float rInterpolatorQueueQ[16];

	// Read input buffer, process data, and fill output buffer.
	int frameCnt,i;
	printout("\nFiltering @ %d ",start_freq);
	printout("with filter %d",filter);
	printout(": process begins...\n\n",0);
	for (frameCnt = 0; frameCnt < samples; frameCnt++) {

		// Get interleaved samples from input file
		input = readSample16(wavfd);
		leftInput = (float) input / 32768;
		#ifdef FFLTIN
		writeSample16(wavin,input);
		#endif

		// Run local oscillator (NCO)
		rLocalOscPhAcc += g_rLoPhInc;
		if (rLocalOscPhAcc > (2.0 * C_PI))
			rLocalOscPhAcc -= (2.0 * C_PI);

		// Half complex mixer
		hcMixer(&leftInput,&rLocalOscPhAcc,r44k);

/*
	##########################################
	 FAKED SIGNAL FOR FFT Y SCALING TESTS:
		leftInput = cos (rLocalOscPhAcc);	amplitude 1.0
	##########################################
*/

		// Decimation filter, R = 4, nTaps = 64 (hardcoded... ;-)
		if (--iDecimatorIdxIn < 0)	// fill buffer backwards (that convolution stuff...)
			iDecimatorIdxIn = 63;	// (well, the filter is symmetrical, but anyway...)

		rDecimatorQueueI[iDecimatorIdxIn] = r44k[0];
		rDecimatorQueueQ[iDecimatorIdxIn] = r44k[1];

		// Decimate Fs and do the heavy stuff at lower speed
		if (++ui44kSampleCounter >= 4) {	// decimation factor = R = 4
	    		ui44kSampleCounter = 0;

			// Fs = 11025 Hz code goes here, first decimation filter:
			decimatorFilter(iDecimatorIdxIn, rDecimatorQueueI, rDecimatorQueueQ, r11k);

			// rI11k and rQ11k now contains the downsampled complex signal
			// ready for processing at Fs = 11025 Hz. Passband = +/-3000Hz
			// with transition bands out to +/-5512.5Hz (stopband).

			// Main selectivity filters
			if (g_iFilterBW == 0) {		// NARROW
				if (--iMainFilterIdxIn < 0) iMainFilterIdxIn = FIR_LEN_CW300H-1;
				rMainFilterQueueI[iMainFilterIdxIn] = r11k[0];
				rMainFilterQueueQ[iMainFilterIdxIn] = r11k[1];
				CW300Filter(iMainFilterIdxIn, rMainFilterQueueI, rMainFilterQueueQ, r11k);
			}
			else if (g_iFilterBW == 1) {	// MEDIUM
				if (--iMainFilterIdxIn < 0) iMainFilterIdxIn = FIR_LEN_CW1K-1;
				rMainFilterQueueI[iMainFilterIdxIn] = r11k[0];
				rMainFilterQueueQ[iMainFilterIdxIn] = r11k[1];
				CW1KFilter(iMainFilterIdxIn, rMainFilterQueueI, rMainFilterQueueQ, r11k);
			}
			else {				// g_iFilterBW == 2 => WIDE
				if (--iMainFilterIdxIn < 0) iMainFilterIdxIn = FIR_LEN_SSB2K4-1;
				rMainFilterQueueI[iMainFilterIdxIn] = r11k[0];
				rMainFilterQueueQ[iMainFilterIdxIn] = r11k[1];
				SSB2K4Filter(iMainFilterIdxIn, rMainFilterQueueI, rMainFilterQueueQ, r11k);
			}

			// "Summing point" of the "Phasing Method Receiver"
			r11k[0] += r11k[1];	// select USB (subtract for LSB)
			r11k[1] = r11k[0];	// same signal to both ears (mono output)

			// Feed a stereo 11k sample to the interpolation filter
			// (two separate channels are used in case we want
			// stereo output in the future for binaural I/Q etc.)
			iInterpolatorIdxIn--;	// backward input, remember that convolution integral...
			if (iInterpolatorIdxIn < 0) iInterpolatorIdxIn = 15;
			rInterpolatorQueueI[iInterpolatorIdxIn] = r11k[0];
			rInterpolatorQueueQ[iInterpolatorIdxIn] = r11k[1];
		}	// end of if (++ui44kSampleCounter >= 4)

/*
		Now we're back in Fs = 44100 Hz business again

		Run interpolation filter at Fs = 44k1

		Note! The interpolator uses the same 64-tap FIR as the decimator but
		since 3 out of 4 input samples are zero, only 16 MAC operations need
		to be computed. This is the reason why the interpolator buffer size
		is only 16. The coefficient set is split into 4 groups (mentally...)
		and the filter computation is cycling through these groups, one
		group per 44k1 sample:

		44k1 sample instant 0 (ui44kSampleCounter == 0):
		- Put new 11k sample into the interpolator queue
		- Compute the filter using coefficients 0, 4, 8 ... 56, 60

		44k1 sample instant 1 (ui44kSampleCounter == 1):
		- Compute the filter using coefficients 1, 5, 9 ... 57, 61

		44k1 sample instant 2 (ui44kSampleCounter == 2):
		- Compute the filter using coefficients 2, 6, 10 ... 58, 62

		44k1 sample instant 3 (ui44kSampleCounter == 3):
		- Compute the filter using coefficients 3, 7, 11 ... 59, 63

		44k1 sample instant 4: (now it starts over again!)
		- Put new 11k sample into the interpolator queue
		- Compute the filter using coefficients 0, 4, 8 ... 56, 60

		etc. etc.
*/

		iInterpolatorParams[0] = ui44kSampleCounter;
		iInterpolatorParams[1] = iInterpolatorIdxIn;
		interpolatorFilter(iInterpolatorParams, rInterpolatorQueueI, rInterpolatorQueueQ, r44k);

		// Write interleaved samples to output buffer
		leftOutput = r44k[0] * g_rOutputGainFactor;	// left
		rightOutput = r44k[1] * g_rOutputGainFactor;	// right

		output = leftOutput * 32768;
		writeSample16(wavout,output);
		#ifdef VERBOSE
			#ifdef VERYVERBOSE		
			printf ("%d\tInL: %f, InR: %f\t\tOutL: %f, OutR: %f\t\tIntIn: %d, IntOut: %d\n", frameCnt, leftInput, rightInput, leftOutput, rightOutput, input, output);
			#endif
		if (frameCnt%1000 == 0) printout("\nFiltering: checkpoint %d reached!\n\n",frameCnt);
		#endif
	}
	printout("\nFiltering: ...process terminated!\n\n",0);

	closeWav16(wavfd);
	closeWav16(wavout);
	#ifdef FFLTIN
	closeWav16(wavin);
	#endif

	return 0;

}

void hcMixer(float *input, float *rLocalOscPhAcc, float output[2]) {

	#ifdef OVERLOAD
	int q_OverLoad = 0;
	#endif

	// Check peak signal level
	float rTmp = abs(*input);

	// Half complex mixer
	float rI44k = *input * cos (*rLocalOscPhAcc);
	float rQ44k = *input * sin (*rLocalOscPhAcc);

	output[0] = rI44k;
	output[1] = rQ44k;

	// 1 dB below full-scale (1.0) activates the overload (clipping) flag
	#ifdef OVERLOAD
	if (rTmp > 0.891251)
		q_OverLoad = 1;
	#endif

}

void decimatorFilter(int iDecimatorIdxIn, float rDecimatorQueueI[64], float rDecimatorQueueQ[64], float output[2]) {
	float rI11k = 0.0;
	float rQ11k = 0.0;			// clear MAC accumulators
	float rTmp;
	int i;

	for (i = 0; i < 64; i++) {
		rTmp = rDecIntCoeffs[i];
		rI11k += rDecimatorQueueI[iDecimatorIdxIn] * rTmp;
		rQ11k += rDecimatorQueueQ[iDecimatorIdxIn] * rTmp;
		if (++iDecimatorIdxIn >= 64) iDecimatorIdxIn = 0;
	}

	rI11k *= 1.0e-8;			// normalize (FIR gain ~10^8 !!!)
	rQ11k *= 1.0e-8;

	output[0] = rI11k;
	output[1] = rQ11k;
}

void CW300Filter(int iMainFilterIdxIn, float rMainFilterQueueI[428], float rMainFilterQueueQ[428], float output[2]) {
	float rI11k = 0.0;
	float rQ11k = 0.0;			// clear MAC accumulators
	int i;

	for (i = 0; i < FIR_LEN_CW300H; i++) {
		rI11k += rMainFilterQueueI[iMainFilterIdxIn] * rCw300HCoeffs_I[i];
		rQ11k += rMainFilterQueueQ[iMainFilterIdxIn] * rCw300HCoeffs_Q[i];
		if (++iMainFilterIdxIn >= FIR_LEN_CW300H) iMainFilterIdxIn = 0;
	}

	rI11k *= 1.0e-8;			// normalize (FIR gain ~10^8 !!!)
	rQ11k *= 1.0e-8;

	output[0] = rI11k;
	output[1] = rQ11k;
}

void CW1KFilter(int iMainFilterIdxIn, float rMainFilterQueueI[428], float rMainFilterQueueQ[428], float output[2]) {
	float rI11k = 0.0;
	float rQ11k = 0.0;			// clear MAC accumulators
	int i;

	for (i = 0; i < FIR_LEN_CW1K; i++) {
		rI11k += rMainFilterQueueI[iMainFilterIdxIn] * rCw1kCoeffs_I[i];
		rQ11k += rMainFilterQueueQ[iMainFilterIdxIn] * rCw1kCoeffs_Q[i];
		if (++iMainFilterIdxIn >= FIR_LEN_CW1K) iMainFilterIdxIn = 0;
	}

	rI11k *= 1.0e-8;			// normalize (FIR gain ~10^8 !!!)
	rQ11k *= 1.0e-8;

	output[0] = rI11k;
	output[1] = rQ11k;
}

void SSB2K4Filter(int iMainFilterIdxIn, float rMainFilterQueueI[428], float rMainFilterQueueQ[428], float output[2]) {
	float rI11k = 0.0;
	float rQ11k = 0.0;		// clear MAC accumulators
	int i;

	for (i = 0; i < FIR_LEN_SSB2K4; i++) {
		rI11k += rMainFilterQueueI[iMainFilterIdxIn] * rSsb2k4Coeffs_I[i];
		rQ11k += rMainFilterQueueQ[iMainFilterIdxIn] * rSsb2k4Coeffs_Q[i];
		if (++iMainFilterIdxIn >= FIR_LEN_SSB2K4) iMainFilterIdxIn = 0;
	}

	rI11k *= 1.0e-8;			// normalize (FIR gain ~10^8 !!!)
	rQ11k *= 1.0e-8;

	output[0] = rI11k;
	output[1] = rQ11k;
}

void interpolatorFilter(int iInterpolatorParams[2], float rInterpolatorQueueI[16], float rInterpolatorQueueQ[16], float output[2]) {

	float rI44k = 0.0;
	float rQ44k = 0.0;			// clear MAC accumulators
	float rTmp;
	int i = iInterpolatorParams[0];		// 0..3, 0 just after a new value has been inserted
	int iInterpolatorIdxIn = iInterpolatorParams[1];

	do {					// loop from newest to oldest sample (they are stored "backwards")
		rTmp = rDecIntCoeffs[i];
		rI44k += rInterpolatorQueueI[iInterpolatorIdxIn] * rTmp;
		rQ44k += rInterpolatorQueueQ[iInterpolatorIdxIn] * rTmp;
		if (++iInterpolatorIdxIn >= 16)
		iInterpolatorIdxIn = 0;
		i += 4;
	} while (i < 64);			// last i == 60, 61, 62 or 63 depending on coeff group used

	rI44k *= 4.0e-8;			// normalize (FIR gain ~10^8 !!!)
	rQ44k *= 4.0e-8;

	output[0] = rI44k;
	output[1] = rQ44k;
}

