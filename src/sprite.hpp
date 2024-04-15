#pragma once

#include <string>
#include "raylib.h"


struct Sprite {
    Texture texture;
    Rectangle src;
};

class SpriteSheet {
    private:
        Texture texture;
        uint32_t tile_size;

    public:
        SpriteSheet(const SpriteSheet&) = delete;
        SpriteSheet& operator=(const SpriteSheet&) = delete;

        SpriteSheet();
        SpriteSheet(std::string file_path, uint32_t tile_size);
        ~SpriteSheet();

        Sprite get_sprite(uint32_t tile_idx);
};
