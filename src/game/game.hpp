#pragma once

#include "../core/renderer.hpp"
#include "../core/resources.hpp"
#include "camera.hpp"
#include "entt/entity/fwd.hpp"
#include "entt/entt.hpp"
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
    float dt;

    Vector2 mouse_position_world;
    Vector2 mouse_position_screen;
    Vector2 mouse_position_grid;

    bool is_lmb_pressed;
    bool is_lmb_released;
    bool is_lmb_down;
    bool is_ui_interacted;

    bool is_w_down;
    bool is_s_down;
    bool is_a_down;
    bool is_d_down;

private:
    entt::registry registry;
    entt::entity player;

private:
    void update();
    void update_input();
    void update_active_item_placement();
    void update_player();

    void draw();
    void draw_renderables();
    void draw_grid_items();
    void draw_active_item_ghost();
    void update_and_draw_quickbar();

public:
    Game();
    void run();
};
