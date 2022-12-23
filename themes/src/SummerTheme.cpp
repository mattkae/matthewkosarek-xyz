#include "SummerTheme.h"
#include "Renderer2d.h"
#include "list.h"
#include "mathlib.h"
#include <vector>

void SummerTheme::load(Renderer2d* renderer) {
	renderer->clearColor = Vector4(0, 181, 286, 255.f).toNormalizedColor();
	sun.sectors = 180;
	sun.radius = renderer->context->width / 4.f;
	sun.load(renderer);
}

void SummerTheme::update(f32 dtSeconds) {
	sun.update(dtSeconds);
}

void SummerTheme::render(Renderer2d* renderer) {
	sun.render(renderer);
}

void SummerTheme::unload() {
	sun.unload();
}

void Sun::load(Renderer2d* renderer) {
	matte::List<Vertex2D> vertices;
	matte::List<u32> indices;
	Vector4 sunColor = Vector4(249, 215, 28, 255).toNormalizedColor();
	vertices.add({ Vector2(0, 0), sunColor, Mat4x4() });

    f32 radiansPerSector = (2.f * PI) / sectors;
	for (i32 i = 0; i <= sectors; i++) {
		f32 radians = radiansPerSector * i;
		f32 cosAngle = cosf(radians);
		f32 sinAngle = sinf(radians);
		Vector2 vertex = Vector2(radius * cosAngle, radius * sinAngle);
		vertices.add({ vertex, sunColor, Mat4x4() });

		u32 first = i;
		u32 second = 0;
		u32 third = i + 1;
		indices.add(first);
		indices.add(second);
		indices.add(third);
	}
	
	mesh.load(&vertices.data[0], vertices.numElements, &indices.data[0], indices.numElements, renderer);
	mesh.model = Mat4x4().translateByVec2(Vector2(renderer->context->width / 2.f, renderer->context->height / 2.f));
	vertices.deallocate();
	indices.deallocate();
}

void Sun::update(f32 dtSeconds) {

}

void Sun::render(Renderer2d* renderer) {
	setShaderMat4(renderer->uniforms.model, mesh.model);
	glBindVertexArray(mesh.vao);
    glDrawElements(GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Sun::unload() {
	mesh.unload();
}
