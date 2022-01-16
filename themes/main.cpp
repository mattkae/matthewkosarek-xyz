#include "Renderer2d.h"
#include "WebglContext.h"
#include "MainLoop.h"
#include "Renderer2d.h"
#include "types.h"
#include "TreeShape.h"
#include "LeafParticleRender.h"
#include "Snowflake.h"

enum Theme {
	Default = 0,
    Autumn,
	Winter
};

struct AutumnTheme {
	TreeShape tree;
	LeafParticleRender leafParticles;

	void load(Renderer2d* renderer);
	void update(float32 dtSeconds);
	void render(Renderer2d* renderer);
	void unload();
};

struct WinterTheme {
	SnowflakeParticleRenderer spr;
	
	void load(Renderer2d* renderer);
	void update(float32 dtSeconds);
	void render(Renderer2d* renderer);
	void unload();
};

void load(Theme theme);
void unload();
void update(float32 dtSeconds, void* userData);
EM_BOOL selectNone(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData);
EM_BOOL selectAutumn(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData);
EM_BOOL selectWinter(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData);

WebglContext context;
Renderer2d renderer;
MainLoop mainLoop;
Theme activeTheme = Theme::Default;
AutumnTheme autumnTheme;
WinterTheme winterTheme;

int main() {
	context.init("#theme_canvas");
	emscripten_set_click_callback("#theme_button_default", NULL, false, selectNone);
	emscripten_set_click_callback("#theme_button_autumn", NULL, false, selectAutumn);
	emscripten_set_click_callback("#theme_button_winter", NULL, false, selectWinter);
	
	return 0;
}

// -- Scene loading, updating, and unloading logic
void load(Theme theme) {
	if (activeTheme == theme) {
		printf("This theme is already active.\n");
		return;
	}

    unload(); // Try and unload before we load, so that we start fresh

	activeTheme = theme;
	renderer.load(&context);
	mainLoop.run(update);

	switch (activeTheme) {
	case Theme::Autumn:
		autumnTheme.load(&renderer);
		break;
	case Theme::Winter:
		winterTheme.load(&renderer);
		break;
	default:
		break;
	}
}

void update(float32 dtSeconds, void* userData) {
	// -- Update
	switch (activeTheme) {
	case Theme::Autumn:
		autumnTheme.update(dtSeconds);
		break;
	case Theme::Winter:
		winterTheme.update(dtSeconds);
		break;
	default:
		break;
	}
	
	// -- Render
	renderer.render();
	switch (activeTheme) {
	case Theme::Autumn:
		autumnTheme.render(&renderer);
		break;
	case Theme::Winter:
		winterTheme.render(&renderer);
		break;
	default:
		break;
	}
}

void unload() {
	switch (activeTheme) {
	case Theme::Autumn:
		autumnTheme.unload();
		break;
	case Theme::Winter:
		winterTheme.unload();
		break;
	default:
		break;
	}
	
	activeTheme = Theme::Default;
	if (mainLoop.isRunning) {
		mainLoop.stop();
		renderer.unload();
	}
}

// -- HTML5 callbacks
EM_BOOL selectNone(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData) {
	printf("Default theme selected\n");
	unload();
	return true;
}

EM_BOOL selectAutumn(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData) {
	printf("Autumn theme selected\n");
	load(Theme::Autumn);
	return true;
}

EM_BOOL selectWinter(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData) {
	printf("Winter theme selected\n");
	load(Theme::Winter);
	return true;
}

// -- Autumn theme
void AutumnTheme::load(Renderer2d* renderer) {
    renderer->clearColor = Vector4(252, 210, 153, 255).toNormalizedColor();
	auto lr = tree.load(renderer);
	leafParticles.load(renderer, &lr);
}

void AutumnTheme::update(float32 dtSeconds) {
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

// -- Winter theme
void WinterTheme::load(Renderer2d* renderer) {
    renderer->clearColor = Vector4(200, 229, 239, 255).toNormalizedColor();
	SnowflakeLoadParameters lp;
	lp.spawnIntervalSeconds = 0.05;
	spr.load(lp, renderer);
}

void WinterTheme::update(float32 dtSeconds) {
	spr.update(dtSeconds);
}

void WinterTheme::render(Renderer2d* renderer) {
	spr.render(renderer);
}

void WinterTheme::unload()  {
	spr.unload();
}
