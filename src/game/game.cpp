#include "game.hpp"

#include <cmath>
#include <cstdint>

// -----------------------------------------------------------------------
// Input
Input::Input() = default;

// -----------------------------------------------------------------------
// GameCamera
GameCamera::GameCamera(float view_width, Vector2 target)
    : view_width(view_width)
    , target(target) {}

// -----------------------------------------------------------------------
// Item
Item::Item() = default;

Item::Item(ItemType type, Sprite sprite)
    : type(type)
    , sprite(sprite) {}

// -----------------------------------------------------------------------
// Inventory
Inventory::Inventory() = default;

Item *Inventory::get_active_item() {
    int idx = this->active_item_idx;
    if (idx >= 0 && (uint32_t)idx < this->items.size()) {
        return &this->items[idx];
    }

    return nullptr;
}

void Inventory::set_active_item(int item_idx) {
    this->active_item_idx = item_idx;
}

void Inventory::clear_active_item() {
    this->active_item_idx = -1;
}

// -----------------------------------------------------------------------
// Cell
Cell::Cell() = default;

Cell::Cell(uint32_t idx, Rectangle rect)
    : idx(idx)
    , rect(rect) {}

uint32_t Cell::get_idx() {
    return this->idx;
}

Rectangle Cell::get_rect() {
    return this->rect;
}

// -----------------------------------------------------------------------
// Game
Grid::Grid() {
    Rectangle bound_rect = this->get_bound_rect();

    for (uint32_t idx = 0; idx < N_ROWS * N_COLS; ++idx) {
        uint32_t row = idx / N_COLS;
        uint32_t col = idx % N_COLS;
        float x = bound_rect.x + (float)col;
        float y = bound_rect.y + (float)row;
        Rectangle rect = {.x = x, .y = y, .width = 1.0, .height = 1.0};
        this->cells[idx] = Cell(idx, rect);
    }
}

Rectangle Grid::get_bound_rect() {
    static Rectangle rect = {
        .x = -(float)N_COLS / 2.0f,
        .y = -(float)N_ROWS / 2.0f,
        .width = (float)N_COLS,
        .height = (float)N_ROWS};
    return rect;
}

Vector2 Grid::round_position(Vector2 position) {
    float x = std::floor(position.x) + 0.5;
    float y = std::floor(position.y) + 0.5;
    return {x, y};
}

bool Grid::can_place_item(const Item *item, Vector2 position) {
    if (!item) return false;
    return true;
}

// -----------------------------------------------------------------------
// Game
Game::Game()
    : renderer(1920, 1080)
    , camera(30.0, {0.0, 0.0}) {

    SpriteSheet &sheet = this->resources.sprite_sheet;
    this->inventory.items.emplace_back(ItemType::WALL, sheet.get_sprite(1));
    this->inventory.items.emplace_back(ItemType::DOOR, sheet.get_sprite(17));
}

void Game::run() {
    while (!WindowShouldClose()) {
        this->update();
        this->draw();
    }
}

void Game::update() {
    this->update_input();
}

void Game::draw() {
    this->renderer.begin_drawing();

    this->renderer.set_camera(this->camera.target, this->camera.view_width);
    this->draw_active_item_ghost();

    this->renderer.set_screen_camera();
    this->update_and_draw_quickbar();

    this->renderer.end_drawing();
}
