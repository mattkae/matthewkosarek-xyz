#include "AutumnTheme.hpp"
#include "../Renderer2d.h"

void AutumnTheme::load(Renderer2d* renderer) {
    renderer->clearColor = Vector4(252, 210, 153, 255).toNormalizedColor();
	auto lr = tree.load(renderer);
	leafParticles.load(renderer, &lr);
}

void AutumnTheme::update(f32 dtSeconds) {
	tree.update(dtSeconds);
	leafParticles.update(dtSeconds);
}

void AutumnTheme::render(Renderer2d* renderer) {
	tree.render(renderer);
	leafParticles.render(renderer);
}

void AutumnTheme::unload()  {
	tree.unload();
	leafParticles.unload();
}