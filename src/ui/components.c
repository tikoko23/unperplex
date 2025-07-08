#include <stdint.h>

#include "clay.h"
#include "ctl/alloc.h"
#include "raylib.h"
#include "raymath.h"

#include "graph.h"
#include "ui/decl/custom.h"
#include "ui/ui.h"
#include "unperplex.h"
#include "ui/components.h"

void COMPONENT(ComplexGraph, Unperplex *U, ComplexGraph *graph) {
    CustomElement *g = tarenaAlloc(&U->ui.arena, sizeof *g);

    *g = (CustomElement) {
        .type = ELEMENT_COMPLEX_GRAPH,
        .graph = graph,
    };

    CLAY({
        .userData = graph,
        .custom = { g },
        .layout.sizing = {
            .width = graph->rctx.width,
            .height = graph->rctx.height,
        },
    });
}

static void forwardMouseInteraction(Clay_ElementId id, Clay_PointerData ptr, intptr_t userdata) {
    Clay_ElementData el_data = Clay_GetElementData(id);
    MouseInteraction *interaction = (MouseInteraction *)userdata;

    MouseInteractionData m_data = {
        .mouse_delta = GetMouseDelta(),
        .element_size = {
            el_data.boundingBox.width,
            el_data.boundingBox.height,
        },
        .relative_pos = Vector2Subtract(
            (Vector2) { ptr.position.x, ptr.position.y },
            (Vector2) { el_data.boundingBox.x, el_data.boundingBox.y }
        ),
    };

    switch (ptr.state) {
    case CLAY_POINTER_DATA_PRESSED_THIS_FRAME:
        m_data.type = MOUSE_INTERACTION_CLICK;
        break;
    case CLAY_POINTER_DATA_PRESSED:
        m_data.type = MOUSE_INTERACTION_HOLD;
        break;
    case CLAY_POINTER_DATA_RELEASED:
        m_data.type = MOUSE_INTERACTION_HOVER;
        break;
    case CLAY_POINTER_DATA_RELEASED_THIS_FRAME:
        m_data.type = MOUSE_INTERACTION_RELEASE;
        break;
    }

    m_data.normalised_pos = Vector2Divide(m_data.relative_pos, m_data.element_size);

    interaction->callback(m_data, interaction->userdata);
}

void COMPONENT(Button, Unperplex *U, ButtonOptions opt) {
    CLAY(opt.style) {
        MouseInteraction *interaction = tarenaAlloc(&U->ui.arena, sizeof *interaction);
        *interaction = opt.interaction;

        Clay_OnHover(forwardMouseInteraction, (intptr_t)interaction);

        switch (opt.type) {
        case BUTTON_CONTENT_CUSTOM:
            opt.content(opt.content_userdata);
            break;
        case BUTTON_CONTENT_TEXT:
            CLAY_TEXT(((Clay_String) {
                .chars = opt.text.data,
                .length = opt.text.length,
            }), opt.text_opt);
            break;
        }
    }
}

