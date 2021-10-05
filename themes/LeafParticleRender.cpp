#include "LeafParticleRender.h"
#include "Renderer2d.h"

void LeafParticleRender::load(Renderer2d *renderer, TreeShape *tree) {
    numVertices = 0;
    numLeaves = 0;

    vertices = new Renderer2dVertex[numVertices];

    useShader(renderer->shader);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Renderer2dVertex), &vertices[0], GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(renderer->attributes.position);
    glVertexAttribPointer(renderer->attributes.position, 2, GL_FLOAT, GL_FALSE, sizeof(Renderer2dVertex), (GLvoid *)0);

    glEnableVertexAttribArray(renderer->attributes.color);
    glVertexAttribPointer(renderer->attributes.color, 4, GL_FLOAT, GL_FALSE, sizeof(Renderer2dVertex), (GLvoid *)offsetof(Renderer2dVertex, color));

    glEnableVertexAttribArray(renderer->attributes.scale);
    glVertexAttribPointer(renderer->attributes.scale, 2, GL_FLOAT, GL_FALSE, sizeof(Renderer2dVertex), (GLvoid *)offsetof(Renderer2dVertex, scale));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void LeafParticleRender::update(float32 dtSeconds) {
    
}

void LeafParticleRender::render(Renderer2d *renderer) {
    setShaderMat4(renderer->uniforms.model, model);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices * sizeof(Renderer2dVertex), &vertices[0]);

    glBindVertexArray(vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6 * numVertices, numLeaves);
    glBindVertexArray(0);
}

void LeafParticleRender::unload() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    delete [] vertices;
}