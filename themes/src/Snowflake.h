#ifndef SNOWFLAKE_H
#define SNOWFLAKE_H

#include "types.h"
#include "mathlib.h"
#include "list.h"

struct Renderer2d;
struct Vertex2D;

struct SnowflakeLoadParameters {
	i32 numSnowflakes = 480;
	f32 windIntervalSeconds = 1.5f;
};

struct SnowflakeUpdateData {
	Vector2 velocity;
	Vector2 position;
    f32 rotateVelocity = 0.f;
	f32 rotation = 0;

	i32 vtxIdx = 0;
	i32 numVertices = 0;
};

struct SnowflakeParticleRenderer {
	f32 xMax = 0;
	f32 yMax = 0;
	f32 windIntervalSeconds = 1.5;
	i32 numSnowflakes = 0;
    Vector2 windSpeed;
	f32 timeUntilNextWindSeconds = 0;
	SnowflakeUpdateData* updateData;

	u32 vao;
	u32 vbo;
	Mat4x4 model;
	matte::List<Vertex2D> vertices;
	
	void load(SnowflakeLoadParameters params, Renderer2d* renderer);
	void update(f32 dtSeconds);
	void render(Renderer2d* renderer);
	void unload();
};

#endif
