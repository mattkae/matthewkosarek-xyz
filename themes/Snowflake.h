#include "types.h"
#include "mathlib.h"
#include "List.h"

struct Renderer2d;
struct Renderer2dVertex;

struct SnowflakeLoadParameters {
	int32 maxSnowflakes = 1000;
	int32 initialSnowflakeCount = 100;
	float32 rateOfSnowfall = 0.1f;
    Vector2 flakeV0 = { 0, 1 };
	float32 flakeSize = 5.f;
	float32 flakeSizeDeviation = 1.f;
	Vector4 snowColor = { 0.8, 0.8, 0.8, 1.0 };
	float32 spawnIntervalSeconds = 0.3;
	float32 windIntervalSeconds = 1.5;
};

struct SnowflakeUpdateData {
	Vector2 v0;
	Vector2 velocity;
	Vector2 position;
	float32 rotation;
	bool onGround = false;

	int32 vtxIdx = 0;
	int32 numVertices = 0;
};

struct SnowflakeParticleRenderer {
	float32 xMax = 0;
	float32 yMax = 0;
	float32 spawnIntervalSeconds = 0.3;
	float32 windIntervalSeconds = 1.5;
	int32 startIndex = 0;
	int32 endIndex = 0;
	int32 numSnowflakes = 0;
    Vector2 windSpeed;
	SnowflakeUpdateData* updateData;
	float32 timeUntilNextSpawnSeconds = 0;
	float32 timeUntilNextWindSeconds = 0;

	uint32 vao;
	uint32 vbo;
	Mat4x4 model;
	List<Renderer2dVertex> vertices;
	
	void load(SnowflakeLoadParameters params, Renderer2d* renderer);
	void update(float32 dtSeconds);
	void render(Renderer2d* renderer);
	void unload();
};
