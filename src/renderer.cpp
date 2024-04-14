#include "renderer.hpp"

#include "camera.hpp"
#include "raylib.h"
#include "rlgl.h"
#include <string>

#define TARGET_FPS 60

Pivot::Pivot(PivotType type, Vector2 position)
    : type(type)
    , position(position) {}

Rectangle Pivot::get_rect(float width, float height) {
    Vector2 offset;
    switch (type) {
        case PivotType::CENTER_BOTTOM: offset = {-0.5f * width, -height}; break;
        case PivotType::CENTER_TOP: offset = {-0.5f * width, 0.0}; break;
        case PivotType::LEFT_CENTER: offset = {0.0, -0.5f * height}; break;
        case PivotType::RIGHT_CENTER: offset = {-width, -0.5f * height}; break;
        case PivotType::CENTER_CENTER: offset = {-0.5f * width, -0.5f * height}; break;
    }

    return {
        .x = position.x + offset.x,
        .y = position.y + offset.y,
        .width = width,
        .height = height};
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

void Renderer::draw(Sprite sprite, Pivot pivot, Color tint, float scale) {
    Rectangle dst = pivot.get_rect(sprite.src.width * scale, sprite.src.height * scale);
    DrawTexturePro(sprite.texture, sprite.src, dst, {0.0, 0.0}, 0.0, tint);
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
    set_camera(camera.target, camera.view_width);
}

void Renderer::set_camera(Vector2 position, float view_width) {
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
