#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include <CL/cl.h>
#include "clay.h"
#include "components.h"
#include "graph.h"
#include "raylib.h"

#include "render.h"
#include "clinit.h"

#define N_CHANNELS 3

static void updateFBTexture(RenderingContext *ctx) {
    Image base = GenImageColor(ctx->width, ctx->height, WHITE);
    ImageFormat(&base, PIXELFORMAT_UNCOMPRESSED_R8G8B8);

    if (ctx->gpu_ref.id != 0) {
        UnloadTexture(ctx->gpu_ref);
    }

    ctx->gpu_ref = LoadTextureFromImage(base);
    UnloadImage(base);
}

RenderingContext rctxNew(const CL_Data *cl, size_t w, size_t h) {
    RenderingContext this = {
        .cl = *cl,
        .width = w,
        .height = h,
        .framebuffer = {
            .data = NULL,
            .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8,
            .width = w,
            .height = h,
            .mipmaps = 1,
        },
    };

    if (rctxResize(&this, w, h)) {
        return (RenderingContext) {};
    }

    return this;
}

void rctxFree(RenderingContext *ctx) {
    free(ctx->framebuffer.data);
    clReleaseMemObject(ctx->render_target);

    ctx->framebuffer = (Image) {
        .data = NULL,
        .format = 0,
        .width = 0,
        .height = 0,
        .mipmaps = 1,
    };
}

cl_int rctxRedrawBuffer(RenderingContext *ctx, cl_kernel kernel) {
    cl_int err;
    size_t work_size[] = {
        ctx->width,
        ctx->height,
    };

    clSetKernelArg(kernel, 0, sizeof (cl_mem), &ctx->render_target);

    cl_event draw_ev;
    if ((err = clEnqueueNDRangeKernel(ctx->cl.queue, kernel, 2, NULL, work_size, NULL, 0, NULL, &draw_ev))) {
        return err;
    }

    clWaitForEvents(1, &draw_ev);
    clReleaseEvent(draw_ev);

    err = clEnqueueReadBuffer(ctx->cl.queue, ctx->render_target, true, 0, ctx->bufsize, ctx->framebuffer.data, 0, NULL, NULL);
    if (err) {
        return err;
    }

    uint8_t (*img)[ctx->height][N_CHANNELS] = ctx->framebuffer.data;
    UpdateTexture(ctx->gpu_ref, img);

    return CL_SUCCESS;
}

void rctxRecalculateBufsize(RenderingContext *rctx) {
    rctx->bufsize = rctx->width * rctx->height * N_CHANNELS;
}

cl_int rctxResize(RenderingContext *ctx, size_t w, size_t h) {
    ctx->width = w;
    ctx->height = h;

    rctxRecalculateBufsize(ctx);

    clReleaseMemObject(ctx->render_target);

    ctx->framebuffer.data = realloc(ctx->framebuffer.data, ctx->bufsize);
    if (!ctx->framebuffer.data) {
        *ctx = (RenderingContext) {};
    }

    cl_int err;
    ctx->render_target = clCreateBuffer(ctx->cl.context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, ctx->bufsize, NULL, &err);
    if (!ctx->render_target) {
        free(ctx->framebuffer.data);
        return err;
    }

    updateFBTexture(ctx);

    return CL_SUCCESS;
}

static Color clayToRaylibColor(Clay_Color color) {
    return (Color) { color.r, color.g, color.b, color.a };
}

void renderClayCommands(const Clay_RenderCommandArray *commands) {
    for (size_t i = 0; i < commands->length; ++i) {
        Clay_RenderCommand *command = commands->internalArray + i;

        Clay_BoundingBox bounds = command->boundingBox;
        Rectangle bounds_rect = {
            .x = bounds.x,
            .y = bounds.y,
            .width = bounds.width,
            .height = bounds.height,
        };

        switch (command->commandType) {
        // Only uses the top left radius
        case CLAY_RENDER_COMMAND_TYPE_RECTANGLE: {
            Clay_RectangleRenderData *rect = &command->renderData.rectangle;
            if (rect->cornerRadius.topLeft == 0) {
                DrawRectangleRec(bounds_rect, clayToRaylibColor(rect->backgroundColor));               
            } else {
                float rad = rect->cornerRadius.topLeft * 2.0f / fminf(bounds.width, bounds.height);
                DrawRectangleRounded(bounds_rect, rad, 8, clayToRaylibColor(rect->backgroundColor));
            }

            break;
        }
        case CLAY_RENDER_COMMAND_TYPE_SCISSOR_START:
            BeginScissorMode(bounds.x, bounds.y, bounds.width, bounds.height);
            break;
        case CLAY_RENDER_COMMAND_TYPE_SCISSOR_END:
            EndScissorMode();
            break;
        case CLAY_RENDER_COMMAND_TYPE_CUSTOM: {
            switch ((uintptr_t)command->renderData.custom.customData) {
            case ELEMENT_COMPLEX_GRAPH: {
                ComplexGraph *graph = command->userData;
                cl_int err = complexGraphRenderFrame(graph);
                if (err) {
                    fprintf(stderr, "OpenCL: %d\n", err);
                    DrawRectangle(bounds.x, bounds.y, bounds.width, bounds.height, RED);
                } else {
                    DrawTexture(complexGraphGetSurface(graph), bounds.x, bounds.y, WHITE);
                }

                break;
            }
            }

            break;
        }
        default:
            puts("bruh");
            break;
        }
    }
}
