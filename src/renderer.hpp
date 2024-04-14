#pragma once

#include "camera.hpp"
#include "raylib.h"
#include "sprite.hpp"
#include <vector>

enum class PivotType {
    CENTER_BOTTOM,
    CENTER_TOP,
    LEFT_CENTER,
    RIGHT_CENTER,
    CENTER_CENTER,
};

class Pivot {
    private:
        PivotType type;
        Vector2 position;

    public:
        Pivot(PivotType type, Vector2 position);
        Rectangle get_rect(float width, float height);
};

class Renderer {
    private:
        Shader shader;
        int screen_width, screen_height;

    public:
        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        Renderer(int screen_width, int screen_height);
        ~Renderer();

        void begin_drawing();
        void end_drawing();

        void draw(Sprite sprite, Pivot pivot, Color tint, float scale = 1.0);

        void set_camera(GameCamera camera);
        void set_camera(Vector2 position, float view_width);
        void set_screen_camera();
};
