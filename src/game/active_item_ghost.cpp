#include "game.hpp"
#include "raylib.h"

void Game::draw_active_item_ghost() {
    if (this->inventory.active_item_idx < 0) return;

    Vector2 position = this->input.mouse_position_world;
    DrawCircleV(position, 0.5, RED);
}
