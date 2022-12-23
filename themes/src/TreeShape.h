#include "Renderer2d.h"
#include "types.h"
#include "mathlib.h"

struct TreeLoadData {
	f32 trunkHeight        = 96.f; // Height of the trunk start
	f32 trunkWidth         = 32.f;  // Width of the trunk start
    f32 trunkHeightScalerMin  = 0.7f;
    f32 trunkHeightScalerMax  = 0.8f;
    f32 trunkWidthScalerMin   = 0.35f;
    f32 trunkWidthScalerMax   = 0.75f;
    i32 divisionsPerBranch   = 2;     // How many branches to split into at each branch split
    i32 numBranchLevels      = 8;     // How many branch levels to display
};

struct TreeBranchLoadData {
	f32 width = 0.f;
	f32 height = 0.f;
	Vector2 position; // Center point
	f32 rotation = 0; // How much we are rotated off of the center point in radians
	Vector4 color = Vector4(101,56,24, 1.f).toNormalizedColor();

    // Calculated while filling in vertices
    Vector2 bottomLeft;
    Vector2 bottomRight;
    Vector2 topLeft;
    Vector2 topRight;
    Vector2 topMidpoint;

	void fillVertices(Vertex2D* vertices, int branchTier);
};

struct TreeBranchUpdateData {
	i32 tier = 0;
	f32 periodOffset = 0;
	f32 period = 0;
	f32 amplitude = 0;
	Vector2 currentOffset;
	Vertex2D* vertices = NULL;
	TreeBranchUpdateData* branchToFollow = NULL;
};

struct TreeShapeLoadResult {
	Vector2 lowerBounds;
	Vector2 upperBounds;
	Vector2 center;
	TreeBranchUpdateData* updateData;
	u32 numBranches = 0;
};

struct TreeShape {
	// Update data
	TreeBranchUpdateData* updateData = NULL;
	Vertex2D* vertices = NULL;
	f32 timeElapsedSeconds = 0.f;
	f32 animateTimePerTier = 1.f;
    f32 animateStaggerPerTier = 0.2f;
	u32 numBranches = 0;

	// Render data
	u32 vao;
	u32 vbo;
	u32 numVertices = 0;
	Mat4x4 model;

	TreeShapeLoadResult load(Renderer2d* renderer);
	void createBranch(TreeLoadData* ld, TreeBranchLoadData* branchList, i32 numBranches, 
		i32* branchIndex, i32 branchLevel, f32 width, f32 height, 
		Vector2 position, f32 rotation, TreeBranchUpdateData* parent, Vertex2D* vertices, TreeShapeLoadResult* lr);
	void update(f32 dtSeconds);
	void render(Renderer2d* renderer);
	void unload();
};

