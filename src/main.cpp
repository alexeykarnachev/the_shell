#include "entt/entity/fwd.hpp"
#include "entt/entt.hpp"
#include "raylib.h"
#include "raymath.h"
#include <array>
#include <stdio.h>
#include <tuple>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define TARGET_FPS 60

#define GRID_N_ROWS 100
#define GRID_N_COLS 100

typedef uint8_t u8;
typedef uint32_t u32;

struct GameCamera {
    Vector2 target = {0.0, 0.0};
    float view_width = 50.0;
};

enum class Cell : u8 {
    NONE = 0,
    EMPTY = 1 << 0,
    WALL = 1 << 1,
};

class Grid {
  private:
    std::array<Cell, GRID_N_ROWS * GRID_N_COLS> cells;

    std::tuple<bool, u32> get_cell_idx_at(Vector2 position) {
        Rectangle bound_rect = this->get_bound_rect();
        bool is_valid = CheckCollisionPointRec(position, bound_rect);
        u32 col = position.x - bound_rect.x;
        u32 row = position.y - bound_rect.y;
        return {is_valid, row * this->n_rows + col};
    }

  public:
    const u32 n_rows = GRID_N_ROWS;
    const u32 n_cols = GRID_N_COLS;

    Grid() {
        this->cells.fill(Cell::EMPTY);
    }

    Rectangle get_bound_rect() {
        static Rectangle rect = {
            .x = -(float)this->n_cols / 2.0f,
            .y = -(float)this->n_rows / 2.0f,
            .width = (float)this->n_cols,
            .height = (float)this->n_rows};
        return rect;
    }

    Rectangle get_cell_rect(u32 idx) {
        Rectangle bound_rect = this->get_bound_rect();
        u32 row = idx / this->n_cols;
        u32 col = idx % this->n_cols;
        float x = bound_rect.x + (float)col;
        float y = bound_rect.y + (float)row;
        return {.x = x, .y = y, .width = 1.0, .height = 1.0};
    }

    Cell get_cell(u32 idx) {
        if (idx >= this->n_rows * this->n_cols) return Cell::NONE;
        return this->cells[idx];
    }

    Cell get_cell(Vector2 position) {
        auto [is_valid, idx] = this->get_cell_idx_at(position);
        if (!is_valid) return Cell::NONE;
        return this->cells[idx];
    }

    void set_cell(u32 idx, Cell cell) {
        if (idx >= this->n_rows * this->n_cols) return;
        this->cells[idx] = cell;
    }

    void set_cell(Vector2 position, Cell cell) {
        auto [is_valid, idx] = this->get_cell_idx_at(position);
        if (!is_valid) return;
        this->cells[idx] = cell;
    }
};

typedef Vector2 Position_C;
typedef float MoveSpeed_C;
struct Player_C {};

class Game {
  private:
    entt::registry registry;
    Grid grid;
    GameCamera camera;
    Shader shader;

    void update() {
        this->update_player_input();
        this->update_camera();
    }

    void update_camera() {
        auto player = this->registry.view<Player_C>().front();
        this->camera.target = this->registry.get<Position_C>(player);
    }

    void update_player_input() {
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

    void draw() {
        float aspect = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
        SetShaderValue(
            this->shader,
            GetShaderLocation(this->shader, "camera.position"),
            &this->camera.target,
            SHADER_UNIFORM_VEC2
        );
        SetShaderValue(
            this->shader,
            GetShaderLocation(this->shader, "camera.view_width"),
            &this->camera.view_width,
            SHADER_UNIFORM_FLOAT
        );
        SetShaderValue(
            this->shader,
            GetShaderLocation(this->shader, "camera.aspect"),
            &aspect,
            SHADER_UNIFORM_FLOAT
        );

        BeginDrawing();
        ClearBackground(BLANK);

        BeginShaderMode(this->shader);
        this->draw_grid();
        this->draw_walls();
        auto player = this->registry.view<Player_C>().front();
        auto position = this->registry.get<Position_C>(player);
        DrawCircleV(position, 0.5, RED);

        EndShaderMode();

        DrawFPS(0, 0);

        EndDrawing();
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
        int n_cells = this->grid.n_rows * this->grid.n_cols;
        for (int i = 0; i < n_cells; ++i) {
            Cell cell = this->grid.get_cell(i);
            if (cell == Cell::EMPTY) continue;

            Rectangle rect = this->grid.get_cell_rect(i);
            DrawRectangleRec(rect, RAYWHITE);
        }
    }

  public:
    Game() {
        SetConfigFlags(FLAG_MSAA_4X_HINT);
        InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "The Shell");
        SetTargetFPS(TARGET_FPS);

        this->shader = LoadShader(
            "resources/shaders/base.vert", "resources/shaders/color.frag"
        );

        auto player = this->registry.create();
        this->registry.emplace<Player_C>(player);
        this->registry.emplace<Position_C>(player, Position_C{0.0, 0.0});
        this->registry.emplace<MoveSpeed_C>(player, 5.0);
    }

    ~Game() {
        UnloadShader(this->shader);
        CloseWindow();
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
