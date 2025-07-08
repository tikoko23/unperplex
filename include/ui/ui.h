#ifndef UNPERPLEX_UI_H
#define UNPERPLEX_UI_H

#include "clay.h"
#include "ctl/alloc.h"

struct Unperplex;

typedef struct UIState {
    TArena arena;

    Clay_RenderCommandArray (*updater)(struct Unperplex *U);

#if UNPERPLEX_UI_HOT_RELOADING
    void *shared_lib_handle;
#endif
} UIState;

UIState uiStateNew(void);

void uiStateBeginLayout(UIState *S);

void uiStateFree(UIState *S);

#endif
