#include "Snowflake.h"
#include "Renderer2d.h"
#include "mathlib.h"
#include "list.h"

const Vector4 snowColor = Vector4(1.0, 0.98, 0.98, 1);

inline void generateSnowflakeShape(matte::List<Vertex2D>* vertices, i32 numArms, f32 radius, f32 innerRadius) {
	f32 dx = ((2 * PI) / numArms) / 3.0;
	for (i32 centerIdx = 0; centerIdx < (3 * numArms); centerIdx+=3) {
	    f32 degreeStart = dx * centerIdx;
		f32 degreeEnd = dx * (centerIdx + 1);

		f32 cosStart = cosf(degreeStart);
		f32 cosEnd = cosf(degreeEnd);

		f32 sinStart = sinf(degreeStart);
		f32 sinEnd = sinf(degreeEnd);

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
    ud->isAlive = false;
}

inline void spawnFlake(SnowflakeParticleRenderer* renderer, SnowflakeUpdateData* ud) {
	ud->velocity = Vector2(randomFloatBetween(-10, 10), randomFloatBetween(-100, -85));
	ud->position = Vector2(randomFloatBetween(0, renderer->xMax), randomFloatBetween(renderer->yMax, renderer->yMax + 256));
	ud->isAlive = true;
}

inline void findAndSpawnNextFlake(SnowflakeParticleRenderer* renderer) {
    for (i32 i = 0; i < renderer->numSnowflakes; i++) {
        if (!renderer->updateData[i].isAlive) {
            spawnFlake(renderer, &renderer->updateData[i]);
            break;
        }
    }
}

void SnowflakeParticleRenderer::load(SnowflakeLoadParameters params, Renderer2d* renderer) {
	spawnIntervalSeconds = params.spawnIntervalSeconds;
	numSnowflakes = params.maxSnowflakes;

	updateData = new SnowflakeUpdateData[params.maxSnowflakes];

	xMax = static_cast<f32>(renderer->context->width);
    yMax = static_cast<f32>(renderer->context->height);

	// Initialize each snow flake with its shape
	for (i32 s = 0; s < numSnowflakes; s++) {
		auto ud = &updateData[s];
	    initFlake(this, ud);
	}

	useShader(renderer->shader);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.numElements * sizeof(Vertex2D), &vertices.data[0], GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(renderer->attributes.position);
    glVertexAttribPointer(renderer->attributes.position, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (GLvoid *)0);

    glEnableVertexAttribArray(renderer->attributes.color);
    glVertexAttribPointer(renderer->attributes.color, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (GLvoid *)offsetof(Vertex2D, color));

	for (i32 idx = 0; idx < 4; idx++) {
		i32 offset = (4 * sizeof(f32)) * idx;
		glEnableVertexAttribArray(renderer->attributes.vMatrix + idx);
		glVertexAttribPointer(renderer->attributes.vMatrix + idx,
							  4,
							  GL_FLOAT,
							  GL_FALSE,
							  sizeof(Vertex2D),
							  (GLvoid *)(offsetof(Vertex2D, vMatrix) + offset));
		//glVertexAttribDivisor(renderer->attributes.vMatrix + idx, 1);
	}

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

inline void updateFlake(SnowflakeParticleRenderer* renderer, SnowflakeUpdateData* ud, i32 s, f32 dtSeconds, bool addWind) {
	if (!ud->isAlive) {
		return;
	}

	if (addWind) ud->velocity += renderer->windSpeed;		
	ud->position += ud->velocity * dtSeconds;

	if (ud->position.y < 0) {
		ud->isAlive = false;
    }

	Mat4x4 m = Mat4x4().translateByVec2(ud->position);
	for (i32 v = ud->vtxIdx; v < (ud->vtxIdx + ud->numVertices); v++) {
		renderer->vertices.data[v].vMatrix = m;
	}
}

void SnowflakeParticleRenderer::update(f32 dtSeconds) {
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

    for (i32 s = 0; s < numSnowflakes; s++) {
        SnowflakeUpdateData* ud = &updateData[s];
        updateFlake(this, ud, s, dtSeconds, addWind);
    }
}

void SnowflakeParticleRenderer::render(Renderer2d* renderer) {
	setShaderMat4(renderer->uniforms.model, model);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.numElements * sizeof(Vertex2D), &vertices.data[0]);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertices.numElements);
    glBindVertexArray(0);
}

void SnowflakeParticleRenderer::unload() {
	glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
	vao = 0;
	vbo = 0;
	vertices.deallocate();
	delete [] updateData;
}
