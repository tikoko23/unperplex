#ifndef UNPERPLEX_COMP_DECL_MOUSE_H
#define UNPERPLEX_COMP_DECL_MOUSE_H

#include "raylib.h"

typedef enum {
    MOUSE_INTERACTION_CLICK,
    MOUSE_INTERACTION_RELEASE,
    MOUSE_INTERACTION_HOLD,
    MOUSE_INTERACTION_HOVER,
} MouseInteractionType;

typedef struct {
    MouseInteractionType type;
    Vector2 element_size;
    Vector2 relative_pos;
    Vector2 normalised_pos;
    Vector2 mouse_delta;
} MouseInteractionData;

typedef void (*MouseInteractionCallback)(MouseInteractionData interaction, void *userdata);

typedef struct {
    MouseInteractionCallback callback;
    void *userdata;
} MouseInteraction;

#endif
