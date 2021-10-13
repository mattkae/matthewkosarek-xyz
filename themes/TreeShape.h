#include "Renderer2d.h"
#include "types.h"
#include "mathlib.h"

struct TreeLoadData {
	float32 trunkHeight        = 96.f; // Height of the trunk start
	float32 trunkWidth         = 32.f;  // Width of the trunk start
    float32 trunkHeightScalerMin  = 0.7f;
    float32 trunkHeightScalerMax  = 0.8f;
    float32 trunkWidthScalerMin   = 0.35f;
    float32 trunkWidthScalerMax   = 0.75f;
    int32 divisionsPerBranch   = 2;     // How many branches to split into at each branch split
    int32 numBranchLevels      = 8;     // How many branch levels to display
};

struct TreeBranchLoadData {
	float32 width = 0.f;
	float32 height = 0.f;
	Vector2 position; // Center point
	float32 rotation = 0; // How much we are rotated off of the center point in radians
	Vector4 color = Vector4(101,56,24, 1.f).toNormalizedColor();

    // Calculated while filling in vertices
    Vector2 bottomLeft;
    Vector2 bottomRight;
    Vector2 topLeft;
    Vector2 topRight;
    Vector2 topMidpoint;

	void fillVertices(Renderer2dVertex* vertices, int branchTier);
};

struct TreeBranchUpdateData {
	int32 tier = 0;
	float32 randomOffset = 0;
	Renderer2dVertex* vertices = NULL;
};

struct TreeShapeLoadResult {
	Vector2 lowerBounds;
	Vector2 upperBounds;
	Vector2 center;
	TreeBranchUpdateData* updateData;
	uint32 numBranches = 0;
};

struct TreeShape {
	// Update data
	TreeBranchUpdateData* updateData = NULL;
	Renderer2dVertex* vertices = NULL;
	float32 timeElapsedSeconds = 0.f;
	float32 animateTimePerTier = 1.f;
    float32 animateStaggerPerTier = 0.2f;
	uint32 numBranches = 0;

	// Render data
	uint32 vao;
	uint32 vbo;
	uint32 numVertices = 0;
	Mat4x4 model;

	TreeShapeLoadResult load(Renderer2d* renderer);
	void createBranch(TreeLoadData* ld, TreeBranchLoadData* branchList, int32 numBranches, 
		int32* branchIndex, int32 branchLevel, float32 width, float32 height, 
		Vector2 position, float32 rotation, Renderer2dVertex* vertices, TreeShapeLoadResult* lr);
	void update(float32 dtSeconds);
	void render(Renderer2d* renderer);
	void unload();
};

