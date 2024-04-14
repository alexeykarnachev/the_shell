#pragma once

#include "raylib.h"

class GameCamera {
public:
    float view_width;
    Vector2 target;

    GameCamera(float view_width, Vector2 target);
};
