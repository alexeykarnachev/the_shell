#include "grid.hpp"

#include "raylib.h"
#include <array>
#include <cstdint>
#include <tuple>

Grid::Grid() {
    this->cells.fill(Cell::EMPTY);
}

std::tuple<bool, uint32_t> Grid::get_cell_idx_at(Vector2 position) {
    Rectangle bound_rect = this->get_bound_rect();
    bool is_valid = CheckCollisionPointRec(position, bound_rect);
    uint32_t col = position.x - bound_rect.x;
    uint32_t row = position.y - bound_rect.y;
    return {is_valid, row * N_ROWS + col};
}

uint32_t Grid::get_cells_count() {
    return N_ROWS * N_COLS;
}

Rectangle Grid::get_bound_rect() {
    static Rectangle rect = {
        .x = -(float)N_COLS / 2.0f,
        .y = -(float)N_ROWS / 2.0f,
        .width = (float)N_COLS,
        .height = (float)N_ROWS};
    return rect;
}

Rectangle Grid::get_cell_rect(uint32_t idx) {
    Rectangle bound_rect = this->get_bound_rect();
    uint32_t row = idx / N_COLS;
    uint32_t col = idx % N_COLS;
    float x = bound_rect.x + (float)col;
    float y = bound_rect.y + (float)row;
    return {.x = x, .y = y, .width = 1.0, .height = 1.0};
}

Cell Grid::get_cell(uint32_t idx) {
    if (idx >= N_ROWS * N_COLS) return Cell::NONE;
    return this->cells[idx];
}

Cell Grid::get_cell(Vector2 position) {
    auto [is_valid, idx] = this->get_cell_idx_at(position);
    if (!is_valid) return Cell::NONE;
    return this->cells[idx];
}

bool Grid::set_cell(uint32_t idx, Cell cell) {
    if (idx >= N_ROWS * N_COLS) return false;
    this->cells[idx] = cell;
    return true;
}

bool Grid::set_cell(Vector2 position, Cell cell) {
    auto [is_valid, idx] = this->get_cell_idx_at(position);
    if (!is_valid) return false;
    this->cells[idx] = cell;
    return true;
}
