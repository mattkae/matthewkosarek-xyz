#include "Snowflake.h"
#include "Renderer2d.h"
#include "mathlib.h"
#include "list.h"
#include <cstdio>

const Vector4 snowColor = Vector4(1.0, 0.98, 0.98, 1);

/**
   Fills in the vertices array vertices that represent a snowflake shape. The snowflake shape consists
   of numArms jutting out of the center radially. The center of the flake is connected. The radius is
   used to determine the length of the arms. The first third of each arm is barren, after which branches
   extends on either side of the arm at an angle of about 60 degrees. Each branch can itself have tiny
   sub branches jutting out of it, but these should be not nearly as large as the regular branches.

   With all of this in mind, we should be able to build a convincing snowflake.

   :param vertices List of vertices to be filled in
   :param numArms Number of arms radially sticking out of the snowflake
   :param radius Length of the snowflake arms
 */
inline void generateSnowflakeShape(matte::List<Vertex2D>* vertices, i32 numArms, f32 radius, f32 armWidthRatio = 0.08f) {
    f32 innerRadius = 0;
    f32 outerRadius = 2 * radius;
	f32 dx = ((2 * PI) / numArms);
    for (i32 armIndex = 0; armIndex < numArms; armIndex++) {
        f32 armAngle = dx * armIndex;
        Vector2 leftStart = Vector2(-armWidthRatio * radius, 0).rotate(armAngle);
        Vector2 leftEnd = Vector2(-armWidthRatio * radius, radius).rotate(armAngle);
        Vector2 rightStart = Vector2(armWidthRatio * radius, 0).rotate(armAngle);
        Vector2 rightEnd = Vector2(armWidthRatio * radius, radius).rotate(armAngle);
        
		vertices->add({ leftStart, snowColor, Mat4x4() });
		vertices->add({ leftEnd, snowColor, Mat4x4() });
		vertices->add({ rightEnd, snowColor, Mat4x4() });
		vertices->add({ leftStart, snowColor, Mat4x4() });
		vertices->add({ rightEnd, snowColor, Mat4x4() });
		vertices->add({ rightStart, snowColor, Mat4x4() });
    }
}

inline void initFlake(SnowflakeParticleRenderer* renderer, SnowflakeUpdateData* ud) {
	ud->vtxIdx = renderer->vertices.numElements;
	generateSnowflakeShape(&renderer->vertices,
                           6,
                           randomFloatBetween(8.f, 16.f));
    
	ud->numVertices = renderer->vertices.numElements - ud->vtxIdx;
	ud->velocity = Vector2(randomFloatBetween(-10, 10), randomFloatBetween(-100, -85));
	ud->position = Vector2(randomFloatBetween(0, renderer->xMax), randomFloatBetween(renderer->yMax, 4 * renderer->yMax));
    ud->rotateVelocity = randomFloatBetween(-PI / 8.f, PI / 8.f);
}

void SnowflakeParticleRenderer::load(SnowflakeLoadParameters params, Renderer2d* renderer) {
	numSnowflakes = params.numSnowflakes;

	updateData = new SnowflakeUpdateData[params.numSnowflakes];

	xMax = static_cast<f32>(renderer->context->width);
    yMax = static_cast<f32>(renderer->context->height);

    vertices.deallocate();
    vertices.growDynamically = true;
    
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
	}

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

inline void resetFlake(SnowflakeParticleRenderer* renderer, SnowflakeUpdateData* ud) {
    ud->position.y = 2 * renderer->yMax;
    ud->velocity = Vector2(randomFloatBetween(-10, 10), randomFloatBetween(-100, -85));
}

inline void updateFlake(SnowflakeParticleRenderer* renderer, SnowflakeUpdateData* ud, i32 s, f32 dtSeconds, bool addWind) {
	if (addWind) ud->velocity += renderer->windSpeed;		
	ud->position += ud->velocity * dtSeconds;
    ud->rotation += ud->rotateVelocity * dtSeconds;

	Mat4x4 m = Mat4x4().translateByVec2(ud->position).rotate2D(ud->rotation);
	for (i32 v = ud->vtxIdx; v < (ud->vtxIdx + ud->numVertices); v++) {
		renderer->vertices.data[v].vMatrix = m;
	}

    if (ud->position.y <= -256) {
        resetFlake(renderer, ud);
    }
}

void SnowflakeParticleRenderer::update(f32 dtSeconds) {
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
