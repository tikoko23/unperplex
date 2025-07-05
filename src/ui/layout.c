#include "ui/layout.h"
#include "ui/components.h"
#include "unperplex.h"

static void uiLayoutRoot(Unperplex *U) {
    CLAY() {
        COMPONENT(ComplexGraph, &U->graph);
        CLAY({
            .layout.sizing = { 50, 60 },
            .backgroundColor = { 255, 255, 0, 255 },
        });
    }
}

Clay_RenderCommandArray uiCalculateLayout(Unperplex *U) {
    Clay_Context *old = Clay_GetCurrentContext();
    Clay_SetCurrentContext(U->clay_ctx);

    cl_uint window_width = GetScreenWidth();
    cl_uint window_height = GetScreenHeight();

    Clay_SetLayoutDimensions((Clay_Dimensions) { window_width, window_height });
    Clay_SetPointerState((Clay_Vector2) { GetMouseX(), GetMouseY() }, IsMouseButtonDown(MOUSE_BUTTON_LEFT));
    Clay_UpdateScrollContainers(false, (Clay_Vector2) { GetMouseWheelMoveV().x, GetMouseWheelMoveV().y }, GetFrameTime());

    Clay_BeginLayout();
    uiLayoutRoot(U);
    Clay_RenderCommandArray commands = Clay_EndLayout();

    Clay_SetCurrentContext(old);
    return commands;
}
