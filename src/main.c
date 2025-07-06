#include <stdint.h>

#include <CL/cl.h>
#include "clay.h"

#include "clinit.h"
#include "graph.h"
#include "render.h"
#include "unperplex.h"

int main(void) {
    Unperplex U = unperplexNew();
    unperplexInitCL(&U, (CL_ContextOptions) {});
    unperplexInitWindow(&U);
    unperplexLogDeviceInfo(&U);

    cl_int err;
    U.graph = (ComplexGraph) {
        .rctx = rctxNew(&U.cl, 640, 480),
        .scale = 1.0f,
    };

    complexGraphReloadProgram(&U.graph);

    bool quit = false;
    while (!quit) {
        switch (unperplexUpdate(&U)) {
        case UNPERPLEX_OK:
            break;
        case UNPERPLEX_QUIT:
            quit = true;
            continue;
        }
    }

    unperplexDeinitWindow(&U);
    unperplexFree(&U);
}
