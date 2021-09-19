#pragma once
#include "types.h"
#include <emscripten.h>
#include <emscripten/html5.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>

EM_BOOL onScreenSizeChanged(int eventType, const EmscriptenUiEvent *uiEvent, void *userData);

struct WebglContext {
	EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context;
	int width = 800;
	int height = 600;

	void init(const char* query, int inWidth = 800, int inHeight = 600) {
		width = inWidth;
		height = inHeight;
		emscripten_set_canvas_element_size( query, width, height);
		
		EmscriptenWebGLContextAttributes attrs;
		emscripten_webgl_init_context_attributes(&attrs);

		attrs.enableExtensionsByDefault = 1;
		attrs.majorVersion = 3;
		attrs.minorVersion = 0;

		context = emscripten_webgl_create_context(query, &attrs);
		makeCurrentContext();

		glClearColor(0, 0, 0, 0.0f);

        emscripten_set_resize_callback(query, this, false, onScreenSizeChanged);
	};
	
	void makeCurrentContext() {
		emscripten_webgl_make_context_current(context);
	};
	
	void destroy() {
		emscripten_webgl_destroy_context(context);
	}
};
