#include "common.hpp"

#include "raylib.h"

Rectangle get_rect_from_pivot(Vector2 position, Pivot pivot, float width, float height) {
    Vector2 offset;
    switch (pivot) {
        case Pivot::CENTER_BOTTOM: offset = {-0.5f * width, -height}; break;
        case Pivot::CENTER_TOP: offset = {-0.5f * width, 0.0}; break;
        case Pivot::LEFT_CENTER: offset = {0.0, -0.5f * height}; break;
        case Pivot::RIGHT_CENTER: offset = {-width, -0.5f * height}; break;
        case Pivot::CENTER_CENTER: offset = {-0.5f * width, -0.5f * height}; break;
    }

    return {
        .x = position.x + offset.x,
        .y = position.y + offset.y,
        .width = width,
        .height = height};
}
