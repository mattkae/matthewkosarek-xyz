#ifndef RENDERER3D_H
#define RENDERER3D_H
#include "mathlib.h"
#include "list.h"
#include "types.h"
#include <string>

struct Renderer3D;

struct Vertex3d {
    Vector4 position;
    Vector4 color;
	Vector4 normal;
};

struct Mesh3d {
    u32 vao;
    u32 vbo;
    u32 ebo;
    matte::List<Vertex3d> vertices;
    matte::List<u32> indices;
    Mat4x4 model;

    void load(Renderer3D* renderer);
    void render(Renderer3D* renderer);
    void unload();
};

struct WebglContext;
struct Renderer3D {
    WebglContext* context = NULL;
	Mat4x4 projection;
    Mat4x4 view;
	u32 shader;
    Vector4 clearColor;

    struct {
		i32 position;
		i32 color;
		i32 normal;
	} attributes;

	struct {
		i32 projection;
        i32 view;
		i32 model;
	} uniforms;

    void load(WebglContext* context);
	void render();
	void unload();
};

Mesh3d Mesh3d_fromObj(Renderer3D* renderer, const char* content, const i32 len);

#endif
