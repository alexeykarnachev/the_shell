#include "game.hpp"

Input::Input() = default;

GameCamera::GameCamera(float view_width, Vector2 target)
    : view_width(view_width)
    , target(target) {}

Item::Item(Sprite sprite)
    : sprite(sprite) {}

Inventory::Inventory() = default;

Game::Game()
    : renderer(1920, 1080)
    , camera(30.0, {0.0, 0.0}) {

    SpriteSheet &sheet = this->resources.sprite_sheet;
    this->inventory.items.emplace_back(Item(sheet.get_sprite(1)));
    this->inventory.items.emplace_back(Item(sheet.get_sprite(17)));
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
