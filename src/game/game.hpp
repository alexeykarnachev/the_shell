#pragma once

#include "../core/renderer.hpp"
#include "../core/resources.hpp"
#include "camera.hpp"
#include "grid.hpp"
#include "inventory.hpp"

class Game {
private:
    Renderer renderer;
    Resources resources;

    Grid grid;
    GameCamera camera;
    Inventory inventory;

private:
    Vector2 mouse_position_world;
    Vector2 mouse_position_screen;
    Vector2 mouse_position_grid;

    bool is_lmb_pressed;
    bool is_lmb_released;
    bool is_lmb_down;
    bool is_ui_interacted;

private:
    void update();
    void update_input();
    void update_active_item_placement();

    void draw();
    void draw_grid_items();
    void draw_active_item_ghost();
    void update_and_draw_quickbar();

public:
    Game();
    void run();
};
