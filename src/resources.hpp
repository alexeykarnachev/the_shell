#pragma once

#include "raylib.h"
#include "sprite.hpp"
#include <unordered_map>

class Resources {
    public:
        SpriteSheet sprite_sheet_32_32;
        SpriteSheet sprite_sheet_walls;

        Resources(const Resources&) = delete;
        Resources& operator=(const Resources&) = delete;

        Resources();
};
