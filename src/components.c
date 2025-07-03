#include "components.h"

void COMPONENT(ComplexGraph, RenderingContext *rctx) {
    CLAY({
        .userData = rctx,
        .custom = { (void *)ELEMENT_COMPLEX_GRAPH },
        .layout.sizing = {
            .width = rctx->width,
            .height = rctx->height,
        },
    });
}
