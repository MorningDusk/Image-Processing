#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS

#include "rotation.h"
#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

char* get_source_code(const char* file_name, size_t* len) {
    FILE* file = fopen(file_name, "rb");
    if (file == NULL) {
        printf("[%s:%d] Failed to open %s\n", __FILE__, __LINE__, file_name);
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    size_t length = (size_t)ftell(file);
    rewind(file);

    char* source_code = (char*)malloc(length + 1);
    fread(source_code, length, 1, file);
    source_code[length] = '\0';
    fclose(file);
    *len = length;

    return source_code;
}

void build_error(cl_program program, cl_device_id device, cl_int err) {
    if (err == CL_BUILD_PROGRAM_FAILURE) {
        size_t log_size;
        char* log;

        err = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        CHECK_ERROR(err);

        log = (char*)malloc(log_size + 1);
        err = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
        CHECK_ERROR(err);

        log[log_size] = '\0';
        printf("Compiler error:\n%s\n", log);
        free(log);
        exit(0);
    };
}

void rotate(const float* input, float* output, const int width, const int height, char *degree) {
    const float theta = atof(degree) * M_PI / 180;
    const float sin_theta = sinf(theta);
    const float cos_theta = cosf(theta);

    cl_int err;

    // Platform ID
    cl_platform_id platform;
    err = clGetPlatformIDs(1, &platform, NULL);
    CHECK_ERROR(err);

    // Device ID
    cl_device_id device;
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    CHECK_ERROR(err);

    // Create Context
    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    CHECK_ERROR(err);

    // Create Command Queue
    cl_command_queue queue = clCreateCommandQueueWithProperties(context, device, 0, &err);
    CHECK_ERROR(err);

    // Create Program Object
    size_t kernel_source_size;
    char* kernel_source = get_source_code("kernel.cl", &kernel_source_size);
    cl_program program = clCreateProgramWithSource(context, 1, (const char**)&kernel_source, &kernel_source_size, &err);
    CHECK_ERROR(err);

    // Build Program
    err = clBuildProgram(program, 1, &device, "", NULL, NULL);
    build_error(program, device, err);
    CHECK_ERROR(err);

    cl_kernel kernel = clCreateKernel(program, "imgRotate", &err);
    CHECK_ERROR(err);

    cl_mem inputBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(int) * width * height, NULL, &err);
    CHECK_ERROR(err);

    cl_mem outputBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(int) * width * height, NULL, &err);
    CHECK_ERROR(err);

    err = clEnqueueWriteBuffer(queue, inputBuffer, CL_TRUE, 0, sizeof(int) * width * height, (void*)input, 0, NULL, NULL);
    CHECK_ERROR(err);

    clSetKernelArg(kernel, 0, sizeof(cl_mem), & inputBuffer);
    CHECK_ERROR(err);

    clSetKernelArg(kernel, 1, sizeof(cl_mem), & outputBuffer);
    CHECK_ERROR(err);

    clSetKernelArg(kernel, 2, sizeof(cl_int), & width);
    CHECK_ERROR(err);

    clSetKernelArg(kernel, 3, sizeof(cl_int), & height);
    CHECK_ERROR(err);

    clSetKernelArg(kernel, 4, sizeof(cl_float), &sin_theta);
    CHECK_ERROR(err);

    clSetKernelArg(kernel, 5, sizeof(cl_float), &cos_theta);
    CHECK_ERROR(err);

    size_t global_size[2] = { width,height };
    size_t local_size[2] = { 16,16 };

    clEnqueueNDRangeKernel(queue, kernel, 2, 0, global_size, local_size, 0, NULL, NULL);
    CHECK_ERROR(err);

    clEnqueueReadBuffer(queue, outputBuffer, CL_TRUE, 0, sizeof(int) * width * height, output, NULL, NULL, NULL);
    CHECK_ERROR(err);

    clReleaseMemObject(inputBuffer);
    clReleaseMemObject(outputBuffer);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
}