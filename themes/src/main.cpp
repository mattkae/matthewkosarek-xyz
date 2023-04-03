#include "WebglContext.h"
#include "MainLoop.h"
#include "Renderer2d.h"
#include "mathlib.h"
#include "types.h"
#include "summer/SummerTheme.h"
#include "autumn/AutumnTheme.hpp"
#include "spring/SpringTheme.hpp"
#include "winter/WinterTheme.hpp"
#include <cstdio>
#include <emscripten/fetch.h>


enum Theme {
	Default = 0,
    Autumn,
	Winter,
	Spring,
	Summer
};

void load(Theme theme);
void unload();
void update(f32 dtSeconds, void* userData);
EM_BOOL selectNone(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData);
EM_BOOL selectAutumn(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData);
EM_BOOL selectWinter(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData);
EM_BOOL selectSpring(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData);
EM_BOOL selectSummer(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData);

WebglContext context;
Renderer2d renderer2d;
MainLoop mainLoop;
Theme activeTheme = Theme::Default;
AutumnTheme autumnTheme;
WinterTheme winterTheme;
SpringTheme springTheme;
SummerTheme summerTheme;

int main() {
	context.init("#theme_canvas");
	emscripten_set_click_callback("#theme_button_default", NULL, false, selectNone);
	emscripten_set_click_callback("#theme_button_autumn", NULL, false, selectAutumn);
	emscripten_set_click_callback("#theme_button_winter", NULL, false, selectWinter);
	emscripten_set_click_callback("#theme_button_spring", NULL, false, selectSpring);
	emscripten_set_click_callback("#theme_button_summer", NULL, false, selectSummer);
	
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
	case Theme::Spring: {
		springTheme.load(&context);
		break;
    }
	case Theme::Summer:
		renderer2d.load(&context);
		summerTheme.load(&renderer2d);
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
	case Theme::Summer:
		summerTheme.update(dtSeconds);
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
		springTheme.render();
		break;
	case Theme::Summer:
		renderer2d.render();
	    summerTheme.render(&renderer2d);
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
	case Theme::Summer:
		summerTheme.unload();
		break;
	default:
		break;
	}
	
	activeTheme = Theme::Default;
	if (mainLoop.isRunning) {
		mainLoop.stop();
		renderer2d.unload();
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

EM_BOOL selectSummer(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData) {
	printf("Summer theme selected\n");
	load(Theme::Summer);
	return true;
}