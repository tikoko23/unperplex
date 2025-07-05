#include <stdint.h>
#include <stdio.h>

#include <CL/cl.h>
#include "etc.h"
#include "raylib.h"
#include "raymath.h"
#include "clay.h"

#include "clinit.h"
#include "graph.h"
#include "program.h"
#include "render.h"
#include "ui/layout.h"
#include "unperplex.h"

#if defined(UI_LAYOUT_H) && defined(__unix__)
#  include <dlfcn.h>
#  define HOT_RELOADING 1
#else
#  define HOT_RELOADING 0
#endif

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

static Clay_RenderCommandArray (*ui_update)(Unperplex *U);
static void *ui_so_handle = NULL;

static void reloadUi(void) {
#if !HOT_RELOADING
    ui_update = uiCalculateLayout;
#else
    if (ui_so_handle) {
        dlclose(ui_so_handle);
    }

    ui_so_handle = dlopen("build/libunperplex_ui.so", RTLD_NOW);
    if (!ui_so_handle) {
        fprintf(stderr, "dlopen: %s\n", dlerror());
    }

    ui_update = dlsym(ui_so_handle, "uiCalculateLayout");
    if (!ui_update) {
        fprintf(stderr, "dlsym: %s\n", dlerror());
    }

#endif
}

int main(void) {
    reloadUi();

    Unperplex U = unperplexNew();

    cl_int err;
    cl_program prog = NULL;
    cl_kernel render_kernel = NULL;
    if ((err = loadDefaultProgram(&U.cl, &prog, &render_kernel))) {
        fprintf(stderr, "OpenCL: %d\n", err);
        return 1;
    }

    U.graph = (ComplexGraph) {
        .rctx = rctxNew(&U.cl, 640, 480),
        .scale = 1.0f,
        .render = render_kernel,
        .prog = prog,
    };

    float movement_speed = 32.0f;
    float zoom_speed = 8.0f;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_F5)) {
            puts("Reloading GPU program...");

            if ((err = loadDefaultProgram(&U.graph.rctx.cl, &U.graph.prog, &U.graph.render))) {
                fprintf(stderr, "OpenCL: %d\n", err);
                break;
            }
        }

        if (IsKeyPressed(KEY_F3)) {
            puts("Reloading UI...");

            reloadUi();
        }

        if (IsKeyPressed(KEY_M)) {
            complexGraphCycleMode(&U.graph, 1);
        }

        Vector2 smooth_translation = getVectorInput(KEY_LEFT, KEY_RIGHT, KEY_DOWN, KEY_UP);
        Vector2 snap_translation = Vector2Multiply(
            (Vector2) { -1.0f, 1.0f },
            Vector2Scale(
                GetMouseDelta(),
                IsMouseButtonDown(MOUSE_BUTTON_LEFT)
            )
        );

        complexGraphTranslate(&U.graph, smooth_translation, true);
        complexGraphTranslate(&U.graph, snap_translation, false);

        cl_uint window_width = GetScreenWidth();
        cl_uint window_height = GetScreenHeight();

        float key_zoom = (IsKeyDown(KEY_X) - IsKeyDown(KEY_Z)) * zoom_speed * GetFrameTime();
        float dzoom = (key_zoom + GetMouseWheelMove()) * 0.1f;
        complexGraphZoomBy(&U.graph, dzoom);

        if (IsWindowResized()) {
            complexGraphResizeOutput(&U.graph, window_width, window_height);
        }

        Clay_RenderCommandArray commands = ui_update(&U);

        BeginDrawing();
        ClearBackground(BLACK);
        renderClayCommands(&commands);
        EndDrawing();
    }

    unperplexFree(&U);
}
