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

double f(double x) {
	return (double)(3.0 * x * x + 2.0 * x + 1.0);
}

int main() {
	srand(time(NULL));
	int SIZE = 536870912;
	cl_int err;
	clock_t start, end;
	cl_event read_event;

	cl_uint num_platforms;
	cl_platform_id platform;
	cl_device_id* devices, device;
	cl_uint num_devices;
	cl_context context;
	cl_command_queue queue;
	char* kernel_source;
	size_t kernel_source_size;
	cl_program program;
	cl_kernel kernel_integral;
	cl_ulong time_start, time_end;

	err = clGetPlatformIDs(1, &platform, NULL);
	CHECK_ERROR(err);

	printf("expression = 3*x*x + 2*x + 1\n");
	printf("range 0 ~ 1000\n");
	printf("N = 536,870,912\n");
	
	err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
	CHECK_ERROR(err);

	context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
	CHECK_ERROR(err);

	queue = clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &err);
	CHECK_ERROR(err);

	kernel_source = get_source_code("int_overlap_kernel.cl", &kernel_source_size);
	
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

	kernel_integral = clCreateKernel(program, "integral", &err);
	CHECK_ERROR(err);

	double* A;
	A = (double*)malloc(sizeof(double) * (SIZE / LOCAL_SIZE));
	double answer = 0, xpos;
	double dx = (1.0 / (double)SIZE);
	start = clock();
	for (xpos = 0; xpos < 1; xpos += dx) {
		answer += dx * f(xpos);
	}
	end = clock();
	printf("On CPU = %f sec\n", (double)((end - start) / CLK_TCK));

	cl_mem bufA;
	bufA = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(double) * (SIZE / LOCAL_SIZE), NULL, &err);
	CHECK_ERROR(err);

	err = clEnqueueWriteBuffer(queue, bufA, CL_TRUE, 0, sizeof(double) * (SIZE / LOCAL_SIZE), A, 0, NULL, NULL);
	CHECK_ERROR(err);

	err = clSetKernelArg(kernel_integral, 0, sizeof(cl_mem), (void*)&bufA);
	CHECK_ERROR(err);
	err = clSetKernelArg(kernel_integral, 1, LOCAL_SIZE * sizeof(double), NULL);
	CHECK_ERROR(err);
	err = clSetKernelArg(kernel_integral, 2, sizeof(cl_int), (void*)&SIZE);

	size_t global_size = SIZE;
	size_t local_size = LOCAL_SIZE;

	clEnqueueNDRangeKernel(queue, kernel_integral, 1, NULL, &global_size, &local_size, 0, NULL, &read_event);

	err = clEnqueueReadBuffer(queue, bufA, CL_TRUE, 0, (SIZE) / (LOCAL_SIZE) * sizeof(double), (void*)A, 0, NULL, NULL);
	CHECK_ERROR(err);

	double compare = 0.0;
	for (int i = 0; i < SIZE / LOCAL_SIZE; i++) 
		compare += A[i];

	clFinish(queue);
	
	clGetEventProfilingInfo(read_event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &time_start, NULL);
	clGetEventProfilingInfo(read_event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &time_end, NULL);

	printf("Elapsed Time On GPU = %lf s\n", (double)(time_end - time_start) / 1000000000);
	printf("On CPU Result = %lf\n", answer);
	printf("On GPU Result = %lf\n", compare);
	
	return 0;
}