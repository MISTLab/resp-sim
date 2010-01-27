#ifndef __TOYAPP_H___
#define __TOYAPP_H___

#include <stdio.h>
#include <stdlib.h>


#define SCALAR			256
#define VECTOR			1
#define BUFFSTREAMING	2
#define VARLEN			8192
#define VIDEOFRAME		4096

#define DATAINT32		4
#define DATAINT16		8
#define DATAINT8		16
#define DATAFLOAT32		32
#define DATADOUBLE64	64
#define CUSTOM			128

#define PLAYBUFFER		BUFFSTREAMING

#define FRAMESIZE		4096		
#define SAMPLERATE		44100		

struct IOType
{
	void	*DataBuffer;	
	int		DataType;		
	int		DataLen;		
	int		MaxDataLen;		
};

struct FileRead_HANDLENUTS
{
	char FileName[260];
	FILE *hFile;
	short *ReadBuffer;

	//////////////////
	int NumInput;
	int NumOutput;

	struct IOType **Input;
	struct IOType **Output;
};

struct Split_HANDLENUTS
{
	//////////////////
	int NumInput;
	int NumOutput;

	struct IOType **Input;
	struct IOType **Output;
};

struct Delay_HANDLENUTS
{
	int DelayType;

	int MsDelay;
	int SampDelay;

	int DelayPos;
	int LenBuffer;
	double *DelayLine;

	//////////////////
	int NumInput;
	int NumOutput;

	struct IOType **Input;
	struct IOType **Output;
};

struct Gain_HANDLENUTS
{
	double GainValue;

	//////////////////
	int NumInput;
	int NumOutput;

	struct IOType **Input;
	struct IOType **Output;
};

struct BoardHandle
{
	FILE *FileOut;
	//////////////////
	int NumInput;
	int NumOutput;

	struct IOType **Input;
	struct IOType **Output;
};

void InitInstance();
void CreateBuffer();
void SetUp();

int FileRead_Init(struct FileRead_HANDLENUTS *Handle,int FrameSize,int SampleRate);
int Split_Init(struct Split_HANDLENUTS *Handle,int FrameSize,int SampleRate);
int Delay_Init(struct Delay_HANDLENUTS *Handle,int FrameSize,int SampleRate);
int Gain_Init(struct Gain_HANDLENUTS *Handle,int FrameSize,int SampleRate);

int FileRead_Process(struct FileRead_HANDLENUTS *Handle,struct IOType **Input,struct IOType **Output);
int Split_Process(struct Split_HANDLENUTS *Handle,struct IOType **Input,struct IOType **Output);
int Delay_Process(struct Delay_HANDLENUTS *Handle,struct IOType **Input,struct IOType **Output);
int Gain_Process(struct Gain_HANDLENUTS *Handle,struct IOType **Input,struct IOType **Output);

int FileRead_Delete(struct FileRead_HANDLENUTS *Handle);
int Split_Delete(struct Split_HANDLENUTS *Handle);
int Delay_Delete(struct Delay_HANDLENUTS *Handle);
int Gain_Delete(struct Gain_HANDLENUTS *Handle);


void DisposeBuffer();
void DeleteInstance();

struct BoardHandle *Board;			
struct FileRead_HANDLENUTS *FileRead_00;
struct Split_HANDLENUTS	 *Split_00;	
struct Delay_HANDLENUTS  *Delay_00;	
struct Delay_HANDLENUTS  *Delay_01;	
struct Gain_HANDLENUTS   *Gain_00;		
struct Gain_HANDLENUTS   *Gain_01;		


#endif


/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// Automatic Code Generation //////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void InitInstance() // Create and Init Nuts' instance
{
	// Create Instance
	Board		=(struct BoardHandle *)malloc((sizeof(struct BoardHandle)));
	FileRead_00	=(struct FileRead_HANDLENUTS *)malloc(sizeof(struct FileRead_HANDLENUTS));
	Split_00	=(struct Split_HANDLENUTS *)malloc(sizeof(struct Split_HANDLENUTS));
	Delay_00	=(struct Delay_HANDLENUTS *)malloc(sizeof(struct Delay_HANDLENUTS));
	Delay_01	=(struct Delay_HANDLENUTS *)malloc(sizeof(struct Delay_HANDLENUTS));
	Gain_00		=(struct Gain_HANDLENUTS *)malloc(sizeof(struct Gain_HANDLENUTS));
	Gain_01		=(struct Gain_HANDLENUTS *)malloc(sizeof(struct Gain_HANDLENUTS));

	// Init NumInput, NumOutput and Buffer arrays 

	FileRead_00->NumInput=0;	FileRead_00->NumOutput=1;	FileRead_00->Input=0;	FileRead_00->Output=0;
	Split_00->NumInput=1;		Split_00->NumOutput=2;		Split_00->Input=0;		Split_00->Output=0;
	Delay_00->NumInput=1;		Delay_00->NumOutput=1;		Delay_00->Input=0;		Delay_00->Output=0;
	Delay_01->NumInput=1;		Delay_01->NumOutput=1;		Delay_01->Input=0;		Delay_01->Output=0;
	Gain_00->NumInput=1;		Gain_00->NumOutput=1;		Gain_00->Input=0;		Gain_00->Output=0;
	Gain_01->NumInput=1;		Gain_01->NumOutput=1;		Gain_01->Input=0;		Gain_01->Output=0;
	Board->NumInput=2;			Board->NumOutput=2;			Board->Input=0;			Board->Output=0;
}

void DeleteInstance() //dispose Nuts' memory  
{
	// Free Nuts instance

	free(Board);
	free(FileRead_00);
	free(Split_00);
	free(Delay_00);
	free(Delay_01);
	free(Gain_00);
	free(Gain_01);
}

void CreateBuffer() //Create buffers for each Nuts connections
{
	int k;
	
	// Each shared buffer pointer between different Nuts depict a connection.

	FileRead_00->Output=(struct IOType **)calloc(FileRead_00->NumOutput,sizeof(struct IOType*));
	for (k=0;k<FileRead_00->NumOutput;k++) FileRead_00->Output[k]=(struct IOType *)calloc(1,sizeof(struct IOType));
	FileRead_00->Output[0]->DataLen=FRAMESIZE;
	FileRead_00->Output[0]->MaxDataLen=FRAMESIZE;
	FileRead_00->Output[0]->DataType=PLAYBUFFER;
	FileRead_00->Output[0]->DataBuffer=calloc(FRAMESIZE,sizeof(double));

	Split_00->Input=(struct IOType **)calloc(Split_00->NumInput,sizeof(struct IOType *));
	for (k=0;k<Split_00->NumInput;k++) Split_00->Input[k]=(struct IOType *)calloc(1,sizeof(struct IOType));
	Split_00->Input[0]->DataLen=FRAMESIZE;
	Split_00->Input[0]->MaxDataLen=FRAMESIZE;
	Split_00->Input[0]->DataType=PLAYBUFFER;
	Split_00->Input[0]->DataBuffer=FileRead_00->Output[0]->DataBuffer; // Shared Memory
	Split_00->Output=(struct IOType **)calloc(Split_00->NumOutput,sizeof(struct IOType *));
	for (k=0;k<Split_00->NumOutput;k++) Split_00->Output[k]=(struct IOType *)calloc(1,sizeof(struct IOType));
	Split_00->Output[0]->DataLen=FRAMESIZE;
	Split_00->Output[0]->MaxDataLen=FRAMESIZE;
	Split_00->Output[0]->DataType=PLAYBUFFER;
	Split_00->Output[0]->DataBuffer=calloc(FRAMESIZE,sizeof(double));
	Split_00->Output[1]->DataLen=FRAMESIZE;
	Split_00->Output[1]->MaxDataLen=FRAMESIZE;
	Split_00->Output[1]->DataType=PLAYBUFFER;
	Split_00->Output[1]->DataBuffer=calloc(FRAMESIZE,sizeof(double));

	Delay_00->Input=(struct IOType **)calloc(Delay_00->NumInput,sizeof(struct IOType *));
	for (k=0;k<Delay_00->NumInput;k++) Delay_00->Input[k]=(struct IOType *)calloc(1,sizeof(struct IOType));
	Delay_00->Input[0]->DataLen=FRAMESIZE;
	Delay_00->Input[0]->MaxDataLen=FRAMESIZE;
	Delay_00->Input[0]->DataType=PLAYBUFFER;
	Delay_00->Input[0]->DataBuffer=Split_00->Output[0]->DataBuffer; // Shared Memory
	Delay_00->Output=(struct IOType **)calloc(Delay_00->NumOutput,sizeof(struct IOType *));
	for (k=0;k<Delay_00->NumOutput;k++) Delay_00->Output[k]=(struct IOType *)calloc(1,sizeof(struct IOType));
	Delay_00->Output[0]->DataLen=FRAMESIZE;
	Delay_00->Output[0]->MaxDataLen=FRAMESIZE;
	Delay_00->Output[0]->DataType=PLAYBUFFER;
	Delay_00->Output[0]->DataBuffer=calloc(FRAMESIZE,sizeof(double));

	Delay_01->Input=(struct IOType **)calloc(Delay_01->NumInput,sizeof(struct IOType *));
	for (k=0;k<Delay_01->NumInput;k++) Delay_01->Input[k]=(struct IOType *)calloc(1,sizeof(struct IOType));
	Delay_01->Input[0]->DataLen=FRAMESIZE;
	Delay_01->Input[0]->MaxDataLen=FRAMESIZE;
	Delay_01->Input[0]->DataType=PLAYBUFFER;
	Delay_01->Input[0]->DataBuffer=Split_00->Output[1]->DataBuffer; // Shared Memory
	Delay_01->Output=(struct IOType **)calloc(Delay_01->NumOutput,sizeof(struct IOType *));
	for (k=0;k<Delay_01->NumOutput;k++) Delay_01->Output[k]=(struct IOType *)calloc(1,sizeof(struct IOType));
	Delay_01->Output[0]->DataLen=FRAMESIZE;
	Delay_01->Output[0]->MaxDataLen=FRAMESIZE;
	Delay_01->Output[0]->DataType=PLAYBUFFER;
	Delay_01->Output[0]->DataBuffer=calloc(FRAMESIZE,sizeof(double));

	Gain_00->Input=(struct IOType **)calloc(Gain_00->NumInput,sizeof(struct IOType *));
	for (k=0;k<Gain_00->NumInput;k++) Gain_00->Input[k]=(struct IOType *)calloc(1,sizeof(struct IOType));
	Gain_00->Input[0]->DataLen=FRAMESIZE;
	Gain_00->Input[0]->MaxDataLen=FRAMESIZE;
	Gain_00->Input[0]->DataType=PLAYBUFFER;
	Gain_00->Input[0]->DataBuffer=Delay_00->Output[0]->DataBuffer; // Shared Memory
	Gain_00->Output=(struct IOType **)calloc(Gain_00->NumOutput,sizeof(struct IOType *));
	for (k=0;k<Gain_00->NumOutput;k++) Gain_00->Output[k]=(struct IOType *)calloc(1,sizeof(struct IOType));
	Gain_00->Output[0]->DataLen=FRAMESIZE;
	Gain_00->Output[0]->MaxDataLen=FRAMESIZE;
	Gain_00->Output[0]->DataType=PLAYBUFFER;
	Gain_00->Output[0]->DataBuffer=calloc(FRAMESIZE,sizeof(double));

	Gain_01->Input=(struct IOType **)calloc(Gain_01->NumInput,sizeof(struct IOType *));
	for (k=0;k<Gain_01->NumInput;k++) Gain_01->Input[k]=(struct IOType *)calloc(1,sizeof(struct IOType));
	Gain_01->Input[0]->DataLen=FRAMESIZE;
	Gain_01->Input[0]->MaxDataLen=FRAMESIZE;
	Gain_01->Input[0]->DataType=PLAYBUFFER;
	Gain_01->Input[0]->DataBuffer=Delay_01->Output[0]->DataBuffer; // Shared Memory
	Gain_01->Output=(struct IOType **)calloc(Gain_01->NumOutput,sizeof(struct IOType *));
	for (k=0;k<Gain_01->NumOutput;k++) Gain_01->Output[k]=(struct IOType *)calloc(1,sizeof(struct IOType));
	Gain_01->Output[0]->DataLen=FRAMESIZE;
	Gain_01->Output[0]->MaxDataLen=FRAMESIZE;
	Gain_01->Output[0]->DataType=PLAYBUFFER;
	Gain_01->Output[0]->DataBuffer=calloc(FRAMESIZE,sizeof(double));

	Board->Output=(struct IOType **)calloc(Board->NumOutput,sizeof(struct IOType *));
	for (k=0;k<Board->NumOutput;k++) Board->Output[k]=(struct IOType *)calloc(1,sizeof(struct IOType));
	Board->Output[0]->DataLen=FRAMESIZE;
	Board->Output[0]->MaxDataLen=FRAMESIZE;
	Board->Output[0]->DataType=PLAYBUFFER;
	Board->Output[0]->DataBuffer=Gain_00->Output[0]->DataBuffer; // Shared Memory
	Board->Output[1]->DataLen=FRAMESIZE;
	Board->Output[1]->MaxDataLen=FRAMESIZE;
	Board->Output[1]->DataType=PLAYBUFFER;
	Board->Output[1]->DataBuffer=Gain_01->Output[0]->DataBuffer; // Shared Memory

}

void DisposeBuffer() // dispose buffer memory
{
	int k;

	free(FileRead_00->Output[0]->DataBuffer);
	for (k=0;k<FileRead_00->NumOutput;k++) free(FileRead_00->Output[k]);
	free(FileRead_00->Output);

	for (k=0;k<Split_00->NumInput;k++) free(Split_00->Input[k]);
	free(Split_00->Input);
	free(Split_00->Output[0]->DataBuffer);
	free(Split_00->Output[1]->DataBuffer);
	for (k=0;k<Split_00->NumOutput;k++) free(Split_00->Output[k]);
	free(Split_00->Output);
	
	for (k=0;k<Delay_00->NumInput;k++) free(Delay_00->Input[k]);
	free(Delay_00->Input);
	free(Delay_00->Output[0]->DataBuffer);
	for (k=0;k<Delay_00->NumOutput;k++) free(Delay_00->Output[k]);
	free(Delay_00->Output);

	for (k=0;k<Delay_01->NumInput;k++) free(Delay_01->Input[k]);
	free(Delay_01->Input);
	free(Delay_01->Output[0]->DataBuffer);
	for (k=0;k<Delay_01->NumOutput;k++) free(Delay_01->Output[k]);
	free(Delay_01->Output);	

	for (k=0;k<Gain_00->NumInput;k++) free(Gain_00->Input[k]);
	free(Gain_00->Input);
	free(Gain_00->Output[0]->DataBuffer);
	for (k=0;k<Gain_00->NumOutput;k++) free(Gain_00->Output[k]);
	free(Gain_00->Output);	

	for (k=0;k<Gain_01->NumInput;k++) free(Gain_01->Input[k]);
	free(Gain_01->Input);
	free(Gain_01->Output[0]->DataBuffer);
	for (k=0;k<Gain_01->NumOutput;k++) free(Gain_01->Output[k]);
	free(Gain_01->Output);	

	for (k=0;k<Board->NumOutput;k++) free(Board->Output[k]);
	free(Board->Output);	
}


void SetUp() //  Nuts Parameters Setup
{
	sprintf(FileRead_00->FileName,"test.raw");  // Set the name of file to read.

	Delay_00->DelayType=0;		// Set the Type and Time Delay for Delay_00.
	Delay_00->SampDelay=10;
	Delay_00->MsDelay=100;

	Delay_01->DelayType=1;		// Set the Type and Time Delay for Delay_01.
	Delay_01->SampDelay=25;
	Delay_01->MsDelay=250;

	Gain_00->GainValue=0.95;	// Set the Gain value for Gain_00.
	
	Gain_01->GainValue=0.75;	// Set the Gain value for Gain_01.
}

/////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// Init Function Section /////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

int FileRead_Init(struct FileRead_HANDLENUTS *Handle,int FrameSize,int SampleRate)
{
	Handle->ReadBuffer=0;
	Handle->hFile=0;
	if (Handle->FileName[0]!=0)
	{
		Handle->hFile=fopen(Handle->FileName,"rb");
		if (Handle->hFile!=0)
		{
			Handle->ReadBuffer=(short *)calloc(FrameSize,sizeof(short));
		}
	}

	return 0;
}

int Split_Init(struct Split_HANDLENUTS *Handle,int FrameSize,int SampleRate)
{
	return 0;
}

int Delay_Init(struct Delay_HANDLENUTS *Handle,int FrameSize,int SampleRate)
{
	Handle->LenBuffer=0;
	if (Handle->DelayType==0)
	{
		Handle->LenBuffer=(int)((double)(Handle->MsDelay)*(double)SampleRate/1000.0);
	}
	else
		Handle->LenBuffer=Handle->SampDelay;

	Handle->DelayLine=(double *)calloc(Handle->LenBuffer,sizeof(double));
	Handle->DelayPos=0;

	return 0;
}

int Gain_Init(struct Gain_HANDLENUTS *Handle,int FrameSize,int SampleRate)
{
	return 0;
}

int Board_Init(struct BoardHandle *Handle,int FrameSize,int SampleRate)
{
	Handle->FileOut=fopen("StereoOutput_16bit.raw","wb");
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// Process Function Section //////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

int Board_OutputProcess(struct BoardHandle *Handle,struct IOType **Output)
{
	int i,k;

	for (k=0;k<Output[0]->DataLen;k++) 
	{
		for (i=0;i<Handle->NumOutput;i++) 
		{
			short DataToWrite=(short)(((double *)Output[i]->DataBuffer)[k]);
			fwrite(&DataToWrite,sizeof(short),1,Handle->FileOut);
		}
	}

	return 0;
}

int FileRead_Process(struct FileRead_HANDLENUTS *Handle,struct IOType **Input,struct IOType **Output)
{
	int i;
	if  ((Handle->hFile!=0) & (Handle->ReadBuffer!=0))
	{
		int NumRead=(int)fread(Handle->ReadBuffer,sizeof(short),Output[0]->DataLen,Handle->hFile);
		for (i=0;i<Output[0]->DataLen;i++) ((double *)Output[0]->DataBuffer)[i]=Handle->ReadBuffer[i];
		
		if (NumRead==Output[0]->DataLen) return 0;
	}
	return 1;
}

int Split_Process(struct Split_HANDLENUTS *Handle,struct IOType **Input,struct IOType **Output)
{
	int i,k;
	if (Input[0]->DataBuffer!=0)
	{
		for (i=0;i<Handle->NumOutput;i++) 
		{
			if (Output[i]->DataBuffer!=0)
			{
				for (k=0;k<Input[0]->DataLen;k++) 
					((double *)Output[i]->DataBuffer)[k]=((double *)Input[0]->DataBuffer)[k];
			}
		}
	}
	return 0;
}

int Delay_MemCpy(void *dest, const void *src, int ByteCount)
{
	int k;
	for (k=0;k<ByteCount;k++) ((char *)dest)[k]=((char *)src)[k];

	return ByteCount;
}

int Delay_Process(struct Delay_HANDLENUTS *Handle,struct IOType **Input,struct IOType **Output)
{
	if (Handle->DelayPos+Input[0]->DataLen>Handle->LenBuffer)
	{
		if (Handle->LenBuffer>Input[0]->DataLen)
		{
			Delay_MemCpy(Output[0]->DataBuffer,(void *)(Handle->DelayLine+Handle->DelayPos),sizeof(double)*(Handle->LenBuffer-Handle->DelayPos));
			Delay_MemCpy((void *)((double*)Output[0]->DataBuffer+(Handle->LenBuffer-Handle->DelayPos)),Handle->DelayLine,sizeof(double)*(Input[0]->DataLen-(Handle->LenBuffer-Handle->DelayPos)));

			Delay_MemCpy((void *)(Handle->DelayLine+Handle->DelayPos),Input[0]->DataBuffer,sizeof(double)*(Handle->LenBuffer-Handle->DelayPos));
			Delay_MemCpy(Handle->DelayLine,&(((double *)Input[0]->DataBuffer)[Handle->LenBuffer-Handle->DelayPos]),sizeof(double)*(Input[0]->DataLen-(Handle->LenBuffer-Handle->DelayPos)));

			Handle->DelayPos=(Handle->DelayPos+Input[0]->DataLen) - Handle->LenBuffer;
		}
		else
		{
			Delay_MemCpy(Output[0]->DataBuffer,(void *)(Handle->DelayLine+Handle->DelayPos),sizeof(double)*(Handle->LenBuffer-Handle->DelayPos));
			Delay_MemCpy((void *)((double *)Output[0]->DataBuffer+(Handle->LenBuffer-Handle->DelayPos)),Input[0]->DataBuffer,sizeof(double)*(Input[0]->DataLen-(Handle->LenBuffer-Handle->DelayPos)));

			Delay_MemCpy(Handle->DelayLine,(void *)((double *)Input[0]->DataBuffer+(Input[0]->DataLen-(Handle->LenBuffer-Handle->DelayPos))),sizeof(double)*(Handle->LenBuffer-Handle->DelayPos));

			Handle->DelayPos=0;
		}
	}
	else
	{
		Delay_MemCpy(Output[0]->DataBuffer,(void *)(Handle->DelayLine+Handle->DelayPos),sizeof(double)*Input[0]->DataLen);
		Delay_MemCpy((void *)(Handle->DelayLine+Handle->DelayPos),Input[0]->DataBuffer,sizeof(double)*(Input[0]->DataLen));

		Handle->DelayPos=(Handle->DelayPos+Input[0]->DataLen) % Handle->LenBuffer; // aggiorno il puntatore ad i nuovi dati da inviare all'uscita nel prossimo istante
	}
	return 0;
}

int Gain_Process(struct Gain_HANDLENUTS *Handle,struct IOType **Input,struct IOType **Output)
{
	int k;
	if (Input[0]->DataBuffer!=0)
	{
		if (Output[0]->DataBuffer!=0)
		{
			for (k=0;k<Input[0]->DataLen;k++) 
				((double *)Output[0]->DataBuffer)[k]=Handle->GainValue*(((double *)Input[0]->DataBuffer)[k]);
		}	
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// Delete Function Section ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

int Board_Delete(struct BoardHandle *Handle)
{
	if (Handle->FileOut!=0) fclose(Handle->FileOut);
	Handle->FileOut=0;

	return 0;
}

int FileRead_Delete(struct FileRead_HANDLENUTS *Handle)
{
	if (Handle->hFile!=0) fclose(Handle->hFile);
	Handle->hFile=0;

	if (Handle->ReadBuffer!=0) free(Handle->ReadBuffer);
	Handle->ReadBuffer=0;

	return 0;
}

int Split_Delete(struct Split_HANDLENUTS *Handle)
{
	return 0;
}

int Delay_Delete(struct Delay_HANDLENUTS *Handle)
{
	if (Handle->DelayLine!=0) free(Handle->DelayLine);
	Handle->DelayLine=0;

	return 0;
}
int Gain_Delete(struct Gain_HANDLENUTS *Handle)
{
	return 0;
}

void WaitForSyncEvent()
{

}

/////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////    Main    //////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

int main()
{
	int done=0;

	InitInstance();
	CreateBuffer();
	SetUp();

	FileRead_Init(FileRead_00,FRAMESIZE,SAMPLERATE);
	Split_Init(Split_00,FRAMESIZE,SAMPLERATE);
	Delay_Init(Delay_00,FRAMESIZE,SAMPLERATE);
	Delay_Init(Delay_01,FRAMESIZE,SAMPLERATE);
	Gain_Init(Gain_00,FRAMESIZE,SAMPLERATE);
	Gain_Init(Gain_01,FRAMESIZE,SAMPLERATE);
	Board_Init(Board,FRAMESIZE,SAMPLERATE);

	while (!done)
	{
		WaitForSyncEvent();

		done=FileRead_Process(FileRead_00,FileRead_00->Input,FileRead_00->Output);
		Split_Process(Split_00,Split_00->Input,Split_00->Output);
		Delay_Process(Delay_00,Delay_00->Input,Delay_00->Output);
		Delay_Process(Delay_01,Delay_01->Input,Delay_01->Output);
		Gain_Process(Gain_00,Gain_00->Input,Gain_00->Output);
		Gain_Process(Gain_01,Gain_01->Input,Gain_01->Output);

		Board_OutputProcess(Board,Board->Output);
	}

	FileRead_Delete(FileRead_00);
	Split_Delete(Split_00);
	Delay_Delete(Delay_00);
	Delay_Delete(Delay_01);
	Gain_Delete(Gain_00);
	Gain_Delete(Gain_01);
	Board_Delete(Board);

	DisposeBuffer();
	DeleteInstance();
	return 0;
}

