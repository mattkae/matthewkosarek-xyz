#include "WebglContext.h"
#include "MainLoop.h"
#include "Renderer2d.h"
#include "Renderer3d.h"
#include "mathlib.h"
#include "types.h"
#include "TreeShape.h"
#include "SummerTheme.h"
#include "LeafParticleRender.h"
#include "Snowflake.h"
#include <cstdio>
#include <emscripten/fetch.h>


enum Theme {
	Default = 0,
    Autumn,
	Winter,
	Spring,
	Summer
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

enum class BunnyAnimationState {
	Loading = 0,
	Loaded,
	PreHop,
	Hopping,
	Idle
};

struct SpringTheme {
	BunnyAnimationState state;
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
EM_BOOL selectSummer(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData);

WebglContext context;
Renderer2d renderer2d;
Renderer3D renderer3d;
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
	case Theme::Spring:
		renderer3d.load(&context);
		springTheme.load(&renderer3d);
		break;
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
		renderer3d.render();
		springTheme.render(&renderer3d);
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

EM_BOOL selectSummer(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData) {
	printf("Summer theme selected\n");
	load(Theme::Summer);
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
	springTheme.state = BunnyAnimationState::Loaded;
	printf("Finished downloading %llu bytes from URL %s.\n", fetch->numBytes, fetch->url);
	const i32 len = fetch->numBytes;
	springTheme.bunnyMesh = Mesh3d_fromObj(&renderer3d, fetch->data, len);
	// The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];
	emscripten_fetch_close(fetch); // Free data associated with the fetch.
}

void onBunnyFail(emscripten_fetch_t *fetch) {
	printf("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
	emscripten_fetch_close(fetch); // Also free data on failure.
}

void SpringTheme::load(Renderer3D* renderer) {
    springTheme.state = BunnyAnimationState::Loading;
    renderer->clearColor = Vector4(160, 231, 160, 255.f).toNormalizedColor();

	emscripten_fetch_attr_t attr;
	emscripten_fetch_attr_init(&attr);
	strcpy(attr.requestMethod, "GET");
	attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
	attr.onsuccess = onBunnySuccess;
	attr.onerror = onBunnyFail;
	emscripten_fetch(&attr, "themes/resources/bunny.obj");
}

inline Vector3 bunnyLerp(Vector3& start, Vector3& target, f32 t) {
	t = 3 * t *t - 2 * t * t * t;
	return start + ((target - start) * t);
}

inline f32 verticalHopLerp(f32 start, f32 target, f32 t) {
	f32 ogt = t;
	t = 3 * t *t - 2 * t * t * t;
	if (ogt >= 0.5f) t = 1.f - t;
	return start + ((target - start) * t);
}

inline f32 rotationLerp(f32 start, f32 target, f32 t) {
	return start + ((target - start) * t);
}

void SpringTheme::update(f32 dtSeconds) {
	switch (state) {
	case BunnyAnimationState::Loading: return;
	case BunnyAnimationState::Loaded: 
		state = BunnyAnimationState::Idle;
		stateTimer = 0.f;
		bunnyHopAnimationTimer = 0.f;
		break;
	case BunnyAnimationState::Idle: {
		bunnyHopAnimationTimer += dtSeconds;
		const f32 HOP_FREQUENCY = 6.f;

		if (bunnyHopAnimationTimer > stateTimer) {
			state = BunnyAnimationState::PreHop;
		    f32 xDir = 1;
			f32 yDir = 1;
			if (bunnyTarget.x > 0) xDir = -1;
			if (bunnyTarget.z > 0) yDir = -1;
			bunnyTarget = bunnyPosition + Vector3(randomFloatBetween(0, xDir * 25), 0, randomFloatBetween(0, yDir * 25));
			auto direction = (bunnyTarget - bunnyPosition);
			auto distance = direction.length();
			direction = direction.normalize();
			numHops = ceil(distance / HOP_FREQUENCY);
			hopCount = 0;
				
			targetRotation = PI - atan2(direction.y, direction.x);
			stateTimer = ((bunnyTarget - bunnyPosition).length() / bunnySpeed) / numHops;
			bunnyHopAnimationTimer = 0.f;
			hopIncrement = (bunnyTarget - bunnyPosition) / numHops;
		}
		break;
	}
	case BunnyAnimationState::PreHop: {
		const f32 ROTATION_TIME = 0.5f;
		bunnyHopAnimationTimer += dtSeconds;
		f32 current = bunnyRotation + (targetRotation - bunnyRotation) * (bunnyHopAnimationTimer / ROTATION_TIME);
		bunnyMesh.model = Mat4x4().rotate(0, current, 0).translate(bunnyPosition);

		if (bunnyHopAnimationTimer > ROTATION_TIME) {
			bunnyRotation = targetRotation;
			bunnyHopAnimationTimer = 0;
			state = BunnyAnimationState::Hopping;
		}
		break;
	}
	case BunnyAnimationState::Hopping: {
		bunnyHopAnimationTimer += dtSeconds;
		f32 t = bunnyHopAnimationTimer / stateTimer;

		Vector3 nextPosition = bunnyPosition + hopIncrement;
	    auto renderPos = bunnyLerp(bunnyPosition, nextPosition, t);
		if ((renderPos - nextPosition).length() < 0.01f) {
			hopCount += 1;
			bunnyHopAnimationTimer = 0.f;
			bunnyPosition = nextPosition;
		}
		
		renderPos.y = verticalHopLerp(0.f, 4.f, t);

		const f32 RMAX = PI / 16.f;
		f32 zRotation = 0;
		f32 start = 0.f;
		f32 end = PI / 8.f;
		f32 startTime = 0.f;
		f32 endTime = 0.f;
		bool disableRot = false;

		if (t >= 0.9f) {
			disableRot = true;
		}
		else if (t >= 0.7f) {
			start = -RMAX;
			end = 0.f;
			startTime = 0.7f;
			endTime = 0.9f;
		}
		else if (t >= 0.50f) {
			start = 0.f;
			end = -RMAX;
			startTime = 0.50f;
			endTime = 0.70f;
		}
		else if (t >= 0.40f) {
		    disableRot = true;
		}
		else if (t >= 0.20f) {
			start = RMAX;
			end = 0.f;
			startTime = 0.20f;
			endTime = 0.40f;
		}
		else {
			start = 0.f;
			end = RMAX;
			startTime = 0.f;
			endTime = 0.20f;
		}

		
		if (!disableRot) {
			f32 totalTime = endTime - startTime;
			zRotation = rotationLerp(start, end, (totalTime - (endTime - t)) / totalTime);
		}

		bunnyMesh.model = Mat4x4().getZRotationMatrix(zRotation).rotate(0, bunnyRotation, 0).translate(renderPos);
		if (hopCount == numHops) {
			bunnyPosition = bunnyTarget;
			bunnyHopAnimationTimer = 0.f;
			state = BunnyAnimationState::Idle;
			stateTimer = randomFloatBetween(0.5f, 1.f);
		}
		break;
	}
	}
}

void SpringTheme::render(Renderer3D* renderer) {
	renderer->render();
	if (state != BunnyAnimationState::Loading) {
		bunnyMesh.render(renderer);
	}
}

void SpringTheme::unload()  {
	bunnyMesh.unload();
}
