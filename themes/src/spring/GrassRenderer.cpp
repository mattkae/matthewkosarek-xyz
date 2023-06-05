#include "GrassRenderer.hpp"
#include "../Shader.h"
#include "../list.h"
#include "../mathlib.h"
#include "../shader_fetcher.hpp"
#include "../WebglContext.h"

const char* VERTEX_SHADER_PATH = "themes/src/shaders/grassrenderer.vert";
const char* FRAGMENT_SHADER_PATH = "themes/src/shaders/grassrenderer.frag";

// https://learnopengl.com/code_viewer_gh.php?code=src/4.advanced_opengl/10.1.instancing_quads/instancing_quads.cpp
const float quadVertices[] = {
    // positions     // colors
    -1.f,  1.f,  1.0f, 0.0f, 0.0f,
    1.f, -1.f,  0.0f, 1.0f, 0.0f,
    -1.f, -1.f,  0.0f, 0.0f, 1.0f,

    -1.f,  1.f,  1.0f, 0.0f, 0.0f,
    1.f, -1.f,  0.0f, 1.0f, 0.0f,
    1.f,  1.f,  0.0f, 1.0f, 1.0f
};

inline void generate_grass_mesh(GrassRenderer* renderer) {
    glGenBuffers(1, &renderer->instance_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->instance_vbo);b
    glBufferData(GL_ARRAY_BUFFER, sizeof(GrassMeshData) * NUM_GRASS_BLADES, &grassBlades[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &renderer->vao);
    glGenBuffers(1, &renderer->vbo);
    glBindVertexArray(renderer->vao);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    // also set instance data
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO); // this attribute comes from a different vertex buffer
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(2, 1); // tell OpenGL this is an instanced vertex attribute.
}

inline void on_shader_fetched(ShaderFetchResult* result) {

}

void GrassRenderer::load(GrassRendererLoadData params) {
    state = GrassRendererState::Loading;
	printf("Compiling Renderer3d shader...\n");
    fetch_shader({ VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH }, on_shader_fetched, this);
	shader = loadShader(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);

	useShader(shader);
	attributes.position = getShaderAttribute(shader, "position");
	attributes.texture_coordinates = getShaderAttribute(shader, "texture_coordinates");
	uniforms.projection = getShaderUniform(shader, "projection");
	uniforms.view = getShaderUniform(shader, "view");
	uniforms.model = getShaderUniform(shader, "model");
	projection = Mat4x4().getPerspectiveProjection(0.1, 1000.f, 0.872f, static_cast<f32>(context->width) / static_cast<f32>(context->height));
	view = Mat4x4().getLookAt({ 0, 25, 75 }, { 0, 15, 0 }, { 0, 1, 0 });

	logger_info("Renderer3d shader compiled.\n");
    
    const f32 COLUMN_INCREMENT = GRASS_BLADES_PER_COL / params.area.x;
    const f32 ROW_INCREMENT = GRASS_BLADES_PER_ROW / params.area.y;
    for (i32 r = 0; r < GRASS_BLADES_PER_ROW; r++) {
        i32 indexOffset = r * GRASS_BLADES_PER_ROW;
        f32 y = ROW_INCREMENT * r;
        for (i32 c = 0; c < GRASS_BLADES_PER_COL; c++) {
            f32 x = COLUMN_INCREMENT * c;
            i32 index = indexOffset + c;
            grassBlades[index].position = Vector3(x, y, 0);
        }
    }
}

void GrassRenderer::update(f32 seconds) {
    if (state == GrassRendererState::Loading) {
        return;
    }
}

	
void GrassRenderer::render() {
    if (state == GrassRendererState::Loading) {
        return;
    }
    
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
	glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	useShader(shader);
	setShaderMat4(uniforms.projection, projection);
	setShaderMat4(uniforms.view, view);
}

void GrassRenderer::unload() {
	glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);
	glDeleteProgram(shader);
}
