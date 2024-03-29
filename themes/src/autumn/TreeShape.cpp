#include "TreeShape.h"
#include "../mathlib.h"
#include <cstdio>
#include <cstdlib>
#include <cfloat>
#include <ctime>

void TreeBranchLoadData::fillVertices(Vertex2D* vertices, int branchTier) {
    bottomLeft = Vector2 { position.x - width / 2.f, position.y }.rotateAround(rotation, position);
    bottomRight = Vector2 { position.x + width / 2.f, position.y }.rotateAround(rotation, position);
    topLeft = (Vector2 { position.x - width / 2.f, position.y + height }).rotateAround(rotation, position);
    topRight = (Vector2 { position.x + width / 2.f, position.y + height }).rotateAround(rotation, position);

    topMidpoint = topLeft + (topRight - topLeft) / 2.f;

    vertices[0] = { bottomLeft, color};
    vertices[1] = { bottomRight, color};
    vertices[2] = { topLeft, color};
    vertices[3] = { topLeft, color};
    vertices[4] = { topRight, color};
    vertices[5] = { bottomRight, color};
};

TreeShapeLoadResult TreeShape::load(Renderer2d* renderer) {
    srand ( time(NULL) );
    
    timeElapsedSeconds = 0;

    TreeLoadData ld;

    numBranches = pow(ld.divisionsPerBranch, ld.numBranchLevels + 1);
    numVertices = 6 * numBranches;

    TreeBranchLoadData* generationData = new TreeBranchLoadData[numBranches];
    updateData = new TreeBranchUpdateData[numBranches];
    vertices = new Vertex2D[numVertices];

    // The load result will contain information that we can pass on to our leaf renderer.
    TreeShapeLoadResult lr;
    lr.lowerBounds = Vector2(FLT_MAX, FLT_MAX);
    lr.upperBounds = Vector2(FLT_MIN, FLT_MIN);
    lr.updateData = updateData;
    lr.numBranches = numBranches;
    i32 branchIndex = 0;
    createBranch(&ld, generationData, numBranches, &branchIndex, 0, ld.trunkWidth, ld.trunkHeight, Vector2 { 300.f, 50.f }, 0, NULL, vertices, &lr);

    useShader(renderer->shader);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex2D), &vertices[0], GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(renderer->attributes.position);
    glVertexAttribPointer(renderer->attributes.position, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (GLvoid *)0);

    glEnableVertexAttribArray(renderer->attributes.color);
    glVertexAttribPointer(renderer->attributes.color, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (GLvoid *)offsetof(Vertex2D, color));

	for (i32 idx = 0; idx < 4; idx++) {
		glEnableVertexAttribArray(renderer->attributes.vMatrix + idx);
		glVertexAttribPointer(renderer->attributes.vMatrix + idx, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (GLvoid *)(offsetof(Vertex2D, vMatrix) + (idx * 16)));
		glVertexAttribDivisor(renderer->attributes.vMatrix + idx, 1);
	}

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    delete [] generationData;

    return lr;
}

const f32 ninetyDegreeRotation = PI / 2.f;

void TreeShape::createBranch(TreeLoadData* ld, TreeBranchLoadData* generationData, i32 numBranches, i32* branchIndex, 
    i32 branchLevel, f32 width, f32 height, Vector2 position, f32 rotation, 
    TreeBranchUpdateData* parent, Vertex2D* vertices, TreeShapeLoadResult* lr) {
    TreeBranchLoadData* branchLoadData = &generationData[*branchIndex];
    branchLoadData->width = width;
    branchLoadData->height = height;
    branchLoadData->position = position;
    branchLoadData->rotation = rotation;
    branchLoadData->fillVertices(&vertices[(*branchIndex) * 6], branchLevel);

    // Fil in the bounds for the LeafRenderer later.
    if (branchLoadData->topMidpoint.x > lr->upperBounds.x) {
        lr->upperBounds.x = branchLoadData->topMidpoint.x;
    }
    if (branchLoadData->topMidpoint.y > lr->upperBounds.y) {
        lr->upperBounds.y = branchLoadData->topMidpoint.y;
    }
    if (branchLoadData->topMidpoint.x < lr->lowerBounds.x) {
        lr->lowerBounds.x = branchLoadData->topMidpoint.x;
    }
    if (branchLoadData->topMidpoint.y < lr->lowerBounds.y) {
        lr->lowerBounds.y = branchLoadData->topMidpoint.y;
    }

    TreeBranchUpdateData* branchUpdateData = &updateData[*branchIndex];
    branchUpdateData->tier = branchLevel;
    branchUpdateData->periodOffset = randomFloatBetween(0.f, 2.f * PI);
    branchUpdateData->period = randomFloatBetween(3.f, 5.f);
    branchUpdateData->amplitude = randomFloatBetween(0.01f, 0.05f);
    branchUpdateData->branchToFollow = parent;
    branchUpdateData->vertices = &vertices[(*branchIndex) * 6];

    if (branchLevel == ld->numBranchLevels) {
        return;
    }
    
    for (int division = 0; division < ld->divisionsPerBranch; division++) {
        // Weight between [0, 1]
        float weight = static_cast<f32>(division) / static_cast<f32>(ld->divisionsPerBranch - 1);
        
        // Normalize the weight between [-1, 1]
        f32 normalizedWeight = (0.5f - (weight)) * 2.f;

        // We want a rotation that takes the current rotation of the branch, and averages it between the two branches.
        f32 branchRotationAmount = randomFloatBetween(PI / 8.f, PI / 3.f);
        f32 branchRotation =  branchLoadData->rotation + (normalizedWeight * branchRotationAmount);
        
        // Since trees are taller vertically, we will find a normalized value that describes how far the direction is from
        // being horizontal. If it is closer to 1, we will make the branch taller on average.
        f32 verticalHeightScaler = (fabs(fabs(branchRotation) - ninetyDegreeRotation) / ninetyDegreeRotation) * 0.1;
        f32 branchWidth = width * randomFloatBetween(ld->trunkWidthScalerMin, ld->trunkWidthScalerMax);
        f32 branchHeight = height * randomFloatBetween(ld->trunkHeightScalerMin + verticalHeightScaler, ld->trunkHeightScalerMax + verticalHeightScaler);


        // We want the branch to start within the previous branch, so we drop it down into it based off of the rotation.
        Vector2 branchOffsetVertical = Vector2{ 0, branchWidth }.rotate(branchRotation);
        
        Vector2 branchPosition = branchLoadData->topLeft + ((branchLoadData->topRight - branchLoadData->topLeft) * weight)  - branchOffsetVertical; // Position of branch along the top of the parent branch
        
        (*branchIndex)++;
        createBranch(ld, generationData, numBranches, branchIndex, branchLevel + 1, branchWidth, branchHeight, branchPosition, branchRotation, branchUpdateData, vertices, lr);
    }
}

void TreeShape::update(f32 dtSeconds) {
    timeElapsedSeconds += dtSeconds;

    for (i32 bIdx = 0; bIdx < numBranches; bIdx++) {
        TreeBranchUpdateData* branchUpdataData = &updateData[bIdx];

        // Fade in simulation. We fade in based on the tier.
        f32 animationStart = (branchUpdataData->tier * animateStaggerPerTier);
        f32 animationEnd = animationStart + animateTimePerTier;

        f32 alpha = 0.f;
        if (timeElapsedSeconds < animationStart) {
            alpha = 0.f;
        }
        else if (timeElapsedSeconds > animationEnd) {
            alpha = 1.f;
        }
        else {
            alpha = (1.f - (animationEnd - timeElapsedSeconds)) / animateTimePerTier;
        }

        i32 startParentIndex = bIdx * 6;

        branchUpdataData->currentOffset.x = branchUpdataData->amplitude * cosf(branchUpdataData->periodOffset + branchUpdataData->period * timeElapsedSeconds);
        branchUpdataData->currentOffset.y = branchUpdataData->amplitude * sinf(branchUpdataData->periodOffset + branchUpdataData->period * timeElapsedSeconds);

        if (branchUpdataData->branchToFollow != NULL) {
            branchUpdataData->currentOffset += branchUpdataData->branchToFollow->currentOffset;
            
            // The root of the branch only moves according to the change of the end of the parent.
            branchUpdataData->vertices[0].color.w = alpha;
            branchUpdataData->vertices[0].position += branchUpdataData->branchToFollow->currentOffset;
            branchUpdataData->vertices[1].color.w = alpha;
            branchUpdataData->vertices[1].position += branchUpdataData->branchToFollow->currentOffset;
            branchUpdataData->vertices[5].color.w = alpha;
            branchUpdataData->vertices[5].position += branchUpdataData->branchToFollow->currentOffset;
        }
        else {
            branchUpdataData->vertices[0].color.w = alpha;
            branchUpdataData->vertices[1].color.w = alpha;
            branchUpdataData->vertices[5].color.w = alpha;
        }


        branchUpdataData->vertices[2].color.w = alpha;
        branchUpdataData->vertices[2].position += branchUpdataData->currentOffset;
        branchUpdataData->vertices[3].color.w = alpha;
        branchUpdataData->vertices[3].position += branchUpdataData->currentOffset;
        branchUpdataData->vertices[4].color.w = alpha;
        branchUpdataData->vertices[4].position += branchUpdataData->currentOffset;
    }
}
	
void TreeShape::render(Renderer2d* renderer) {
    setShaderMat4(renderer->uniforms.model, model);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices * sizeof(Vertex2D), &vertices[0]);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, numVertices);
    glBindVertexArray(0);
}
	
void TreeShape::unload() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    delete[] vertices;
    delete [] updateData;
    timeElapsedSeconds = 0;
    vertices = NULL;
    updateData = NULL;
}
