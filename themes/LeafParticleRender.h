#include "Renderer2d.h"
#include "mathlib.h"
#include "types.h"

struct TreeShapeLoadResult;

struct LeafParticleLoadData {
    Vector2 initPosition;
    Vector4 initColor;
    int numLeaves = 48;
};

enum LeafParticleState {
    OnTree,
    Falling,
    OnGround,
    Remerging
};

struct LeafParticleUpdateData {
    LeafParticleState state = LeafParticleState::Remerging;

    Renderer2dVertex* vertexToFollow = NULL;
    Vector4 color = Vector4(1.f, 0.f, 0.f, 0.f);
    float32 scale = 1.f;

    float32 timeElapsedSeconds = 0.f;
    int32 fallChance = -1;
    Vector2 fallPosition;
    float32 fallVerticalVelocity;
    float32 fallHorizontalFrequency;
    
    float32 resetTime = 0.f;

    Renderer2dVertex* vertexPtr = NULL;
};

struct LeafParticleRender {
    float32 elapsedTimeSeconds = 0.5;
    float32 fallIntervalSeconds = 1.f;

    // Update data
    int32 numLeaves = 0;

    LeafParticleUpdateData* updateData = NULL;
    Renderer2dVertex* vertices = NULL;

    // Render data
	uint32 vao;
	uint32 vbo;
	uint32 numVertices = 0;
	Mat4x4 model;

    void load(Renderer2d* renderer, TreeShapeLoadResult* lr);
	void update(float32 dtSeconds);
	void render(Renderer2d* renderer);
	void unload();
};