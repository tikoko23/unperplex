#include <math.h>
#include <stdint.h>

#include "ctl/alloc.h"
#include "ctl/str.h"
#include "raylib.h"

#define WANT_ANIM_TIME_CONVERSIONS
#include "ui/animation.h"
#include "ui/components.h"
#include "ui/layout.h"

#define RAYLIB_MAX_MOUSE_BUTTON 6

static Animation anim = {
    .epoch = (clock_t)-1,
};

static void helloWorldCallback(MouseInteractionData data, void *userdata) {
    if (data.type != MOUSE_INTERACTION_CLICK
     || !IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        return;
    }

    puts(userdata);
}

static void buttonContents(void *userdata) {
    CLAY({
        .layout.sizing = {
            .width = CLAY_SIZING_FIXED(20),
            .height = CLAY_SIZING_FIXED(30),
        },
        .backgroundColor = { 0, 255, 255, 255 },
        .cornerRadius = (uintptr_t)userdata,
    });
}

static void uiLayoutRoot(Unperplex *U) {
    ButtonOptions *opt = tarenaAlloc(&U->frame_arena, (sizeof *opt) * 2);
    opt[0] = (ButtonOptions) {
        .type = BUTTON_CONTENT_TEXT,
        .interaction = {
            .userdata = "hello world!",
            .callback = helloWorldCallback,
        },
        .text = tsvNewFromL("button"),
        .text_opt = CLAY_TEXT_CONFIG({
            .textColor = { 255, 255, 255, 255 },
            .fontSize = 40,
        }),
        .style = {
            .backgroundColor = { 0, 200, 0, 255 },
            .cornerRadius = CLAY_CORNER_RADIUS(animationQuery(&anim) * 20),
            .layout.padding = CLAY_PADDING_ALL(10),
        },
    };

    opt[1] = (ButtonOptions) {
        .type = BUTTON_CONTENT_CUSTOM,
        .interaction = {
            .userdata = "idk",
            .callback = helloWorldCallback,
        },
        .content = buttonContents,
        .content_userdata = (void *)2,
        .style = {
            .backgroundColor = { 255, 0, 255, 100 },
            .layout.padding = CLAY_PADDING_ALL(10),
        }
    };

    CLAY({
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .childGap = 10,
            .childAlignment = {
                .x = CLAY_ALIGN_X_CENTER,
                .y = CLAY_ALIGN_Y_CENTER,
            },
            .sizing = {
                .width = CLAY_SIZING_GROW(),
                .height = CLAY_SIZING_GROW(),
            },
        },
    }) {
        // COMPONENT(ComplexGraph, &U->graph);
        COMPONENT(Button, opt + 0);
        COMPONENT(Button, opt + 1);
    };
}

static bool anyMouseButtonPressed(void) {
    bool any_button_pressed = false;

    for (size_t i = 0; i <= RAYLIB_MAX_MOUSE_BUTTON; ++i) {
        any_button_pressed |= IsMouseButtonDown(any_button_pressed);
    }

    return any_button_pressed;
}

static double ease(double t) {
    return 0.5f - cos(2.0f * t * M_PI) / 2.0f;
}

Clay_RenderCommandArray uiCalculateLayout(Unperplex *U) {
    if (anim.epoch == (clock_t)-1) {
        anim = animationNew(ANIMATION_LOOP, ease, DUR_S(1));
        animationScheduleFromNow(&anim, DUR_S(1));
    }

    Clay_Context *old = Clay_GetCurrentContext();
    Clay_SetCurrentContext(U->clay_ctx);

    cl_uint window_width = GetScreenWidth();
    cl_uint window_height = GetScreenHeight();

    Clay_SetLayoutDimensions((Clay_Dimensions) { window_width, window_height });
    Clay_SetPointerState((Clay_Vector2) { GetMouseX(), GetMouseY() }, anyMouseButtonPressed());
    Clay_UpdateScrollContainers(false, (Clay_Vector2) { GetMouseWheelMoveV().x, GetMouseWheelMoveV().y }, GetFrameTime());

    Clay_BeginLayout();
    uiLayoutRoot(U);
    Clay_RenderCommandArray commands = Clay_EndLayout();

    Clay_SetCurrentContext(old);
    return commands;
}
