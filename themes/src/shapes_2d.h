#ifndef SHAPES_2D
#define SHAPES_2D

#include "mathlib.h"
#include "renderer_2d.h"
#include "types.h"

class RectangularGradient
{
 public:
    RectangularGradient(Renderer2d& renderer,
                        Vector4 top,
                        Vector4 bottom,
                        f32 width,
                        f32 height,
                        Vector2 position);
    ~RectangularGradient();
    void render();

 private:
    Renderer2d& renderer;
    Vector4 top;
    Vector4 bottom;
    f32 width;
    f32 height;
    Mesh2D mesh;
};


/// Supports full and partial circles
class Circleish
{
public:
    Circleish(
        Renderer2d& renderer,
        f32 radius,
        Vector4 color,
        i32 segments,
        i32 segment_start = 0,
        i32 segment_end = 0);
    ~Circleish();
    void render();

    Renderer2d& renderer;
    Mesh2D mesh;
};

#endif
