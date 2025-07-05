#ifndef UNPERPLEX_H
#define UNPERPLEX_H

#include "clinit.h"
#include "graph.h"
#include "render.h"

typedef struct {
    void *clay_mem;
    Clay_Context *clay_ctx;
    ComplexGraph graph;
    CL_Data cl;
} Unperplex;

Unperplex unperplexNew(void);

int unperplexInitCL(Unperplex *U, CL_ContextOptions opt);

void unperplexInitWindow(Unperplex *U);
void unperplexDeinitWindow(Unperplex *U);

void unperplexLogDeviceInfo(Unperplex *U);

void unperplexFree(Unperplex *U);

#endif
