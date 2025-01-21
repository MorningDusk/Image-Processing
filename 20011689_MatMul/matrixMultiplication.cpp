#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <CL/cl.h>

static int ROW_A = 100;
static int COL_A = 100;
static int COL_B = 100;

#define CHECK_ERROR(err) \
  if (err != CL_SUCCESS) { \
    printf("[%s:%d] OpenCL error %d\n", __FILE__, __LINE__, err); \
    exit(EXIT_FAILURE); \
  }
#pragma warning(disable:4996)

char* get_source_code(const char* file_name, size_t* len) {
    char* source_code;
    char buf[2] = "\0";
    int cnt = 0;
    size_t length;
    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        printf("[%s:%d] Failed to open %s\n", __FILE__, __LINE__, file_name);
        exit(EXIT_FAILURE);
    }
    fseek(file, 0, SEEK_END);
    length = (size_t)ftell(file);
    rewind(file);
    source_code = (char*)malloc(length + 1);
    fread(source_code, length, 1, file);
    for (int i = 0; i < length; i++) {
        buf[0] = source_code[i];
        if (buf[0] == '\n') cnt++;
    }
    source_code[length - cnt] = '\0';
        fclose(file);
        *len = length - cnt;
        return source_code;
}

void mat_mul_opencl(float* A, float* B, float* C, int ROW_A, int COL_A, int COL_B) {
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    char* kernel_source;
    size_t kernel_source_size;
    cl_kernel kernel;
    cl_int err;

    err = clGetPlatformIDs(1, &platform, NULL);
    CHECK_ERROR(err);

    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    CHECK_ERROR(err);

    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    CHECK_ERROR(err);

    queue = clCreateCommandQueue(context, device, 0, &err);
    CHECK_ERROR(err);

    kernel_source = get_source_code("kernel.cl", &kernel_source_size);

    program = clCreateProgramWithSource(context, 1, (const char**)&kernel_source, &kernel_source_size, &err);
    CHECK_ERROR(err);

    err = clBuildProgram(program, 1, &device, "", NULL, NULL);
    if (err == CL_BUILD_PROGRAM_FAILURE) {
        size_t log_size;
        char* log;

        err = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        CHECK_ERROR(err);

        log = (char*)malloc(log_size + 1);
        err = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
        CHECK_ERROR(err);

        log[log_size] = '\0';
        printf("Compiler error : \n%s\n", log);
        free(log);
        exit(0);
    }
    CHECK_ERROR(err);

    kernel = clCreateKernel(program, "mulMatrix", &err);
    CHECK_ERROR(err);

    cl_mem bufferA, bufferB, bufferC;

    bufferA = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * ROW_A * COL_A, NULL, &err);
    CHECK_ERROR(err);

    bufferB = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * COL_A * COL_B, NULL, &err);
    CHECK_ERROR(err);

    bufferC = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * ROW_A * COL_B, NULL, &err);
    CHECK_ERROR(err);

    err = clEnqueueWriteBuffer(queue, bufferA, CL_FALSE, 0, sizeof(float) * ROW_A * COL_A, A, 0, NULL, NULL);
    CHECK_ERROR(err);

    err = clEnqueueWriteBuffer(queue, bufferB, CL_FALSE, 0, sizeof(float) * COL_A * COL_B, B, 0, NULL, NULL);
    CHECK_ERROR(err);

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufferA);
    CHECK_ERROR(err);

    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferB);
    CHECK_ERROR(err);

    err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufferC);
    CHECK_ERROR(err);

    err = clSetKernelArg(kernel, 3, sizeof(cl_int), &ROW_A);
    CHECK_ERROR(err);

    err = clSetKernelArg(kernel, 4, sizeof(cl_int), &COL_A);
    CHECK_ERROR(err);

    err = clSetKernelArg(kernel, 5, sizeof(cl_int), &COL_B);
    CHECK_ERROR(err);

    size_t global_size[2] = { COL_B, ROW_A };
    size_t local_size[2] = { 16, 16 };

    global_size[0] = (global_size[0] + local_size[0] - 1) / local_size[0] * local_size[0];
    global_size[1] = (global_size[1] + local_size[1] - 1) / local_size[1] * local_size[1];

    err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global_size, local_size, 0, NULL, NULL);
    CHECK_ERROR(err);

    err = clEnqueueReadBuffer(queue, bufferC, CL_TRUE, 0, sizeof(float) * ROW_A * COL_B, C, 0, NULL, NULL);
    CHECK_ERROR(err);

    clReleaseMemObject(bufferA);
    clReleaseMemObject(bufferB);
    clReleaseMemObject(bufferC);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    printf("Finished\n");
}

int main() {
    
    float* A = (float*)malloc(sizeof(float) * ROW_A * COL_A);
    float* B = (float*)malloc(sizeof(float) * COL_A * COL_B);
    float* C = (float*)malloc(sizeof(float) * ROW_A * COL_B);
    int i, j, k;

    for (i = 0; i < ROW_A; i++) {
        for (j = 0; j < COL_A; j++) {
            A[i * COL_A + j] = (float)(rand() % 1000) / 100.0f;
        }
    }
    for (i = 0; i < COL_A; i++) {
        for (j = 0; j < COL_B; j++) {
            B[i * COL_B + j] = (float)(rand() % 1000) / 100.0f;
        }
    }

    printf("Matrix Multiplication: ");
    printf("C[%d x %d] = A[%d x %d] x B[%d x %d]\n", ROW_A, COL_B, ROW_A, COL_A, COL_A, COL_B);

    mat_mul_opencl(A, B, C, ROW_A, COL_A, COL_B);

    printf("Verifying in progress...\n");
   
    float sum;
    for (i = 0; i < ROW_A; i++) {
        for (j = 0; j < COL_B; j++) {
            sum = 0.0f;
            for (k = 0; k < COL_A; k++) {
                sum += C[i * COL_A + k] * B[k * COL_B + j];
            }
            if (C[i * COL_B + j] - sum > 0.1 || C[i * COL_B + j] - sum < -0.1) {
                printf("Verification failed! C[%d][%d]: %f vs. %f\n", i, j, C[i * COL_B + j], sum);
                 return 0;
            }
        }
    }
    printf("Verification success!\n");

    free(A);
    free(B);
    free(C);
    return 0;
}


/*
__kernel void mulMatrix(__global float* A, __global float* B, __global float* C, int ROW_A, int COL_A, int COL_B) {
    int i, j, k;
    for (i = 0; i < ROW_A; i++) {
        for (j = 0; j < COL_B; j++) {
            C[i * COL_B + j] = 0.0f;
            for (k = 0; k < COL_A; k++) {
                C[i * COL_B + j] += C[i * COL_A + k] * B[k * COL_B + j];
            }
        }
    }
}
*/