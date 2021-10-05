#include "Renderer2d.h"
#include "Shader.h"
#include "WebglContext.h"
#include "mathlib.h"

// Note: In the 'transform' attribute, the transform.x is the scale,
// transform.y is the rotation, and transform.zw is the translatiob.
const char* renderer2dVertexShader = 
"attribute vec2 position; \n"
"attribute vec4 color; \n"
"attribute vec4 transform; \n"
"uniform mat4 projection; \n"
"uniform mat4 model; \n"
"varying lowp vec4 VertexColor; \n"
"void main() { \n"
"    vec4 fragmentPosition = projection * model * vec4(position.x * transform.x, position.y * transform.x, 1, 1); \n"
"    gl_Position = fragmentPosition; \n"
"    VertexColor = color; \n"
"}";

const char* renderer2dFragmentShader = 
"varying lowp vec4 VertexColor; \n"
"void main() { \n"
"    gl_FragColor = VertexColor; \n"
"}";

void Renderer2d::load(WebglContext* context) {
	printf("Compiling Renderer2d shader...\n");
	shader = loadShader(renderer2dVertexShader, renderer2dFragmentShader);

	useShader(shader);
	attributes.position = getShaderAttribute(shader, "position");
	attributes.color = getShaderAttribute(shader, "color");
	attributes.scale = getShaderAttribute(shader, "scale");
	uniforms.projection = getShaderUniform(shader, "projection");
	uniforms.model = getShaderUniform(shader, "model");
	projection = Mat4x4().getOrthographicMatrix(0, context->width, 0, context->height);

	printf("Renderer2d shader compiled.\n");
}
	
void Renderer2d::render() {
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


void Renderer2dShape::load(Renderer2dVertex* inVertices, uint32 inNumVertices, Renderer2d* renderer) {
	numVertices = inNumVertices;
	useShader(renderer->shader);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, inNumVertices * sizeof(Renderer2dVertex), &inVertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(renderer->attributes.position);
	glVertexAttribPointer(renderer->attributes.position, 2, GL_FLOAT, GL_FALSE, sizeof(Renderer2dVertex), (GLvoid *)0);

	glEnableVertexAttribArray(renderer->attributes.color);
	glVertexAttribPointer(renderer->attributes.color, 4, GL_FLOAT, GL_FALSE, sizeof(Renderer2dVertex), (GLvoid *)offsetof(Renderer2dVertex, color));

	glEnableVertexAttribArray(renderer->attributes.scale);
	glVertexAttribPointer(renderer->attributes.scale, 2, GL_FLOAT, GL_FALSE, sizeof(Renderer2dVertex), (GLvoid *)offsetof(Renderer2dVertex, scale));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Renderer2dShape::render(Renderer2d* renderer, GLenum drawType) {
	setShaderMat4(renderer->uniforms.model, model);

	glBindVertexArray(vao);
	glDrawArrays(drawType, 0, numVertices);
	glBindVertexArray(0);
}

void Renderer2dShape::unload() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}
