#ifndef GRASS_RENDERER_HPP
#define GRASS_RENDERER_HPP

#include "../Renderer3d.h"
#include "../mathlib.h"
#include "../types.h"
#include <GLES2/gl2.h>

const i32 GRASS_BLADES_PER_ROW = 24;
const i32 GRASS_BLADES_PER_COL = 24;
const i32 NUM_GRASS_BLADES = GRASS_BLADES_PER_ROW * GRASS_BLADES_PER_COL;

struct GrassRendererLoadData {
    Vector2 origin = Vector2(0, 0);
    Vector2 area = Vector2(480, 480);
    f32 grassHeight = 12.f;
};

struct GrassMeshData {
    Vector3 position;
};

enum  class GrassRendererState {
    None = 0,
    Loading = 1,
    Ready = 2
};

struct GrassRenderer {
    GrassRendererState state;
    WebglContext* context = NULL;
	Mat4x4 projection;
    Mat4x4 view;
	u32 shader;
    Vector4 clearColor;

    struct {
		i32 position;
		i32 texture_coordinates;
	} attributes;

	struct {
		i32 projection;
        i32 view;
		i32 model;
	} uniforms;
    
    GrassMeshData grassBlades[NUM_GRASS_BLADES];
    GLuint vao;
    GLuint instance_vbo;
    GLuint vbo;
    
    void load(GrassRendererLoadData params);
	void update(f32 dtSeconds);
	void render();
	void unload();
};

#endif
