#pragma once

#include "WebglContext.h"
#include "types.h"
#include "Shader.h"
#include "mathlib.h"

struct WebglContext;

/// Responsible for rendering Mesh2Ds
struct Renderer2d {
	WebglContext* context = NULL;
	Mat4x4 projection;
	u32 shader;
    Vector4 clearColor;

	struct {
		i32 position;
		i32 color;
		i32 vMatrix;
	} attributes;

	struct {
		i32 projection;
		i32 model;
	} uniforms;

    /// Load with the provided context and shader programs. If the shaders are NULL, the default
    /// shader is used
	void load(WebglContext* context, const char* vertexShader = NULL, const char* fragmentShader = NULL);
	void render();
	void unload();
};

struct Vertex2D {
	Vector2 position;
	Vector4 color;
	Mat4x4 vMatrix;
};

struct Mesh2D {
	u32 vao;
	u32 vbo;
	u32 ebo = 0;
	u32 numVertices = 0;
	u32 numIndices = 0;
	Mat4x4 model;

	void load(Vertex2D* vertices, u32 numVertices, Renderer2d* renderer);
	void load(Vertex2D* vertices,
			  u32 numVertices,
			  u32* indices,
			  u32 numIndices,
			  Renderer2d* renderer);
	void render(Renderer2d* renderer, GLenum drawType = GL_TRIANGLES);
	void unload();
};
