#include "game.hpp"
#include "raylib.h"
#include "raymath.h"

Vector2 get_mouse_position_world(
    Vector2 screen_size, Vector2 mouse_position_screen, GameCamera &camera
);

void Game::update_input() {
    Vector2 screen_size = this->renderer.get_screen_size();
    Vector2 mouse_position_screen = GetMousePosition();

    this->input.mouse_position_screen = mouse_position_screen;
    this->input.mouse_position_world = get_mouse_position_world(
        screen_size, mouse_position_screen, this->camera
    );

    this->input.is_lmb_pressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    this->input.is_lmb_released = IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
    this->input.is_lmb_down = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
}

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
