#pragma once
#include "types.h"
#include "Renderer2d.h"
#include <vector>

struct Sun {
    f32 radius = 20.f;
	i32 sectors = 180;
	Mesh2D mesh;

	void load(Renderer2d* renderer);
	void update(f32 dtSeconds);
	void render(Renderer2d* renderer);
	void unload();
};

struct SummerTheme {
	Sun sun;
	void load(Renderer2d* renderer);
	void update(f32 dtSeconds);
	void render(Renderer2d* renderer);
	void unload();
};
