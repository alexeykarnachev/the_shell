#include "game.hpp"

#include "raylib.h"
#include "raymath.h"
#include <cmath>
#include <cstdint>

Vector2 get_mouse_position_world(
    Vector2 screen_size, Vector2 mouse_position_screen, GameCamera &camera
) {
    Vector2 cursor = Vector2Divide(mouse_position_screen, screen_size);
    float aspect = screen_size.x / screen_size.y;
    float view_height = camera.view_width / aspect;
    Vector2 center = camera.target;
    float left = center.x - 0.5 * camera.view_width;
    float top = center.y - 0.5 * view_height;
    float x = left + camera.view_width * cursor.x;
    float y = top + view_height * cursor.y;
    return {x, y};
}

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

Cell::Cell(Vector2 position)
    : position(position) {}

Vector2 Cell::get_position() {
    return this->position;
}

// -----------------------------------------------------------------------
// Grid
Grid::Grid() {
    Rectangle bound_rect = this->get_bound_rect();

    for (uint32_t idx = 0; idx < N_ROWS * N_COLS; ++idx) {
        uint32_t row = idx / N_COLS;
        uint32_t col = idx % N_COLS;
        float x = bound_rect.x + (float)col + 0.5;
        float y = bound_rect.y + (float)row + 0.5;
        Vector2 position = {x, y};
        this->cells[idx] = Cell(position);
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

Cell *Grid::get_cell(Vector2 position) {
    Rectangle bound_rect = this->get_bound_rect();
    if (!CheckCollisionPointRec(position, bound_rect)) return nullptr;

    uint32_t col = position.x - bound_rect.x;
    uint32_t row = position.y - bound_rect.y;
    uint32_t idx = row * N_ROWS + col;

    if (idx >= N_ROWS * N_COLS) return nullptr;
    return &this->cells[idx];
}

Vector2 Grid::round_position(Vector2 position) {
    float x = std::floor(position.x) + 0.5;
    float y = std::floor(position.y) + 0.5;
    return {x, y};
}

bool Grid::can_place_item(const Item *item, Vector2 position) {
    if (!item) return false;

    Cell *cell = this->get_cell(position);
    if (!cell) return false;

    if (cell->item.type != ItemType::NONE) return false;

    return true;
}

bool Grid::place_item(const Item *item, Vector2 position) {
    if (!this->can_place_item(item, position)) return false;

    Cell *cell = this->get_cell(position);
    cell->item = *item;

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
    this->update_active_item_placement();
}

void Game::draw() {
    this->renderer.begin_drawing();

    this->renderer.set_camera(this->camera.target, this->camera.view_width);
    this->draw_grid_items();
    this->draw_active_item_ghost();

    this->renderer.set_screen_camera();
    this->update_and_draw_quickbar();

    this->renderer.end_drawing();
}

void Game::update_input() {
    Vector2 screen_size = this->renderer.get_screen_size();
    Vector2 mouse_position_screen = GetMousePosition();

    this->input.mouse_position_screen = mouse_position_screen;
    this->input.mouse_position_world = get_mouse_position_world(
        screen_size, mouse_position_screen, this->camera
    );
    this->input.mouse_position_grid = this->grid.round_position(
        this->input.mouse_position_world
    );

    this->input.is_lmb_pressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    this->input.is_lmb_released = IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
    this->input.is_lmb_down = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
}

void Game::update_active_item_placement() {
    Vector2 position = this->input.mouse_position_grid;
    Item *item = this->inventory.get_active_item();
    Cell *cell = this->grid.get_cell(position);

    if (!item) return;
    if (!cell) return;
    if (this->input.is_ui_interacted) return;
    if (!this->input.is_lmb_down) return;
    if (!this->grid.can_place_item(item, position)) return;

    grid.place_item(item, position);
}

void Game::draw_grid_items() {
    for (Cell &cell : this->grid.cells) {
        if (cell.item.type == ItemType::NONE) continue;

        float base_scale = 1.0 / cell.item.sprite.src.width;
        Renderable renderable = Renderable::create_sprite(
            cell.item.sprite, Pivot::CENTER_CENTER, base_scale
        );
        this->renderer.draw_renderable(renderable, cell.get_position());
    }
}

void Game::draw_active_item_ghost() {
    Vector2 position = this->input.mouse_position_grid;
    Item *item = this->inventory.get_active_item();

    if (this->input.is_ui_interacted) return;
    if (!item) return;

    Color color;
    if (this->grid.can_place_item(item, position)) {
        color = ColorAlpha(GREEN, 0.3);
    } else {
        color = ColorAlpha(RED, 0.3);
    }

    float base_scale = 1.0 / item->sprite.src.width;

    Renderable renderable = Renderable::create_sprite(
        item->sprite, Pivot::CENTER_CENTER, base_scale, 1.0, color
    );
    this->renderer.draw_renderable(renderable, position);
}

void Game::update_and_draw_quickbar() {
    Vector2 screen_size = this->renderer.get_screen_size();
    static float item_size = 60.0;
    static float pad = 15.0;
    static Color pane_color{20, 20, 20, 255};

    int n_items = inventory.items.size();
    float pane_width = item_size * n_items + 3.0 * pad;
    float pane_height = item_size + 2.0 * pad;
    float x = 0.5 * (screen_size.x - pane_width);
    float y = screen_size.y - 0.5 * pane_height;

    Vector2 position = {x, y};
    Renderable renderable = Renderable::create_rectangle(
        Pivot::LEFT_CENTER, pane_width, pane_height, 1.0, pane_color
    );
    this->renderer.draw_renderable(renderable, position);
    this->input.is_ui_interacted = renderable.check_collision_with_point(
        position, this->input.mouse_position_screen
    );

    position.x += pad;
    for (uint32_t i = 0; i < inventory.items.size(); ++i) {
        Item &item = inventory.items[i];

        position.x += 0.5 * item_size;
        float base_scale = item_size / item.sprite.src.width;
        Renderable renderable = Renderable::create_sprite(
            item.sprite, Pivot::CENTER_CENTER, base_scale
        );

        bool is_hovered = renderable.check_collision_with_point(
            position, this->input.mouse_position_screen
        );
        if (!is_hovered) {
            renderable.scale = 1.0;
        } else if (this->input.is_lmb_released) {
            inventory.set_active_item(i);
            renderable.scale = 1.1;
        } else if (this->input.is_lmb_down) {
            renderable.scale = 0.9;
        } else {
            renderable.scale = 1.1;
        }

        this->renderer.draw_renderable(renderable, position);
        position.x += pad + 0.5 * item_size;
    }
}
