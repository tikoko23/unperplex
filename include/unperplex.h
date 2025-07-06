#ifndef UNPERPLEX_H
#define UNPERPLEX_H

#include "ctl/alloc.h"

#include "clinit.h"
#include "graph.h"
#include "render.h"

typedef struct {
    void *clay_mem;
    Clay_Context *clay_ctx;
    ComplexGraph graph;
    CL_Data cl;
    TArena frame_arena;
} Unperplex;

Unperplex unperplexNew(void);

int unperplexInitCL(Unperplex *U, CL_ContextOptions opt);

void unperplexInitWindow(Unperplex *U);
void unperplexDeinitWindow(Unperplex *U);

void unperplexLogDeviceInfo(Unperplex *U);

int unperplexUpdate(Unperplex *U);

void unperplexFree(Unperplex *U);

#endif
