#include "shapes_2d.h"
#include "Renderer2d.h"
#include "mathlib.h"

RectangularGradient::RectangularGradient(Renderer2d& renderer,
                                         Vector4 top,
                                         Vector4 bottom,
                                         f32 width,
                                         f32 height,
                                         Vector2 position)
    : renderer{renderer},
      top{top},
      bottom{bottom},
      width{width},
      height{height}
{
    Vertex2D vertices[6];
    vertices[0].position = position;
    vertices[0].color = bottom;
    vertices[1].position = { position.x + width, position.y };
    vertices[1].color = bottom;
    vertices[2].position = { position.x, position.y + height };
    vertices[2].color = top;
    vertices[3] = vertices[2];
    vertices[4] = vertices[1];
    vertices[5].position = { position.x + width, position.y + height };
    vertices[5].color = top;
    mesh.load(vertices, 6, &renderer);
}

RectangularGradient::~RectangularGradient()
{
    mesh.unload();
}

void RectangularGradient::render()
{
    mesh.render(&renderer);
}
