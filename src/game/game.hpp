#pragma once

#include "../core/renderer.hpp"
#include "../core/resources.hpp"
#include <vector>

class Input {
public:
    Vector2 mouse_position_world;
    Vector2 mouse_position_screen;

    bool is_lmb_pressed;
    bool is_lmb_released;
    bool is_lmb_down;

    Input();
};

class GameCamera {
public:
    float view_width;
    Vector2 target;

    GameCamera(float view_width, Vector2 target);
};

class Item {
public:
    Sprite sprite;

    Item(Sprite sprite);
};

class Inventory {
public:
    std::vector<Item> items;
    int active_item_idx = -1;

    Inventory();
};

class Game {
private:
    Renderer renderer;
    Resources resources;
    Input input;

    GameCamera camera;
    Inventory inventory;

    void update();
    void draw();

    void update_input();
    void update_and_draw_quickbar();
    void draw_active_item_ghost();

public:
    Game();
    void run();
};
