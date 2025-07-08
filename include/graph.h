#ifndef UNPERPLEX_GRAPH_H
#define UNPERPLEX_GRAPH_H

#include "render.h"

typedef enum ComplexGraphRenderMode {
    GRAPH_RENDER_CONTINUOUS = 0,
    GRAPH_RENDER_DISCRETE,
    GRAPH_RENDER_N_MODES,
} ComplexGraphRenderMode;

typedef struct ComplexGraph {
    RenderingContext rctx;
    ComplexGraphRenderMode mode;
    cl_program prog;
    cl_kernel render;
    float scale;
    Vector2 offset;
    Vector2 offset_velocity;
    float velocity_decay;
} ComplexGraph;

void complexGraphTranslate(ComplexGraph *this, Vector2 d_offset, bool smooth);
void complexGraphZoomBy(ComplexGraph *this, float d_zoom);
void complexGraphCycleMode(ComplexGraph *this, size_t n);
cl_int complexGraphResizeOutput(ComplexGraph *this, size_t w, size_t h);

Texture2D complexGraphGetSurface(const ComplexGraph *this);

cl_int complexGraphReloadProgram(ComplexGraph *this);
cl_int complexGraphRenderFrame(ComplexGraph *this);

void complexGraphFree(ComplexGraph *this);

#endif
