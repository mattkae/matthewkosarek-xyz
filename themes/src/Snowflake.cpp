#include "Snowflake.h"
#include "Renderer2d.h"
#include "mathlib.h"
#include "list.h"
#include <cstdio>

/*

  What else to do?

  - Windstream that blows a certain selection of snowflakes in a loop-dee-loop pattern
  - Snowflakes that land on the ground and melt
  - Snowflakes that spin along the Y-axis for a three dimensional effect
  
 */

const Vector4 snowColor = Vector4(1.0, 0.98, 0.98, 1);
const Vector2 NUM_ARMS_RANGE = Vector2(6.f, 8.f);
const Vector2 RADIUS_RANGE = Vector2(8.f, 32.f);
const Vector2 VELOCITY_RANGE_X = Vector2(-10.f, 10.f);
const Vector2 VELOCITY_RANGE_Y = Vector2(-100.f, -85.f);
const Vector2 ROTATION_VELOCITY_RANGE = Vector2(-PI / 8.f, PI / 8.f);
const Vector2 WIND_VELOCITY_RANGE_X = Vector2(-3.f, 3.f);
const Vector2 WIND_VELOCITY_RANGE_Y = Vector2(3.f, 10.f);
const f32 GRAVITY = 5.f;

inline void generateSnowflakeArm(f32 width, f32 height, f32 angle, matte::List<Vertex2D>* vertices, Mat4x4 transform = Mat4x4()) {
    f32 halfWidth = width / 2.f;
    Vector2 leftStart = transform * Vector2(-halfWidth, 0).rotate(angle);
    Vector2 leftEnd = transform * Vector2(-halfWidth, height).rotate(angle);
    Vector2 rightStart = transform * Vector2(halfWidth, 0).rotate(angle);
    Vector2 rightEnd = transform * Vector2(halfWidth, height).rotate(angle);

    vertices->add({ leftStart, snowColor, Mat4x4() });
    vertices->add({ leftEnd, snowColor, Mat4x4() });
    vertices->add({ rightEnd, snowColor, Mat4x4() });
    vertices->add({ leftStart, snowColor, Mat4x4() });
    vertices->add({ rightEnd, snowColor, Mat4x4() });
    vertices->add({ rightStart, snowColor, Mat4x4() });
}

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
        generateSnowflakeArm(armWidthRatio * radius, radius, armAngle, vertices);
        f32 armLeftAngle = DEG_TO_RAD(60.f);
        f32 armRightAngle = DEG_TO_RAD(-60.f);

        const i32 NUM_SUB_ARMS = 4;
        for (i32 subArmIndex = 0; subArmIndex < NUM_SUB_ARMS; subArmIndex++) {
            f32 height = (radius / static_cast<f32>(subArmIndex));
            f32 width = (armWidthRatio / (subArmIndex + 1)) * height;
            f32 transY = (radius / (NUM_SUB_ARMS + 1)) * (subArmIndex + 1);
            Vector2 translation = Vector2(0, transY).rotate(armAngle);
            generateSnowflakeArm(width, height, armAngle, vertices, Mat4x4().translateByVec2(translation).rotate2D(armLeftAngle));
            generateSnowflakeArm(width, height, armAngle, vertices, Mat4x4().translateByVec2(translation).rotate2D(armRightAngle));
        }
    }
}

inline void initFlake(SnowflakeParticleRenderer* renderer, SnowflakeUpdateData* ud) {
    ud->radius = randomFloatBetween(RADIUS_RANGE.x, RADIUS_RANGE.y);
	ud->vtxIdx = renderer->vertices.numElements;
	generateSnowflakeShape(&renderer->vertices,
                           randomFloatBetween(NUM_ARMS_RANGE.x, NUM_ARMS_RANGE.y),
                           ud->radius);
    
	ud->numVertices = renderer->vertices.numElements - ud->vtxIdx;
	ud->velocity = Vector2(randomFloatBetween(VELOCITY_RANGE_X.x, VELOCITY_RANGE_X.y), randomFloatBetween(VELOCITY_RANGE_Y.x, VELOCITY_RANGE_Y.y));
	ud->position = Vector2(randomFloatBetween(0, renderer->xMax), randomFloatBetween(renderer->yMax, 4 * renderer->yMax));
    ud->rotateVelocity = randomFloatBetween(ROTATION_VELOCITY_RANGE.x, ROTATION_VELOCITY_RANGE.y);
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
    ud->rotation = 0;
}

inline void updateFlake(SnowflakeParticleRenderer* renderer, SnowflakeUpdateData* ud, i32 s, f32 dtSeconds) {
    ud->velocity = ud->velocity + Vector2(0, -(GRAVITY * dtSeconds));
	if (addWind) ud->velocity += renderer->windSpeed;	
	ud->position += ud->velocity * dtSeconds;
    ud->rotation += ud->rotateVelocity * dtSeconds;

	Mat4x4 m = Mat4x4().translateByVec2(ud->position).rotate2D(ud->rotation);
	for (i32 v = ud->vtxIdx; v < (ud->vtxIdx + ud->numVertices); v++) {
		renderer->vertices.data[v].vMatrix = m;
	}

    if (ud->position.y <= -ud->radius) {
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
