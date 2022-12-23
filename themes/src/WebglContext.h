#pragma once
#include "types.h"
#include <cstring>
#include <emscripten.h>
#include <emscripten/html5.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>

struct WebglContext {
	EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context;
	f32 width = 800;
	f32 height = 600;
	char query[128];;

	void init(const char* inQuery);
	void makeCurrentContext();
	void destroy() ;
};
