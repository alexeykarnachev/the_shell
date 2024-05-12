#include "sprite.hpp"

#include "json.hpp"
#include "raylib.h"
#include <fstream>
#include <string>

using json = nlohmann::json;

json load_json(std::string file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + file_path);
    }

    auto data = json::parse(file);
    file.close();
    return data;
}

SpriteSheet::SpriteSheet(
    std::string image_file_path, uint32_t tile_width, uint32_t tile_height
) {
    this->texture = LoadTexture(image_file_path.c_str());
    SetTextureFilter(this->texture, TEXTURE_FILTER_BILINEAR);

    uint32_t n_rows = this->texture.height / tile_height;
    uint32_t n_cols = this->texture.width / tile_width;
    uint32_t n_tiles = n_rows * n_cols;
    for (uint32_t i = 0; i < n_tiles; ++i) {
        uint32_t row = i / n_cols;
        uint32_t col = i % n_cols;
        Rectangle src = {
            .x = (float)col * tile_width,
            .y = (float)row * tile_height,
            .width = (float)tile_width,
            .height = (float)tile_height};
        this->source_rects.emplace_back(src);
    }
}

SpriteSheet::SpriteSheet(std::string image_file_path, std::string ase_json_file_path) {
    this->texture = LoadTexture(image_file_path.c_str());
    SetTextureFilter(this->texture, TEXTURE_FILTER_BILINEAR);

    json ase = load_json(ase_json_file_path);
    for (auto frame : ase["frames"]) {
        auto frame_data = frame["frame"];
        Rectangle src = {
            .x = frame_data["x"],
            .y = frame_data["y"],
            .width = frame_data["w"],
            .height = frame_data["h"]};
        this->source_rects.emplace_back(src);
    }
}

SpriteSheet::~SpriteSheet() {
    UnloadTexture(this->texture);
}

Sprite SpriteSheet::get_sprite(uint32_t tile_idx) {
    if (tile_idx >= this->source_rects.size()) return Sprite();
    Rectangle src = this->source_rects[tile_idx];
    return {.texture = this->texture, .src = src};
}
