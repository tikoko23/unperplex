#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "clay.h"
#include "ctl/str.h"

#include "graph.h"

#define COMPONENT(name, ...) __COMPONENT_ ## name ## __(__VA_ARGS__)

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

typedef enum {
    ELEMENT_COMPLEX_GRAPH = 1,
} CustomElementType;

typedef enum {
    BUTTON_CONTENT_TEXT,
    BUTTON_CONTENT_CUSTOM,
} ButtonContentType;

typedef void (*ButtonContentCallback)(void *userdata);

typedef struct {
    MouseInteraction interaction;
    Clay_ElementDeclaration style;

    union {
        struct {
            Clay_TextElementConfig *text_opt;
            TStringView text;
        };
        struct {
            void *content_userdata;
            ButtonContentCallback content;
        };
    };

    ButtonContentType type;
} ButtonOptions;

void COMPONENT(ComplexGraph, ComplexGraph *graph);
void COMPONENT(Button, ButtonOptions *opt);

#endif
