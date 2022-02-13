#include "Snowflake.h"
#include "Renderer2d.h"
#include "mathlib.h"
#include "List.h"

const Vector4 snowColor = Vector4(1.0, 0.98, 0.98, 1);

inline void generateSnowflakeShape(List<Renderer2dVertex>* vertices, int32 numArms, float32 radius, float32 innerRadius) {
	float32 dx = ((2 * PI) / numArms) / 3.0;
	for (int32 centerIdx = 0; centerIdx < (3 * numArms); centerIdx+=3) {
	    float32 degreeStart = dx * centerIdx;
		float32 degreeEnd = dx * (centerIdx + 1);

		float32 cosStart = cosf(degreeStart);
		float32 cosEnd = cosf(degreeEnd);

		float32 sinStart = sinf(degreeStart);
		float32 sinEnd = sinf(degreeEnd);

		Vector2 leftEnd = Vector2(radius * cosStart, radius * sinStart);
		Vector2 rightEnd = Vector2(radius * cosEnd, radius * sinEnd);
		Vector2 diff = (rightEnd - leftEnd) / 2.0;
		Vector2 leftStart = Vector2(-diff.x, -diff.y) + Vector2(innerRadius * cosStart, innerRadius * sinStart);
		Vector2 rightStart = diff + Vector2(innerRadius * cosEnd, innerRadius * sinEnd);

		vertices->add({ leftStart, snowColor, Mat4x4() });
		vertices->add({ leftEnd, snowColor, Mat4x4() });
		vertices->add({ rightEnd, snowColor, Mat4x4() });
		vertices->add({ rightEnd, snowColor, Mat4x4() });
		vertices->add({ rightStart, snowColor, Mat4x4() });
		vertices->add({ leftStart, snowColor, Mat4x4() });
	}
}

inline void initFlake(SnowflakeParticleRenderer* renderer, SnowflakeUpdateData* ud) {
	ud->vtxIdx = renderer->vertices.numElements;
	generateSnowflakeShape(&renderer->vertices, randomIntBetween(4, 16), randomFloatBetween(8.f, 16.f), randomFloatBetween(2.f, 6.f));
	ud->numVertices = renderer->vertices.numElements - ud->vtxIdx;
}

inline void spawnFlake(SnowflakeParticleRenderer* renderer, SnowflakeUpdateData* ud) {
	ud->velocity = Vector2(randomFloatBetween(-10, 10), randomFloatBetween(-100, -85));
	ud->position = Vector2(randomFloatBetween(0, renderer->xMax), randomFloatBetween(renderer->yMax, renderer->yMax + 256));
	ud->isAlive = true;
}

inline void findAndSpawnNextFlake(SnowflakeParticleRenderer* renderer) {
    do {
	    renderer->endIndex++;

		if (renderer->endIndex >= renderer->numSnowflakes)
			renderer->endIndex = 0;
	} while (renderer->updateData[renderer->endIndex].isAlive);

	spawnFlake(renderer, &renderer->updateData[renderer->endIndex]);
}

void SnowflakeParticleRenderer::load(SnowflakeLoadParameters params, Renderer2d* renderer) {
	startIndex = 0;
	spawnIntervalSeconds = params.spawnIntervalSeconds;
	endIndex = params.initialSnowflakeCount;
	numSnowflakes = params.maxSnowflakes;

	updateData = new SnowflakeUpdateData[params.maxSnowflakes];

	xMax = static_cast<float32>(renderer->context->width);
    yMax = static_cast<float32>(renderer->context->height);
	
	// Initialize each snow flake with its shape
	for (int32 s = 0; s < numSnowflakes; s++) {
		auto ud = &updateData[s];
	    initFlake(this, ud);

		if (s < endIndex) {
			spawnFlake(this, ud);
		}
	}

	useShader(renderer->shader);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.numElements * sizeof(Renderer2dVertex), &vertices.data[0], GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(renderer->attributes.position);
    glVertexAttribPointer(renderer->attributes.position, 2, GL_FLOAT, GL_FALSE, sizeof(Renderer2dVertex), (GLvoid *)0);

    glEnableVertexAttribArray(renderer->attributes.color);
    glVertexAttribPointer(renderer->attributes.color, 4, GL_FLOAT, GL_FALSE, sizeof(Renderer2dVertex), (GLvoid *)offsetof(Renderer2dVertex, color));

	for (int32 idx = 0; idx < 4; idx++) {
		int32 offset = (4 * sizeof(float32)) * idx;
		glEnableVertexAttribArray(renderer->attributes.vMatrix + idx);
		glVertexAttribPointer(renderer->attributes.vMatrix + idx,
							  4,
							  GL_FLOAT,
							  GL_FALSE,
							  sizeof(Renderer2dVertex),
							  (GLvoid *)(offsetof(Renderer2dVertex, vMatrix) + offset));
		//glVertexAttribDivisor(renderer->attributes.vMatrix + idx, 1);
	}

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

inline void updateFlake(SnowflakeParticleRenderer* renderer, SnowflakeUpdateData* ud, int32 s, float32 dtSeconds, bool addWind) {
	// Once the snowflake has been set to die in this interval, we try and increment the
	// startIndex
	if (!ud->isAlive && renderer->startIndex == s) {
		renderer->startIndex = (renderer->startIndex + 1) % renderer->numSnowflakes;
		return;
	}

	if (addWind) ud->velocity += renderer->windSpeed;		
	ud->position += ud->velocity * dtSeconds;

	if (ud->position.y < 0)
		ud->isAlive = false;

	Mat4x4 m = Mat4x4().translateByVec2(ud->position);
	for (int32 v = ud->vtxIdx; v < (ud->vtxIdx + ud->numVertices); v++) {
		renderer->vertices.data[v].vMatrix = m;
	}
}

void SnowflakeParticleRenderer::update(float32 dtSeconds) {
	timeUntilNextSpawnSeconds -= dtSeconds;
	if (timeUntilNextSpawnSeconds < 0) {
		timeUntilNextSpawnSeconds = spawnIntervalSeconds;
		findAndSpawnNextFlake(this);
	}

	bool addWind = false;
	timeUntilNextWindSeconds -= dtSeconds;
	if (timeUntilNextWindSeconds < 0) {
		timeUntilNextWindSeconds = windIntervalSeconds;
		windSpeed = Vector2(randomFloatBetween(-10, 10), randomFloatBetween(-10, 0));
		addWind = true;
	}

	if (startIndex < endIndex) {
		for (int32 s = startIndex; s < endIndex; s++) {
			SnowflakeUpdateData* ud = &updateData[s];
			updateFlake(this, ud, s, dtSeconds, addWind);
		}
	}
	else {
		int32 endRange = startIndex - numSnowflakes;
		for (int32 s = endIndex - 1; s >= endRange; s--) {
			SnowflakeUpdateData* ud;
			if (s < 0)
				ud = &updateData[numSnowflakes + s];
			else
				ud = &updateData[s];
			
			updateFlake(this, ud, s, dtSeconds, addWind);
		}
	}
}

void SnowflakeParticleRenderer::render(Renderer2d* renderer) {
	auto startVertex = &updateData[startIndex];
	auto endVertex = &updateData[endIndex];
	int32 numVertices = (endVertex->vtxIdx + endVertex->numVertices) - startVertex->vtxIdx;
	setShaderMat4(renderer->uniforms.model, model);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices * sizeof(Renderer2dVertex), &vertices.data[startVertex->vtxIdx]);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, numVertices);
    glBindVertexArray(0);
}

void SnowflakeParticleRenderer::unload() {
	glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
	vertices.deallocate();
	delete [] updateData;
}
