#ifndef UNPERPLEX_COMP_DECL_CUSTOM
#define UNPERPLEX_COMP_DECL_CUSTOM

#include "graph.h"

typedef enum {
    ELEMENT_COMPLEX_GRAPH = 1,
} CustomElementType;

typedef struct {
    CustomElementType type;

    union {
        ComplexGraph *graph;
    };
} CustomElement;

#endif
