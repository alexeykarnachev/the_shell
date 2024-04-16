#include "common.hpp"
#include "entt/entity/fwd.hpp"
#include "entt/entt.hpp"
#include "raylib.h"
#include "raymath.h"
#include "renderer.hpp"
#include "resources.hpp"
#include <cstdint>
#include <tuple>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define TARGET_FPS 60

#define GRID_N_ROWS 100
#define GRID_N_COLS 100

static const Color PANE_COLOR = {20, 20, 20, 255};

typedef uint8_t u8;
typedef uint32_t u32;

typedef Vector2 WorldPosition_C;
typedef Vector2 ScreenPosition_C;
typedef float MoveSpeed_C;
struct Player_C {};
struct Button_C {
    bool is_disabled = false;
    bool is_down = false;
};
struct Renderable_C {
    Renderable rend;
    int z;
};

class Game {
  private:
    Renderer renderer;
    Resources resources;

    entt::registry registry;
    GameCamera camera;
    Grid grid;

    void create_quick_bar() {
        Vector2 screen_size = renderer.get_screen_size();

        int n_items = 10;
        float pane_width = 800.0;
        float padding = 5.0;
        float gap = 5.0;
        float item_width = (pane_width - 2.0 * padding - (n_items - 1) * gap) / n_items;
        float item_height = item_width;
        float pane_height = item_height + 2.0 * padding;

        float x = 0.5 * (screen_size.x - pane_width);
        float y = screen_size.y - 0.5 * pane_height;

        auto pane = registry.create();
        registry.emplace<ScreenPosition_C>(pane, x, y);
        registry.emplace<Renderable_C>(
            pane,
            Renderable_C{
                Renderable::create_rectangle(
                    Pivot::LEFT_CENTER, pane_width, pane_height, 1.0, PANE_COLOR
                ),
                0}
        );

        x += padding + 0.5 * item_width;
        for (int i = 0; i < n_items; ++i) {
            auto item = registry.create();
            registry.emplace<ScreenPosition_C>(item, x, y);
            registry.emplace<Renderable_C>(
                item,
                Renderable_C{
                    Renderable::create_sprite(
                        this->resources.sprite_sheet.get_sprite(i),
                        Pivot::CENTER_CENTER,
                        2.0
                    ),
                    1}
            );
            registry.emplace<Button_C>(item);

            x += item_width + gap;
        }
    }

    void update() {
        this->update_buttons();
        this->update_player();
        this->update_camera();
    }

    void update_buttons() {
        Vector2 cursor = GetMousePosition();

        bool is_lmb_down = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
        bool is_lmb_released = IsMouseButtonReleased(MOUSE_LEFT_BUTTON);

        auto view = registry.view<Renderable_C, ScreenPosition_C, Button_C>();
        for (auto entity : view) {
            auto [renderable, position, button] = view.get(entity);
            bool is_hovered = renderable.rend.check_collision_with_point(
                position, cursor
            );

            if (!is_hovered) {
                button.is_down = false;
                renderable.rend.scale = 1.0;
            } else if (is_lmb_released) {
                button.is_down = false;
                renderable.rend.scale = 1.0;
                printf("CLICK!\n");
            } else if (is_lmb_down) {
                button.is_down = true;
                renderable.rend.scale = 0.9;
            } else {
                button.is_down = false;
                renderable.rend.scale = 1.1;
            }
        }
    }

    void update_player() {
        float dt = GetFrameTime();

        auto player = registry.view<Player_C>().front();
        auto &position = registry.get<WorldPosition_C>(player);
        auto move_speed = registry.get<MoveSpeed_C>(player);

        Vector2 step = {0.0, 0.0};
        if (IsKeyDown(KEY_W)) step.y -= 1.0;
        if (IsKeyDown(KEY_S)) step.y += 1.0;
        if (IsKeyDown(KEY_A)) step.x -= 1.0;
        if (IsKeyDown(KEY_D)) step.x += 1.0;

        step = Vector2Scale(Vector2Normalize(step), move_speed * dt);
        position = Vector2Add(position, step);
    }

    void update_camera() {
        auto player = registry.view<Player_C>().front();
        this->camera.target = registry.get<WorldPosition_C>(player);
    }

    void draw() {
        registry.sort<Renderable_C>([](const Renderable_C &lhs, const Renderable_C &rhs) {
            return lhs.z < rhs.z;
        });

        renderer.begin_drawing();
        renderer.set_camera(this->camera);

        renderer.draw_grid(this->grid.get_bound_rect(), 1.0);

        {
            auto view = registry.view<Renderable_C, WorldPosition_C>();
            for (auto entity : view) {
                auto [renderable, position] = view.get(entity);
                renderer.draw_renderable(renderable.rend, position);
            }
        }

        renderer.set_screen_camera();
        {
            auto view = registry.view<Renderable_C, ScreenPosition_C>();
            for (auto entity : view) {
                auto [renderable, position] = view.get(entity);
                renderer.draw_renderable(renderable.rend, position);
            }
        }

        renderer.end_drawing();
    }

  public:
    Game()
        : renderer(Renderer(SCREEN_WIDTH, SCREEN_HEIGHT))
        , resources(Resources())
        , camera(GameCamera(50.0, {0.0, 0.0})) {

        // player
        auto player = registry.create();
        registry.emplace<Player_C>(player);
        registry.emplace<WorldPosition_C>(player, WorldPosition_C{0.0, 0.0});
        registry.emplace<MoveSpeed_C>(player, 5.0);
        registry.emplace<Renderable_C>(
            player, Renderable_C{Renderable::create_circle(0.5), 0}
        );

        // ui
        this->create_quick_bar();
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
