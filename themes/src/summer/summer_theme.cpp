#include "summer_theme.h"
#include "../renderer_2d.h"
#include "../list.h"
#include "../mathlib.h"
#include "../shaders/sun_frag.h"
#include "../shaders/sun_vert.h"

SummerTheme::SummerTheme(WebglContext* context)
{
    renderer.load(context);
    load(context);
}

SummerTheme::~SummerTheme()
{
    unload();
}

void SummerTheme::load(WebglContext* context) {
    renderer.load(context, shader_sun_vert, shader_sun_frag);
	renderer.clearColor = Vector4(0, 181, 286, 255.f).toNormalizedColor();
	sun.sectors = 180;
	sun.radius = renderer.context->width / 6.f;
	sun.load(&renderer);

	sky.load(context);
	water.load(context);
	// Align the fake reflection with the sun disk: pointing up and into the
	// scene (x = 0 keeps the glitter column centered under the sun).
	water.setSun(Vector3(0.f, 0.4f, -1.f), Vector4(249, 215, 28, 255).toNormalizedColor());
}

void SummerTheme::update(f32 dtSeconds) {
	sun.update(dtSeconds);
	sky.update(dtSeconds);
	water.update(dtSeconds);
}

void SummerTheme::render() {
    renderer.render();

	// Gradient sky + clouds behind everything. It uses its own shader and
	// disables depth writes, so restore the sun shader + projection and
	// re-enable depth writes before drawing the sun.
	sky.render();
	useShader(renderer.shader);
	setShaderMat4(renderer.uniforms.projection, renderer.projection);
	glDepthMask(GL_TRUE);
	// sky.render() disabled blending; the sun's soft glowing edge needs it back,
	// otherwise its semi-transparent rim writes opaque as a hard white border.
	glEnable(GL_BLEND);

	sun.render(&renderer);
	// Drawn last: the opaque water paints over the lower half of the sun,
	// hiding the part below the waterline while reflecting the part above.
	water.render();
}

void SummerTheme::unload() {
	water.unload();
	sky.unload();
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
	mesh.model = Mat4x4().translateByVec2(Vector2(renderer->context->width / 2.f, renderer->context->height * (3.f/ 4.f)));

	timeUniform = getShaderUniform(renderer->shader, "time");

	vertices.deallocate();
	indices.deallocate();
}

void Sun::update(f32 dtSeconds) {
	elapsedTime += dtSeconds;
}

void Sun::render(Renderer2d* renderer) {
	setShaderMat4(renderer->uniforms.model, mesh.model);
	glUniform1f(timeUniform, elapsedTime);
	glBindVertexArray(mesh.vao);
    glDrawElements(GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Sun::unload() {
	mesh.unload();
}
