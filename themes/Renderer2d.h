#pragma once

#include "WebglContext.h"
#include "types.h"
#include "Shader.h"
#include "mathlib.h"

struct WebglContext;

struct Renderer2d {
	WebglContext* context = NULL;
	Mat4x4 projection;
	uint32 shader;
    Vector4 clearColor;

	struct {
		int32 position;
		int32 color;
		int32 vMatrix;
	} attributes;

	struct {
		int32 projection;
		int32 model;
	} uniforms;

	void load(WebglContext* context);
	void render();
	void unload();
};

struct Renderer2dVertex {
	Vector2 position;
	Vector4 color;
	Mat4x4 vMatrix;
};

struct Renderer2dShape {
	uint32 vao;
	uint32 vbo;
	uint32 numVertices = 0;
	Mat4x4 model;

	void load(Renderer2dVertex* vertices, uint32 numVertices, Renderer2d* renderer);
	void render(Renderer2d* renderer, GLenum drawType = GL_TRIANGLES);
	void unload();
};
