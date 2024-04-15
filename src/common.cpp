#include "common.hpp"

#include "raylib.h"

Primitive Primitive::create_rect(Pivot pivot, float width, float height) {
    return {.type = PrimitiveType::RECTANGLE, .rectangle = {pivot, width, height}};
}

Primitive Primitive::create_circle(float radius) {
    return {.type = PrimitiveType::CIRCLE, .circle = {radius}};
}

bool Primitive::check_collision_with_point(
    Vector2 prim_position, Vector2 point_position
) {
    bool is_hit = false;
    switch (this->type) {
        case PrimitiveType::RECTANGLE: {
            Rectangle rect = get_rect_from_pivot(
                prim_position,
                this->rectangle.pivot,
                this->rectangle.width,
                this->rectangle.hight
            );
            is_hit = CheckCollisionPointRec(point_position, rect);
            break;
        }
        case PrimitiveType::CIRCLE: {
            is_hit = CheckCollisionPointCircle(
                point_position, prim_position, this->circle.radius
            );
            break;
        }
    }

    return is_hit;
}

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
