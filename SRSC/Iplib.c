/*************************************************************************** 
 * File: iplib.c                                                           *
 *                                                                         *
 * Desc: general purpose image processing routines                         * 
 ***************************************************************************/


#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include "ip.h"

image_ptr read_pnm(char *filename, int *rows, int *cols, int *type);
int getnum(FILE *fp);

/***************************************************************************
 * Func: read_pnm                                                          *
 *                                                                         *
 * Desc: reads a portable bitmap file                                      *
 *                                                                         *
 * Params: filename - name of image file to read                           *
 *         rows - number of rows in the image                              *
 *         cols - number of columns in the image                           *
 *         type - file type                                                *
 *                                                                         *
 * Returns: pointer to the image just read into memory                     *
 ***************************************************************************/

image_ptr read_pnm(char *filename, int *rows, int *cols, int *type)
    {
    int i;                     /* index variable */
    int row_size;              /* size of image row in bytes */
    int maxval;                /* maximum value of pixel */
    FILE *fp;                  /* input file pointer */
    int firstchar, secchar;    /* first 2 characters in the input file */
    image_ptr ptr;             /* pointer to image buffer */
    unsigned long offset;      /* offset into image buffer */
    unsigned long total_size;  /* size of image in bytes */
    unsigned long total_bytes; /* number of total bytes written to file */
    float scale;               /* number of bytes per pixel */

    /* open input file */
    if((fp = fopen(filename, "rb")) == NULL)
	{
	printf("Unable to open %s for reading\n",filename);
	exit(1);
	}

    firstchar = getc(fp);		// 파일 명의 첫번째, 두번째 글자를 읽는다
    secchar = getc(fp);

    if(firstchar != 'P')
	{
	printf("You silly goof... This is not a PPM file!\n");		// PPM파일이 아니라고 하고 종료
	exit(1);
	}

    *cols = getnum(fp);		// 이미지의 열을 읽는다
    *rows = getnum(fp);		// 이미지의 행을 읽는다
    *type = secchar - '0';		// 이미지의 형식을 읽는다

    switch(secchar)
	{
	case '4':            /* PBM */
	    scale = 0.125;
	    maxval = 1;
	    break;
	case '5':            /* PGM */
	    scale = 1.0;
	    maxval = getnum(fp);
	    break;
	case '6':             /* PPM */
	    scale = 3.0;
	    maxval = getnum(fp);
	    break;
	default :             /* Error */
	    printf("read_pnm: This is not a Portable bitmap RAWBITS file\n");
	    exit(1);
	    break;
	}

    row_size = (*cols) * scale;		// 행 크기
    total_size = (unsigned long) (*rows) * row_size;	// 총 크기

    ptr = (image_ptr) IP_MALLOC(total_size);		// 이미지 크기 포인터를 만들어 할당한다

    if(ptr == NULL)
	{
	printf("Unable to malloc %lu bytes\n",total_size);		// 포인터가 널이면 종료
	exit(1);
	}

    total_bytes=0;		// 총 크기값 변수 선언
    offset = 0;		// 오프셋 값 선언
    for(i=0; i<(*rows); i++)
	{
	total_bytes+=fread(ptr+offset, 1, row_size, fp);		// 이미지를 읽어 총 크기를 계산
	offset += row_size;		// 오프셋 크기에 행 크기를 더한다
	}

    if(total_size != total_bytes)
	{
	printf("Failed miserably trying to read %ld bytes\nRead %ld bytes\n",		// 이미지 크기와 읽은 이미지 크기가 다르면 종료
		total_size, total_bytes);
	exit(1);
	}

    fclose(fp);
    return ptr;		// 이미지 크기 포인터 값 반환
    }

/***************************************************************************
 * Func: getnum                                                            *
 *                                                                         *
 * Desc: reads an ASCII number from a portable bitmap file header          *
 *                                                                         *
 * Param: fp - pointer to file being read                                  *
 *                                                                         *
 * Returns: the number read                                                *
 ***************************************************************************/

int getnum(FILE *fp)
    {
    char c;               /* character read in from file */
    int i;                /* number accumulated and returned */

    do
	{
	c = getc(fp);		// 파일 헤더의 ASCII 값을 읽는다
	}
    while((c==' ') || (c=='\t') || (c=='\n') || (c=='\r'));

    if((c<'0') || (c>'9'))
	if(c == '#')                   /* chew off comments */
	    {
	    while(c == '#')
		{
		while(c != '\n')
		    c = getc(fp);
		c = getc(fp);
		}
	    }
	else
	    {
	    printf("Garbage in ASCII fields\n");
	    exit(1);
	    }

    i=0;
    do
	{
	i=i*10+(c-'0');         /* convert ASCII to int */
	c = getc(fp);
	}
    while((c>='0') && (c<='9'));

    return i;
    }

/***************************************************************************
 * Func: write_pnm                                                         *
 *                                                                         *
 * Desc: writes out a portable bitmap file                                 *
 *                                                                         *
 * Params: ptr - pointer to image in memory                                *
 *         filename _ name of file to write image to                       *
 *         rows - number of rows in the image                              *
 *         cols - number of columns in the image                           *
 *         magic_number - number that defines what type of file it is      *
 *                                                                         *
 * Returns: nothing                                                        *
 ***************************************************************************/

void write_pnm(image_ptr ptr, char *filename, int rows, 
	       int cols, int magic_number)
    {
    FILE *fp;             /* file pointer for output file */
    long offset;          /* current offset into image buffer */
    long total_bytes;     /* number of bytes written to output file */
    long total_size;      /* size of image buffer */
    int row_size;         /* size of row in bytes */
    int i;                /* index variable */
    float scale;          /* number of bytes per image pixel */

    switch(magic_number)
	{
	case 4:            /* PBM */
	    scale = 0.125;
	    break;
	case 5:            /* PGM */
	    scale = 1.0;
	    break;
	case 6:             /* PPM */
	    scale = 3.0;
	    break;
	default :             /* Error */
	    printf("write_pnm: This is not a Portable bitmap RAWBITS file\n");
	    exit(1);
	    break;
	}

    /* open new output file */
    if((fp=fopen(filename, "wb")) == NULL)
	{
	printf("Unable to open %s for output\n",filename);
	exit(1);
	}

    /* print out the portable bitmap header */
    fprintf(fp, "P%d\n%d %d\n", magic_number, cols, rows);
    if(magic_number != 4)
	fprintf(fp, "255\n");

    row_size = cols * scale;		// 이미지의 행 사이즈를 계산
    total_size = (long) row_size *rows;		// 이미지의 총 크기 계산
    offset = 0;		// 오프셋 변수 선언
    total_bytes = 0;		// 이미지의 크기 변수 선언
    for(i=0; i<rows; i++)
	{
	total_bytes += fwrite(ptr+offset, 1, row_size, fp);		// 이미지를 읽어 총 크기를 계산
	offset += row_size;		// 오프셋에 행 사이즈값을 더한다
	}

    if(total_bytes != total_size)
	printf("Tried to write %ld bytes...Only wrote %ld\n",		// 이미지 크기와 읽은 이미지 크기가 다르면 종료 
		total_size, total_bytes);

    fclose(fp);
    }

/****************************************************************************
 * Func: pnm_open                                                           *
 *                                                                          *
 * Desc: opens a pnm file and determines rows, cols, and maxval             *
 *                                                                          *
 * Params: rows- pointer to number of rows in the image                     *
 *         cols - pointer number of columns in the image                    *
 *         maxval - pointer to max value                                    *
 *         filename - name of image file                                    *
 ****************************************************************************/

FILE *pnm_open(int *rows, int *cols, int *maxval, char *filename)
    {
    int firstchar, secchar;
    float scale;
    unsigned long row_size;
    FILE *fp;

    if((fp = fopen(filename, "rb")) == NULL)		// 파일명을 읽을 수 없으면 종료
	{
	printf("Unable to open %s for reading\n",filename);
	exit(1);
	}

    firstchar = getc(fp);		// 파일 명의 첫번째, 두번째 글자를 읽는다
    secchar = getc(fp);

    if(firstchar != 'P')
	{
	printf("You silly goof... This is not a PPM file!\n");		// PPM파일이 아니라고 하고 종료
	exit(1);
	}

    *cols = getnum(fp);		// 이미지의 열을 읽는다
    *rows = getnum(fp);		// 이미지의 행을 읽는다

    switch(secchar)
	{
	case '4':            /* PBM */
	    scale = 0.125;
	    *maxval = 1;
	    break;
	case '5':            /* PGM */
	    scale = 1.0;
	    *maxval = getnum(fp);
	    break;
	case '6':             /* PPM */
	    scale = 3.0;
	    *maxval = getnum(fp);
	    break;
	default :             /* Error */
	    printf("read_pnm: This is not a Portable bitmap RAWBITS file\n");
	    exit(1);
	    break;
	}

    row_size = (*cols) * scale;		// 행 크기
    return fp;		// 이미지 파일을 반환
    }


/****************************************************************************
 * Func: read_mesh                                                          *
 *                                                                          *
 * Desc: reads mesh data into a mesh structure				    *
 *                                                                          *
 * Params: filename - name of input mesh file                               *
 *                                                                          *
 * Returns: mesh structure storing width, height, x data  and y data        *
 ****************************************************************************/
mesh *read_mesh(char *filename)
    {
    FILE *fp;
    mesh *mesh_data;
    int width, height, mesh_size;

    /* open mesh file for input */
    if((fp = fopen(filename, "rb")) == NULL)
	{
	printf("Unable to open mesh file %s for reading\n", filename);
	exit(1);
	}

    mesh_data = malloc(sizeof(mesh));		// 메쉬의 크기를 할당
    /* read dimensions of mesh */
    fread(&width, sizeof(int), 1, fp);
    fread(&height, sizeof(int), 1, fp);
    mesh_data->width = width;
    mesh_data->height = height;
    mesh_size = width * height;

    /* allocate memory for mesh data */
    mesh_data->x_data = malloc(sizeof(float) * mesh_size);
    mesh_data->y_data = malloc(sizeof(float) * mesh_size);

    fread(mesh_data->x_data, sizeof(float), mesh_size, fp);		// 메쉬의 가로 크기를 할당하고 읽는다
    fread(mesh_data->y_data, sizeof(float), mesh_size, fp);		// 메쉬의 세로 크기를 할당하고 읽는다

    return(mesh_data);		// 메쉬 정보를 반환한다
    }
