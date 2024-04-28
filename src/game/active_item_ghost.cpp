#include "game.hpp"
#include "raylib.h"

void Game::draw_active_item_ghost() {
    Item *item = this->inventory.get_active_item();
    if (!item) return;

    Vector2 position = this->input.mouse_position_grid;

    Color color;
    if (this->grid.can_place_item(item, position)) {
        color = ColorAlpha(GREEN, 0.3);
    } else {
        color = ColorAlpha(RED, 0.3);
    }

    float base_scale = 1.0 / item->sprite.src.width;

    Renderable renderable = Renderable::create_sprite(
        item->sprite, Pivot::CENTER_CENTER, base_scale, 1.0, color
    );
    this->renderer.draw_renderable(renderable, position);
}
