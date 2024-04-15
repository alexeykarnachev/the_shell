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

typedef uint8_t u8;
typedef uint32_t u32;

typedef Vector2 WorldPosition_C;
typedef Vector2 ScreenPosition_C;
typedef float MoveSpeed_C;
typedef Color Color_C;
struct Player_C {};
enum Button_C {
    DISABLED,
    COLD,
    HOVERED,
    PRESSED,
};

struct DrawPrimitive_C {
    Primitive prim;
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
        registry.emplace<DrawPrimitive_C>(
            pane,
            DrawPrimitive_C{
                Primitive::create_rect(Pivot::LEFT_CENTER, pane_width, pane_height), 0}
        );
        registry.emplace<Color_C>(pane, DARKGRAY);

        x += padding;
        for (int i = 0; i < n_items; ++i) {
            auto item = registry.create();
            registry.emplace<ScreenPosition_C>(item, x, y);
            registry.emplace<DrawPrimitive_C>(
                item,
                DrawPrimitive_C{
                    Primitive::create_rect(Pivot::LEFT_CENTER, item_width, item_height),
                    1}
            );
            registry.emplace<Button_C>(item, Button_C::COLD);

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

        auto view = registry.view<DrawPrimitive_C, ScreenPosition_C, Button_C>();
        for (auto entity : view) {
            auto [draw_primitive, position, button] = view.get(entity);
            bool is_hovered = draw_primitive.prim.check_collision_with_point(
                position, cursor
            );

            Color color = BLACK;
            if (is_hovered) color = GRAY;

            registry.emplace_or_replace<Color_C>(entity, color);
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
        registry.sort<DrawPrimitive_C>([](const DrawPrimitive_C &lhs,
                                          const DrawPrimitive_C &rhs) {
            return lhs.z < rhs.z;
        });

        renderer.begin_drawing();
        renderer.set_camera(this->camera);

        renderer.draw_grid(this->grid.get_bound_rect(), 1.0);

        {
            auto view = registry.view<DrawPrimitive_C, WorldPosition_C>();
            for (auto entity : view) {
                auto [draw_primitive, position] = view.get(entity);
                Color color = registry.get_or_emplace<Color_C>(entity, RED);
                renderer.draw_primitive(draw_primitive.prim, position, color);
            }
        }

        renderer.set_screen_camera();
        {
            auto view = registry.view<DrawPrimitive_C, ScreenPosition_C>();
            for (auto entity : view) {
                auto [draw_primitive, position] = view.get(entity);
                Color color = registry.get_or_emplace<Color_C>(entity, RED);
                renderer.draw_primitive(draw_primitive.prim, position, color);
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
        registry.emplace<DrawPrimitive_C>(
            player, DrawPrimitive_C{Primitive::create_circle(0.5), 0}
        );
        registry.emplace<Color_C>(player, GREEN);

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
