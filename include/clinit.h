#ifndef CL_INIT_H
#define CL_INIT_H

#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>
#include <CL/cl_platform.h>

typedef struct {
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
} CL_Data;

typedef void (*CL_ErrorCallback)(const char *, const void *, size_t, void *);

typedef struct {
    cl_device_type device_type;
    CL_ErrorCallback err_callback;
    const cl_context_properties *ctx_properties;
    const cl_queue_properties *queue_properties;
} CL_ContextOptions;

cl_int getCLContext(CL_Data *out, CL_ContextOptions opt);
char *getCLDeviceString(cl_device_id device, cl_device_info info_type);

void CL_DataFree(CL_Data *this);

#endif
