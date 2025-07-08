#ifndef UNPERPLEX_COMP_DECL_BUTTON_H
#define UNPERPLEX_COMP_DECL_BUTTON_H

#include "clay.h"
#include "ctl/str.h"

#include "mouse.h"

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

#endif
