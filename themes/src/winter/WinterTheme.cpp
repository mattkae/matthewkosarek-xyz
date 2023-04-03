#include "WinterTheme.hpp"
#include "../Renderer2d.h"

void WinterTheme::load(Renderer2d* renderer) {
    renderer->clearColor = Vector4(200, 229, 239, 255).toNormalizedColor();
	SnowflakeLoadParameters lp;
	spr.load(lp, renderer);
}

void WinterTheme::update(f32 dtSeconds) {
	spr.update(dtSeconds);
}

void WinterTheme::render(Renderer2d* renderer) {
	spr.render(renderer);
}

void WinterTheme::unload()  {
	spr.unload();
}
