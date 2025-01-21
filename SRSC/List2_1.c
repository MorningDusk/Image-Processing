/***************************************************************************
 * File: arithlut.c                                                        *
 *                                                                         *
 * Desc: This program performs arithmetic point operations via LUTs        *
 ***************************************************************************/


#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include "ip.h"

#define operation(VALUE)        ((float) VALUE)       // 영상의 밝기를 조절해주는 식

extern void write_pnm(image_ptr ptr, char filein[], int rows,
		      int cols, int magic_number);      // 영상을 써주는 함수
extern image_ptr read_pnm(char *filename, int *rows, int *cols,
		      int *type);       // 영상을 읽는 함수
extern image_ptr read_bmp(char* filename, int* rows, int* cols, int* type);
void ReadBitmapHeader(char* filename, char* filout);

int main(int argc, char *argv[])
    {
    char filein[100];                   /* name of input file */
    char fileout[100];                  /* name of output file */
    int rows, cols;                     /* image rows and columns */
    unsigned long i, j;                    /* counting index */
    unsigned long bytes_per_pixel;      /* number of bytes per image pixel */
    unsigned char LUT[256];             /* array for Look-up table */
    image_ptr buffer;                   /* pointer to image buffer */
    unsigned long number_of_pixels;     /* total number of pixels in image */
    int temp;                           /* temporary variable */
    int type;                           /* what type of image data */

    /* set input filename and output file name */
    if(argc == 3)
	{
	strcpy(filein, argv[1]);        // 입력 영상의 파일
	strcpy(fileout, argv[2]);       // 출력 영상의 파일
	}
    else
	{
	printf("Input name of input file\n");
	gets(filein);       // 입력 영상의 파일명을 읽는다
	printf("\nInput name of output file\n");
	gets(fileout);      // 출력 영상의 파일명을 읽는다
	printf("\n");
	}

    ReadBitmapHeader(filein, fileout);
    // buffer = read_pnm(filein, &rows, &cols, &type);     // 입력 이미지 파일을 읽는다

    /* initialize Look-up table */
   
    /*
    for (i = 0; i < 256; i++) {
        temp = operation(i);
        CLIP(temp, 0, 255);
        LUT[i] = temp;
    }
     */  

    
    /* determine bytes_per_pixel, 3 for color, 1 for gray-scale */
    /*
    if (type == PPM)
        bytes_per_pixel = 3;
    else
        bytes_per_pixel = 1;

    number_of_pixels = bytes_per_pixel * rows * cols;
    */

    /* process image via the Look-up table */
    /*
    for (i = 0; i < number_of_pixels; i++)
        buffer[i] = LUT[buffer[i]];
    
    write_pnm(buffer, fileout, rows, cols, type);
    IP_FREE(buffer);
    */



    return 0;
    }