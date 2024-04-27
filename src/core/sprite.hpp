#pragma once

#include <string>
#include "raylib.h"
#include <vector>


struct Sprite {
    Texture texture;
    Rectangle src;
};

class SpriteSheet {
    private:
        Texture texture;
        std::vector<Rectangle> source_rects;

    public:
        SpriteSheet(const SpriteSheet&) = delete;
        SpriteSheet& operator=(const SpriteSheet&) = delete;

        SpriteSheet();
        SpriteSheet(std::string image_file_path, uint32_t tile_width, uint32_t tile_height);
        SpriteSheet(std::string image_file_path, std::string ase_json_file_path);
        ~SpriteSheet();

        Sprite get_sprite(uint32_t tile_idx);
};
