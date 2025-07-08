#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "clay.h"

#include "graph.h"
#include "unperplex.h"

#include "decl/button.h"
#include "decl/custom.h"
#include "decl/mouse.h"

#define COMPONENT(name, ...) __COMPONENT_ ## name ## __(__VA_ARGS__)

void COMPONENT(ComplexGraph, Unperplex *U, ComplexGraph *graph);
void COMPONENT(Button, Unperplex *U, ButtonOptions opt);

#endif
