#include "LeafParticleRender.h"
#include "Renderer2d.h"
#include "mathlib.h"
#include "TreeShape.h"
#include "types.h"
#include <math.h>

const i32 verticesPerLeaf = 6; 
const f32 leafRadius = 3.f;
const i32 fallChanceMax = 100;

inline void updateLeaf(Vertex2D* vertices, Vector2 position, Vector4 color, f32 scale) {
    f32 radius = scale * leafRadius;
    Vector2 bottomLeft = Vector2(-radius, -radius) + position;
    Vector2 bottomRight = Vector2(radius, -radius) + position;
    Vector2 topLeft = Vector2(-radius, radius) + position;
    Vector2 topRight = Vector2(radius, radius) + position;

    vertices[0] = { bottomLeft, color, Mat4x4() };
    vertices[1] = { bottomRight, color, Mat4x4() };
    vertices[2] = { topLeft, color, Mat4x4() };
    vertices[3] = { topLeft, color, Mat4x4() };
    vertices[4] = { topRight, color, Mat4x4() };
    vertices[5] = { bottomRight, color, Mat4x4() };
}

void LeafParticleRender::load(Renderer2d *renderer, TreeShapeLoadResult* lr) {
    LeafParticleLoadData ld;
    ld.numLeaves = 256;
    numLeaves = ld.numLeaves;
    numVertices = ld.numLeaves * verticesPerLeaf;

    updateData = new LeafParticleUpdateData[numLeaves];
    vertices = new Vertex2D[numVertices];

    for (i32 leafIdx = 0; leafIdx < numLeaves; leafIdx++) {
        i32 randomBranch = randomIntBetween(0, lr->numBranches);
        i32 randomVertex = randomIntBetween(0, 6); // TODO: Manually entering num vertices per branch.
        updateData[leafIdx].vertexToFollow = &lr->updateData[randomBranch].vertices[randomVertex];
        updateData[leafIdx].fallChance = randomIntBetween(0, fallChanceMax);
        updateData[leafIdx].color = Vector4(randomFloatBetween(0.3, 0.9), randomFloatBetween(0.1, 0.6), 0, 1);
        updateData[leafIdx].vertexPtr = &vertices[leafIdx * verticesPerLeaf];
        updateData[leafIdx].resetTime = randomFloatBetween(4.f, 6.f);
    }

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
	}
	
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void LeafParticleRender::update(f32 dtSeconds) {
    elapsedTimeSeconds += dtSeconds;

    // Every time the fallIntervalSeconds passes, we remove one leaf
    // from the tree and send it barrelling towards the earth.
    i32 fallRoll;
    bool didGenerateFall = false;
    if (elapsedTimeSeconds >= fallIntervalSeconds) {
        fallRoll = randomIntBetween(0, fallChanceMax);
        didGenerateFall = true;
        elapsedTimeSeconds = 0;
    }

    for (i32 leafIdx = 0; leafIdx < numLeaves; leafIdx++) {
        auto updateDataItem = &updateData[leafIdx];

        if (didGenerateFall) {
            if (updateDataItem->state == LeafParticleState::OnTree && updateDataItem->fallChance == fallRoll) {
                updateDataItem->state = LeafParticleState::Falling;
                updateDataItem->fallPosition = updateDataItem->vertexToFollow->position;
                updateDataItem->fallVerticalVelocity = -randomFloatBetween(15.f, 25.f);
                updateDataItem->fallHorizontalFrequency = randomFloatBetween(3.f, 5.f);
            }
        }

        switch (updateDataItem->state) {
        case (LeafParticleState::Remerging): {
            updateDataItem->timeElapsedSeconds += dtSeconds;

            if (updateDataItem->timeElapsedSeconds >= updateDataItem->resetTime) {
                updateDataItem->timeElapsedSeconds = 0.f;
                updateDataItem->state = LeafParticleState::OnTree;
                updateDataItem->color.w = 1.f;
                updateDataItem->scale = 1.f;
            }
            else {
                updateDataItem->color.w = (updateDataItem->timeElapsedSeconds / updateDataItem->resetTime);
                updateDataItem->scale = (updateDataItem->timeElapsedSeconds / updateDataItem->resetTime);
            }

            updateLeaf(updateDataItem->vertexPtr, updateDataItem->vertexToFollow->position, updateDataItem->color, updateDataItem->scale);
            break;
        }
        case (LeafParticleState::OnGround): {
            updateDataItem->timeElapsedSeconds += dtSeconds;

            if (updateDataItem->timeElapsedSeconds >= updateDataItem->resetTime) {
                updateDataItem->timeElapsedSeconds = 0.f;
                updateDataItem->color.w = 0.f;
                updateDataItem->state = LeafParticleState::Remerging;
            }
            else {
                updateDataItem->color.w = 1.f - (updateDataItem->timeElapsedSeconds / updateDataItem->resetTime);
                updateLeaf(updateDataItem->vertexPtr, updateDataItem->fallPosition, updateDataItem->color, updateDataItem->scale);
            }
            break;
        }
        case (LeafParticleState::Falling): {
            updateDataItem->timeElapsedSeconds += dtSeconds;
            const f32 xPosUpdate = cosf(updateDataItem->fallHorizontalFrequency * updateDataItem->timeElapsedSeconds);
            updateDataItem->fallPosition.x += xPosUpdate;
            updateDataItem->fallPosition.y += updateDataItem->fallVerticalVelocity * dtSeconds;
            if (updateDataItem->fallPosition.y <= 50.f) { // TODO: Hardcoded ground for now
                updateDataItem->fallPosition.y = 50.f;
                updateDataItem->state = LeafParticleState::OnGround;
                updateDataItem->timeElapsedSeconds = 0;
                updateDataItem->resetTime = randomFloatBetween(2.f, 5.f); // TODO: Hardcoded reset interval
            }
            updateLeaf(updateDataItem->vertexPtr, updateDataItem->fallPosition, updateDataItem->color, updateDataItem->scale);
            break;
        }
        case (LeafParticleState::OnTree): {
            updateLeaf(updateDataItem->vertexPtr, updateDataItem->vertexToFollow->position, updateDataItem->color, updateDataItem->scale);
            break;
        }
        }
    }
}

void LeafParticleRender::render(Renderer2d *renderer) {
    setShaderMat4(renderer->uniforms.model, model);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices * sizeof(Vertex2D), &vertices[0]);

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
