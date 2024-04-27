#include "game.hpp"
#include "raylib.h"
#include <cstdint>
#include <cstdio>
#include <vector>

static const Color PANE_COLOR{20, 20, 20, 255};

void Game::update_and_draw_quickbar() {
    Vector2 screen_size = this->renderer.get_screen_size();
    static float item_size = 60.0;
    static float pad = 15.0;

    int n_items = inventory.items.size();
    float pane_width = item_size * n_items + 3.0 * pad;
    float pane_height = item_size + 2.0 * pad;
    float x = 0.5 * (screen_size.x - pane_width);
    float y = screen_size.y - 0.5 * pane_height;

    Vector2 position = {x, y};
    this->renderer.draw_renderable(
        Renderable::create_rectangle(
            Pivot::LEFT_CENTER, pane_width, pane_height, 1.0, PANE_COLOR
        ),
        position
    );
    position.x += pad;

    for (uint32_t i = 0; i < inventory.items.size(); ++i) {
        Item &item = inventory.items[i];

        position.x += 0.5 * item_size;
        float base_scale = item_size / item.sprite.src.width;
        Renderable renderable = Renderable::create_sprite(
            item.sprite, Pivot::CENTER_CENTER, base_scale
        );

        bool is_hovered = renderable.check_collision_with_point(
            position, this->input.mouse_position_screen
        );
        if (!is_hovered) {
            renderable.scale = 1.0;
        } else if (this->input.is_lmb_released) {
            inventory.active_item_idx = i;
            renderable.scale = 1.1;
        } else if (this->input.is_lmb_down) {
            renderable.scale = 0.9;
        } else {
            renderable.scale = 1.1;
        }

        this->renderer.draw_renderable(renderable, position);
        position.x += pad + 0.5 * item_size;
    }
}
