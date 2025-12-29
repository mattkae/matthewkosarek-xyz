#pragma once
#include "../types.h"
#include "../renderer_2d.h"
#include "../theme.h"
#include <vector>

struct Sun {
    f32 radius = 20.f;
	i32 sectors = 180;
	Mesh2D mesh;
	f32 elapsedTime = 0.f;
	i32 timeUniform = -1;

	void load(Renderer2d* renderer);
	void update(f32 dtSeconds);
	void render(Renderer2d* renderer);
	void unload();
};

class SummerTheme : public Theme {
public:
    SummerTheme(WebglContext*);
    ~SummerTheme();
	Sun sun;
	void load(WebglContext*);
	void update(f32 dtSeconds);
	void render();
	void unload();
private:
    Renderer2d renderer;
};
