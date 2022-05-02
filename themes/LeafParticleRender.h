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

    Vertex2D* vertexToFollow = NULL;
    Vector4 color = Vector4(1.f, 0.f, 0.f, 0.f);
    f32 scale = 1.f;

    f32 timeElapsedSeconds = 0.f;
    i32 fallChance = -1;
    Vector2 fallPosition;
    f32 fallVerticalVelocity;
    f32 fallHorizontalFrequency;
    
    f32 resetTime = 0.f;

    Vertex2D* vertexPtr = NULL;
};

struct LeafParticleRender {
    f32 elapsedTimeSeconds = 0.5;
    f32 fallIntervalSeconds = 1.f;

    // Update data
    i32 numLeaves = 0;

    LeafParticleUpdateData* updateData = NULL;
    Vertex2D* vertices = NULL;

    // Render data
	u32 vao;
	u32 vbo;
	u32 numVertices = 0;
	Mat4x4 model;

    void load(Renderer2d* renderer, TreeShapeLoadResult* lr);
	void update(f32 dtSeconds);
	void render(Renderer2d* renderer);
	void unload();
};