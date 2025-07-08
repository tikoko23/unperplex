#ifndef UNPERPLEX_H
#define UNPERPLEX_H

#include "ctl/alloc.h"

#include "clinit.h"
#include "graph.h"
#include "render.h"
#include "ui/ui.h"

typedef struct Unperplex {
    void *clay_mem;
    Clay_Context *clay_ctx;
    ComplexGraph graph;
    CL_Data cl;
    TArena frame_arena;

    struct {
        UIState ui;
    };
} Unperplex;

typedef enum {
    UNPERPLEX_OK = 0,
    UNPERPLEX_QUIT,
} UnperplexStatus;

void unperplexClearCache(void);

Unperplex unperplexNew(void);

int unperplexInitCL(Unperplex *U, CL_ContextOptions opt);

void unperplexInitWindow(Unperplex *U);
void unperplexDeinitWindow(Unperplex *U);

void unperplexLogDeviceInfo(Unperplex *U);

int unperplexUpdate(Unperplex *U);

void unperplexFree(Unperplex *U);

#endif
