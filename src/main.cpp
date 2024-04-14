#include "camera.hpp"
#include "entt/entity/fwd.hpp"
#include "entt/entt.hpp"
#include "grid.hpp"
#include "raylib.h"
#include "raymath.h"
#include "renderer.hpp"
#include "resources.hpp"
#include "rlgl.h"
#include "sprite.hpp"
#include <array>
#include <stdio.h>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define TARGET_FPS 60

#define GRID_N_ROWS 100
#define GRID_N_COLS 100

typedef uint8_t u8;
typedef uint32_t u32;

typedef Vector2 Position_C;
typedef float MoveSpeed_C;
struct Player_C {};

class Game {
  private:
    Renderer renderer;
    Resources resources;

    entt::registry registry;
    GameCamera camera;
    Grid grid;

    bool show_inventory = false;

    void update() {
        this->update_player();
        this->update_camera();
    }

    void update_player() {
        float dt = GetFrameTime();

        auto player = this->registry.view<Player_C>().front();
        auto &position = this->registry.get<Position_C>(player);
        auto move_speed = this->registry.get<MoveSpeed_C>(player);

        Vector2 step = {0.0, 0.0};
        if (IsKeyDown(KEY_W)) step.y -= 1.0;
        if (IsKeyDown(KEY_S)) step.y += 1.0;
        if (IsKeyDown(KEY_A)) step.x -= 1.0;
        if (IsKeyDown(KEY_D)) step.x += 1.0;

        step = Vector2Scale(Vector2Normalize(step), move_speed * dt);
        position = Vector2Add(position, step);
    }

    void update_camera() {
        auto player = this->registry.view<Player_C>().front();
        this->camera.target = this->registry.get<Position_C>(player);
    }

    void draw() {
        renderer.begin_drawing();
        renderer.set_camera(this->camera);

        this->draw_grid();
        this->draw_walls();
        auto player = this->registry.view<Player_C>().front();
        auto position = this->registry.get<Position_C>(player);
        DrawCircleV(position, 0.5, RED);

        renderer.end_drawing();
    }

    void draw_grid() {
        Rectangle rect = this->grid.get_bound_rect();
        for (float x = rect.x; x <= rect.x + rect.width; x += 1.0) {
            DrawLine(x, rect.y, x, rect.y + rect.height, GRAY);
        }
        for (float y = rect.y; y <= rect.y + rect.height; y += 1.0) {
            DrawLine(rect.x, y, rect.x + rect.width, y, GRAY);
        }
    }

    void draw_walls() {
        for (uint32_t i = 0; i < this->grid.get_cells_count(); ++i) {
            Cell cell = this->grid.get_cell(i);
            if (cell == Cell::EMPTY) continue;

            Rectangle rect = this->grid.get_cell_rect(i);
            DrawRectangleRec(rect, RAYWHITE);
        }
    }

  public:
    Game()
        : renderer(Renderer(SCREEN_WIDTH, SCREEN_HEIGHT))
        , resources(Resources())
        , camera(GameCamera(50.0, {0.0, 0.0})) {

        auto player = this->registry.create();
        this->registry.emplace<Player_C>(player);
        this->registry.emplace<Position_C>(player, Position_C{0.0, 0.0});
        this->registry.emplace<MoveSpeed_C>(player, 5.0);
    }

    void run() {
        while (!WindowShouldClose()) {
            this->update();
            this->draw();
        }
    }
};

int main() {
    Game game;
    game.run();
}
