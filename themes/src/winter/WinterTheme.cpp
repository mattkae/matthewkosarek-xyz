#include "WinterTheme.hpp"
#include "../Renderer2d.h"

WinterTheme::WinterTheme(WebglContext* context)
{
    renderer.load(context);
    load();
}

WinterTheme::~WinterTheme()
{
    unload();
}

void WinterTheme::load() {
    renderer.clearColor = Vector4(200, 229, 239, 255).toNormalizedColor();
	SnowflakeLoadParameters lp;
	spr.load(lp, &renderer);
}

void WinterTheme::update(f32 dtSeconds) {
	spr.update(dtSeconds);
}

void WinterTheme::render() {
    renderer.render();
	spr.render(&renderer);
}

void WinterTheme::unload()  {
	spr.unload();
}
