#include "Renderer2d.h"
#include "Shader.h"
#include "WebglContext.h"
#include "mathlib.h"
#include <cstdio>

// Note: In the 'transform' attribute, the transform.x is the scale,
// transform.y is the rotation, and transform.zw is the translatiob.
const char* Vertex2DShader = 
"attribute vec2 position; \n"
"attribute vec4 color; \n"
"attribute mat4 vMatrix; \n"
"uniform mat4 projection; \n"
"uniform mat4 model; \n"
"varying lowp vec4 VertexColor; \n"
"void main() { \n"
"    vec4 fragmentPosition = projection * model * vMatrix * vec4(position.x, position.y, 0, 1); \n"
"    gl_Position = fragmentPosition; \n"
"    VertexColor = color; \n"
"}";

const char* renderer2dFragmentShader = 
"varying lowp vec4 VertexColor; \n"
"void main() { \n"
"    gl_FragColor = VertexColor; \n"
"}";

void Renderer2d::load(WebglContext* inContext) {
	context = inContext;
	printf("Compiling Renderer2d shader...\n");
	shader = loadShader(Vertex2DShader, renderer2dFragmentShader);

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

void Mesh2D::render(Renderer2d* renderer, GLenum drawType) {
	setShaderMat4(renderer->uniforms.model, model);

	glBindVertexArray(vao);
	glDrawArrays(drawType, 0, numVertices);
	glBindVertexArray(0);
}

void Mesh2D::unload() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}
