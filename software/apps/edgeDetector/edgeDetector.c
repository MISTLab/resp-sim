#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "ReadWriteBitmaps.h"

#define WIDTH 80
#define HEIGHT 72
#define DEPTH 3
#define INPUT_FILE "./software/apps/edgeDetector/input.bmp"
#define OUTPUT_FILE "output.bmp"

void rgb2grey(unsigned char* inputImage, unsigned char* outputImage){
  int  i = 0;
  for(i = 0; i < WIDTH*HEIGHT; i++){
      outputImage[i] = inputImage[i*3];
      outputImage[i] = 255 - outputImage[i];
  }
}

void edgeDetection(unsigned char* inputImage, unsigned char* outputImage){
  unsigned int X, Y;
  int I, J;
  long sumX, sumY;
  int nColors, SUM;

  int GX[3][3];
  int GY[3][3];
    
  /* 3x3 GX Sobel mask.  Ref: www.cee.hw.ac.uk/hipr/html/sobel.html */
  GX[0][0] = -1; GX[0][1] = 0; GX[0][2] = 1;
  GX[1][0] = -2; GX[1][1] = 0; GX[1][2] = 2;
  GX[2][0] = -1; GX[2][1] = 0; GX[2][2] = 1;

  /* 3x3 GY Sobel mask.  Ref: www.cee.hw.ac.uk/hipr/html/sobel.html */
  GY[0][0] =  1; GY[0][1] =  2; GY[0][2] =  1;
  GY[1][0] =  0; GY[1][1] =  0; GY[1][2] =  0;
  GY[2][0] = -1; GY[2][1] = -2; GY[2][2] = -1;

  for(Y=0; Y<=(HEIGHT-1); Y++)  {
    for(X=0; X<=(WIDTH-1); X++)  {
      sumX = 0;
      sumY = 0;
      if(Y==0 || Y==HEIGHT-1)
        SUM = 0;
      else if(X==0 || X==WIDTH-1)
        SUM = 0;
      else {
        for(I=-1; I<=1; I++)  {
          for(J=-1; J<=1; J++)  {
            sumX = sumX + (int) inputImage[X+I + (Y+J) * WIDTH]*GX[I+1][J+1];
          }
        }
        for(I=-1; I<=1; I++)  {
          for(J=-1; J<=1; J++)  {
            sumY = sumY + (int) inputImage[X+I + (Y+J) * WIDTH]*GY[I+1][J+1];
          }
        }
        SUM = abs(sumX) + abs(sumY);
      }
      if(SUM>255) SUM=255;
      if(SUM<0) SUM=0;
      outputImage[X + Y *WIDTH] = 255 - (unsigned char)(SUM);
    }
  }
}

void edgeOverlapping(unsigned char* inputImage, unsigned char* edgeImage, unsigned char* outputImage){
  int  i = 0, k = 0;
  for(i = 0; i < WIDTH*HEIGHT; i++){
    if(edgeImage[i] < 100){
      outputImage[i*3] = edgeImage[i];
      outputImage[i*3+1] = 255;
      outputImage[i*3+2] = 255;
    }
    else
      for(k = 0; k <3; k++)
        outputImage[i*3+k] = inputImage[i*3+k];
  }
}

int main(){
  char* input_file_name = INPUT_FILE;
  char* output_file_name = OUTPUT_FILE;
  unsigned char inputImage[WIDTH*HEIGHT*DEPTH];
  unsigned char greyImage[WIDTH*HEIGHT];
  unsigned char edgeImage[WIDTH*HEIGHT];
  unsigned char outputImage[WIDTH*HEIGHT*DEPTH];
  unsigned char * bitmap_palette = NULL;
  unsigned char bitmap_type = 1;
  unsigned short int bitmap_width = WIDTH;
  unsigned short int bitmap_height = HEIGHT;

  read_bitmap(input_file_name, inputImage);
  rgb2grey(inputImage, greyImage);
  edgeDetection(greyImage, edgeImage);
  edgeOverlapping(inputImage, edgeImage, outputImage);
  write_bitmap(output_file_name, bitmap_type, bitmap_width, bitmap_height, outputImage, bitmap_palette);
  
  return 0;
}
