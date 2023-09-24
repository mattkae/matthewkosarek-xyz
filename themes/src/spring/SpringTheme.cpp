#include "SpringTheme.hpp"
#include "../Renderer3d.h"
#include "../shader_fetcher.hpp"
#include <cstdio>
#include <emscripten/fetch.h>

void onBunnySuccess(emscripten_fetch_t *fetch) {
    SpringTheme* springTheme = (SpringTheme*)fetch->userData;
	springTheme->state = SpringThemeState::LoadedBunny;
	printf("Finished downloading %llu bytes from URL %s.\n", fetch->numBytes, fetch->url);
	const i32 len = fetch->numBytes;
	springTheme->bunnyMesh = Mesh3d_fromObj(&springTheme->renderer, fetch->data, len);
	// The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];
	emscripten_fetch_close(fetch); // Free data associated with the fetch.
}

void onBunnyFail(emscripten_fetch_t *fetch) {
	printf("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
	emscripten_fetch_close(fetch); // Also free data on failure.
}

inline void fetch_bunny(SpringTheme* theme) {
    emscripten_fetch_attr_t attr;
	emscripten_fetch_attr_init(&attr);
	strcpy(attr.requestMethod, "GET");
	attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
	attr.onsuccess = onBunnySuccess;
	attr.onerror = onBunnyFail;
	auto* bunny_fetch = emscripten_fetch(&attr, "themes/resources/bunny.obj");
    bunny_fetch->userData = theme;
}

inline void on_shaders_loader(ShaderFetchResult* result) {
    SpringTheme* theme = (SpringTheme*)result->user_data;
    theme->renderer.load(theme->renderer.context, result->vertex.c_str(), result->fragment.c_str());
	theme->state = SpringThemeState::LoadedShader;
	fetch_bunny(theme);
}

void SpringTheme::load(WebglContext* context) {
    state = SpringThemeState::Loading;
	renderer.context = context;
    renderer.clearColor = Vector4(160, 231, 160, 255.f).toNormalizedColor();

	fetch_shader(
        {
            "themes/src/_shaders/renderer3d.vert",
            "themes/src/_shaders/renderer3d.frag"
        },
        on_shaders_loader,
        this
    );
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
	case SpringThemeState::Loading: return;
	case SpringThemeState::LoadedShader: return;
	case SpringThemeState::LoadedBunny: 
		state = SpringThemeState::Idle;
		stateTimer = 0.f;
		bunnyHopAnimationTimer = 0.f;
		break;
	case SpringThemeState::Idle: {
		bunnyHopAnimationTimer += dtSeconds;
		const f32 HOP_FREQUENCY = 6.f;

		if (bunnyHopAnimationTimer > stateTimer) {
			state = SpringThemeState::PreHop;
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
	case SpringThemeState::PreHop: {
		const f32 ROTATION_TIME = 0.5f;
		bunnyHopAnimationTimer += dtSeconds;
		f32 current = bunnyRotation + (targetRotation - bunnyRotation) * (bunnyHopAnimationTimer / ROTATION_TIME);
		bunnyMesh.model = Mat4x4().rotate(0, current, 0).translate(bunnyPosition);

		if (bunnyHopAnimationTimer > ROTATION_TIME) {
			bunnyRotation = targetRotation;
			bunnyHopAnimationTimer = 0;
			state = SpringThemeState::Hopping;
		}
		break;
	}
	case SpringThemeState::Hopping: {
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
			state = SpringThemeState::Idle;
			stateTimer = randomFloatBetween(0.5f, 1.f);
		}
		break;
	}
	}
}

void SpringTheme::render() {
	renderer.render();
	if (state != SpringThemeState::Loading) {
		bunnyMesh.render(&renderer);
	}
}

void SpringTheme::unload()  {
    renderer.unload();
	bunnyMesh.unload();
}
