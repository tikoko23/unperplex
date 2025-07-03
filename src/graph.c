#include "raylib.h"
#include "raymath.h"
#include "render.h"

#include "graph.h"

void complexGraphTranslate(ComplexGraph *this, Vector2 d_offset, bool smooth) {
    if (!smooth) {
        this->offset = Vector2Add(this->offset, d_offset);
    } else {
        this->offset_velocity = Vector2Add(this->offset_velocity, d_offset);
    }

    float dt = GetFrameTime();
    float decay = Clamp((1.0f - this->velocity_decay * dt), 0.0f, 1.0f);
    Vector2 net_movement = Vector2Scale(this->offset_velocity, decay * dt / sqrtf(this->scale));
    this->offset = Vector2Add(this->offset, net_movement);
}

void complexGraphZoomBy(ComplexGraph *this, float d_zoom) {
    this->scale *= powf(2, d_zoom);
}

void complexGraphCycleMode(ComplexGraph *this, size_t n) {
    this->mode += n;
    this->mode %= GRAPH_RENDER_N_MODES;
}

cl_int complexGraphResizeOutput(ComplexGraph *this, size_t w, size_t h) {
    return rctxResize(&this->rctx, w, h);
}

Texture2D complexGraphGetSurface(const ComplexGraph *this) {
    return this->rctx.gpu_ref;
}

cl_int complexGraphRenderFrame(ComplexGraph *this) {
    cl_uint truncated_ctx_w = this->rctx.width; 
    cl_uint truncated_ctx_h = this->rctx.width; 
    cl_float2 truncated_offset = { this->offset.x, this->offset.y };
    cl_float truncated_scale = this->scale;
    cl_uint truncated_mode = this->mode;

    setKernelArg(this->render, 1, truncated_ctx_w);
    setKernelArg(this->render, 2, truncated_ctx_h);
    setKernelArg(this->render, 3, truncated_offset);
    setKernelArg(this->render, 4, truncated_scale);
    setKernelArg(this->render, 5, truncated_mode);

    return rctxRedrawBuffer(&this->rctx, this->render);
}

void complexGraphFree(ComplexGraph *this) {
    rctxFree(&this->rctx);

    clReleaseProgram(this->prog);
    clReleaseKernel(this->render);
}
