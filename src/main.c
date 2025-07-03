#include <math.h>
#include <stdint.h>
#include <stdio.h>

#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>
#include <raylib.h>
#include <raymath.h>

#include "clinit.h"
#include "program.h"
#include "render.h"

typedef enum {
    COLOR_CONTINUOUS = 0,
    COLOR_DISCRETE,
    COLOR_N_MODES,
} ColorMode;

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

int main(void) {
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

    const size_t target_width = 640;
    const size_t target_height = 480;

    InitWindow(target_width, target_height, "idk");
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    RenderingContext rctx = rctxNew(&cl, 640, 480);

    cl_uint cmode = COLOR_CONTINUOUS;
    float movement_speed = 32.0f;
    float zoom_speed = 8.0f;
    float scale = 1.0f;
    cl_float2 offset = { 0, 0 };

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_F5)) {
            if ((err = loadDefaultProgram(&cl, &prog, &render_kernel))) {
                fprintf(stderr, "OpenCL: %d\n", err);
                break;
            }
        }

        float movement_mul = GetFrameTime() / sqrtf(scale);

        if (IsKeyDown(KEY_LEFT)) {
            offset.x -= movement_speed * movement_mul;
        }

        if (IsKeyDown(KEY_RIGHT)) {
            offset.x += movement_speed * movement_mul;
        }

        if (IsKeyDown(KEY_DOWN)) {
            offset.y -= movement_speed * movement_mul;
        }

        if (IsKeyDown(KEY_UP)) {
            offset.y += movement_speed * movement_mul;
        }

        if (IsKeyPressed(KEY_M)) {
            ++cmode;
            cmode %= COLOR_N_MODES;
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 d = GetMouseDelta();
            offset.x -= d.x;
            offset.y += d.y;
        }

        cl_uint window_width = GetScreenWidth();
        cl_uint window_height = GetScreenHeight();

        float key_zoom = (IsKeyDown(KEY_X) - IsKeyDown(KEY_Z)) * zoom_speed * GetFrameTime();
        float dzoom = (key_zoom + GetMouseWheelMove()) * 0.1f;
        scale *= powf(2.0f, dzoom);

        if (IsWindowResized()) {
            rctxResize(&rctx, window_width, window_height);
        }

        BeginDrawing();

        ClearBackground(BLACK);

        clSetKernelArg(render_kernel, 1, sizeof window_width, &window_width);
        clSetKernelArg(render_kernel, 2, sizeof window_height, &window_height);
        clSetKernelArg(render_kernel, 3, sizeof offset, &offset);
        clSetKernelArg(render_kernel, 4, sizeof scale, &scale);
        clSetKernelArg(render_kernel, 5, sizeof cmode, &cmode);

        if ((err = rctxRedrawBuffer(&rctx, render_kernel))) {
            fprintf(stderr, "rctxRedrawBuffer: OpenCL: %d\n", err);
            break;
        }

        DrawTexture(rctxGetSurface(&rctx), 0, 0, WHITE);

        EndDrawing();
    }

    CloseWindow();

    rctxFree(&rctx);

    clReleaseProgram(prog);
    clReleaseKernel(render_kernel);
    CL_DataFree(&cl);
}
