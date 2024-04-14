#pragma once

#include "raylib.h"
#include "sprite.hpp"

class Resources {
    private:
        SpriteSheet sprite_sheet;

    public:
        Resources(const Resources&) = delete;
        Resources& operator=(const Resources&) = delete;

        Resources();
};
