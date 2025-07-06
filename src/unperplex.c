#include <stdio.h>

#include <CL/cl.h>
#include "ctl/alloc.h"
#include "raylib.h"
#include "raymath.h"

#include "clinit.h"
#include "def.h"
#include "etc.h"
#include "raylib.h"
#include "ui/layout.h"
#include "unperplex.h"

#define DEVICE_INFO_ARENA_SIZE 2048
#define FRAME_ARENA_SIZE (1024 * 1024)
#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 480

static void clayErrHandler(Clay_ErrorData err) {
    fprintf(stderr, "Clay: %.*s\n", err.errorText.length, err.errorText.chars);
}

static void reloadUi(Unperplex *U) {
#if !UNPERPLEX_UI_HOT_RELOADING
    U->ui.updater = uiCalculateLayout;
#else
    if (U->ui.shared_lib_handle) {
        dlclose(U->ui.shared_lib_handle);
    }

    U->ui.shared_lib_handle = dlopen("build/libunperplex_ui.so", RTLD_NOW);
    if (!U->ui.shared_lib_handle) {
        fprintf(stderr, "dlopen: %s\n", dlerror());
        return;
    }

    U->ui.updater = dlsym(U->ui.shared_lib_handle, "uiCalculateLayout");
    if (!U->ui.updater) {
        fprintf(stderr, "dlsym: %s\n", dlerror());
    }
#endif
}

Unperplex unperplexNew(void) {
    Unperplex U = {};

    size_t clay_memsize = Clay_MinMemorySize(); 
    U.clay_mem = malloc(clay_memsize);

    Clay_Arena clay_arena = Clay_CreateArenaWithCapacityAndMemory(clay_memsize, U.clay_mem);
    U.clay_ctx = Clay_Initialize(clay_arena, (Clay_Dimensions) { DEFAULT_WIDTH, DEFAULT_HEIGHT }, (Clay_ErrorHandler) { clayErrHandler });

    U.frame_arena = tarenaNew(FRAME_ARENA_SIZE);

    reloadUi(&U);

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

int unperplexUpdate(Unperplex *U) {
    tarenaReset(&U->frame_arena);

    if (WindowShouldClose()) {
        return UNPERPLEX_QUIT; 
    }

    if (IsKeyPressed(KEY_F3)) {
        puts("Reloading UI...");
        reloadUi(U);
    }

    if (IsKeyPressed(KEY_F5)) {
        puts("Reloading GPU program...");
        if (complexGraphReloadProgram(&U->graph)) {
            return UNPERPLEX_QUIT;
        }
    }

    if (IsKeyPressed(KEY_M)) {
        complexGraphCycleMode(&U->graph, 1);
    }

    Vector2 smooth_translation = getVectorInput(KEY_LEFT, KEY_RIGHT, KEY_DOWN, KEY_UP);
    Vector2 snap_translation = Vector2Multiply(
        (Vector2) { -1.0f, 1.0f },
        Vector2Scale(
            GetMouseDelta(),
            IsMouseButtonDown(MOUSE_BUTTON_LEFT)
        )
    );

    float movement_speed = 32.0f;
    float zoom_speed = 8.0f;

    complexGraphTranslate(&U->graph, smooth_translation, true);
    complexGraphTranslate(&U->graph, snap_translation, false);

    cl_uint window_width = GetScreenWidth();
    cl_uint window_height = GetScreenHeight();

    float key_zoom = (IsKeyDown(KEY_X) - IsKeyDown(KEY_Z)) * zoom_speed * GetFrameTime();
    float dzoom = (key_zoom + GetMouseWheelMove()) * 0.1f;
    complexGraphZoomBy(&U->graph, dzoom);

    if (IsWindowResized()) {
        complexGraphResizeOutput(&U->graph, window_width, window_height);
    }

    Clay_RenderCommandArray commands = U->ui.updater(U);

    BeginDrawing();
    ClearBackground(BLACK);
    renderClayCommands(&commands);
    EndDrawing();

    return UNPERPLEX_OK;
}

void unperplexFree(Unperplex *U) {
    complexGraphFree(&U->graph);

    CL_DataFree(&U->cl);
    free(U->clay_mem);

    tarenaFree(&U->frame_arena);
}
