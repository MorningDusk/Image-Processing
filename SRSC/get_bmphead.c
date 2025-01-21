/***************************************************************************
 * File: get_bmphead.c                                                     *
 *                                                                         *
 * Desc: This program reads header part of bmp format image                *
 ***************************************************************************/
  
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <fcntl.h>
#include "ip.h"

typedef unsigned short int USHORT;
typedef unsigned long int  ULONG; 

struct BITMAPHEADER {
  USHORT bmpType;                       // BMP 파일의 종류를 나타내는 식별자
  ULONG  bmpSize;                       // BMP 파일의 크기(바이트 단위)
  USHORT bmpReserved1;                  // 예약
  USHORT bmpReserved2;                  // 예약
  ULONG  bmpOffset;                     // 이미지 정보가 시작되는 위치(Offset)
  ULONG  bmpHeaderSize;                 // Header의 크기(바이트 단위)
  ULONG  bmpWidth;                      // 가로 크기(픽셀 단위)
  ULONG  bmpHeight;                     // 세로 크기(픽셀 단위)
  USHORT bmpPlanes;                     // Color Plane의 수(1로 셋팅)
  USHORT bmpBitCount;                   // 한 픽셀에 할당된 비트수
  ULONG  bmpCompression;                // 압축 기법 사용여부
  ULONG  bmpBitmapSize;                 // 비트맵 이미지의 크기
  ULONG  bmpXPelsPerMeter;              // 수평 해상도
  ULONG  bmpYPelsPerMeter;              // 수직 해상도
  ULONG  bmpColors;                     // 사용된 색상수
  ULONG  bmpClrImportant;
}; 

struct BITMAPHEADER bmp_header;                // Bitmap 파일의 Header 구조체
#define WIDTHBYTES(bits) (((bits)+31)/32*4)
#define operation(VALUE)        ((float) VALUE)

image_ptr read_pnm(char* filename, int* rows, int* cols, int* type);

void ReadBitmapHeader(char *filename, char *fileout) {
int          read_fd, type, temp;                              // 파일을 읽고 쓰는데 사용할 Descriptor
FILE* outfile;
unsigned char LUT[256];

read_fd = open(filename, O_RDONLY);     // bmp파일을 open한다.
if (read_fd == -1) {
    printf("Can't open file %s\n", filename);
    exit(1);
}

read(read_fd, &bmp_header.bmpType, sizeof(bmp_header.bmpType));
read(read_fd, &bmp_header.bmpSize, sizeof(bmp_header.bmpSize));
read(read_fd, &bmp_header.bmpReserved1, sizeof(bmp_header.bmpReserved1));
read(read_fd, &bmp_header.bmpReserved2, sizeof(bmp_header.bmpReserved2));
read(read_fd, &bmp_header.bmpOffset, sizeof(bmp_header.bmpOffset));
read(read_fd, &bmp_header.bmpHeaderSize, sizeof(bmp_header.bmpHeaderSize));
read(read_fd, &bmp_header.bmpWidth, sizeof(bmp_header.bmpWidth));
read(read_fd, &bmp_header.bmpHeight, sizeof(bmp_header.bmpHeight));
read(read_fd, &bmp_header.bmpPlanes, sizeof(bmp_header.bmpPlanes));
read(read_fd, &bmp_header.bmpBitCount, sizeof(bmp_header.bmpBitCount));
read(read_fd, &bmp_header.bmpCompression, sizeof(bmp_header.bmpCompression));
read(read_fd, &bmp_header.bmpBitmapSize, sizeof(bmp_header.bmpBitmapSize));
read(read_fd, &bmp_header.bmpXPelsPerMeter, sizeof(bmp_header.bmpXPelsPerMeter));
read(read_fd, &bmp_header.bmpYPelsPerMeter, sizeof(bmp_header.bmpYPelsPerMeter));
read(read_fd, &bmp_header.bmpColors, sizeof(bmp_header.bmpColors));
read(read_fd, &bmp_header.bmpClrImportant, sizeof(bmp_header.bmpClrImportant));

  printf("bmpOffset: %d\n", bmp_header.bmpOffset);
  printf("bmpWidth: %d\n", bmp_header.bmpWidth);
  printf("bmpHeight: %d\n", bmp_header.bmpHeight);

  close(read_fd);
}

int main(int argc, char* argv[])
{
    char filein[100];
    char fileout[100];
    char* buff;
    FILE* write_fp, * read_fp;
    int i;
    if (argc == 3)
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

    if ((read_fp = fopen(filein, "rb")) == NULL)
    {
        printf("unable to open %s for input\n", filein);
        exit(1);
    }
    if ((write_fp = fopen(fileout, "wb")) == NULL)
    {
        printf("unable to open %s for output\n", fileout);
        exit(1);
    }

    ReadBitmapHeader(filein, fileout);      // 입력, 출력 파일을 읽는다
    fprintf(write_fp, "P%d\n%d %d\n255\n", 5, bmp_header.bmpHeight, bmp_header.bmpWidth);       // header 정보를 읽어내어 offset, height, width를 출력
    fseek(read_fp, -(long)(bmp_header.bmpWidth * bmp_header.bmpHeight), SEEK_END);      // 총 pixel값 계산
    buff = (char*)malloc(bmp_header.bmpWidth);
    for (i = 0; i < bmp_header.bmpHeight; i++) {
    fread(buff, bmp_header.bmpWidth, 1, read_fp);       
    fwrite(buff, bmp_header.bmpWidth, 1, write_fp);
    fseek(read_fp, 0, SEEK_CUR);
        }

    return 0;
}