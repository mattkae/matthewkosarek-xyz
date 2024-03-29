#include "Renderer3d.h"
#include "Shader.h"
#include "list.h"
#include "mathlib.h"
#include "WebglContext.h"
#include "Logger.h"
#include <cstdio>

// Note: In the 'transform' attribute, the transform.x is the scale,
// transform.y is the rotation, and transform.zw is the translatiob.
EM_BOOL onScreenSizeChanged_3D(int eventType, const EmscriptenUiEvent *uiEvent, void *userData) {
    Renderer3d* renderer = (Renderer3d*)userData;
    
	EMSCRIPTEN_RESULT result = emscripten_set_canvas_element_size( renderer->context->query, uiEvent->documentBodyClientWidth, uiEvent->documentBodyClientHeight);
	if (result != EMSCRIPTEN_RESULT_SUCCESS) {
		logger_error("Failed to resize element at query: %s\n", renderer->context->query);
	}
	//renderer->projection = Mat4x4().getOrthographicMatrix(0, renderer->context->width, 0, renderer->context->height);

    return true;
}

void Renderer3d::load(WebglContext* inContext, const char* vertexShader, const char* fragmentShader) {
	context = inContext;
	printf("Compiling Renderer3d shader...\n");
	shader = loadShader(vertexShader, fragmentShader);

	useShader(shader);
	attributes.position = getShaderAttribute(shader, "position");
	attributes.color = getShaderAttribute(shader, "color");
	attributes.normal = getShaderAttribute(shader, "normal");
	uniforms.projection = getShaderUniform(shader, "projection");
	uniforms.view = getShaderUniform(shader, "view");
	uniforms.model = getShaderUniform(shader, "model");
	projection = Mat4x4().getPerspectiveProjection(0.1, 1000.f, 0.872f, static_cast<f32>(context->width) / static_cast<f32>(context->height));
	view = Mat4x4().getLookAt({ 0, 25, 75 }, { 0, 15, 0 }, { 0, 1, 0 });

	logger_info("Renderer3d shader compiled.\n");

	emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, false, onScreenSizeChanged_3D);
}
	
void Renderer3d::render() {
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

void Renderer3d::unload() {
	glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);
	glDeleteProgram(shader);
}

enum LineType {
	LineType_None,
	LineType_Comment,
	LineType_mtl,
	LineType_v,
	LineType_f,
	LineType_Unsupported
};

struct LineItem {
	LineType type = LineType_None;

	i32 idx = 0;

	union {
		f32 vertices[3];
		i32 indices[3];
	} v;
};

inline i32 readPastSpaces(i32 i, const char* content) {
	while (content[i] == ' ') i++;
	return i;
}

inline i32 readPastLine(i32 i, const char* content) {
	while (content[i] != '\n' && content[i] != '\0') i++;
	return i;
}

inline i32 readToken(i32 i, const char* content, char* output) {
	i32 tidx = 0;
	i = readPastSpaces(i, content);
	while (content[i] != ' ' && content[i] != '\n' && content[i] != '\0') {
	    output[tidx] = content[i];
		i++;
		tidx++;
	}
    output[tidx] = '\0';
	return i;
}

Mesh3d Mesh3d_fromObj(Renderer3d* renderer, const char* content, const i32 len) {
	Mesh3d result;
	result.vertices.allocate(2048);
	result.indices.allocate(2048);

	LineItem lt;
	lt.type = LineType_None;
	i32 lineNumber = 0;
	i32 i = 0;
    while (content[i] != '\0') {
		i = readPastSpaces(i, content);
		if (lt.type == LineType_None) {
			lineNumber++;
			char type[32];
		    i = readToken(i, content, type);
			
			if (strncmp(type, "#", 1) == 0) {
				lt.type = LineType_Comment;
			}
			else if (strncmp(type, "mtllib", 6) == 0) {
				lt.type = LineType_mtl;
			}
			else if (strncmp(type, "v", 1) == 0) {
			    lt.type = LineType_v;
			}
			else if (strncmp(type, "f", 1) == 0) {
				lt.type = LineType_f;
			}
			else {
				i++;
				//lt.type = LineType_Unsupported;
				//logger_error("Unknown type %s, %d", type, lineNumber);
			}
		}
		else {
			char buffer[32];
			switch (lt.type) {
			case LineType_mtl:
				i = readToken(i, content, buffer);
				break;
			case LineType_v: {
				while (content[i] != '\n' && content[i] != '\0') {
					i = readToken(i, content, buffer);
					lt.v.vertices[lt.idx] = atof(buffer);
					lt.idx++;
				}

				float fColor = randomFloatBetween(0.8, 1);
				result.vertices.add({
					Vector4(lt.v.vertices[0], lt.v.vertices[1], lt.v.vertices[2], 1.f),
					Vector4(fColor, fColor, fColor, 1)
				});
				break;
			}
			case LineType_f: {
				while (content[i] != '\n' && content[i] != '\0') {
					i = readToken(i, content, buffer);
					lt.v.indices[lt.idx] = atoi(buffer);
					lt.idx++;
				}
				
				auto v1idx = lt.v.indices[0] - 1;
				auto v2idx = lt.v.indices[1] - 1;
				auto v3idx = lt.v.indices[2] - 1;

				result.indices.add(v1idx);
				result.indices.add(v2idx);
				result.indices.add(v3idx);

				auto& v1 = result.vertices[v1idx];
				auto& v2 = result.vertices[v2idx];
				auto& v3 = result.vertices[v3idx];
			    Vector3 normal = (v1.position - v2.position).cross(v1.position - v3.position).toVector3().normalize();
				v1.normal = normal;
				v2.normal = normal;
				v3.normal = normal;
				break;
			}
			default:
				i = readPastLine(i, content);
				break;
			}

			lt = LineItem();
		}
	}

	printf("Completed Mesh3d loading.\n");
	result.load(renderer);
	return result;
}

void Mesh3d::load(Renderer3d* renderer) {
	glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.numElements * sizeof(Vertex3d), &vertices.data[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.numElements * sizeof(GLuint), &indices.data[0], GL_STATIC_DRAW);

    // Position
    glEnableVertexAttribArray(renderer->attributes.position);
    glVertexAttribPointer(renderer->attributes.position, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex3d), (GLvoid *)0);

    // Color
    glEnableVertexAttribArray(renderer->attributes.color);
    glVertexAttribPointer(renderer->attributes.color, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex3d), (GLvoid *)offsetof(Vertex3d, color));

	// Normal
    glEnableVertexAttribArray(renderer->attributes.normal);
    glVertexAttribPointer(renderer->attributes.normal, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex3d), (GLvoid *)offsetof(Vertex3d, normal));
	
    glBindVertexArray(0);
}

void Mesh3d::unload() {
	if (vao) glDeleteVertexArrays(1, &vao);
	if (vbo) glDeleteBuffers(1, &vbo);
    if (ebo) glDeleteBuffers(1, &ebo);
	vertices.deallocate();
	indices.deallocate();
}

void Mesh3d::render(Renderer3d* renderer) {
	setShaderMat4(renderer->uniforms.model, model);

	glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.numElements, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
