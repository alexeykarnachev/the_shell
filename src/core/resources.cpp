#include "resources.hpp"

#include "sprite.hpp"

Resources::Resources()
    : sprite_sheet(SpriteSheet(
        "resources/sprites/sheet_16_16.png", "resources/sprites/sheet_16_16.json"
    )) {}
