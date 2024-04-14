#include "sprite.hpp"

#include "raylib.h"
#include "raymath.h"
#include <string>

SpriteSheet::SpriteSheet(std::string file_path, uint32_t tile_size) {
    this->tile_size = tile_size;
    this->texture = LoadTexture(file_path.c_str());
    SetTextureFilter(this->texture, TEXTURE_FILTER_BILINEAR);
}

SpriteSheet::~SpriteSheet() {
    UnloadTexture(this->texture);
}

Sprite SpriteSheet::get_sprite(uint32_t tile_idx) {
    static uint32_t n_cols = this->texture.width / this->tile_size;
    float row = tile_idx / (float)n_cols;
    float col = tile_idx % n_cols;
    Rectangle src = {
        .x = col * this->tile_size,
        .y = row * this->tile_size,
        .width = (float)this->tile_size,
        .height = (float)this->tile_size};

    return {.texture = this->texture, .src = src};
}
