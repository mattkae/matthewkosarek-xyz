#include "WebglContext.h"
#include <cstdio>


void WebglContext::init(const char* inQuery) {
    strcpy(query, inQuery);
    float64 inWidth, inHeight;
    emscripten_get_element_css_size(query, &inWidth, &inHeight);
    width = static_cast<int32>(inWidth);
    height = static_cast<int32>(inHeight);
    emscripten_set_canvas_element_size( query, width, height);
    
    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);

    attrs.enableExtensionsByDefault = 1;
    attrs.majorVersion = 3;
    attrs.minorVersion = 0;

    context = emscripten_webgl_create_context(query, &attrs);
    makeCurrentContext();

    glClearColor(0, 0, 0, 0.0f);
};

void WebglContext::makeCurrentContext() {
    emscripten_webgl_make_context_current(context);
};

void WebglContext::destroy() {
    emscripten_webgl_destroy_context(context);
}
