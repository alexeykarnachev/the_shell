#pragma once
#include <array>
#include <cstdint>
#include "raylib.h"

enum class Cell : uint8_t {
    NONE = 0,
    EMPTY = 1 << 0,
    WALL = 1 << 1,
};

class Grid {
  private:
    static constexpr uint32_t N_ROWS = 100;
    static constexpr uint32_t N_COLS = 100;
    std::array<Cell, N_ROWS * N_COLS> cells;

    std::tuple<bool, uint32_t> get_cell_idx_at(Vector2 position);

  public:
    Grid();

    uint32_t get_cells_count();
    Rectangle get_bound_rect();
    Rectangle get_cell_rect(uint32_t idx);
    Cell get_cell(uint32_t idx);
    Cell get_cell(Vector2 position);
    bool set_cell(uint32_t idx, Cell cell);
    bool set_cell(Vector2 position, Cell cell);
};
