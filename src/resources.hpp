#pragma once

#include "raylib.h"
#include "sprite.hpp"

class Resources {
    public:
        SpriteSheet sprite_sheet;

        Resources(const Resources&) = delete;
        Resources& operator=(const Resources&) = delete;

        Resources();
};
