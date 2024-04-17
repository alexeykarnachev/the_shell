#pragma once
#include <array>
#include <cstdint>
#include "raylib.h"
#include "common.hpp"

enum class CellType : uint8_t {
    EMPTY = 1 << 0,
    WALL = 1 << 1,
};

class Cell {
private:
    uint32_t idx;
    Rectangle rect;

public:
    CellType type;

    Cell();
    Cell(uint32_t idx, Rectangle rect, CellType type);

    uint32_t get_idx();
    Rectangle get_rect();
};

struct CellNeighborhood {
    Cell *left = nullptr;
    Cell *top = nullptr;
    Cell *right = nullptr;
    Cell *bottom = nullptr;
    Cell *left_top = nullptr;
    Cell *right_top = nullptr;
    Cell *right_bottom = nullptr;
    Cell *left_bottom = nullptr;
};

typedef std::array<Cell*, 8> CellNeighborhoodArray;

class Grid {
  private:
    static constexpr uint32_t N_ROWS = 100;
    static constexpr uint32_t N_COLS = 100;
    std::array<Cell, N_ROWS * N_COLS> cells;

  public:
    Grid();

    uint32_t get_cells_count();
    Rectangle get_bound_rect();
    Cell* get_cell(uint32_t idx);
    Cell* get_cell(Vector2 position);
    CellNeighborhood get_cell_neighborhood(uint32_t idx);
    CellNeighborhood get_cell_neighborhood(Vector2 position);
    CellNeighborhoodArray get_cell_neighborhood_array(uint32_t idx);
    CellNeighborhoodArray get_cell_neighborhood_array(Vector2 position);
};
