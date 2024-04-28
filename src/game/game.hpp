#pragma once

#include "../core/renderer.hpp"
#include "../core/resources.hpp"
#include "raylib.h"
#include <array>
#include <cstdint>
#include <vector>

// -----------------------------------------------------------------------
// Input
class Input {
public:
    Vector2 mouse_position_world;
    Vector2 mouse_position_screen;
    Vector2 mouse_position_grid;

    bool is_lmb_pressed;
    bool is_lmb_released;
    bool is_lmb_down;
    bool is_ui_interacted;

    Input();
};

// -----------------------------------------------------------------------
// GameCamera
class GameCamera {
public:
    float view_width;
    Vector2 target;

    GameCamera(float view_width, Vector2 target);
};

// -----------------------------------------------------------------------
// Item
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

// -----------------------------------------------------------------------
// Inventory
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

// -----------------------------------------------------------------------
// Cell
class Cell {
private:
    Vector2 position;

public:
    Item item;

    Cell();
    Cell(Vector2 position);

    Vector2 get_position();
};

// -----------------------------------------------------------------------
// Grid
class Grid {
private:
    static constexpr uint32_t N_ROWS = 100;
    static constexpr uint32_t N_COLS = 100;

protected:
    friend class Game;
    std::array<Cell, N_ROWS * N_COLS> cells;

public:
    Grid();

    Rectangle get_bound_rect();
    Cell *get_cell(Vector2 position);
    Vector2 round_position(Vector2 position);
    bool can_place_item(const Item *item, Vector2 position);
    bool place_item(const Item *item, Vector2 position);
};

// -----------------------------------------------------------------------
// Game
class Game {
private:
    Renderer renderer;
    Resources resources;
    Input input;

    Grid grid;
    GameCamera camera;
    Inventory inventory;

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
