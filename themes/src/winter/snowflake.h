#ifndef SNOWFLAKE_H
#define SNOWFLAKE_H

#include "../types.h"
#include "../mathlib.h"
#include "../list.h"
#include "windfield.hpp"

struct Renderer2d;
struct Vertex2D;

struct SnowflakeLoadParameters {
	i32 numSnowflakes = 480;
	f32 windIntervalSeconds = 1.5f;
};

struct SnowflakeInstanceData {
	Vector2 position;
	f32 rotation;
	f32 scale;
	f32 seed;
};

struct SnowflakeUpdateData {
	Vector2 velocity;
	Vector2 position;
    f32 rotateVelocity = 0.f;
	f32 rotation = 0;
    f32 scale;
	f32 seed;
};

struct SnowflakeParticleRenderer {
	f32 xMax = 0;
	f32 yMax = 0;
	f32 windIntervalSeconds = 1.5;
	i32 numSnowflakes = 0;
	f32 timeUntilNextWindSeconds = 0;
    WindField<100, 100, 10> wind;
	SnowflakeUpdateData* updateData;

	u32 vao;
	u32 quadVbo;           // Base quad geometry
	u32 instanceVbo;       // Instance data (position, rotation, scale, seed)
	u32 shader;            // Custom snowflake shader
	Mat4x4 model;

	struct {
		i32 position;
		i32 instancePos;
		i32 instanceRot;
		i32 instanceScale;
		i32 instanceSeed;
	} attributes;

	struct {
		i32 projection;
		i32 model;
	} uniforms;

	void load(SnowflakeLoadParameters params, Renderer2d* renderer);
	void update(f32 dtSeconds);
	void render(Renderer2d* renderer);
	void unload();
};

#endif
