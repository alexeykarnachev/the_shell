#include "entt/entity/fwd.hpp"
#include "entt/entt.hpp"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <array>
#include <stdio.h>
#include <tuple>
#include <vector>

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

enum UIElementState {
    COLD,
    HOT,
    ACTIVE,
};

enum UIElementType {
    PANE,
    BUTTON,
};

struct UIElement {
    UIElementState state;
    UIElementType type;
    Rectangle rect;
};

typedef Vector2 Position_C;
typedef float MoveSpeed_C;
struct Player_C {};

class Game {
  private:
    entt::registry registry;
    Grid grid;
    GameCamera camera;
    Shader world_shader;
    Shader ui_shader;

    std::vector<UIElement> ui_elements;

    void update() {
        this->update_ui();
        this->update_player_input();
        this->update_camera();
    }

    void update_ui() {
        this->ui_elements.clear();

        // quick bar
        {
            int n_buttons = 10;
            float padding = 10.0;
            float gap = 10.0;

            float pane_width = 800.0;
            float button_width = (pane_width - 2.0 * padding - (n_buttons - 1) * gap)
                                 / n_buttons;
            float button_height = button_width;
            float pane_height = button_height + 2.0 * padding;

            // pane
            float pane_x = (SCREEN_WIDTH - pane_width) / 2.0;
            float pane_y = SCREEN_HEIGHT - pane_height;
            Rectangle rect = {
                .x = pane_x, .y = pane_y, .width = pane_width, .height = pane_height};
            this->ui_elements.emplace_back(UIElement{
                .state = UIElementState::COLD, .type = UIElementType::PANE, .rect = rect}
            );

            // buttons
            float button_x = pane_x + padding;
            float button_y = pane_y + padding;
            for (int i = 0; i < n_buttons; ++i) {
                rect = {
                    .x = button_x,
                    .y = button_y,
                    .width = button_width,
                    .height = button_height};
                UIElementState state = UIElementState::COLD;
                bool is_hover = CheckCollisionPointRec(GetMousePosition(), rect);
                if (is_hover && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                    state = UIElementState::ACTIVE;
                } else if (is_hover) {
                    state = UIElementState::HOT;
                }
                this->ui_elements.emplace_back(UIElement{
                    .state = state, .type = UIElementType::BUTTON, .rect = rect});

                button_x += button_width + gap;
            }
        }
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

    void update_camera() {
        auto player = this->registry.view<Player_C>().front();
        this->camera.target = this->registry.get<Position_C>(player);
    }

    void draw() {
        BeginDrawing();
        ClearBackground(BLANK);

        BeginShaderMode(this->world_shader);
        float aspect = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
        SetShaderValue(
            this->world_shader,
            GetShaderLocation(this->world_shader, "camera.position"),
            &this->camera.target,
            SHADER_UNIFORM_VEC2
        );
        SetShaderValue(
            this->world_shader,
            GetShaderLocation(this->world_shader, "camera.view_width"),
            &this->camera.view_width,
            SHADER_UNIFORM_FLOAT
        );
        SetShaderValue(
            this->world_shader,
            GetShaderLocation(this->world_shader, "camera.aspect"),
            &aspect,
            SHADER_UNIFORM_FLOAT
        );
        this->draw_grid();
        this->draw_walls();
        auto player = this->registry.view<Player_C>().front();
        auto position = this->registry.get<Position_C>(player);
        DrawCircleV(position, 0.5, RED);
        EndShaderMode();

        BeginShaderMode(this->ui_shader);
        Vector2 screen_size = {SCREEN_WIDTH, SCREEN_HEIGHT};
        SetShaderValue(
            this->ui_shader,
            GetShaderLocation(this->ui_shader, "screen_size"),
            &screen_size,
            SHADER_UNIFORM_VEC2
        );
        this->draw_ui();
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

    void draw_ui() {
        for (UIElement &element : this->ui_elements) {
            switch (element.type) {
                case UIElementType::PANE:
                    DrawRectangleRec(element.rect, {50, 50, 50, 255});
                    break;
                case UIElementType::BUTTON: {
                    Color color;
                    switch (element.state) {
                        case UIElementState::COLD: color = {100, 100, 100, 255}; break;
                        case UIElementState::HOT: color = {130, 130, 130, 255}; break;
                        case UIElementState::ACTIVE: color = {160, 160, 160, 255}; break;
                    }

                    DrawRectangleRec(element.rect, color);
                    break;
                }
            }
        }
    }

  public:
    Game() {
        SetConfigFlags(FLAG_MSAA_4X_HINT);
        InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "The Shell");
        SetTargetFPS(TARGET_FPS);
        rlDisableBackfaceCulling();

        this->world_shader = LoadShader(
            "resources/shaders/world.vert", "resources/shaders/color.frag"
        );
        this->ui_shader = LoadShader(
            "resources/shaders/screen.vert", "resources/shaders/ui.frag"
        );

        auto player = this->registry.create();
        this->registry.emplace<Player_C>(player);
        this->registry.emplace<Position_C>(player, Position_C{0.0, 0.0});
        this->registry.emplace<MoveSpeed_C>(player, 5.0);
    }

    ~Game() {
        UnloadShader(this->world_shader);
        UnloadShader(this->ui_shader);
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
