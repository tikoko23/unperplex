#include <stdio.h>

#include "clinit.h"
#include "ctl/alloc.h"

static void errCallback(const char* errinfo, const void* private_info, size_t cb, void* user_data) {
    fprintf(stderr, "\x1b[91mCL Error: %s\x1b[0m\n", errinfo);
}

cl_int getCLContext(CL_Data *out, CL_ContextOptions opt) {
    cl_int err;
    if ((err = clGetPlatformIDs(1, &out->platform, NULL))) {
        return err;
    }

    if ((err = clGetDeviceIDs(out->platform, opt.device_type, 1, &out->device, NULL))) {
        return err;
    }

    if (!opt.err_callback) {
        opt.err_callback = errCallback;
    }

    out->context = clCreateContext(opt.ctx_properties, 1, &out->device, opt.err_callback, opt.err_callback_userdata, &err);
    if (!out->context) {
        return err;
    }

    out->queue = clCreateCommandQueueWithProperties(out->context, out->device, opt.queue_properties, &err);
    if (!out->queue) {
        clReleaseContext(out->context);
        return err;
    }

    return CL_SUCCESS;
}

char *getCLDeviceString(TArena *arena, cl_device_id device, cl_device_info info_type) {
    size_t info_len;
    clGetDeviceInfo(device, info_type, 0, NULL, &info_len);

    char *device_info = tarenaAlloc(arena, info_len + 1);
    if (!device_info) {
        return NULL;
    }

    clGetDeviceInfo(device, info_type, info_len, device_info, NULL);
    device_info[info_len] = '\0';

    return device_info;
}

void CL_DataFree(CL_Data *this) {
    clReleaseCommandQueue(this->queue);
    clReleaseContext(this->context);
    clReleaseDevice(this->device);
}
