#include "grass_renderer.hpp"
#include "../renderer_3d.h"
#include "../shader.h"
#include "../shaders/grass_frag.h"
#include "../shaders/grass_vert.h"
#include "mathlib.h"
#include <cmath>
#include <cstddef>

void GrassRenderer::load(GrassRendererLoadData params, Renderer3d *renderer) {
  bladeHeight = params.grassHeight;

  // Place blades randomly within a circle. Using r = R*sqrt(u) with a
  // uniform u in [0,1] gives uniform areal density (no center clustering).
  const f32 radius = fminf(params.area.x, params.area.y) * 0.5f;
  for (i32 i = 0; i < NUM_GRASS_BLADES; i++) {
    f32 r = radius * sqrtf(randomFloatBetween(0.f, 1.f));
    f32 theta = randomFloatBetween(0.f, 2.f * PI);
    f32 x = params.origin.x + r * cosf(theta);
    f32 z = params.origin.y + r * sinf(theta);
    grassBlades[i].position = Vector3(x, 0, z);
    grassBlades[i].top_offset =
        Vector2(randomFloatBetween(0.f, 2.f * PI), // sway phase
                randomFloatBetween(0.5f, 1.5f)     // height scale
        );
  }

  // Compile grass shader
  shader = loadShader(shader_grass_vert, shader_grass_frag);
  useShader(shader);

  // Attribute locations
  attributes.position = getShaderAttribute(shader, "position");
  attributes.instancePos = getShaderAttribute(shader, "instancePos");
  attributes.instancePhase = getShaderAttribute(shader, "instancePhase");
  attributes.instanceHeight = getShaderAttribute(shader, "instanceHeight");

  // Uniform locations
  uniforms.projection = getShaderUniform(shader, "projection");
  uniforms.view = getShaderUniform(shader, "view");
  uniforms.time = getShaderUniform(shader, "time");
  uniforms.bladeWidth = getShaderUniform(shader, "bladeWidth");
  uniforms.bladeHeight = getShaderUniform(shader, "bladeHeight");
  uniforms.swayAmount = getShaderUniform(shader, "swayAmount");

  // Base quad: two triangles forming a unit quad
  // x in [-0.5, 0.5], y in [0, 1]
  Vector2 quadVertices[] = {Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f),
                            Vector2(0.5f, 1.0f),  Vector2(-0.5f, 0.0f),
                            Vector2(0.5f, 1.0f),  Vector2(-0.5f, 1.0f)};

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Static quad VBO
  glGenBuffers(1, &quadVbo);
  glBindBuffer(GL_ARRAY_BUFFER, quadVbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(attributes.position);
  glVertexAttribPointer(attributes.position, 2, GL_FLOAT, GL_FALSE,
                        sizeof(Vector2), (GLvoid *)0);

  // Dynamic instance VBO
  glGenBuffers(1, &instanceVbo);
  glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
  glBufferData(GL_ARRAY_BUFFER, NUM_GRASS_BLADES * sizeof(GrassInstanceData),
               NULL, GL_DYNAMIC_DRAW);

  // instancePos: vec3 (x, y, z) at offset 0
  glEnableVertexAttribArray(attributes.instancePos);
  glVertexAttribPointer(attributes.instancePos, 3, GL_FLOAT, GL_FALSE,
                        sizeof(GrassInstanceData),
                        (GLvoid *)offsetof(GrassInstanceData, x));
  glVertexAttribDivisor(attributes.instancePos, 1);

  // instancePhase: float at offset 12 (after 3 floats)
  glEnableVertexAttribArray(attributes.instancePhase);
  glVertexAttribPointer(attributes.instancePhase, 1, GL_FLOAT, GL_FALSE,
                        sizeof(GrassInstanceData),
                        (GLvoid *)offsetof(GrassInstanceData, phaseOffset));
  glVertexAttribDivisor(attributes.instancePhase, 1);

  // instanceHeight: float at offset 16 (after phaseOffset)
  glEnableVertexAttribArray(attributes.instanceHeight);
  glVertexAttribPointer(attributes.instanceHeight, 1, GL_FLOAT, GL_FALSE,
                        sizeof(GrassInstanceData),
                        (GLvoid *)offsetof(GrassInstanceData, heightScale));
  glVertexAttribDivisor(attributes.instanceHeight, 1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void GrassRenderer::update(f32 dtSeconds) { time += dtSeconds; }

void GrassRenderer::render(Renderer3d *renderer) {
  useShader(shader);
  setShaderMat4(uniforms.projection, renderer->projection);
  setShaderMat4(uniforms.view, renderer->view);
  setShaderFloat(uniforms.time, time);
  setShaderFloat(uniforms.bladeWidth, bladeWidth);
  setShaderFloat(uniforms.bladeHeight, bladeHeight);
  setShaderFloat(uniforms.swayAmount, swayAmount);

  // Build and upload instance data
  GrassInstanceData instanceData[NUM_GRASS_BLADES];
  for (i32 i = 0; i < NUM_GRASS_BLADES; i++) {
    instanceData[i].x = grassBlades[i].position.x;
    instanceData[i].y = grassBlades[i].position.y;
    instanceData[i].z = grassBlades[i].position.z;
    instanceData[i].phaseOffset = grassBlades[i].top_offset.x;
    instanceData[i].heightScale = grassBlades[i].top_offset.y;
  }

  glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0,
                  NUM_GRASS_BLADES * sizeof(GrassInstanceData), instanceData);

  glBindVertexArray(vao);
  glDrawArraysInstanced(GL_TRIANGLES, 0, 6, NUM_GRASS_BLADES);
  glBindVertexArray(0);
}

void GrassRenderer::unload() {
  if (vao)
    glDeleteVertexArrays(1, &vao);
  if (quadVbo)
    glDeleteBuffers(1, &quadVbo);
  if (instanceVbo)
    glDeleteBuffers(1, &instanceVbo);
  if (shader)
    glDeleteProgram(shader);
  vao = 0;
  quadVbo = 0;
  instanceVbo = 0;
  shader = 0;
}
