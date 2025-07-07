#ifndef UNPERPLEX_H
#define UNPERPLEX_H

#include "ctl/alloc.h"

#include "clinit.h"
#include "ctl/str.h"
#include "graph.h"
#include "render.h"

typedef struct Unperplex {
    void *clay_mem;
    Clay_Context *clay_ctx;
    ComplexGraph graph;
    CL_Data cl;
    TArena frame_arena;

    struct {
        Clay_RenderCommandArray (*updater)(struct Unperplex *U);
#if UNPERPLEX_UI_HOT_RELOADING
        void *shared_lib_handle;
#endif
    } ui;
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
