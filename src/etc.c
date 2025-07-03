#include "etc.h"
#include "raylib.h"

Vector2 getVectorInput(KeyboardKey h_neg, KeyboardKey h_pos, KeyboardKey v_neg, KeyboardKey v_pos) {
    return (Vector2) {
        IsKeyDown(h_pos) - IsKeyDown(h_neg),
        IsKeyDown(v_pos) - IsKeyDown(v_neg),
    };
}
