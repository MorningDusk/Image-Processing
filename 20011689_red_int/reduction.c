#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>
#include<time.h>

#define VECTOR_SIZE 16777216
#define LOCAL_SIZE 256

#define CHECK_ERROR(err) \
    if(err != CL_SUCCESS) { \
        printf("[%s:%d] OpenCL error %d\n", __FILE__, __LINE__, err); \
        exit(EXIT_FAILURE); \
    }

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

int main() {
	srand(time(NULL));
	int SIZE = VECTOR_SIZE;
	cl_int err, i, j;
	clock_t start, end;

	cl_platform_id platform;
	cl_device_id device;
	cl_uint num_devices;
	cl_context context;
	cl_command_queue* queues, queue;
	char* kernel_source;
	size_t kernel_source_size;
	cl_program program;
	cl_kernel kernel_reduction;

	err = clGetPlatformIDs(1, &platform, NULL);
	CHECK_ERROR(err);

	err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
	CHECK_ERROR(err);

	context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
	CHECK_ERROR(err);

	queue = clCreateCommandQueueWithProperties(context, device, 0, &err);
	CHECK_ERROR(err);

	kernel_source = get_source_code("red_kernel.cl", &kernel_source_size);

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

	kernel_reduction = clCreateKernel(program, "reduction", &err);
	CHECK_ERROR(err);

	int* A, * B;
	A = (int*)malloc(sizeof(int) * VECTOR_SIZE);
	B = (int*)malloc(sizeof(int) * (VECTOR_SIZE / LOCAL_SIZE));
	double answer = 0, sum = 0;
	for (i = 0; i < VECTOR_SIZE; i++) {
		A[i] = rand() % 1024;
	}
	start = clock();
	for (i = 0; i < VECTOR_SIZE; i++) {
		sum += A[i];
	}
	answer = sum / VECTOR_SIZE;
	end = clock();
	printf("On CPU = %f sec\n", (double)((end - start) / CLK_TCK));

	cl_mem bufA, bufB;
	bufA = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int) * VECTOR_SIZE, NULL, &err);
	bufB = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int) * (VECTOR_SIZE / LOCAL_SIZE), NULL, &err);
	CHECK_ERROR(err);

	err = clEnqueueWriteBuffer(queue, bufA, CL_TRUE, 0, sizeof(int) * VECTOR_SIZE, A, 0, NULL, NULL);
	CHECK_ERROR(err);
	err = clEnqueueWriteBuffer(queue, bufB, CL_TRUE, 0, sizeof(int) * (VECTOR_SIZE / LOCAL_SIZE), B, 0, NULL, NULL);
	CHECK_ERROR(err);

	err = clSetKernelArg(kernel_reduction, 0, sizeof(cl_mem), (void*)&bufA);
	CHECK_ERROR(err);
	err = clSetKernelArg(kernel_reduction, 1, sizeof(cl_mem), (void*)&bufB);
	CHECK_ERROR(err);
	err = clSetKernelArg(kernel_reduction, 2, LOCAL_SIZE * sizeof(int), NULL);
	CHECK_ERROR(err);
	err = clSetKernelArg(kernel_reduction, 3, sizeof(cl_int), (void*)&SIZE);

	size_t global_size = VECTOR_SIZE;
	size_t local_size = LOCAL_SIZE;

	start = clock();
	clEnqueueNDRangeKernel(queue, kernel_reduction, 1, NULL, &global_size, &local_size, 0, NULL, NULL);
	end = clock();
	printf("On GPU = %f sec\n", (double)((end - start) / CLK_TCK));

	err = clEnqueueReadBuffer(queue, bufB, CL_TRUE, 0, (VECTOR_SIZE) / (LOCAL_SIZE) * sizeof(int), (void*)B, 0, NULL, NULL);
	CHECK_ERROR(err);

	double compare = 0.0f;
	for (i = 0; i < VECTOR_SIZE / LOCAL_SIZE; i++) {
		compare += B[i];
	}
	compare = compare / (double)VECTOR_SIZE;

	printf("On CPU Result = %lf\nOn GPU Result = %lf\n", answer, compare);

	return 0;
}