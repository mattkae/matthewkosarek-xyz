#include "types.h"
#include "mathlib.h"
#include "list.h"

struct Renderer2d;
struct Vertex2D;

struct SnowflakeLoadParameters {
	i32 numSnowflakes = 1000;
	f32 rateOfSnowfall = 0.1f;
    Vector2 flakeV0 = { 0, 1 };
	f32 flakeSize = 5.f;
	f32 flakeSizeDeviation = 1.f;
	Vector4 snowColor = { 0.8, 0.8, 0.8, 1.0 };
	f32 windIntervalSeconds = 1.5;
};

struct SnowflakeUpdateData {
	Vector2 v0;
	Vector2 velocity;
	Vector2 position;
	f32 rotation;
	bool onGround = false;

	i32 vtxIdx = 0;
	i32 numVertices = 0;
};

struct SnowflakeParticleRenderer {
	f32 xMax = 0;
	f32 yMax = 0;
	f32 windIntervalSeconds = 1.5;
	i32 numSnowflakes = 0;
    Vector2 windSpeed;
	SnowflakeUpdateData* updateData;
	f32 timeUntilNextWindSeconds = 0;

	u32 vao;
	u32 vbo;
	Mat4x4 model;
	matte::List<Vertex2D> vertices;
	
	void load(SnowflakeLoadParameters params, Renderer2d* renderer);
	void update(f32 dtSeconds);
	void render(Renderer2d* renderer);
	void unload();
};
