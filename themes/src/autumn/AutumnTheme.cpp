#include "AutumnTheme.hpp"
#include "../Renderer2d.h"
#include "../shapes_2d.h"
#include <memory>

void AutumnTheme::load(Renderer2d* renderer) {
    renderer->clearColor = Vector4(252, 210, 153, 255).toNormalizedColor();
	auto lr = tree.load(renderer);
	leafParticles.load(renderer, &lr);
    background = new RectangularGradient(*renderer,
                                         Vector4{135, 206, 235, 255}.toNormalizedColor(),
                                         Vector4(252, 210, 153, 255).toNormalizedColor(),
                                         renderer->get_width(),
                                         renderer->get_height(),
                                         {0, 0});
}

void AutumnTheme::update(f32 dtSeconds) {
	tree.update(dtSeconds);
	leafParticles.update(dtSeconds);
}

void AutumnTheme::render(Renderer2d* renderer) {
    background->render();
	tree.render(renderer);
	leafParticles.render(renderer);
}

void AutumnTheme::unload()  {
	tree.unload();
	leafParticles.unload();
    delete background;
}
