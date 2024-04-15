#include "entt/entity/fwd.hpp"
#include "entt/entt.hpp"
#include "raylib.h"
#include "raymath.h"
#include "renderer.hpp"
#include "resources.hpp"
#include <cstdint>

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
typedef Primitive DrawPrimitive_C;
typedef Color Tint_C;

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

        renderer.draw_grid(this->grid.get_bound_rect(), 1.0);

        auto view = registry.view<Position_C, DrawPrimitive_C>();
        for (auto entity : view) {
            auto [position, primitive] = view.get(entity);
            auto tint_p = registry.try_get<Tint_C>(entity);
            Color color = tint_p ? *tint_p : RED;
            renderer.draw_primitive(primitive, position, color);
        }

        renderer.end_drawing();
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
        this->registry.emplace<DrawPrimitive_C>(
            player,
            DrawPrimitive_C{.type = PrimitiveType::CIRCLE, .circle = {.radius = 0.5}}
        );
        this->registry.emplace<Tint_C>(player, GREEN);
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
