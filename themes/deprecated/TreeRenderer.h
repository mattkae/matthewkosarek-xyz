#pragma once

#include "WebglContext.h"
#include "types.h"
#include "Shader.h"
#include "mathlib.h"

struct WebglContext;

struct TreeRenderer {
	Mat4x4 projection;
	u32 shader;
    Vector4 clearColor;

	struct {
		i32 position;
		i32 color;
	} attributes;

	struct {
		i32 projection;
		i32 model;
	} uniforms;

    f32 timeElapsed = 0.f;
    f32 frequency = 5.f;

	void load(WebglContext* context);
	void render(f32 dtSeconds);
	void unload();
};

struct TreeRendererVertex {
	Vector2 position;
	Vector4 color;
};
