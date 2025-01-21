#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "ip.h"

/*
double bilinear(image_ptr buffer, double x, double y, int rows, int cols, float x_scale, float y_scale) {

	double NW, NE, SW, SE;
	double EWweight, NSweight, EWtop, EWbottom;

	NW = buffer[Y_Source * cols + X_Source];
	NE = buffer[Y_Source * cols + X_Source + 1];
	SW = buffer[(Y_Source + 1) * cols + X_Source];
	SE = buffer[(Y_Source + 1) * cols + X_Source + 1];

	EWweight = (x / x_scale) - (float)X_Source;
	NSweight = (y / y_scale) - (float)Y_Source;

	EWtop = NW + EWweight * (NE - NW);
	EWbottom = SW + EWweight * (SE - SW);

	return EWtop + y * (EWbottom - EWtop);
}
 */