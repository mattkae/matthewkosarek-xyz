#include "AutumnTheme.hpp"
#include "../shapes_2d.h"
#include <memory>

namespace
{
    const int NUM_HILLS = 3;
}

AutumnTheme::AutumnTheme(WebglContext* context)
{
    renderer.load(context);
    load();
}

AutumnTheme::~AutumnTheme()
{
    unload();
}

void AutumnTheme::load() {
    renderer.clearColor = Vector4(252, 210, 153, 255).toNormalizedColor();
	auto lr = tree.load(&renderer);
	leafParticles.load(&renderer, &lr);
    background = new RectangularGradient(
        renderer,
        Vector4{135, 206, 235, 255}.toNormalizedColor(),
        Vector4(252, 210, 153, 255).toNormalizedColor(),
        renderer.get_width(),
        renderer.get_height(),
        {0, 0});


    background_hill = new Circleish(
        renderer,
        1000,
        Vector4(137, 129, 33, 255).toNormalizedColor(),
        100,
        0,
        50);
    background_hill->mesh.model = background_hill->mesh.model.translateByVec2({1200, -700});
    
    tree_hill = new Circleish(
        renderer,
        500,
        Vector4{ 76, 75, 22, 255 }.toNormalizedColor(),
        100,
        0,
        50);
    tree_hill->mesh.model = tree_hill->mesh.model.translateByVec2(Vector2(300, -290));
}

void AutumnTheme::update(f32 dtSeconds) {
	tree.update(dtSeconds);
	leafParticles.update(dtSeconds);
}

void AutumnTheme::render() {
    renderer.render();
    background->render();
    background_hill->render();
	tree.render(&renderer);
    tree_hill->render();
	leafParticles.render(&renderer);
}

void AutumnTheme::unload()  {
	tree.unload();
	leafParticles.unload();
    delete background;
    delete background_hill;
    delete tree_hill;
}
