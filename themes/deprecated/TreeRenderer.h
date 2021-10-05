#pragma once

#include "WebglContext.h"
#include "types.h"
#include "Shader.h"
#include "mathlib.h"

struct WebglContext;

struct TreeRenderer {
	Mat4x4 projection;
	uint32 shader;
    Vector4 clearColor;

	struct {
		int32 position;
		int32 color;
	} attributes;

	struct {
		int32 projection;
		int32 model;
	} uniforms;

    float32 timeElapsed = 0.f;
    float32 frequency = 5.f;

	void load(WebglContext* context);
	void render(float32 dtSeconds);
	void unload();
};

struct TreeRendererVertex {
	Vector2 position;
	Vector4 color;
};
