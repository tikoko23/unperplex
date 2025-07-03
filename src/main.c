#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include <CL/cl.h>
#include "etc.h"
#include "raylib.h"
#include "raymath.h"
#include "clay.h"

#include "clinit.h"
#include "components.h"
#include "graph.h"
#include "program.h"
#include "render.h"

static Vector2 globalMousePos(cl_float2 offset, cl_uint window_width, cl_uint window_height, float scale) {
    Vector2 mouse = GetMousePosition();
    Vector2 centered = Vector2Subtract(mouse, (Vector2) { window_width / 2.0f, window_height / 2.0f });
    Vector2 inverted = { centered.x, -centered.y };
    return Vector2Scale(inverted, scale);
}

static cl_int loadDefaultProgram(CL_Data *cl, cl_program *prog, cl_kernel *kernel) {
    cl_int err;
    if (*kernel != NULL) {
        clReleaseKernel(*kernel);
    }

    if (*prog != NULL) {
        clReleaseProgram(*prog);
    }

    if ((err = loadProgramFromFile(cl, prog, "cl/complex_fn.cl"))) {
        return err;
    }

    if ((err = clBuildProgram(*prog, 1, &cl->device, NULL, NULL, NULL))) {
        clReleaseProgram(*prog);
        return err;
    }

    *kernel = clCreateKernel(*prog, "funcRender", &err);
    if (*kernel == NULL) {
        clReleaseProgram(*prog);
        return err;
    }

    return CL_SUCCESS;
}

static void clayErrHandler(Clay_ErrorData err) {
    fprintf(stderr, "Clay: %.*s\n", err.errorText.length, err.errorText.chars);
}

int main(void) {
    const size_t target_width = 640;
    const size_t target_height = 480;

    size_t clay_memsize = Clay_MinMemorySize(); 
    void *clay_mem = malloc(clay_memsize);
    Clay_Arena clay_arena = Clay_CreateArenaWithCapacityAndMemory(clay_memsize, clay_mem);
    Clay_Initialize(clay_arena, (Clay_Dimensions) { target_width, target_height }, (Clay_ErrorHandler) { clayErrHandler });

    cl_int err;
    CL_Data cl;
    CL_ContextOptions ctx_opt = {
        .device_type = CL_DEVICE_TYPE_GPU,
    };

    if ((err = getCLContext(&cl, ctx_opt))) {
        fprintf(stderr, "OpenCL: failed to create context: %d\n", err);
        return 1;
    }

    char *device_name = getCLDeviceString(cl.device, CL_DEVICE_NAME);
    char *cl_version = getCLDeviceString(cl.device, CL_DEVICE_VERSION);

    printf("Chosen device: %s\n", device_name);
    printf("Using %s\n", cl_version);

    free(device_name);
    free(cl_version);

    cl_program prog = NULL;
    cl_kernel render_kernel = NULL;
    if ((err = loadDefaultProgram(&cl, &prog, &render_kernel))) {
        fprintf(stderr, "OpenCL: %d\n", err);
        return 1;
    }

    SetTraceLogLevel(LOG_WARNING);

    InitWindow(target_width, target_height, "idk");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(144);

    ComplexGraph graph = {
        .rctx = rctxNew(&cl, 640, 480),
        .scale = 1.0f,
        .render = render_kernel,
        .prog = prog,
    };

    float movement_speed = 32.0f;
    float zoom_speed = 8.0f;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_F5)) {
            if ((err = loadDefaultProgram(&cl, &prog, &render_kernel))) {
                fprintf(stderr, "OpenCL: %d\n", err);
                break;
            }
        }

        if (IsKeyPressed(KEY_M)) {
            complexGraphCycleMode(&graph, 1);
        }

        Vector2 smooth_translation = getVectorInput(KEY_LEFT, KEY_RIGHT, KEY_DOWN, KEY_UP);
        Vector2 snap_translation = Vector2Multiply(
            (Vector2) { -1.0f, 1.0f },
            Vector2Scale(
                GetMouseDelta(),
                IsMouseButtonDown(MOUSE_BUTTON_LEFT)
            )
        );

        complexGraphTranslate(&graph, smooth_translation, true);
        complexGraphTranslate(&graph, snap_translation, false);

        cl_uint window_width = GetScreenWidth();
        cl_uint window_height = GetScreenHeight();

        float key_zoom = (IsKeyDown(KEY_X) - IsKeyDown(KEY_Z)) * zoom_speed * GetFrameTime();
        float dzoom = (key_zoom + GetMouseWheelMove()) * 0.1f;
        complexGraphZoomBy(&graph, dzoom);

        if (IsWindowResized()) {
            complexGraphResizeOutput(&graph, window_width, window_height);
        }

        Clay_SetLayoutDimensions((Clay_Dimensions) { window_width, window_height });
        Clay_SetPointerState((Clay_Vector2) { GetMouseX(), GetMouseY() }, IsMouseButtonDown(MOUSE_BUTTON_LEFT));
        Clay_UpdateScrollContainers(false, (Clay_Vector2) { GetMouseWheelMoveV().x, GetMouseWheelMoveV().y }, GetFrameTime());

        Clay_BeginLayout();

        CLAY() {
            COMPONENT(ComplexGraph, &graph);
        }

        Clay_RenderCommandArray commands = Clay_EndLayout();

        BeginDrawing();
        ClearBackground(BLACK);
        renderClayCommands(&commands);
        EndDrawing();
    }

    complexGraphFree(&graph);

    CloseWindow();
    CL_DataFree(&cl);

    free(clay_mem);
}
