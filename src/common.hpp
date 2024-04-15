#pragma once

#include "raylib.h"

enum class Pivot {
    CENTER_BOTTOM,
    CENTER_TOP,
    LEFT_CENTER,
    RIGHT_CENTER,
    CENTER_CENTER,
};

Rectangle get_rect_from_pivot(Vector2 position, Pivot pivot, float width, float height);
