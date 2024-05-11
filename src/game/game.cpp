#include "game.hpp"

#include "raylib.h"
#include "raymath.h"
#include "registry.hpp"
#include <cmath>
#include <cstdint>
#include <cstdio>

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

Game::Game()
    : renderer(1920, 1080)
    , camera(30.0, {0.0, 0.0}) {

    SpriteSheet &sheet = this->resources.sprite_sheet;
    this->inventory.items.emplace_back(ItemType::WALL, sheet.get_sprite(sheet_0::wall));
    this->inventory.items.emplace_back(ItemType::DOOR, sheet.get_sprite(sheet_0::door));
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
    this->is_ui_interacted = false;
    this->update_and_draw_quickbar();

    this->renderer.end_drawing();
}

void Game::update_input() {
    Vector2 screen_size = this->renderer.get_screen_size();
    Vector2 mouse_position_screen = GetMousePosition();

    this->mouse_position_screen = mouse_position_screen;
    this->mouse_position_world = get_mouse_position_world(
        screen_size, mouse_position_screen, this->camera
    );
    this->mouse_position_grid = this->grid.round_position(this->mouse_position_world);

    this->is_lmb_pressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    this->is_lmb_released = IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
    this->is_lmb_down = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
}

void Game::update_active_item_placement() {
    Vector2 position = this->mouse_position_grid;
    Item *item = this->inventory.get_active_item();
    Cell *cell = this->grid.get_cell(position);

    if (!item) return;
    if (!cell) return;
    if (this->is_ui_interacted) return;
    if (!this->is_lmb_down) return;
    if (!this->grid.can_place_item(item, position)) return;

    grid.place_item(item, position, this->resources.sprite_sheet);
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
    Vector2 position = this->mouse_position_grid;
    Item *item = this->inventory.get_active_item();

    if (this->is_ui_interacted) return;
    if (!item) return;

    Color color;
    if (this->grid.can_place_item(item, position)) {
        color = ColorAlpha(GREEN, 0.3);
    } else {
        color = ColorAlpha(RED, 0.3);
    }

    float base_scale = 1.0 / item->sprite.src.width;

    Sprite sprite = this->grid.suggest_item_sprite(
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
    this->is_ui_interacted |= renderable.check_collision_with_point(
        position, this->mouse_position_screen
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
            position, this->mouse_position_screen
        );
        if (!is_hovered) {
            renderable.scale = 1.0;
        } else if (this->is_lmb_released) {
            inventory.set_active_item(i);
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
