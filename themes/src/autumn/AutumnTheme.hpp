#ifndef AUTUMN_THEME_HPP
#define AUTUMN_THEME_HPP

#include "TreeShape.h"
#include "LeafParticleRender.h"
#include "../types.h"
#include <memory>

struct Renderer2d;
class RectangularGradient;

struct AutumnTheme {
	TreeShape tree;
	LeafParticleRender leafParticles;
    RectangularGradient* background;

	void load(Renderer2d* renderer);
	void update(f32 dtSeconds);
	void render(Renderer2d* renderer);
	void unload();
};

#endif
