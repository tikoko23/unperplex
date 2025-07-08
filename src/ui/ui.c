#include "ui/ui.h"
#include "ctl/alloc.h"

#define UI_ARENA_SIZE (1024 * 1024)

UIState uiStateNew(void) {
    return (UIState) {
        .arena = tarenaNew(UI_ARENA_SIZE),
    };
}

void uiStateBeginLayout(UIState *S) {
    tarenaReset(&S->arena);
}

void uiStateFree(UIState *S) {
    tarenaFree(&S->arena);
}
