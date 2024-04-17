#include "resources.hpp"

#include "sprite.hpp"

Resources::Resources()
    : sprite_sheet_32_32(SpriteSheet("resources/sprites/sheet_32_32.png", 32, 32))
    , sprite_sheet_walls(
          SpriteSheet("resources/sprites/walls.png", "resources/sprites/walls.json")
      ) {}
