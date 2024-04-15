#pragma once

#include "raylib.h"
enum class Pivot {
    CENTER_BOTTOM,
    CENTER_TOP,
    LEFT_CENTER,
    RIGHT_CENTER,
    CENTER_CENTER,
};

enum class PrimitiveType {
    CIRCLE,
    RECTANGLE,
};

typedef struct CirclePrimitive {
    float radius;
} Circle;

typedef struct RectanglePrimitive {
    Pivot pivot;
    float width;
    float hight;
} RectanglePrimitive;

class Primitive {
public:
    PrimitiveType type;
    union {
        CirclePrimitive circle;
        RectanglePrimitive rectangle;
    };

    static Primitive create_rect(Pivot pivot, float width, float height);
    static Primitive create_circle(float radius);

    bool check_collision_with_point(Vector2 prim_position, Vector2 point_position);
};

Rectangle get_rect_from_pivot(Vector2 position, Pivot pivot, float width, float height);
