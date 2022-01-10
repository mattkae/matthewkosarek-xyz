#include "types.h"
#include "mathlib.h"

struct Renderer2d;
struct SnowflakeUpdateData;
struct SnowflakeRenderData;

struct SnowflakeLoadParameters {
	int32 maxSnowflakes = 10000;
	float32 rateOfSnowfall = 0.1f;
    Vector3 flakeV0 = { 0, 1, 0 };
	float32 flakeSize = 5.f;
	float32 flakeSizeDeviation = 1.f;
	Vector4 snowColor = { 0.8, 0.8, 0.8, 1.0 };
};

struct SnowflakeParticleRenderer {
	int32 activeIndex = 0;
    Vector3 windSpeed = { 0, 0, 0 };
	SnowflakeUpdateData* updateData;
	SnowflakeRenderData* renderData;
	
	void load(SnowflakeLoadParameters params, Renderer2d* renderer);o
	void update(float32 dtSeconds);
	void render(Renderer2d* renderer);
	void unload();
};

struct SnowflakeUpdateData {
	Vector3 velocity;
	Vector3 position;
};

struct SnowflakeRenderData {
	
};
