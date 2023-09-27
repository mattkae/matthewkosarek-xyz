#ifndef AUTUMN_THEME_HPP
#define AUTUMN_THEME_HPP

#include "TreeShape.h"
#include "LeafParticleRender.h"
#include "../types.h"
#include "../theme.h"
#include <memory>
#include <vector>

struct Renderer2d;
class RectangularGradient;
class Circleish;

class AutumnTheme : public Theme {
public:
    AutumnTheme(Renderer2d* renderer);
    ~AutumnTheme();
	TreeShape tree;
	LeafParticleRender leafParticles;
    RectangularGradient* background;
    Circleish* tree_hill;
    Circleish* background_hill;

	void load(Renderer2d* renderer);
	void update(f32 dtSeconds);
	void render();
	void unload();
private:
    Renderer2d* renderer;
};

#endif
