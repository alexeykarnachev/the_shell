#pragma once

#include "camera.hpp"
#include "common.hpp"
#include "grid.hpp"
#include "raylib.h"
#include "sprite.hpp"
#include <vector>

class Renderer {
    private:
        Shader shader;
        int screen_width, screen_height;

    public:
        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        Renderer(int screen_width, int screen_height);
        ~Renderer();

        Vector2 get_screen_size();

        void begin_drawing();
        void end_drawing();

        void draw_primitive(Primitive primitive, Vector2 position, Color color);
        void draw_sprite(Sprite sprite, Vector2 position, Pivot pivot, Color tint, float scale = 1.0);
        void draw_grid(Rectangle bound_rect, float step, Color color = GRAY);
        void draw_grid(Grid& grid, Color color = GRAY);

        void set_camera(GameCamera camera);
        void set_camera(Vector2 position, float view_width);
        void set_screen_camera();
};
