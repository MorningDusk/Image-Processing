__kernel void mulMatrix(__global float *A,__global float *B,__global float *C, int ROW_A, int COL_A, int COL_B){
        int i, j, k;
for (i = 0; i < ROW_A; i++) {
for (j = 0; j < COL_B; j++) {
C[i*COL_B + j] = 0.0f;
for (k = 0; k < COL_A; k++) {
C[i*COL_B + j] += C[i*COL_A + k] * B[k*COL_B + j];
}
}
}
}