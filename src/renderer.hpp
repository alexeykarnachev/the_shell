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
    float hight;
} RectanglePrimitive;

typedef struct RenderableSprite {
    Sprite sprite;
    Pivot pivot;
    float scale;
} RenderableSprite;

class Renderable {
public:
    RenderableType type; 
    union {
        RenderableCircle circle;
        RenderableRectangle rectangle;
        RenderableSprite sprite;
    };

    static Renderable create_circle(float radius);
    static Renderable create_rectangle(Pivot pivot, float width, float height);
    static Renderable create_sprite(Sprite sprite, Pivot pivot, float scale);

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

        void draw_renderable(Renderable renderable, Vector2 position, Color color);
        void draw_grid(Rectangle bound_rect, float step, Color color = GRAY);
        void draw_grid(Grid& grid, Color color = GRAY);

        void set_camera(GameCamera camera);
        void set_camera(Vector2 position, float view_width);
        void set_screen_camera();
};
