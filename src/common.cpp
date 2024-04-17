#include "common.hpp"

#include "raylib.h"
#include "raymath.h"

Rectangle get_rect_from_pivot(Vector2 position, Pivot pivot, float width, float height) {
    Vector2 offset;
    switch (pivot.type) {
        case PivotType::CENTER_BOTTOM: offset = {-0.5f * width, -height}; break;
        case PivotType::CENTER_TOP: offset = {-0.5f * width, 0.0}; break;
        case PivotType::LEFT_CENTER: offset = {0.0, -0.5f * height}; break;
        case PivotType::LEFT_BOTTOM: offset = {0.0, -height}; break;
        case PivotType::RIGHT_CENTER: offset = {-width, -0.5f * height}; break;
        case PivotType::CENTER_CENTER: offset = {-0.5f * width, -0.5f * height}; break;
    }
    offset = Vector2Add(offset, pivot.offset);

    return {
        .x = position.x + offset.x,
        .y = position.y + offset.y,
        .width = width,
        .height = height};
}
