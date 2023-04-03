#ifndef SPRING_THEME_HPP
#define SPRING_THEME_HPP

#include "../mathlib.h"
#include "../types.h"
#include "../Renderer3d.h"


enum class SpringThemeState {
	Loading = 0,
	LoadedShader,
	LoadedBunny,
	PreHop,
	Hopping,
	Idle
};

struct SpringTheme {
	Renderer3d renderer;
	SpringThemeState state;
	f32 bunnySpeed = 5.f;
	Vector3 bunnyPosition = Vector3(0, 0, 0);
	Vector3 bunnyTarget = Vector3(0, 0, 0);
	Vector3 hopIncrement = Vector3(0, 0, 0);

    f32 numHops = 0;
	f32 hopCount = 0;
	f32 bunnyHopAnimationTimer = 0.f;
	f32 stateTimer = 0.f;
	f32 bunnyRotation = 0.f;
	f32 targetRotation = 0.f;

	Mesh3d bunnyMesh;

	void load(WebglContext*);
	void update(f32 dtSeconds);
	void render();
	void unload();
};

#endif