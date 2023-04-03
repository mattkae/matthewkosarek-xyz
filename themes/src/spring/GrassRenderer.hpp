#ifndef GRASS_RENDERER_HPP
#define GRASS_RENDERER_HPP

#include "Renderer3d.h"
#include "mathlib.h"
#include "types.h"

const i32 GRASS_BLADES_PER_ROW = 24;
const i32 GRASS_BLADES_PER_COL = 24;
const i32 NUM_GRASS_BLADES = GRASS_BLADES_PER_ROW * GRASS_BLADES_PER_COL;

struct GrassRendererLoadData {
    Vector2 origin = Vector2(0, 0);
    Vector2 area = Vector2(480, 480);
    f32 grassHeight = 12.f;
};

struct GrassUpdateData {
    Vector3 position;
    Vector2 top_offset;
};

struct GrassRenderer {
    
    GrassUpdateData grassBlades[NUM_GRASS_BLADES];
    
    void load(GrassRendererLoadData params, Renderer3d* renderer);
	void update(f32 dtSeconds);
	void render(Renderer3d* renderer);
	void unload();
};

#endif
