#include "shapes_2d.h"
#include "Renderer2d.h"
#include "mathlib.h"
#include "list.h"

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


Circleish::Circleish(
    Renderer2d& renderer,
    f32 radius,
    Vector4 color,
    i32 segments,
    i32 segment_start,
    i32 segment_end)
    : renderer{renderer}
{
    matte::List<Vertex2D> vertices;
	matte::List<u32> indices;
	vertices.add({ Vector2(0, 0), color, Mat4x4() });

    if (segment_end == 0)
        segment_end = segments;

    f32 radiansPerSector = (2.f * PI) / segments;
	for (i32 i = segment_start; i <= segment_end; i++) {
		f32 radians = radiansPerSector * i;
		f32 cosAngle = cosf(radians);
		f32 sinAngle = sinf(radians);
		Vector2 vertex = Vector2(radius * cosAngle, radius * sinAngle);
		vertices.add({ vertex, color, Mat4x4() });

		u32 first = i;
		u32 second = 0;
		u32 third = i + 1;
		indices.add(first);
		indices.add(second);
		indices.add(third);
	}
	
	mesh.load(&vertices.data[0], vertices.numElements, &indices.data[0], indices.numElements, &renderer);
	vertices.deallocate();
	indices.deallocate();
}

Circleish::~Circleish()
{
    mesh.unload();
}

void Circleish::render()
{
    mesh.render(&renderer);
}
