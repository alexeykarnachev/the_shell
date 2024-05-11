#pragma once

#include "../core/sprite.hpp"
#include "item.hpp"
#include "raylib.h"
#include <array>
#include <cstdint>

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

class CellNeighbors {
public:
    std::array<Cell *, 4> cells;

    CellNeighbors();
};

// -----------------------------------------------------------------------
// Grid
enum class WallType {
    NONE,
    HORIZONTAL,
    VERTICAL,
};

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
    CellNeighbors get_cell_neighbors(Vector2 position);
    WallType get_wall_type(Vector2 position);
    Vector2 round_position(Vector2 position);
    bool can_place_item(const Item *item, Vector2 position);
    bool place_item(const Item *item, Vector2 position, SpriteSheet &sheet);
    Sprite suggest_item_sprite(Vector2 position, ItemType item_type, SpriteSheet &sheet);
};
