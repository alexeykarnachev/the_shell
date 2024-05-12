#pragma once

#include "core/renderer.hpp"
#include "core/resources.hpp"
#include "entt/entity/fwd.hpp"
#include "entt/entt.hpp"

namespace the_shell {
// -----------------------------------------------------------------------
// sheet indexes
namespace sheet_0 {
static constexpr uint32_t wall = 1;
static constexpr uint32_t door = 17;
}  // namespace sheet_0

// -----------------------------------------------------------------------
// enums
enum class WallType {
    NONE,
    HORIZONTAL,
    VERTICAL,
};

enum class ItemType {
    NONE,
    WALL,
    DOOR,
};

// -----------------------------------------------------------------------
// camera
class Camera {
public:
    float view_width;
    Vector2 target;

    Camera(float view_width, Vector2 target);
};

// -----------------------------------------------------------------------
// item
class Item {
public:
    ItemType type = ItemType::NONE;
    Sprite sprite;

    Item();
    Item(ItemType type, Sprite sprite);

    bool is_none();
    bool is_wall();
    bool is_door();
    bool is_wall_or_door();
};

// -----------------------------------------------------------------------
// cell
class Cell {
private:
    Vector2 position;

public:
    Item item;

    Cell();
    Cell(Vector2 position);

    Vector2 get_position();
    Rectangle get_rect();
};

class CellNeighbors {
public:
    std::array<Cell *, 4> cells;

    CellNeighbors();
};

// -----------------------------------------------------------------------
// game
class Game {
private:
    Renderer renderer;
    Resources resources;

    Camera camera;

    // -------------------------------------------------------------------
    // grid
    static constexpr uint32_t N_ROWS = 100;
    static constexpr uint32_t N_COLS = 100;
    std::array<Cell, N_ROWS * N_COLS> cells;

    // -------------------------------------------------------------------
    // inventory
    int active_item_idx = -1;
    std::vector<Item> items;

    // -------------------------------------------------------------------
    // inputs
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

    // -------------------------------------------------------------------
    // entities
    entt::registry registry;
    entt::entity player;

    // -------------------------------------------------------------------
    // update
    void update();
    void update_input();
    void update_active_item_placement();
    void update_player();
    void update_collisions();

    // -------------------------------------------------------------------
    // draw
    void draw();
    void draw_renderables();
    void draw_grid_items();
    void draw_active_item_ghost();
    void update_and_draw_quickbar();

    // -------------------------------------------------------------------
    // other

    // inventory
    Item *get_active_item();
    void set_active_item(int item_idx);
    void clear_active_item();

    // grid
    Rectangle get_bound_rect();
    Cell *get_cell(Vector2 position);
    CellNeighbors get_cell_neighbors(Vector2 position);
    WallType get_wall_type(Vector2 position);
    Vector2 round_position(Vector2 position);
    bool can_place_item(const Item *item, Vector2 position);
    bool place_item(const Item *item, Vector2 position, SpriteSheet &sheet);
    Sprite suggest_item_sprite(Vector2 position, ItemType item_type, SpriteSheet &sheet);

public:
    Game();
    void run();
};
}  // namespace the_shell
