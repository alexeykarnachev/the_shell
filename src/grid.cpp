#include "grid.hpp"

#include "common.hpp"
#include "raylib.h"

Cell::Cell()
    : idx(0)
    , rect({.x = 0.0, .y = 0.0, .width = 1.0, .height = 1.0})
    , type(CellType::EMPTY) {}

Cell::Cell(uint32_t idx, Rectangle rect, CellType type)
    : idx(idx)
    , rect(rect)
    , type(type) {}

uint32_t Cell::get_idx() {
    return this->idx;
}

Rectangle Cell::get_rect() {
    return this->rect;
}

Grid::Grid() {
    Rectangle bound_rect = this->get_bound_rect();

    for (uint32_t idx = 0; idx < N_ROWS * N_COLS; ++idx) {
        uint32_t row = idx / N_COLS;
        uint32_t col = idx % N_COLS;
        float x = bound_rect.x + (float)col;
        float y = bound_rect.y + (float)row;
        Rectangle rect = {.x = x, .y = y, .width = 1.0, .height = 1.0};
        this->cells[idx] = Cell(idx, rect, CellType::EMPTY);
    }
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

Cell *Grid::get_cell(uint32_t idx) {
    if (idx >= N_ROWS * N_COLS) return nullptr;
    return &this->cells[idx];
}

Cell *Grid::get_cell(Vector2 position) {
    Rectangle bound_rect = this->get_bound_rect();
    if (!CheckCollisionPointRec(position, bound_rect)) return nullptr;

    uint32_t col = position.x - bound_rect.x;
    uint32_t row = position.y - bound_rect.y;
    uint32_t idx = row * N_ROWS + col;
    return this->get_cell(idx);
}

CellNeighborhood Grid::get_cell_neighborhood(uint32_t idx) {
    CellNeighborhood nb;
    if (idx >= N_ROWS * N_COLS) return nb;

    uint32_t row = idx / N_COLS;
    uint32_t col = idx % N_COLS;

    if (col > 0) nb.left = this->get_cell(idx - 1);
    if (row > 0) nb.top = this->get_cell(idx - N_COLS);
    if (col < N_COLS - 1) nb.right = this->get_cell(idx + 1);
    if (row < N_ROWS - 1) nb.bottom = this->get_cell(idx + N_COLS);

    if (col > 0 && row > 0) nb.left_top = this->get_cell(idx - N_COLS - 1);
    if (col < N_COLS - 1 && row > 0) nb.right_top = this->get_cell(idx - N_COLS + 1);
    if (col < N_COLS - 1 && row < N_ROWS - 1)
        nb.right_bottom = this->get_cell(idx + N_COLS + 1);
    if (col > 0 && row < N_ROWS - 1) nb.left_bottom = this->get_cell(idx + N_COLS - 1);

    return nb;
}

CellNeighborhood Grid::get_cell_neighborhood(Vector2 position) {
    CellNeighborhood nb;
    Cell *cell = this->get_cell(position);
    if (!cell) return nb;
    return this->get_cell_neighborhood(cell->get_idx());
}

CellNeighborhoodArray Grid::get_cell_neighborhood_array(uint32_t idx) {
    CellNeighborhood nb = this->get_cell_neighborhood(idx);
    return {
        nb.left,
        nb.top,
        nb.right,
        nb.bottom,
        nb.left_top,
        nb.right_top,
        nb.right_bottom,
        nb.left_bottom};
}

CellNeighborhoodArray Grid::get_cell_neighborhood_array(Vector2 position) {
    CellNeighborhood nb = this->get_cell_neighborhood(position);
    return {
        nb.left,
        nb.top,
        nb.right,
        nb.bottom,
        nb.left_top,
        nb.right_top,
        nb.right_bottom,
        nb.left_bottom};
}
