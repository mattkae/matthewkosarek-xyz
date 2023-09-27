#ifndef AUTUMN_THEME_HPP
#define AUTUMN_THEME_HPP

#include "TreeShape.h"
#include "LeafParticleRender.h"
#include "../types.h"
#include "../theme.h"
#include "../Renderer2d.h"
#include <memory>
#include <vector>

class RectangularGradient;
class Circleish;

class AutumnTheme : public Theme {
public:
    AutumnTheme(WebglContext*);
    ~AutumnTheme();
	TreeShape tree;
	LeafParticleRender leafParticles;
    RectangularGradient* background;
    Circleish* tree_hill;
    Circleish* background_hill;

	void load();
	void update(f32 dtSeconds);
	void render();
	void unload();
private:
    Renderer2d renderer;
};

#endif
