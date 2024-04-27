#include "renderer.hpp"

#include "raylib.h"
#include "rlgl.h"

#define TARGET_FPS 60

Renderable Renderable::create_circle(float radius, float scale, Color color) {
    return {
        .type = RenderableType::CIRCLE,
        .circle = {radius},
        .scale = scale,
        .color = color};
}

Renderable Renderable::create_rectangle(
    Pivot pivot, float width, float height, float scale, Color color
) {
    return {
        .type = RenderableType::RECTANGLE,
        .rectangle = {pivot, width, height},
        .scale = scale,
        .color = color};
}

Renderable Renderable::create_sprite(
    Sprite sprite, Pivot pivot, float base_scale, float scale, Color color
) {
    return {
        .type = RenderableType::SPRITE,
        .sprite = {sprite, pivot, base_scale},
        .scale = scale,
        .color = color};
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
                this->rectangle.width * this->scale,
                this->rectangle.height * this->scale
            );
            is_hit = CheckCollisionPointRec(point_position, rect);
            break;
        }
        case RenderableType::CIRCLE: {
            is_hit = CheckCollisionPointCircle(
                point_position, prim_position, this->circle.radius * this->scale
            );
            break;
        }
        case RenderableType::SPRITE: {
            float scale = this->sprite.base_scale * this->scale;
            Rectangle rect = get_rect_from_pivot(
                prim_position,
                this->sprite.pivot,
                this->sprite.sprite.src.width * scale,
                this->sprite.sprite.src.height * scale
            );
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

void Renderer::draw_renderable(Renderable renderable, Vector2 position) {
    switch (renderable.type) {
        case RenderableType::CIRCLE:
            DrawCircleV(
                position, renderable.circle.radius * renderable.scale, renderable.color
            );
            return;
        case RenderableType::RECTANGLE: {
            Rectangle rect = get_rect_from_pivot(
                position,
                renderable.rectangle.pivot,
                renderable.rectangle.width * renderable.scale,
                renderable.rectangle.height * renderable.scale
            );
            DrawRectangleRec(rect, renderable.color);
            return;
        }
        case RenderableType::SPRITE: {
            float scale = renderable.sprite.base_scale * renderable.scale;
            Rectangle dst = get_rect_from_pivot(
                position,
                renderable.sprite.pivot,
                renderable.sprite.sprite.src.width * scale,
                renderable.sprite.sprite.src.height * scale
            );
            DrawTexturePro(
                renderable.sprite.sprite.texture,
                renderable.sprite.sprite.src,
                dst,
                {0.0, 0.0},
                0.0,
                renderable.color
            );
            return;
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
