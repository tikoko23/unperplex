#include <CL/cl.h>
#include <stdio.h>

#include "clinit.h"
#include "unperplex.h"
#include "ctl/alloc.h"

#define DEVICE_INFO_ARENA_SIZE 2048
#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 480

static void clayErrHandler(Clay_ErrorData err) {
    fprintf(stderr, "Clay: %.*s\n", err.errorText.length, err.errorText.chars);
}

Unperplex unperplexNew(void) {
    Unperplex U = {};

    size_t clay_memsize = Clay_MinMemorySize(); 
    U.clay_mem = malloc(clay_memsize);

    Clay_Arena clay_arena = Clay_CreateArenaWithCapacityAndMemory(clay_memsize, U.clay_mem);
    U.clay_ctx = Clay_Initialize(clay_arena, (Clay_Dimensions) { DEFAULT_WIDTH, DEFAULT_HEIGHT }, (Clay_ErrorHandler) { clayErrHandler });

    return U;
}

int unperplexInitCL(Unperplex *U, CL_ContextOptions opt) {
    cl_int err;

    if (!opt.device_type) {
        opt.device_type = CL_DEVICE_TYPE_GPU;
    }

    if ((err = getCLContext(&U->cl, opt))) {
        fprintf(stderr, "OpenCL: failed to create context: %d\n", err);
        return err;
    }

    return 0;
}

void unperplexInitWindow(Unperplex *U) {
    SetTraceLogLevel(LOG_WARNING);

    InitWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "idk");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(144);
}

void unperplexDeinitWindow(Unperplex *U) {
    CloseWindow();
}

void unperplexLogDeviceInfo(Unperplex *U) {
    TArena strings = tarenaNew(DEVICE_INFO_ARENA_SIZE);

    char *device_name = getCLDeviceString(&strings, U->cl.device, CL_DEVICE_NAME);
    char *cl_version = getCLDeviceString(&strings, U->cl.device, CL_DEVICE_VERSION);

    cl_uint clock_mhz;
    getCLDeviceInfo(U->cl.device, CL_DEVICE_MAX_CLOCK_FREQUENCY, clock_mhz);

    printf("Chosen device: %s\n", device_name);
    printf("Using %s\n", cl_version);
    printf("Max clock frequency: %u.%u GHz\n", clock_mhz / 1000, clock_mhz % 1000 / 10);

    tarenaFree(&strings);
}

void unperplexFree(Unperplex *U) {
    complexGraphFree(&U->graph);

    CL_DataFree(&U->cl);
    free(U->clay_mem);
}
