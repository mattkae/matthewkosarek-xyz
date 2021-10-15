#include "LeafParticleRender.h"
#include "Renderer2d.h"
#include "mathlib.h"
#include "TreeShape.h"
#include "types.h"
#include <math.h>

const int32 verticesPerLeaf = 6; 
const float32 leafRadius = 3.f;
const int32 fallChanceMax = 100;

inline void updateLeaf(Renderer2dVertex* vertices, Vector2 position, Vector4 color) {
    Vector2 bottomLeft = Vector2(-leafRadius, -leafRadius) + position;
    Vector2 bottomRight = Vector2(leafRadius, -leafRadius) + position;
    Vector2 topLeft = Vector2(-leafRadius, leafRadius) + position;
    Vector2 topRight = Vector2(leafRadius, leafRadius) + position;

    vertices[0] = { bottomLeft, color };
    vertices[1] = { bottomRight, color };
    vertices[2] = { topLeft, color };
    vertices[3] = { topLeft, color };
    vertices[4] = { topRight, color };
    vertices[5] = { bottomRight, color };
}

void LeafParticleRender::load(Renderer2d *renderer, TreeShapeLoadResult* lr) {
    LeafParticleLoadData ld;
    ld.numLeaves = 640;
    numLeaves = ld.numLeaves;
    numVertices = ld.numLeaves * verticesPerLeaf;

    updateData = new LeafParticleUpdateData[numLeaves];
    vertices = new Renderer2dVertex[numVertices];

    for (int32 leafIdx = 0; leafIdx < numLeaves; leafIdx++) {
        int32 randomBranch = randomIntBetween(0, lr->numBranches);
        int32 randomVertex = randomIntBetween(0, 6); // TODO: Manually entering num vertices per branch.
        updateData[leafIdx].vertexToFollow = &lr->updateData[randomBranch].vertices[randomVertex];
        updateData[leafIdx].fallChance = randomIntBetween(0, fallChanceMax);
        updateData[leafIdx].color = Vector4(randomFloatBetween(0.3, 0.9), randomFloatBetween(0.1, 0.6), 0, 1);
        updateData[leafIdx].vertexPtr = &vertices[leafIdx * verticesPerLeaf];
    }

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

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void LeafParticleRender::update(float32 dtSeconds) {
    elapsedTimeSeconds += dtSeconds;

    // Every time the fallIntervalSeconds passes, we remove one leaf
    // from the tree and send it barrelling towards the earth.
    int32 fallRoll;
    bool didGenerateFall = false;
    if (elapsedTimeSeconds >= fallIntervalSeconds) {
        fallRoll = randomIntBetween(0, fallChanceMax);
        didGenerateFall = true;
        elapsedTimeSeconds = 0;
    }

    for (int32 leafIdx = 0; leafIdx < numLeaves; leafIdx++) {
        auto updateDataItem = &updateData[leafIdx];

        if (didGenerateFall) {
            if (!updateDataItem->isFalling && updateDataItem->fallChance == fallRoll) {
                updateDataItem->isFalling = true;
                updateDataItem->fallPosition = updateDataItem->vertexToFollow->position;
                updateDataItem->fallVerticalVelocity = -randomFloatBetween(15.f, 25.f);
                updateDataItem->fallHorizontalFrequency = randomFloatBetween(3.f, 5.f);
            }
        }

        if (updateDataItem->onGround) {
            updateDataItem->timeFallingSeconds += dtSeconds;

            if (updateDataItem->timeFallingSeconds >= updateDataItem->resetTime) {
                updateDataItem->onGround = false;
                updateDataItem->isFalling = false;
                updateDataItem->color.w = 1.f;
            }
            else {
                updateDataItem->color.w = 1.f - (updateDataItem->timeFallingSeconds / updateDataItem->resetTime);
                updateLeaf(updateDataItem->vertexPtr, updateDataItem->fallPosition, updateDataItem->color);
            }
        }
        else if (updateDataItem->isFalling) {
            updateDataItem->timeFallingSeconds += dtSeconds;
            const float32 xPosUpdate = cosf(updateDataItem->fallHorizontalFrequency * updateDataItem->timeFallingSeconds);
            updateDataItem->fallPosition.x += xPosUpdate;
            updateDataItem->fallPosition.y += updateDataItem->fallVerticalVelocity * dtSeconds;
            if (updateDataItem->fallPosition.y <= 25.f) { // TODO: Hardcoded ground for now
                updateDataItem->fallPosition.y = 25.f;
                updateDataItem->onGround = true;
                updateDataItem->timeFallingSeconds = 0;
                updateDataItem->resetTime = randomFloatBetween(2.f, 5.f); // TODO: Hardcoded reset interval
            }
            updateLeaf(updateDataItem->vertexPtr, updateDataItem->fallPosition, updateDataItem->color);
        }
        else {
            updateLeaf(updateDataItem->vertexPtr, updateDataItem->vertexToFollow->position, updateDataItem->color);
        }
    }
}

void LeafParticleRender::render(Renderer2d *renderer) {
    setShaderMat4(renderer->uniforms.model, model);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices * sizeof(Renderer2dVertex), &vertices[0]);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, numVertices);
    glBindVertexArray(0);
}

void LeafParticleRender::unload() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    delete [] vertices;

    elapsedTimeSeconds = 0;
}