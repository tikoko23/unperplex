#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "clay.h"

#include "graph.h"

#define COMPONENT(name, ...) __COMPONENT_ ## name ## __(__VA_ARGS__)

typedef enum {
    ELEMENT_COMPLEX_GRAPH = 1,
    ELEMENT_TEXT_LABEL,
} CustomElementType;

void COMPONENT(ComplexGraph, ComplexGraph *graph);

#endif
