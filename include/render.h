#ifndef RENDER_H
#define RENDER_H

#include <stdint.h>

#include <CL/cl.h>
#include "clay.h"
#include "raylib.h"

#include "clinit.h"

typedef struct {
    size_t width;
    size_t height;
    size_t bufsize;
    cl_mem render_target;
    CL_Data cl;
    Image framebuffer;
    Texture2D gpu_ref;
} RenderingContext;

RenderingContext rctxNew(const CL_Data *cl, size_t w, size_t h);
void rctxFree(RenderingContext *ctx);

cl_int rctxRedrawBuffer(RenderingContext *ctx, cl_kernel kernel);

void rctxRecalculateBufsize(RenderingContext *rctx);
cl_int rctxResize(RenderingContext *ctx, size_t w, size_t h);

void renderClayCommands(const Clay_RenderCommandArray *commands);

#endif
