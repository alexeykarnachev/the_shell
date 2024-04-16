#pragma once

#include "camera.hpp"
#include "common.hpp"
#include "grid.hpp"
#include "raylib.h"
#include "sprite.hpp"
#include <vector>

enum class RenderableType {
    CIRCLE, 
    RECTANGLE,
    SPRITE,
};

typedef struct RenderableCircle {
    float radius;
} Circle;

typedef struct RenderableRectangle {
    Pivot pivot;
    float width;
    float height;
} RectanglePrimitive;

typedef struct RenderableSprite {
    Sprite sprite;
    Pivot pivot;
    float base_scale;
} RenderableSprite;

class Renderable {
public:
    RenderableType type;
    union {
        RenderableCircle circle;
        RenderableRectangle rectangle;
        RenderableSprite sprite;
    };
    float scale;
    Color color;

    static Renderable create_circle(float radius, float scale = 1.0, Color color = RED);
    static Renderable create_rectangle(Pivot pivot, float width, float height, float scale = 1.0, Color color = RED);
    static Renderable create_sprite(Sprite sprite, Pivot pivot, float base_scale = 1.0, float scale = 1.0, Color color = BLANK);

    bool check_collision_with_point(Vector2 prim_position, Vector2 point_position);
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

        Vector2 get_screen_size();

        void begin_drawing();
        void end_drawing();

        void draw_renderable(Renderable renderable, Vector2 position);

        void draw_grid(Rectangle bound_rect, float step, Color color = GRAY);
        void draw_grid(Grid& grid, Color color = GRAY);

        void set_camera(GameCamera camera);
        void set_camera(Vector2 position, float view_width);
        void set_screen_camera();
};
