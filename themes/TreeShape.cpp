#include "TreeShape.h"
#include "mathlib.h"
#include <cstdio>
#include <cstdlib>


void TreeBranchLoadData::fillVertices(Renderer2dVertex* vertices, int branchTier) {
    bottomLeft = Vector2 { position.x - width / 2.f, position.y }.rotateAbout(rotation, position);
    bottomRight = Vector2 { position.x + width / 2.f, position.y }.rotateAbout(rotation, position);
    topLeft = (Vector2 { position.x - width / 2.f, position.y + height }).rotateAbout(rotation, position);
    topRight = (Vector2 { position.x + width / 2.f, position.y + height }).rotateAbout(rotation, position);

    topMidpoint = topLeft + (topRight - topLeft) / 2.f;

    vertices[0] = { bottomLeft, color, Vector4(0, 0, 0, 0) };
    vertices[1] = { bottomRight, color, Vector4(0, 0, 0, 0) };
    vertices[2] = { topLeft, color, Vector4(0, 0, 0, 0) };
    vertices[3] = { topLeft, color, Vector4(0, 0, 0, 0) };
    vertices[4] = { topRight, color, Vector4(0, 0, 0, 0) };
    vertices[5] = { bottomRight, color, Vector4(0, 0, 0, 0) };
};

inline float32 randomFloatBetween(float32 min, float32 max) {
    float32 random = static_cast<float32>(rand()) / static_cast<float32>(RAND_MAX);
    return (max - min) * random + min;
}

void TreeShape::load(Renderer2d* renderer) {
    TreeLoadData ld;

    numBranches = pow(ld.divisionsPerBranch, ld.numBranchLevels + 1);
    numVertices = 6 * numBranches;

    TreeBranchLoadData* generationData = new TreeBranchLoadData[numBranches];
    updateData = new TreeBranchUpdateData[numBranches];
    vertices = new Renderer2dVertex[numVertices];

    int32 branchIndex = 0;
    createBranch(&ld, generationData, numBranches, &branchIndex, 0, ld.trunkWidth, ld.trunkHeight, Vector2 { 400.f, 50.f }, 0, vertices);

    useShader(renderer->shader);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Renderer2dVertex), &vertices[0], GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(renderer->attributes.position);
    glVertexAttribPointer(renderer->attributes.position, 2, GL_FLOAT, GL_FALSE, sizeof(Renderer2dVertex), (GLvoid *)0);

    glEnableVertexAttribArray(renderer->attributes.color);
    glVertexAttribPointer(renderer->attributes.color, 4, GL_FLOAT, GL_FALSE, sizeof(Renderer2dVertex), (GLvoid *)offsetof(Renderer2dVertex, color));

    glEnableVertexAttribArray(renderer->attributes.transform);
	glVertexAttribPointer(renderer->attributes.transform, 4, GL_FLOAT, GL_FALSE, sizeof(Renderer2dVertex), (GLvoid *)offsetof(Renderer2dVertex, transform));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    delete [] generationData;
}

const float32 ninetyDegreeRotation = PI / 2.f;

void TreeShape::createBranch(TreeLoadData* ld, TreeBranchLoadData* generationData, int32 numBranches, int32* branchIndex, int32 branchLevel, float32 width, float32 height, Vector2 position, float32 rotation, Renderer2dVertex* vertices) {
    TreeBranchLoadData* branchLoadData = &generationData[*branchIndex];
    branchLoadData->width = width;
    branchLoadData->height = height;
    branchLoadData->position = position;
    branchLoadData->rotation = rotation;
    branchLoadData->fillVertices(&vertices[(*branchIndex) * 6], branchLevel);

    TreeBranchUpdateData* branchUpdateData = &updateData[*branchIndex];
    branchUpdateData->tier = branchLevel;
    branchUpdateData->randomOffset = randomFloatBetween(-1.f, 1.f);

    if (branchLevel == ld->numBranchLevels) {
        return;
    }
    
    for (int division = 0; division < ld->divisionsPerBranch; division++) {
        // Weight between [0, 1]
        float weight = static_cast<float32>(division) / static_cast<float32>(ld->divisionsPerBranch - 1);
        
        // Normalize the weight between [-1, 1]
        float32 normalizedWeight = (0.5f - (weight)) * 2.f;

        // We want a rotation that takes the current rotation of the branch, and averages it between the two branches.
        float32 branchRotationAmount = randomFloatBetween(PI / 8.f, PI / 3.f);
        float32 branchRotation =  branchLoadData->rotation + (normalizedWeight * branchRotationAmount);
        
        // Since trees are taller vertically, we will find a normalized value that describes how far the direction is from
        // being horizontal. If it is closer to 1, we will make the branch taller on average.
        float32 verticalHeightScaler = (fabs(fabs(branchRotation) - ninetyDegreeRotation) / ninetyDegreeRotation) * 0.1;
        float32 branchWidth = width * randomFloatBetween(ld->trunkWidthScalerMin, ld->trunkWidthScalerMax);
        float32 branchHeight = height * randomFloatBetween(ld->trunkHeightScalerMin + verticalHeightScaler, ld->trunkHeightScalerMax + verticalHeightScaler);


        // We want the branch to start within the previous branch, so we drop it down into it based off of the rotation.
        Vector2 branchOffsetVertical = Vector2{ 0, branchWidth }.rotate(branchRotation);
        
        Vector2 branchPosition = branchLoadData->topLeft + ((branchLoadData->topRight - branchLoadData->topLeft) * weight)  - branchOffsetVertical; // Position of branch along the top of the parent branch
        
        (*branchIndex)++;
        createBranch(ld, generationData, numBranches, branchIndex, branchLevel + 1, branchWidth, branchHeight, branchPosition, branchRotation, vertices);
    }
}

void TreeShape::update(float32 dtSeconds) {
    timeElapsedSeconds += dtSeconds;

    for (int32 bIdx = 0; bIdx < numBranches; bIdx++) {
        TreeBranchUpdateData* branchUpdataData = &updateData[bIdx];

        // Fade in simulation. We fade in based on the tier.
        float32 animationStart = (branchUpdataData->tier * animateStaggerPerTier);
        float32 animationEnd = animationStart + animateTimePerTier;

        float32 alpha = 0.f;
        if (timeElapsedSeconds < animationStart) {
            alpha = 0.f;
        }
        else if (timeElapsedSeconds > animationEnd) {
            alpha = 1.f;
        }
        else {
            alpha = (1.f - (animationEnd - timeElapsedSeconds)) / animateTimePerTier;
        }

        int startParentIndex = bIdx * 6;
        for (int32 sIdx = 0; sIdx < 6; sIdx++) {
            int32 vidx = startParentIndex + sIdx;
            vertices[vidx].color.w = alpha;

            // Wind simualtion
            vertices[vidx].position.x += ((branchUpdataData->randomOffset + branchUpdataData->tier) * 0.01f) * sinf(timeElapsedSeconds);
        }
    }
}
	
void TreeShape::render(Renderer2d* renderer) {
    setShaderMat4(renderer->uniforms.model, model);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices * sizeof(Renderer2dVertex), &vertices[0]);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, numVertices);
    glBindVertexArray(0);
}
	
void TreeShape::unload() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    delete[] vertices;
    delete [] updateData;
}
