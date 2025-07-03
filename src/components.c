#include "components.h"
#include "graph.h"

void COMPONENT(ComplexGraph, ComplexGraph *graph) {
    CLAY({
        .userData = graph,
        .custom = { (void *)ELEMENT_COMPLEX_GRAPH },
        .layout.sizing = {
            .width = graph->rctx.width,
            .height = graph->rctx.height,
        },
    });
}
