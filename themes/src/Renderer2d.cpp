#include "Renderer2d.h"
#include "Shader.h"
#include "WebglContext.h"
#include "mathlib.h"
#include <cstdio>
#include "shaders/renderer2d_vert.h"
#include "shaders/renderer2d_frag.h"

// Note: In the 'transform' attribute, the transform.x is the scale,
// transform.y is the rotation, and transform.zw is the translation.

void Renderer2d::load(WebglContext* inContext, const char* inVertexShader, const char* inFragmentShader) {
    auto vertexShader = inVertexShader ? inVertexShader : shader_renderer2d_vert;
    auto fragmentShader = inFragmentShader ? inFragmentShader : shader_renderer2d_frag;
	context = inContext;
	printf("Compiling Renderer2d shader...\n");
	shader = loadShader(vertexShader, fragmentShader);

	useShader(shader);
	attributes.position = getShaderAttribute(shader, "position");
	attributes.color = getShaderAttribute(shader, "color");
	attributes.vMatrix = getShaderAttribute(shader, "vMatrix");
	uniforms.projection = getShaderUniform(shader, "projection");
	uniforms.model = getShaderUniform(shader, "model");
	projection = Mat4x4().getOrthographicMatrix(0, context->width, 0, context->height);

	printf("Renderer2d shader compiled.\n");
}
	
void Renderer2d::render() {
	projection = Mat4x4().getOrthographicMatrix(0, context->width, 0, context->height);
	
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
}

void Renderer2d::unload() {
	glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);
	glDeleteProgram(shader);
}


void Mesh2D::load(Vertex2D* inVertices, u32 inNumVertices, Renderer2d* renderer) {
	ebo = 0;
	numVertices = inNumVertices;
	useShader(renderer->shader);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, inNumVertices * sizeof(Vertex2D), &inVertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(renderer->attributes.position);
	glVertexAttribPointer(renderer->attributes.position, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (GLvoid *)0);

	glEnableVertexAttribArray(renderer->attributes.color);
	glVertexAttribPointer(renderer->attributes.color, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (GLvoid *)offsetof(Vertex2D, color));

	for (i32 idx = 0; idx < 4; idx++) {
			glEnableVertexAttribArray(renderer->attributes.vMatrix + idx);
			glVertexAttribPointer(renderer->attributes.vMatrix + idx, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (GLvoid *)(offsetof(Vertex2D, vMatrix) + (idx * 16)));
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


void Mesh2D::load(Vertex2D* vertices,
		  u32 numVertices,
		  u32* indices,
		  u32 inNumIndices,
		  Renderer2d* renderer) {
	load(vertices, numVertices, renderer);
	glBindVertexArray(vao);
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, inNumIndices * sizeof(u32), &indices[0], GL_STATIC_DRAW);
	numIndices = inNumIndices;
	glBindVertexArray(0);
}

void Mesh2D::render(Renderer2d* renderer, GLenum drawType) {
	setShaderMat4(renderer->uniforms.model, model);

	if (ebo == 0) {
		glBindVertexArray(vao);
		glDrawArrays(drawType, 0, numVertices);
		glBindVertexArray(0);
	}
	else {
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}

void Mesh2D::unload() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	if (ebo != 0) {
		glDeleteBuffers(1, &ebo);
		ebo = 0;
	}
	vao = 0;
	vbo = 0;
}
