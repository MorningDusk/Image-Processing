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
  USHORT bmpType;                       // BMP ������ ������ ��Ÿ���� �ĺ���
  ULONG  bmpSize;                       // BMP ������ ũ��(����Ʈ ����)
  USHORT bmpReserved1;                  // ����
  USHORT bmpReserved2;                  // ����
  ULONG  bmpOffset;                     // �̹��� ������ ���۵Ǵ� ��ġ(Offset)
  ULONG  bmpHeaderSize;                 // Header�� ũ��(����Ʈ ����)
  ULONG  bmpWidth;                      // ���� ũ��(�ȼ� ����)
  ULONG  bmpHeight;                     // ���� ũ��(�ȼ� ����)
  USHORT bmpPlanes;                     // Color Plane�� ��(1�� ����)
  USHORT bmpBitCount;                   // �� �ȼ��� �Ҵ�� ��Ʈ��
  ULONG  bmpCompression;                // ���� ��� ��뿩��
  ULONG  bmpBitmapSize;                 // ��Ʈ�� �̹����� ũ��
  ULONG  bmpXPelsPerMeter;              // ���� �ػ�
  ULONG  bmpYPelsPerMeter;              // ���� �ػ�
  ULONG  bmpColors;                     // ���� �����
  ULONG  bmpClrImportant;
}; 

struct BITMAPHEADER bmp_header;                // Bitmap ������ Header ����ü
#define WIDTHBYTES(bits) (((bits)+31)/32*4)
#define operation(VALUE)        ((float) VALUE)

image_ptr read_pnm(char* filename, int* rows, int* cols, int* type);

void ReadBitmapHeader(char *filename, char *fileout) {
int          read_fd, type, temp;                              // ������ �а� ���µ� ����� Descriptor
FILE* outfile;
unsigned char LUT[256];

read_fd = open(filename, O_RDONLY);     // bmp������ open�Ѵ�.
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
        strcpy(filein, argv[1]);        // �Է� ������ ����
        strcpy(fileout, argv[2]);       // ��� ������ ����
    }
    else
    {
        printf("Input name of input file\n");
        gets(filein);       // �Է� ������ ���ϸ��� �д´�
        printf("\nInput name of output file\n");
        gets(fileout);      // ��� ������ ���ϸ��� �д´�
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

    ReadBitmapHeader(filein, fileout);      // �Է�, ��� ������ �д´�
    fprintf(write_fp, "P%d\n%d %d\n255\n", 5, bmp_header.bmpHeight, bmp_header.bmpWidth);       // header ������ �о�� offset, height, width�� ���
    fseek(read_fp, -(long)(bmp_header.bmpWidth * bmp_header.bmpHeight), SEEK_END);      // �� pixel�� ���
    buff = (char*)malloc(bmp_header.bmpWidth);
    for (i = 0; i < bmp_header.bmpHeight; i++) {
    fread(buff, bmp_header.bmpWidth, 1, read_fp);       
    fwrite(buff, bmp_header.bmpWidth, 1, write_fp);
    fseek(read_fp, 0, SEEK_CUR);
        }

    return 0;
}