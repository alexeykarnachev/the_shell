#include "camera.hpp"

#include "raylib.h"

GameCamera::GameCamera(float view_width, Vector2 target)
    : view_width(view_width)
    , target(target) {}
