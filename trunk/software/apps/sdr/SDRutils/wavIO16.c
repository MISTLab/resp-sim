#include "wavIO16.h"

int openWavToRead16(char path[512], int *samples){

	int i;

	printout("\nOpening ",0);
	printout(path,0);
	printout("...\n\n",0);

	int wavfile = open(path, O_RDONLY);
	if(wavfile == -1) {
		printout("\nERROR: File ",0);
		printout(path,0);
		printout(" cannot be opened!\n\n",0);
		return -1;
	}

	char	myChunkID[5];
	int 	myChunkSize;
	char	myFormat[5];
	char	mySubChunk1ID[5];
	int	mySubChunk1Size;
	short 	myAudioFormat;
	short 	myChannels;
	int   	mySampleRate;
	int   	myByteRate;
	short 	myBlockAlign;
	short 	myBitsPerSample;
	char	mySubChunk2ID[5];
	int	myDataSize;

//	lseek( wavfile, 0, SEEK_SET);
	read( wavfile, (char*) myChunkID, 4);
	myChunkID[4]='\0';
//	lseek( wavfile, 4, SEEK_SET);
	read( wavfile, (char*) &myChunkSize, 4);
//	lseek( wavfile, 8, SEEK_SET);
	read( wavfile, (char*) &myFormat, 4);
	myFormat[4]='\0';
//	lseek( wavfile, 12, SEEK_SET);
	read( wavfile, (char*) &mySubChunk1ID, 4);
	mySubChunk1ID[4]='\0';
//	lseek( wavfile, 16, SEEK_SET);
	read( wavfile, (char*) &mySubChunk1Size, 4);
//	lseek( wavfile, 20, SEEK_SET);
	read( wavfile, (char*) &myAudioFormat, 2);
//	lseek( wavfile, 22, SEEK_SET);
	read( wavfile, (char*) &myChannels, 2);
//	lseek( wavfile, 24, SEEK_SET);
	read( wavfile, (char*) &mySampleRate, 4);
//	lseek( wavfile, 28, SEEK_SET);
	read( wavfile, (char*) &myByteRate, 4);
//	lseek( wavfile, 32, SEEK_SET);
	read( wavfile, (char*) &myBlockAlign, 2);
//	lseek( wavfile, 34, SEEK_SET);
	read( wavfile, (char*) &myBitsPerSample, 2);
//	lseek( wavfile, 36, SEEK_SET);
	read( wavfile, (char*) &mySubChunk2ID, 4);
	mySubChunk2ID[4]='\0';
//	lseek( wavfile, 40, SEEK_SET);
	read( wavfile, (char*) &myDataSize, 4);

	int myNumSamples = myDataSize / ( myChannels * myBitsPerSample/8 );

	#ifdef VERBOSE
	#ifdef VERYVERBOSE
	printf("\n ChunkID: %s\n ChunkSize: %d\n Format: %s\n SubChunk1ID: %s\n SubChunk1Size: %d\n AudioFormat: %d\n Channels: %d\n SampleRate: %d\n ByteRate: %d\n BlockAlign: %d\n BitsPerSample: %d\n SubChunk2ID: %s\n DataSize: %d\n NumSamples: %d\n\n", myChunkID, myChunkSize, myFormat, mySubChunk1ID, mySubChunk1Size, myAudioFormat, myChannels, mySampleRate, myByteRate, myBlockAlign, myBitsPerSample, mySubChunk2ID, myDataSize, myNumSamples);
	#endif
	#endif

	(*samples) = myNumSamples;

	if (myAudioFormat!=1 || mySampleRate!=44100 || myBitsPerSample!=16 || myChannels!=1) {
		printout("\nERROR: File ",0);
		printout(path,0);
		printout(" has an unknown format or a requirement (16-bit, 44100 Hz, mono wav) is missing!\n\n",0);
		return -1;
	}
	return wavfile;

}

int openWavToWrite16(char path[512], int samples){

	int i;

	printout("\nCreating new file ",0);
	printout(path,0);
	printout("...\n\n",0);

	int wavfile = open(path, O_WRONLY|O_CREAT);
	if(wavfile == -1) {
		printout("\nERROR: File ",0);
		printout(path,0);
		printout(" cannot be created!\n\n",0);
		return;
	}

	char	myChunkID[5];
	char	myFormat[5];
	char	mySubChunk1ID[5];
	int	mySubChunk1Size = 16;
	short 	myAudioFormat = 1;
	short 	myChannels = 1;
	int   	mySampleRate = 44100;
	short 	myBitsPerSample = 16;
	int   	myByteRate = mySampleRate * myChannels * myBitsPerSample/8;
	short 	myBlockAlign = myChannels * myBitsPerSample/8;
	char	mySubChunk2ID[5];
	int	myDataSize = samples * myChannels * myBitsPerSample/8;
	int 	myChunkSize = myDataSize + 32;

//	lseek( wavfile, 0, SEEK_SET);
	write( wavfile, (char*) "RIFF", 4);
//	lseek( wavfile, 4, SEEK_SET);
	write( wavfile, (char*) &myChunkSize, 4);
//	lseek( wavfile, 8, SEEK_SET);
	write( wavfile, (char*) "WAVE", 4);
//	lseek( wavfile, 12, SEEK_SET);
	write( wavfile, (char*) "fmt ", 4);
//	lseek( wavfile, 16, SEEK_SET);
	write( wavfile, (char*) &mySubChunk1Size, 4);
//	lseek( wavfile, 20, SEEK_SET);
	write( wavfile, (char*) &myAudioFormat, 2);
//	lseek( wavfile, 22, SEEK_SET);
	write( wavfile, (char*) &myChannels, 2);
//	lseek( wavfile, 24, SEEK_SET);
	write( wavfile, (char*) &mySampleRate, 4);
//	lseek( wavfile, 28, SEEK_SET);
	write( wavfile, (char*) &myByteRate, 4);
//	lseek( wavfile, 32, SEEK_SET);
	write( wavfile, (char*) &myBlockAlign, 2);
//	lseek( wavfile, 34, SEEK_SET);
	write( wavfile, (char*) &myBitsPerSample, 2);
//	lseek( wavfile, 36, SEEK_SET);
	write( wavfile, (char*) "data", 4);
//	lseek( wavfile, 40, SEEK_SET);
	write( wavfile, (char*) &myDataSize, 4);

	return wavfile;

}

int readSample16(int wavfile) {

	int myIntData;
	int16_t my16Data;

	read( wavfile, (int16_t*) &my16Data, 2);
	myIntData=my16Data;

	return myIntData;

}

void writeSample16(int wavfile, int sample) {
	write( wavfile, (char*) &sample, 2);
}

void seekFirstSample16(int wavfile) {
	lseek( wavfile, 44, SEEK_SET);	
}

void closeWav16(int wavfile) {
	printout("\nClosing file %d...\n\n",wavfile);
	close(wavfile);
}

