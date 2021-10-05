#include "Renderer2d.h"
#include "mathlib.h"
#include "types.h"

struct TreeShape;

struct LeafParticleLoadData {
    Vector2 initPosition;
    Vector4 initColor;
    int numLeaves = 48;
};

struct LeafParticleUpdateData {
    bool canFall = false;
    Vector4 color = Vector4(1.f, 0.f, 0.f, 0.f);
    float32 scale = 1.f;
    Vector2 fallDirection;
};

struct LeafParticleRender {
    // Update data
    int32 numLeaves = 0;
    Renderer2dVertex* vertices = NULL;

    // Render data
	uint32 vao;
	uint32 vbo;
	uint32 numVertices = 0;
	Mat4x4 model;

    void load(Renderer2d* renderer, TreeShape* tree);
	void update(float32 dtSeconds);
	void render(Renderer2d* renderer);
	void unload();
};