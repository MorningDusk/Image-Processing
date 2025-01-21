#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "ip.h"

/****************************************************************************
 * Func: scale_pnm                                                          *
 *                                                                          *
 * Desc: scale an image using nearest neighbor interpolation                *
 *                                                                          *
 * Params: buffer - pointer to image in memory                              *
 *         fileout - name of output file                                    *
 *         rows - number of rows in image                                   *
 *         cols - number of columns in image                                *
 *         x_scale - scale factor in X direction                            *
 *         y_scale - scale factor in Y direction                            *
 *         type - graphics file type (5 = PGM    6 = PPM)                   *
 ****************************************************************************/ 
void scale_pnm(image_ptr buffer, char *fileout, int rows, int cols,
	       float x_scale, float y_scale, int type) 
    {
    unsigned long x,y;          /* loop indices for columns and rows */
    unsigned long index;        /* index into line buffer */
    unsigned long source_index; /* address of source pixel in image buffer */
    unsigned char *line_buff;   /* output line buffer */
    int new_rows, new_cols;     /* values of rows and columns for new image */
    unsigned line;              /* number of pixels in one scan line */
    FILE *fp;                   /* output file pointer */
    unsigned long X_Source, Y_Source;  /* x and y address of source pixel */
    pixel_ptr color_buff;       /* pointer to a color image in memory */
	double NW, NE, SW, SE;
	double EWweight, NSweight, EWtop, EWbottom;

    /* open new output file */
    if((fp=fopen(fileout, "wb")) == NULL)
	{
	printf("Unable to open %s for output\n",fileout);
	exit(1); 
	}

    new_cols = cols * x_scale;
    new_rows = rows * y_scale;

    /* print out the portable bitmap header */
    fprintf(fp, "P%d\n%d %d\n255\n", type, new_cols, new_rows);

    if(type == 5)        /* PGM file */
	line = new_cols;
    else                 /* PPM file */
	{
	line = new_cols * 3;
	color_buff = (pixel_ptr) buffer;
	}

    line_buff = (unsigned char *) malloc(line);

    for(y=0; y<new_rows; y++)
	{
	index = 0;
	for(x=0; x<new_cols; x++)
	    {
		
	    X_Source = (unsigned long)((x / x_scale) + 0.5);
	    Y_Source = (unsigned long)((y / y_scale) + 0.5);
	    source_index = Y_Source * cols + X_Source;

	    if(type == 5)      /* PGM */
		line_buff[index++] = buffer[source_index];
	    else               /* PPM */
		{
		line_buff[index++] = color_buff[source_index].r;
		line_buff[index++] = color_buff[source_index].g;
		line_buff[index++] = color_buff[source_index].b;
		}
		
	    }
	fwrite(line_buff, 1, line, fp);
	}
    fclose(fp);
    }


void bilinear_pnm(image_ptr buffer, char* fileout, int rows, int cols,
	float x_scale, float y_scale, int type)
{
	unsigned long x, y;          /* loop indices for columns and rows */
	unsigned long index;        /* index into line buffer */
	unsigned long source_index; /* address of source pixel in image buffer */
	unsigned char* line_buff;   /* output line buffer */
	int new_rows, new_cols;     /* values of rows and columns for new image */
	unsigned line;              /* number of pixels in one scan line */
	FILE* fp;                   /* output file pointer */
	unsigned long X_Source, Y_Source;  /* x and y address of source pixel */
	pixel_ptr color_buff;       /* pointer to a color image in memory */
	double NW, NE, SW, SE;
	double EWweight, NSweight, EWtop, EWbottom;

	/* open new output file */
	if ((fp = fopen(fileout, "wb")) == NULL)
	{
		printf("Unable to open %s for output\n", fileout);
		exit(1);
	}

	new_cols = cols * x_scale;
	new_rows = rows * y_scale;

	/* print out the portable bitmap header */
	fprintf(fp, "P%d\n%d %d\n255\n", type, new_cols, new_rows);

	if (type == 5)        /* PGM file */
		line = new_cols;
	else                 /* PPM file */
	{
		line = new_cols * 3;
		color_buff = (pixel_ptr)buffer;
	}

	line_buff = (unsigned char*)malloc(line);

	for (y = 0; y < new_rows; y++)
	{
		index = 0;
		for (x = 0; x < new_cols; x++)
		{

			X_Source = (unsigned long)((x / x_scale) + 0.5);
			Y_Source = (unsigned long)((y / y_scale) + 0.5);
			source_index = Y_Source * cols + X_Source;

			NW = buffer[Y_Source * cols + X_Source];		// 북서쪽 점 위치
			NE = buffer[Y_Source * cols + X_Source + 1];	// 북동쪽 점 위치
			SW = buffer[(Y_Source + 1) * cols + X_Source];	// 남서쪽 점 위치
			SE = buffer[(Y_Source + 1) * cols + X_Source + 1];		// 남동쪽 점 위치

			EWweight = (x / x_scale) - (float)X_Source;		// 가로 가중치
			NSweight = (y / y_scale) - (float)Y_Source;		// 세로 가중치

			EWtop = NW + EWweight * (NE - NW);		// 가로선의 위쪽
			EWbottom = SW + EWweight * (SE - SW);	// 가로선의 아래쪽
			
			if (type == 5) {     /* PGM */
				line_buff[index++]= (unsigned char)EWtop + NSweight * (EWbottom - EWtop);	// interpolation 계산
			}
			else               /* PPM */
			{
				line_buff[index++] = color_buff[source_index].r;
				line_buff[index++] = color_buff[source_index].g;
				line_buff[index++] = color_buff[source_index].b;
			}

		}
		fwrite(line_buff, 1, line, fp);
	}
	fclose(fp);
}
 
void cubic_pnm(image_ptr buffer, char* fileout, int rows, int cols,
	float x_scale, float y_scale, int type)
{
	unsigned long x, y;          /* loop indices for columns and rows */
	unsigned long index;        /* index into line buffer */
	unsigned long source_index; /* address of source pixel in image buffer */
	unsigned char* line_buff;   /* output line buffer */
	int new_rows, new_cols;     /* values of rows and columns for new image */
	unsigned line;              /* number of pixels in one scan line */
	FILE* fp;                   /* output file pointer */
	unsigned long X_Source, Y_Source;  /* x and y address of source pixel */
	pixel_ptr color_buff;       /* pointer to a color image in memory */
	double NW, NE, SW, SE;
	double EWweight, NSweight, EWtop, EWbottom, dest_pixel;

	/* open new output file */
	if ((fp = fopen(fileout, "wb")) == NULL)
	{
		printf("Unable to open %s for output\n", fileout);
		exit(1);
	}

	new_cols = cols * x_scale;
	new_rows = rows * y_scale;

	/* print out the portable bitmap header */
	fprintf(fp, "P%d\n%d %d\n255\n", type, new_cols, new_rows);

	if (type == 5)        /* PGM file */
		line = new_cols;
	else                 /* PPM file */
	{
		line = new_cols * 3;
		color_buff = (pixel_ptr)buffer;
	}

	line_buff = (unsigned char*)malloc(line);

	for (y = 0; y < new_rows; y++)
	{
		index = 0;
		for (x = 0; x < new_cols; x++)
		{
			X_Source = (unsigned long)((x / x_scale) + 0.5);
			Y_Source = (unsigned long)((y / y_scale) + 0.5);
			source_index = Y_Source * cols + X_Source;

			NW = buffer[Y_Source * cols + X_Source];		// 북서쪽 점 위치
			NE = buffer[Y_Source * cols + X_Source + 1];	// 북동쪽 점 위치
			SW = buffer[(Y_Source + 1) * cols + X_Source];	// 남서쪽 점 위치
			SE = buffer[(Y_Source + 1) * cols + X_Source + 1];		// 남동쪽 점 위치

			EWweight = (x / x_scale) - (float)X_Source;		// 가로 가중치
			NSweight = (y / y_scale) - (float)Y_Source;		// 세로 가중치

			EWtop = NW + EWweight * (NE - NW);		// 가로선의 위쪽
			EWbottom = SW + EWweight * (SE - SW);	// 가로선의 아래쪽

			dest_pixel = EWtop + NSweight * (EWbottom - EWtop);		// third interpolation 계산

			if (type == 5)      /* PGM */
				line_buff[index++] = dest_pixel;
			else               /* PPM */
			{
				line_buff[index++] = color_buff[source_index].r;
				line_buff[index++] = color_buff[source_index].g;
				line_buff[index++] = color_buff[source_index].b;
			}

		}
		fwrite(line_buff, 1, line, fp);
	}
	fclose(fp);
}