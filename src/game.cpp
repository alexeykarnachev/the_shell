#include "game.hpp"

#include "raylib.h"
#include "raymath.h"
#include <cmath>
#include <cstdint>
#include <cstdio>

namespace the_shell {
// -----------------------------------------------------------------------
// camera
Camera::Camera(float view_width, Vector2 target)
    : view_width(view_width)
    , target(target) {}

// -----------------------------------------------------------------------
// item
Item::Item() = default;

Item::Item(ItemType type, Sprite sprite)
    : type(type)
    , sprite(sprite) {}

bool Item::is_none() {
    return this->type == ItemType::NONE;
}

bool Item::is_wall() {
    return this->type == ItemType::WALL;
}

bool Item::is_door() {
    return this->type == ItemType::DOOR;
}

bool Item::is_wall_or_door() {
    return this->is_wall() || this->is_door();
}

// -----------------------------------------------------------------------
// cell
Cell::Cell() = default;

Cell::Cell(Vector2 position)
    : position(position) {}

Vector2 Cell::get_position() {
    return this->position;
}

Rectangle Cell::get_rect() {
    return {
        .x = this->position.x - 0.5f,
        .y = this->position.y - 0.5f,
        .width = 1.0,
        .height = 1.0
    };
}

CellNeighbors::CellNeighbors() {
    this->cells.fill(nullptr);
}

// -----------------------------------------------------------------------
// components
struct Position_C : public Vector2 {
    Position_C(const Vector2 &vec)
        : Vector2{vec.x, vec.y} {}
};

struct Renderable_C : public Renderable {};

// -----------------------------------------------------------------------
// game
Game::Game()
    : renderer(1920, 1080)
    , camera(30.0, {0.0, 0.0}) {

    // -------------------------------------------------------------------
    // inventory
    SpriteSheet &sheet = this->resources.sprite_sheet;
    this->items.emplace_back(ItemType::WALL, sheet.get_sprite(sheet_0::wall));
    this->items.emplace_back(ItemType::DOOR, sheet.get_sprite(sheet_0::door));

    // -------------------------------------------------------------------
    // entities
    this->player = this->registry.create();
    this->registry.emplace<Position_C>(this->player, Position_C({0.0, 0.0}));
    this->registry.emplace<Renderable_C>(
        this->player, Renderable_C::create_circle(0.5, 1.0, BLUE)
    );

    // -------------------------------------------------------------------
    // grid
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

void Game::run() {
    while (!WindowShouldClose()) {
        this->update();
        this->draw();
    }
}

// -----------------------------------------------------------------------
// update
void Game::update() {
    this->update_input();
    this->update_active_item_placement();
    this->update_player();
    this->update_collisions();
}

void Game::update_input() {
    this->dt = GetFrameTime();

    Vector2 screen_size = this->renderer.get_screen_size();
    Vector2 mouse_position_screen = GetMousePosition();

    this->mouse_position_screen = mouse_position_screen;

    {  // mouse_position_world
        Vector2 cursor = Vector2Divide(mouse_position_screen, screen_size);
        float aspect = screen_size.x / screen_size.y;
        float view_height = camera.view_width / aspect;
        Vector2 center = camera.target;
        float left = center.x - 0.5 * camera.view_width;
        float top = center.y - 0.5 * view_height;
        float x = left + camera.view_width * cursor.x;
        float y = top + view_height * cursor.y;
        this->mouse_position_world = {.x = x, .y = y};
    }

    this->mouse_position_grid = this->round_position(this->mouse_position_world);

    this->is_lmb_pressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    this->is_lmb_released = IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
    this->is_lmb_down = IsMouseButtonDown(MOUSE_LEFT_BUTTON);

    this->is_w_down = IsKeyDown(KEY_W);
    this->is_s_down = IsKeyDown(KEY_S);
    this->is_a_down = IsKeyDown(KEY_A);
    this->is_d_down = IsKeyDown(KEY_D);
}

void Game::update_active_item_placement() {
    Vector2 mouse_position = this->mouse_position_grid;

    Item *item = this->get_active_item();
    Cell *cell = this->get_cell(mouse_position);

    if (!item) return;
    if (!cell) return;
    if (this->is_ui_interacted) return;
    if (!this->is_lmb_down) return;
    if (!this->can_place_item(item, mouse_position)) return;

    this->place_item(item, mouse_position, this->resources.sprite_sheet);
}

void Game::update_player() {
    static float speed = 3.0;

    auto &position = registry.get<Position_C>(this->player);
    Vector2 step = Vector2Zero();
    if (this->is_w_down) step.y -= 1.0;
    if (this->is_s_down) step.y += 1.0;
    if (this->is_a_down) step.x -= 1.0;
    if (this->is_d_down) step.x += 1.0;

    step = Vector2Scale(Vector2Normalize(step), this->dt * speed);
    position = Position_C(Vector2Add(step, position));
}

void Game::update_collisions() {
    auto view = registry.view<Position_C>();
    for (auto entity : view) {
        auto [position] = view.get(entity);

        CellNeighbors nb = this->get_cell_neighbors(position);
        for (uint32_t i = 0; i < nb.cells.size(); ++i) {
            Cell *cell = nb.cells[i];
            if (!cell || cell->item.is_none()) continue;

            Rectangle rect = cell->get_rect();
            Vector2 mtv = get_circle_rect_mtv(position, 0.5, rect);
            position = Vector2Add(position, mtv);
        }
    }
}

// -----------------------------------------------------------------------
// draw
void Game::draw() {
    this->renderer.begin_drawing();

    this->renderer.set_camera(this->camera.target, this->camera.view_width);
    this->draw_renderables();
    this->draw_grid_items();
    this->draw_active_item_ghost();

    this->renderer.set_screen_camera();
    this->is_ui_interacted = false;
    this->update_and_draw_quickbar();

    this->renderer.end_drawing();
}

void Game::draw_renderables() {
    auto view = this->registry.view<Renderable_C, Position_C>();
    for (auto entity : view) {
        auto [renderable, position] = view.get(entity);
        renderer.draw_renderable(renderable, position);
    }
}

void Game::draw_grid_items() {
    for (Cell &cell : this->cells) {
        if (cell.item.type == ItemType::NONE) continue;

        float base_scale = 1.0 / cell.item.sprite.src.width;
        Renderable renderable = Renderable::create_sprite(
            cell.item.sprite, Pivot::CENTER_CENTER, base_scale
        );
        this->renderer.draw_renderable(renderable, cell.get_position());
    }
}

void Game::draw_active_item_ghost() {
    Vector2 position = this->mouse_position_grid;
    Item *item = this->get_active_item();

    if (this->is_ui_interacted) return;
    if (!item) return;

    Color color;
    if (this->can_place_item(item, position)) {
        color = ColorAlpha(GREEN, 0.3);
    } else {
        color = ColorAlpha(RED, 0.3);
    }

    float base_scale = 1.0 / item->sprite.src.width;

    Sprite sprite = this->suggest_item_sprite(
        position, item->type, this->resources.sprite_sheet
    );
    Renderable renderable = Renderable::create_sprite(
        sprite, Pivot::CENTER_CENTER, base_scale, 1.0, color
    );
    this->renderer.draw_renderable(renderable, position);
}

void Game::update_and_draw_quickbar() {
    Vector2 screen_size = this->renderer.get_screen_size();
    static float item_size = 60.0;
    static float pad = 15.0;
    static Color pane_color{20, 20, 20, 255};

    int n_items = this->items.size();
    float pane_width = item_size * n_items + 3.0 * pad;
    float pane_height = item_size + 2.0 * pad;
    float x = 0.5 * (screen_size.x - pane_width);
    float y = screen_size.y - 0.5 * pane_height;

    Vector2 position = {x, y};
    Renderable renderable = Renderable::create_rectangle(
        Pivot::LEFT_CENTER, pane_width, pane_height, 1.0, pane_color
    );
    this->renderer.draw_renderable(renderable, position);
    this->is_ui_interacted |= renderable.check_collision_with_point(
        position, this->mouse_position_screen
    );

    position.x += pad;
    for (uint32_t i = 0; i < this->items.size(); ++i) {
        Item &item = this->items[i];

        position.x += 0.5 * item_size;
        float base_scale = item_size / item.sprite.src.width;
        Renderable renderable = Renderable::create_sprite(
            item.sprite, Pivot::CENTER_CENTER, base_scale
        );

        bool is_hovered = renderable.check_collision_with_point(
            position, this->mouse_position_screen
        );
        if (!is_hovered) {
            renderable.scale = 1.0;
        } else if (this->is_lmb_released) {
            this->set_active_item(i);
            renderable.scale = 1.1;
        } else if (this->is_lmb_down) {
            renderable.scale = 0.9;
        } else {
            renderable.scale = 1.1;
        }

        this->renderer.draw_renderable(renderable, position);
        position.x += pad + 0.5 * item_size;
    }
}

// -----------------------------------------------------------------------
// other
Item *Game::get_active_item() {
    int idx = this->active_item_idx;
    if (idx >= 0 && (uint32_t)idx < this->items.size()) {
        return &this->items[idx];
    }

    return nullptr;
}

void Game::set_active_item(int item_idx) {
    this->active_item_idx = item_idx;
}

void Game::clear_active_item() {
    this->active_item_idx = -1;
}

Rectangle Game::get_bound_rect() {
    static Rectangle rect = {
        .x = -(float)N_COLS / 2.0f,
        .y = -(float)N_ROWS / 2.0f,
        .width = (float)N_COLS,
        .height = (float)N_ROWS
    };
    return rect;
}

Cell *Game::get_cell(Vector2 position) {
    Rectangle bound_rect = this->get_bound_rect();
    if (!CheckCollisionPointRec(position, bound_rect)) return nullptr;

    uint32_t col = position.x - bound_rect.x;
    uint32_t row = position.y - bound_rect.y;
    uint32_t idx = row * N_ROWS + col;

    if (idx >= N_ROWS * N_COLS) return nullptr;
    return &this->cells[idx];
}

CellNeighbors Game::get_cell_neighbors(Vector2 position) {
    CellNeighbors nb;

    Cell *mid = this->get_cell(position);
    if (mid) {
        Vector2 p = mid->get_position();
        nb.cells[0] = this->get_cell({p.x - 1.0f, p.y});
        nb.cells[1] = this->get_cell({p.x, p.y - 1.0f});
        nb.cells[2] = this->get_cell({p.x + 1.0f, p.y});
        nb.cells[3] = this->get_cell({p.x, p.y + 1.0f});
    }

    return nb;
}

WallType Game::get_wall_type(Vector2 position) {
    Cell *mid = this->get_cell(position);
    if (!mid) return WallType::NONE;

    if (!mid->item.is_wall_or_door()) return WallType::NONE;

    CellNeighbors nb = this->get_cell_neighbors(position);
    bool walls[4];
    for (int i = 0; i < 4; ++i) {
        walls[i] = nb.cells[i] && nb.cells[i]->item.is_wall_or_door();
    }

    bool is_horizontal = walls[0] || walls[2];
    bool is_vertical = walls[1] || walls[3];

    if (is_horizontal && !is_vertical) return WallType::HORIZONTAL;
    if (!is_horizontal && is_vertical) return WallType::VERTICAL;
    return WallType::NONE;
}

Vector2 Game::round_position(Vector2 position) {
    float x = std::floor(position.x) + 0.5;
    float y = std::floor(position.y) + 0.5;
    return {x, y};
}

bool Game::can_place_item(const Item *item, Vector2 position) {
    static float build_radius = 4.0;

    Cell *cell = this->get_cell(position);
    if (!item) return false;
    if (!cell) return false;

    auto player_position = registry.get<Position_C>(this->player);
    if (Vector2Distance(position, player_position) > build_radius) {
        return false;
    }

    switch (item->type) {
        case ItemType::WALL: {
            if (cell->item.type != ItemType::NONE) return false;
            CellNeighbors nb = this->get_cell_neighbors(position);
            for (int i = 0; i < 4; ++i) {
                Cell *cell = nb.cells[i];
                if (!cell) continue;
                bool is_door = cell->item.is_door();
                auto wall_type = this->get_wall_type(cell->get_position());
                if (i % 2 == 0) {
                    if (is_door && wall_type == WallType::VERTICAL) return false;
                } else {
                    if (is_door && wall_type == WallType::HORIZONTAL) return false;
                }
            }
            return true;
        }
        case ItemType::DOOR: {
            if (cell->item.type != ItemType::WALL) return false;
            if (this->get_wall_type(position) != WallType::NONE) return true;
            return false;
        }
        case ItemType::NONE: return false;
        default: return false;
    }
}

bool Game::place_item(const Item *item, Vector2 position, SpriteSheet &sheet) {
    if (!this->can_place_item(item, position)) return false;

    Cell *cell = this->get_cell(position);
    cell->item = *item;

    cell->item.sprite = this->suggest_item_sprite(
        cell->get_position(), cell->item.type, sheet
    );
    for (Cell *cell : this->get_cell_neighbors(position).cells) {
        cell->item.sprite = this->suggest_item_sprite(
            cell->get_position(), cell->item.type, sheet
        );
    }

    return true;
}

Sprite Game::suggest_item_sprite(
    Vector2 position, ItemType item_type, SpriteSheet &sheet
) {
    CellNeighbors neighbors = this->get_cell_neighbors(position);
    uint8_t idx = 0;

    switch (item_type) {
        case ItemType::WALL:
            idx = sheet_0::wall;
            for (int i = 0; i < 4; ++i) {
                Cell *nb = neighbors.cells[i];
                if (nb && nb->item.is_wall_or_door()) {
                    idx += 1 << (4 - i - 1);
                }
            }
            break;
        case ItemType::DOOR: {
            idx = sheet_0::door;
            WallType wall_type = this->get_wall_type(position);
            if (wall_type == WallType::VERTICAL) idx += 1;
            break;
        }
        case ItemType::NONE: {
            break;
        }
    }

    return sheet.get_sprite(idx);
}

}  // namespace the_shell
