/***************************************************************************
 * File: makeczp.c                                                         *
 *                                                                         *
 * Desc: This program generates circular zone plate image                  *
 *                                                                         *
 *                      phi 2   phi 2                                      *
 * f(x,y) = 127.5(1+cos(---X  + ---Y  ))                                   *
 *                       V       H                                         * 
 ***************************************************************************/ 

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h> 
#include "ip.h"

  
void makeczp(image_ptr buffer, int rows, int cols, int V, int H)
    {

    int i,j;                    /* counting index */
	  double x,y;		
	  double cos_val;

    /* process image via the Look-up table */
  for(i=0; i<rows; i++) {
	  for (j = 0; j < cols; j++) {
		  // 영상의 중점을 원점으로 만든어 x,y의 좌표를 계산한다.
		  x = 0;
		  y = 0;

		  // circular zone plate 파형 생성
		  cos_val = 127.5 * (1 + cos((PI / V) * pow(x, 2) + (PI / H) * pow(y, 2)));

		  // 영상 저장
		  buffer[i * cols + j] = cos_val;
	  }
	}

    //write_pnm(buffer, fileout, rows, cols, type);
    //IP_FREE(buffer);

    }
