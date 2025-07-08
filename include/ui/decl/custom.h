#ifndef UNPERPLEX_COMP_DECL_CUSTOM
#define UNPERPLEX_COMP_DECL_CUSTOM

#include "graph.h"

typedef enum CustomElementType {
    ELEMENT_COMPLEX_GRAPH = 1,
} CustomElementType;

typedef struct CustomElement {
    CustomElementType type;

    union {
        ComplexGraph *graph;
    };
} CustomElement;

#endif
