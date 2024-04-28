#pragma once

#include "../core/renderer.hpp"
#include "../core/resources.hpp"
#include "raylib.h"
#include <array>
#include <cstdint>
#include <vector>

class Input {
public:
    Vector2 mouse_position_world;
    Vector2 mouse_position_screen;
    Vector2 mouse_position_grid;

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

enum class ItemType {
    NONE,
    WALL,
    DOOR,
};

class Item {
public:
    ItemType type = ItemType::NONE;
    Sprite sprite;

    Item();
    Item(ItemType type, Sprite sprite);
};

class Inventory {
private:
    int active_item_idx = -1;

public:
    std::vector<Item> items;

    Inventory();

    Item *get_active_item();
    void set_active_item(int item_idx);
    void clear_active_item();
};

class Cell {
private:
    uint32_t idx;
    Rectangle rect;

public:
    Item item;

    Cell();
    Cell(uint32_t idx, Rectangle rect);

    uint32_t get_idx();
    Rectangle get_rect();
};

class Grid {
private:
    static constexpr uint32_t N_ROWS = 100;
    static constexpr uint32_t N_COLS = 100;
    std::array<Cell, N_ROWS * N_COLS> cells;

public:
    Grid();

    Rectangle get_bound_rect();
    Vector2 round_position(Vector2 position);
    bool can_place_item(const Item *item, Vector2 position);
};

class Game {
private:
    Renderer renderer;
    Resources resources;
    Input input;

    Grid grid;
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
