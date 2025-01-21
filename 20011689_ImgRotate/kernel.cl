__kernel void imgRotate(__global float* input, __global float* output, int w, int h, float sinTheta, float cosTheta) {
	const int ix = get_global_id(0);
	const int iy = get_global_id(1);
	float x0 = w/2.0f;
	float y0 = h/2.0f;
	float xoff = ix-x0;
	float yoff = iy-y0;
	int src_x = (int)(xoff * cosTheta + yoff * sinTheta + x0);
	int src_y = (int)(yoff * cosTheta - xoff * sinTheta + y0);
	if (((int)src_x >= 0) && ((int)src_x < w) && ((int)src_y >= 0) && ((int)src_y < h)) {
		output[iy * w + ix] = input[src_y * w + src_x];
	}	
	else {
		output[iy * w + ix] = 0.0f;
	}
}