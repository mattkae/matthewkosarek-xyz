#include "snowflake.h"
#include "../renderer_2d.h"
#include "../mathlib.h"
#include "../list.h"
#include "../shader.h"
#include "../shaders/snowflake_vert.h"
#include "../shaders/snowflake_frag.h"
#include <cstdio>

/*

  What else to do?

  - Windstream that blows a certain selection of snowflakes in a loop-dee-loop pattern
  - Snowflakes that land on the ground and melt
  - Snowflakes that spin along the Y-axis for a three dimensional effect

 */

const Vector2 SCALE_RANGE = Vector2(16.f, 48.f);
const Vector2 VELOCITY_RANGE_X = Vector2(-10.f, 10.f);
const Vector2 VELOCITY_RANGE_Y = Vector2(-100.f, -85.f);
const Vector2 ROTATION_VELOCITY_RANGE = Vector2(-PI / 8.f, PI / 8.f);
const Vector2 WIND_VELOCITY_RANGE_X = Vector2(-3.f, 3.f);
const Vector2 WIND_VELOCITY_RANGE_Y = Vector2(3.f, 10.f);
const f32 GRAVITY = 5.f;

inline void initFlake(SnowflakeParticleRenderer* renderer, SnowflakeUpdateData* ud) {
    ud->scale = randomFloatBetween(SCALE_RANGE.x, SCALE_RANGE.y);
    ud->seed = randomFloatBetween(0.f, 1000.f);

	// Scale velocity based on star size - larger stars fall faster
	f32 sizeRatio = (ud->scale - SCALE_RANGE.x) / (SCALE_RANGE.y - SCALE_RANGE.x);
	f32 velocityY = VELOCITY_RANGE_Y.x + (VELOCITY_RANGE_Y.y - VELOCITY_RANGE_Y.x) * (0.5f + sizeRatio * 0.5f);

	ud->velocity = Vector2(randomFloatBetween(VELOCITY_RANGE_X.x, VELOCITY_RANGE_X.y), velocityY);
	ud->position = Vector2(randomFloatBetween(0, renderer->xMax), randomFloatBetween(renderer->yMax, 4 * renderer->yMax));
    ud->rotateVelocity = randomFloatBetween(ROTATION_VELOCITY_RANGE.x, ROTATION_VELOCITY_RANGE.y);
    ud->rotation = 0.f;
}

void SnowflakeParticleRenderer::load(SnowflakeLoadParameters params, Renderer2d* renderer) {
	numSnowflakes = params.numSnowflakes;
	updateData = new SnowflakeUpdateData[params.numSnowflakes];

	xMax = static_cast<f32>(renderer->context->width);
    yMax = static_cast<f32>(renderer->context->height);

	// Initialize each snowflake
	for (i32 s = 0; s < numSnowflakes; s++) {
		auto ud = &updateData[s];
	    initFlake(this, ud);
	}

	// Load custom snowflake shader
	shader = loadShader(shader_snowflake_vert, shader_snowflake_frag);
	useShader(shader);

	// Get attribute and uniform locations
	attributes.position = getShaderAttribute(shader, "position");
	attributes.instancePos = getShaderAttribute(shader, "instancePos");
	attributes.instanceRot = getShaderAttribute(shader, "instanceRot");
	attributes.instanceScale = getShaderAttribute(shader, "instanceScale");
	attributes.instanceSeed = getShaderAttribute(shader, "instanceSeed");
	uniforms.projection = getShaderUniform(shader, "projection");
	uniforms.model = getShaderUniform(shader, "model");

	// Create base quad geometry (just a square from -1 to 1)
	Vector2 quadVertices[] = {
		Vector2(-1.0f, -1.0f),
		Vector2( 1.0f, -1.0f),
		Vector2( 1.0f,  1.0f),
		Vector2(-1.0f, -1.0f),
		Vector2( 1.0f,  1.0f),
		Vector2(-1.0f,  1.0f)
	};

	// Setup VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

	// Create and setup quad VBO (static geometry)
    glGenBuffers(1, &quadVbo);
    glBindBuffer(GL_ARRAY_BUFFER, quadVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(attributes.position);
    glVertexAttribPointer(attributes.position, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2), (GLvoid*)0);

	// Create instance VBO (dynamic data)
	glGenBuffers(1, &instanceVbo);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
	glBufferData(GL_ARRAY_BUFFER, numSnowflakes * sizeof(SnowflakeInstanceData), NULL, GL_DYNAMIC_DRAW);

	// Setup instance attributes
	glEnableVertexAttribArray(attributes.instancePos);
	glVertexAttribPointer(attributes.instancePos, 2, GL_FLOAT, GL_FALSE, sizeof(SnowflakeInstanceData), (GLvoid*)offsetof(SnowflakeInstanceData, position));
	glVertexAttribDivisor(attributes.instancePos, 1);

	glEnableVertexAttribArray(attributes.instanceRot);
	glVertexAttribPointer(attributes.instanceRot, 1, GL_FLOAT, GL_FALSE, sizeof(SnowflakeInstanceData), (GLvoid*)offsetof(SnowflakeInstanceData, rotation));
	glVertexAttribDivisor(attributes.instanceRot, 1);

	glEnableVertexAttribArray(attributes.instanceScale);
	glVertexAttribPointer(attributes.instanceScale, 1, GL_FLOAT, GL_FALSE, sizeof(SnowflakeInstanceData), (GLvoid*)offsetof(SnowflakeInstanceData, scale));
	glVertexAttribDivisor(attributes.instanceScale, 1);

	glEnableVertexAttribArray(attributes.instanceSeed);
	glVertexAttribPointer(attributes.instanceSeed, 1, GL_FLOAT, GL_FALSE, sizeof(SnowflakeInstanceData), (GLvoid*)offsetof(SnowflakeInstanceData, seed));
	glVertexAttribDivisor(attributes.instanceSeed, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

inline void resetFlake(SnowflakeParticleRenderer* renderer, SnowflakeUpdateData* ud) {
    ud->position.y = 2 * renderer->yMax;

	// Scale velocity based on star size - larger stars fall faster
	f32 sizeRatio = (ud->scale - SCALE_RANGE.x) / (SCALE_RANGE.y - SCALE_RANGE.x);
	f32 velocityY = VELOCITY_RANGE_Y.x + (VELOCITY_RANGE_Y.y - VELOCITY_RANGE_Y.x) * (0.5f + sizeRatio * 0.5f);

    ud->velocity = Vector2(randomFloatBetween(VELOCITY_RANGE_X.x, VELOCITY_RANGE_X.y), velocityY);
    ud->rotation = 0;
}

inline void updateFlake(SnowflakeParticleRenderer* renderer, SnowflakeUpdateData* ud, i32 s, f32 dtSeconds) {
    ud->velocity = ud->velocity + Vector2(0, -(GRAVITY * dtSeconds));
	//if (addWind) ud->velocity += renderer->windSpeed;
	ud->position += ud->velocity * dtSeconds;
    ud->rotation += ud->rotateVelocity * dtSeconds;

    if (ud->position.y <= -ud->scale) {
        resetFlake(renderer, ud);
    }
}

void SnowflakeParticleRenderer::update(f32 dtSeconds) {
	timeUntilNextWindSeconds -= dtSeconds;
	if (timeUntilNextWindSeconds < 0) {
		timeUntilNextWindSeconds = randomFloatBetween(2.5f, 10.f);
	}

    for (i32 s = 0; s < numSnowflakes; s++) {
        SnowflakeUpdateData* ud = &updateData[s];
        updateFlake(this, ud, s, dtSeconds);
    }
}

void SnowflakeParticleRenderer::render(Renderer2d* renderer) {
	useShader(shader);
	setShaderMat4(uniforms.projection, renderer->projection);
	setShaderMat4(uniforms.model, model);

	// Prepare instance data
	SnowflakeInstanceData* instanceData = new SnowflakeInstanceData[numSnowflakes];
	for (i32 s = 0; s < numSnowflakes; s++) {
		instanceData[s].position = updateData[s].position;
		instanceData[s].rotation = updateData[s].rotation;
		instanceData[s].scale = updateData[s].scale;
		instanceData[s].seed = updateData[s].seed;
	}

	// Upload instance data
    glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, numSnowflakes * sizeof(SnowflakeInstanceData), instanceData);

	// Draw instanced
    glBindVertexArray(vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, numSnowflakes);
    glBindVertexArray(0);

	delete[] instanceData;
}

void SnowflakeParticleRenderer::unload() {
	glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &quadVbo);
	glDeleteBuffers(1, &instanceVbo);
	glDeleteProgram(shader);
	vao = 0;
	quadVbo = 0;
	instanceVbo = 0;
	shader = 0;
	delete[] updateData;
}
