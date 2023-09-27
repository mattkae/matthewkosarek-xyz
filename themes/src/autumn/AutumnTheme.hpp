#ifndef AUTUMN_THEME_HPP
#define AUTUMN_THEME_HPP

#include "TreeShape.h"
#include "LeafParticleRender.h"
#include "../types.h"
#include <memory>
#include <vector>

struct Renderer2d;
class RectangularGradient;
class Circleish;

struct AutumnTheme {
	TreeShape tree;
	LeafParticleRender leafParticles;
    RectangularGradient* background;
    Circleish* tree_hill;
    Circleish* background_hill;

	void load(Renderer2d* renderer);
	void update(f32 dtSeconds);
	void render(Renderer2d* renderer);
	void unload();
};

#endif
