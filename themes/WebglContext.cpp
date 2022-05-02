#include "WebglContext.h"
#include <cstdio>


EM_BOOL onResize(int eventType, const EmscriptenUiEvent *uiEvent, void *userData) {
	WebglContext* context = (WebglContext*)userData;

    f64 inWidth, inHeight;
    emscripten_get_element_css_size(context->query, &inWidth, &inHeight);

    context->width = static_cast<f32>(inWidth);
    context->height = static_cast<f32>(inHeight);

    return true;
}

void WebglContext::init(const char* inQuery) {
    strcpy(query, inQuery);
    f64 inWidth, inHeight;
    emscripten_get_element_css_size(query, &inWidth, &inHeight);
    width = static_cast<f32>(inWidth);
    height = static_cast<f32>(inHeight);
    emscripten_set_canvas_element_size( query, width, height);
    
    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);

    attrs.enableExtensionsByDefault = 1;
    attrs.majorVersion = 3;
    attrs.minorVersion = 0;

    context = emscripten_webgl_create_context(query, &attrs);
    makeCurrentContext();

    glClearColor(0, 0, 0, 0.0f);

    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, false, onResize);
};

void WebglContext::makeCurrentContext() {
    emscripten_webgl_make_context_current(context);
};

void WebglContext::destroy() {
    emscripten_webgl_destroy_context(context);
}
