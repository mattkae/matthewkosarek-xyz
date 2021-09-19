#include "Renderer2d.h"
#include "types.h"
#include "mathlib.h"

struct TreeBranch {
	float32 width = 0.f;
	float32 height = 0.f;
	Vector2 position; // Center point
	float32 rotation = 0; // How much we are rotated off of the center point in radians
	Vector4 color = Vector4 { 1.f, 0.f, 0.f, 1.f };

	void fillVertices(Renderer2dVertex* vertices) {
		Vector2 bottomLeft = Vector2 { position.x - width / 2.f, position.y };
		Vector2 bottomRight = Vector2 { position.x + width / 2.f, position.y };
		Vector2 topLeft = (Vector2 { position.x - width / 2.f, position.y + height }).rotate(rotation);
		Vector2 topRight = (Vector2 { position.x + width / 2.f, position.y + height }).rotate(rotation);

		vertices[0] = { bottomLeft, color };
		vertices[1] = { bottomRight, color };
		vertices[2] = { topLeft, color };
		vertices[3] = { topLeft, color };
		vertices[4] = { topRight, color };
	    vertices[5] = { bottomRight, color };
	}
};

struct TreeShape {
	Renderer2dShape shape;

	float32 height             = 100.f; // Height of the whole tree
	float32 width              = 50.f;  // Width of the whole tree
    int32 divisionsPerBranch   = 2;     // How many branches to split into at each branch split
    int32 numBranchLevels      = 8;     // How many branch levels to display
	float32 animateTimeSeconds = 2.f;   // How long it takes the tree to form

	void load(Renderer2d* renderer) {
		int32 numBranches = divisionsPerBranch * numBranchLevels;
		TreeBranch* branches = new TreeBranch[numBranches];
		int32 numVertices = 6 * numBranches;
		Renderer2dVertex* vertices = new Renderer2dVertex[numVertices];
		int32 branchIndex = 0;

		createBranch(branches, numBranches, &branchIndex, 0, width, height, Vector2 { 0.f, 0.f }, 0, vertices);

		shape.load(vertices, numVertices, renderer);

		delete[] branches;
		delete[] vertices;
	}

	void createBranch(TreeBranch* branchList, int32 numBranches, int32* branchIndex, int32 branchLevel, float32 width, float32 height, Vector2 position, float32 rotation, Renderer2dVertex* vertices) {
		TreeBranch* branch = &branchList[*branchIndex];
		branch->width = width;
		branch->height = height;
		branch->position = position;
		branch->rotation = rotation;
		branch->fillVertices(&vertices[(*branchIndex) * 6]);

		if (branchLevel == numBranchLevels) {
			return;
		}

		float32 branchWidth = width / divisionsPerBranch;
		float32 branchHeight = height / divisionsPerBranch;
		for (int division = 0; division < divisionsPerBranch; division++) {
			float32 branchXPosition = position.x + (((divisionsPerBranch - division) / (divisionsPerBranch - 1)) * width) - width / 2.0;
			Vector2 branchPosition = (Vector2 { branchXPosition, position.y + height }).rotate(rotation);
			float32 branchRotation = 0;
			(*branchIndex)++;
			createBranch(branchList, numBranches, branchIndex, branchLevel + 1, branchWidth, branchHeight, branchPosition, branchRotation, vertices);
		}
	}

	void update(float32 dtSeconds) {

	}
	
	void render(Renderer2d* renderer) {
		shape.render(renderer);
	}
	
	void unload() {
		shape.unload();
	}
};

