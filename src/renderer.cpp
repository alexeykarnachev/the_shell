#include "renderer.hpp"

#include "camera.hpp"
#include "raylib.h"
#include "rlgl.h"
#include <string>

#define TARGET_FPS 60

Renderable Renderable::create_circle(float radius) {
    return {.type = RenderableType::CIRCLE, .circle = {radius}};
}

Renderable Renderable::create_rectangle(Pivot pivot, float width, float height) {
    return {.type = RenderableType::RECTANGLE, .rectangle = {pivot, width, height}};
}

Renderable Renderable::create_sprite(Sprite sprite, Pivot pivot, float scale) {
    return {.type = RenderableType::SPRITE, .sprite = {sprite, pivot, scale}};
}

bool Renderable::check_collision_with_point(
    Vector2 prim_position, Vector2 point_position
) {
    bool is_hit = false;
    switch (this->type) {
        case RenderableType::RECTANGLE: {
            Rectangle rect = get_rect_from_pivot(
                prim_position,
                this->rectangle.pivot,
                this->rectangle.width,
                this->rectangle.hight
            );
            is_hit = CheckCollisionPointRec(point_position, rect);
            break;
        }
        case RenderableType::CIRCLE: {
            is_hit = CheckCollisionPointCircle(
                point_position, prim_position, this->circle.radius
            );
            break;
        }
        case RenderableType::SPRITE: {
            Sprite sprite = this->sprite.sprite;
            Pivot pivot = this->sprite.pivot;
            float scale = this->sprite.scale;
            float width = sprite.src.width * scale;
            float height = sprite.src.height * scale;
            Rectangle rect = get_rect_from_pivot(prim_position, pivot, width, height);
            is_hit = CheckCollisionPointRec(point_position, rect);
            break;
        }
    }

    return is_hit;
}

Renderer::Renderer(int screen_width, int screen_height) {
    this->screen_width = screen_width;
    this->screen_height = screen_height;

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screen_width, screen_height, "The Shell");
    SetTargetFPS(TARGET_FPS);
    rlDisableBackfaceCulling();

    this->shader = LoadShader(
        "resources/shaders/shader.vert", "resources/shaders/shader.frag"
    );
}

Renderer::~Renderer() {
    UnloadShader(shader);
    CloseWindow();
}

Vector2 Renderer::get_screen_size() {
    return {(float)this->screen_width, (float)this->screen_height};
}

void Renderer::draw_renderable(Renderable renderable, Vector2 position, Color color) {
    switch (renderable.type) {
        case RenderableType::CIRCLE:
            DrawCircleV(position, renderable.circle.radius, color);
            break;
        case RenderableType::RECTANGLE: {
            Rectangle rect = get_rect_from_pivot(
                position,
                renderable.rectangle.pivot,
                renderable.rectangle.width,
                renderable.rectangle.hight
            );
            DrawRectangleRec(rect, color);
            break;
        }
        case RenderableType::SPRITE: {
            Sprite sprite = renderable.sprite.sprite;
            Pivot pivot = renderable.sprite.pivot;
            float scale = renderable.sprite.scale;
            float width = sprite.src.width * scale;
            float height = sprite.src.height * scale;
            Rectangle dst = get_rect_from_pivot(position, pivot, width, height);
            DrawTexturePro(sprite.texture, sprite.src, dst, {0.0, 0.0}, 0.0, color);
            break;
        }
    }
}

void Renderer::draw_grid(Rectangle bound_rect, float step, Color color) {
    for (float x = bound_rect.x; x <= bound_rect.x + bound_rect.width; x += step) {
        DrawLine(x, bound_rect.y, x, bound_rect.y + bound_rect.height, GRAY);
    }
    for (float y = bound_rect.y; y <= bound_rect.y + bound_rect.height; y += step) {
        DrawLine(bound_rect.x, y, bound_rect.x + bound_rect.width, y, GRAY);
    }
}

void Renderer::draw_grid(Grid &grid, Color color) {
    this->draw_grid(grid.get_bound_rect(), 1.0, color);
}

void Renderer::begin_drawing() {
    BeginDrawing();
    ClearBackground(BLACK);
    BeginShaderMode(shader);
}

void Renderer::end_drawing() {
    EndShaderMode();
    DrawFPS(0, 0);
    EndDrawing();
}

void Renderer::set_camera(GameCamera camera) {
    this->set_camera(camera.target, camera.view_width);
}

void Renderer::set_camera(Vector2 position, float view_width) {
    rlDrawRenderBatchActive();

    int position_loc = GetShaderLocation(shader, "camera.position");
    int view_width_loc = GetShaderLocation(shader, "camera.view_width");
    int aspect_loc = GetShaderLocation(shader, "camera.aspect");

    float aspect = (float)screen_width / screen_height;

    SetShaderValue(shader, position_loc, &position, SHADER_UNIFORM_VEC2);
    SetShaderValue(shader, view_width_loc, &view_width, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader, aspect_loc, &aspect, SHADER_UNIFORM_FLOAT);
}

void Renderer::set_screen_camera() {
    Vector2 position = {screen_width / 2.0f, screen_height / 2.0f};
    set_camera(position, screen_width);
}
