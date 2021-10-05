#include "TreeRenderer.h"
#include "WebglContext.h"
#include "mathlib.h"
#include <GLES2/gl2.h>

const char* TreeRendererVertexShader = 
"attribute vec2 position; \n"
"attribute vec4 color; \n"
"uniform mat4 projection; \n"
"uniform mat4 model; \n"
"varying lowp vec4 VertexColor; \n"
"void main() { \n"
"    vec4 fragmentPosition = projection * model * vec4(position.x, position.y, 1, 1); \n"
"    gl_Position = fragmentPosition; \n"
"    VertexColor = color; \n"
"}";

const char* TreeRendererFragmentShader = 
"varying lowp vec4 VertexColor; \n"
"void main() { \n"
"    gl_FragColor = VertexColor; \n"
"}";

void TreeRenderer::load(WebglContext* context) {
	printf("Compiling TreeRenderer shader...\n");
	shader = loadShader(TreeRendererVertexShader, TreeRendererFragmentShader);

	useShader(shader);
	attributes.position = getShaderAttribute(shader, "position");
	attributes.color = getShaderAttribute(shader, "color");
	uniforms.projection = getShaderUniform(shader, "projection");
	uniforms.model = getShaderUniform(shader, "model");
	projection = Mat4x4().getOrthographicMatrix(0, context->width, 0, context->height);

	printf("TreeRenderer shader compiled.\n");
}
	
void TreeRenderer::render(float32 dtSeconds) {
    timeElapsed += dtSeconds;

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

void TreeRenderer::unload() {
	glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);
	glDeleteProgram(shader);
}
