#include "Renderer3d.h"
#include "Shader.h"
#include "mathlib.h"
#include "WebglContext.h"
#include "Logger.h"

// Note: In the 'transform' attribute, the transform.x is the scale,
// transform.y is the rotation, and transform.zw is the translatiob.
const char* vertexShader = 
"attribute vec4 position; \n"
"attribute vec4 color; \n"
"uniform mat4 projection; \n"
"uniform mat4 model; \n"
"varying lowp vec4 VertexColor; \n"
"void main() { \n"
"    vec4 fragmentPosition = projection * model * position; \n"
"    gl_Position = fragmentPosition; \n"
"    VertexColor = color; \n"
"}";

const char* fragmentShader = 
"varying lowp vec4 VertexColor; \n"
"void main() { \n"
"    gl_FragColor = VertexColor; \n"
"}";

EM_BOOL onScreenSizeChanged_3D(int eventType, const EmscriptenUiEvent *uiEvent, void *userData) {
    Renderer3D* renderer = (Renderer3D*)userData;
    
	EMSCRIPTEN_RESULT result = emscripten_set_canvas_element_size( renderer->context->query, uiEvent->documentBodyClientWidth, uiEvent->documentBodyClientHeight);
	if (result != EMSCRIPTEN_RESULT_SUCCESS) {
		logger_error("Failed to resize element at query: %s\n", renderer->context->query);
	}
	//renderer->projection = Mat4x4().getOrthographicMatrix(0, renderer->context->width, 0, renderer->context->height);

    return true;
}

void Renderer3D::load(WebglContext* inContext) {
	context = inContext;
	printf("Compiling Renderer2d shader...\n");
	shader = loadShader(vertexShader, fragmentShader);

	useShader(shader);
	attributes.position = getShaderAttribute(shader, "position");
	attributes.color = getShaderAttribute(shader, "color");
	uniforms.projection = getShaderUniform(shader, "projection");
	uniforms.model = getShaderUniform(shader, "model");
	projection = Mat4x4().getPerspectiveProjection(0.1, 100.f, DEG_TO_RAD(60.f), static_cast<f32>(context->width) / static_cast<f32>(context->height));

	logger_info("Renderer2d shader compiled.\n");

	emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, false, onScreenSizeChanged_3D);
}
	
void Renderer3D::render() {
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

void Renderer3D::unload() {
	glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);
	glDeleteProgram(shader);
}
