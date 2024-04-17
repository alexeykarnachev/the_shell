#pragma once

#include "raylib.h"

enum class PivotType {
    CENTER_BOTTOM,
    CENTER_TOP,
    LEFT_CENTER,
    LEFT_BOTTOM,
    RIGHT_CENTER,
    CENTER_CENTER,
};

struct Pivot {
    PivotType type;
    Vector2 offset = {0.0, 0.0};
};

Rectangle get_rect_from_pivot(Vector2 position, Pivot pivot, float width, float height);
