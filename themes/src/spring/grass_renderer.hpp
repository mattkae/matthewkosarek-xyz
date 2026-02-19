#ifndef GRASS_RENDERER_HPP
#define GRASS_RENDERER_HPP

#include "../renderer_3d.h"
#include "mathlib.h"
#include "types.h"

const i32 GRASS_BLADES_PER_ROW = 48;
const i32 GRASS_BLADES_PER_COL = 48;
const i32 NUM_GRASS_BLADES = GRASS_BLADES_PER_ROW * GRASS_BLADES_PER_COL;

struct GrassRendererLoadData {
  Vector2 origin = Vector2(0, 0);
  Vector2 area = Vector2(480, 480);
  f32 grassHeight = 4.f;
};

struct GrassUpdateData {
  Vector3 position;
  Vector2 top_offset; // top_offset.x stores per-blade sway phase offset
};

struct GrassInstanceData {
  float x, y, z;
  float phaseOffset;
  float heightScale;
};

struct GrassRenderer {
  GrassUpdateData grassBlades[NUM_GRASS_BLADES];

  u32 vao = 0;
  u32 quadVbo = 0;
  u32 instanceVbo = 0;
  u32 shader = 0;
  f32 time = 0.f;
  f32 bladeWidth = 1.5f;
  f32 bladeHeight = 6.f;
  f32 swayAmount = 0.3f;

  struct {
    i32 position;
    i32 instancePos;
    i32 instancePhase;
    i32 instanceHeight;
  } attributes;

  struct {
    i32 projection;
    i32 view;
    i32 time;
    i32 bladeWidth;
    i32 bladeHeight;
    i32 swayAmount;
  } uniforms;

  void load(GrassRendererLoadData params, Renderer3d *renderer);
  void update(f32 dtSeconds);
  void render(Renderer3d *renderer);
  void unload();
};

#endif
