#pragma once
#include "math/vector.h"

struct Rect {
    Rect() {}
    Rect(Vector2 size, Vector2 position) : position(position), size(size) {}
    Vector2 position; /*!< Position of this rect (top-left corner). */
    Vector2 size; /*!< Size of this rect. */
};