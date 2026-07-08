#include "sky_model.h"
#include "../webgl_context.h"
#include "../mathlib.h"
#include "../shaders/sky_vert.h"
#include "../shaders/sky_frag.h"

SkyModel::SkyModel() {}

SkyModel::~SkyModel() { unload(); }

void SkyModel::load(WebglContext* inContext) {
    context = inContext;
    shader = loadShader(shader_sky_vert, shader_sky_frag);
    useShader(shader);

    attributes.position = getShaderAttribute(shader, "position");
    uniforms.time       = getShaderUniform(shader, "time");
    uniforms.horizon    = getShaderUniform(shader, "horizon");

    // Full-screen quad in clip space (two triangles).
    Vector2 verts[6] = {
        Vector2(-1.f, -1.f), Vector2(1.f, -1.f), Vector2(-1.f, 1.f),
        Vector2(-1.f,  1.f), Vector2(1.f, -1.f), Vector2( 1.f, 1.f)
    };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glEnableVertexAttribArray(attributes.position);
    glVertexAttribPointer(attributes.position, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2), (GLvoid*)0);

    glBindVertexArray(0);
}

void SkyModel::update(f32 dtSeconds) {
    elapsedTime += dtSeconds;
}

void SkyModel::render() {
    useShader(shader);

    // Background: paint every pixel first, without touching the depth buffer so
    // it can neither reject nor be rejected by the sun/water drawn afterward.
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    setShaderFloat(uniforms.time, elapsedTime);
    setShaderFloat(uniforms.horizon, horizon);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void SkyModel::unload() {
    if (vao) { glDeleteVertexArrays(1, &vao); vao = 0; }
    if (vbo) { glDeleteBuffers(1, &vbo); vbo = 0; }
    if (shader) { glDeleteProgram(shader); shader = 0; }
}
