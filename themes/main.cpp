#include "WebglContext.h"
#include "MainLoop.h"
#include "Renderer2d.h"
#include "Renderer3d.h"
#include "types.h"
#include "TreeShape.h"
#include "LeafParticleRender.h"
#include "Snowflake.h"
#include <cstdio>
#include <emscripten/fetch.h>


enum Theme {
	Default = 0,
    Autumn,
	Winter,
	Spring
};

struct AutumnTheme {
	TreeShape tree;
	LeafParticleRender leafParticles;

	void load(Renderer2d* renderer);
	void update(f32 dtSeconds);
	void render(Renderer2d* renderer);
	void unload();
};

struct WinterTheme {
	SnowflakeParticleRenderer spr;
	
	void load(Renderer2d* renderer);
	void update(f32 dtSeconds);
	void render(Renderer2d* renderer);
	void unload();
};

struct SpringTheme {
	void load(Renderer3D* renderer);
	void update(f32 dtSeconds);
	void render(Renderer3D* renderer);
	void unload();
};

void load(Theme theme);
void unload();
void update(f32 dtSeconds, void* userData);
EM_BOOL selectNone(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData);
EM_BOOL selectAutumn(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData);
EM_BOOL selectWinter(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData);
EM_BOOL selectSpring(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData);

WebglContext context;
Renderer2d renderer2d;
Renderer3D renderer3d;
MainLoop mainLoop;
Theme activeTheme = Theme::Default;
AutumnTheme autumnTheme;
WinterTheme winterTheme;
SpringTheme springTheme;

int main() {
	context.init("#theme_canvas");
	emscripten_set_click_callback("#theme_button_default", NULL, false, selectNone);
	emscripten_set_click_callback("#theme_button_autumn", NULL, false, selectAutumn);
	emscripten_set_click_callback("#theme_button_winter", NULL, false, selectWinter);
	emscripten_set_click_callback("#theme_button_spring", NULL, false, selectSpring);
	
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
	mainLoop.run(update);

	switch (activeTheme) {
	case Theme::Autumn:
		renderer2d.load(&context);
		autumnTheme.load(&renderer2d);
		break;
	case Theme::Winter:
		renderer2d.load(&context);
		winterTheme.load(&renderer2d);
		break;
	case Theme::Spring:
		renderer3d.load(&context);
		springTheme.load(&renderer3d);
		break;
	default:
		break;
	}
}

void update(f32 dtSeconds, void* userData) {
	// -- Update
	switch (activeTheme) {
	case Theme::Autumn:
		autumnTheme.update(dtSeconds);
		break;
	case Theme::Winter:
		winterTheme.update(dtSeconds);
		break;
	case Theme::Spring:
		springTheme.update(dtSeconds);
		break;
	default:
		break;
	}
	
	// -- Render
	switch (activeTheme) {
	case Theme::Autumn:
		renderer2d.render();
		autumnTheme.render(&renderer2d);
		break;
	case Theme::Winter:
		renderer2d.render();
		winterTheme.render(&renderer2d);
		break;
	case Theme::Spring:
		renderer3d.render();
		springTheme.render(&renderer3d);
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
	case Theme::Spring:
		springTheme.unload();
		break;
	default:
		break;
	}
	
	activeTheme = Theme::Default;
	if (mainLoop.isRunning) {
		mainLoop.stop();
		renderer2d.unload();
		renderer3d.unload();
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

EM_BOOL selectSpring(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData) {
	printf("Spring theme selected\n");
	load(Theme::Spring);
	return true;
}

// -- Autumn theme
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

// -- Winter theme
void WinterTheme::load(Renderer2d* renderer) {
    renderer->clearColor = Vector4(200, 229, 239, 255).toNormalizedColor();
	SnowflakeLoadParameters lp;
	lp.spawnIntervalSeconds = 0.05;
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

// -- Spring theme
void onBunnySuccess(emscripten_fetch_t *fetch) {
  printf("Finished downloading %llu bytes from URL %s.\n", fetch->numBytes, fetch->url);
  // The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];
  emscripten_fetch_close(fetch); // Free data associated with the fetch.
}

void onBunnyFail(emscripten_fetch_t *fetch) {
  printf("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
  emscripten_fetch_close(fetch); // Also free data on failure.
}

void SpringTheme::load(Renderer3D* renderer) {
    renderer->clearColor = Vector4(160, 231, 160, 255.f).toNormalizedColor();

	emscripten_fetch_attr_t attr;
	emscripten_fetch_attr_init(&attr);
	strcpy(attr.requestMethod, "GET");
	attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
	attr.onsuccess = onBunnySuccess;
	attr.onerror = onBunnyFail;
	emscripten_fetch(&attr, "themes/resources/bunny.obj");
}

void SpringTheme::update(f32 dtSeconds) {
}

void SpringTheme::render(Renderer3D* renderer) {
}

void SpringTheme::unload()  {
}