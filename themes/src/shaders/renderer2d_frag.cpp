#include "renderer2d_frag.h"

const char* shader_renderer2d_frag = "varying lowp vec4 VertexColor; \n"
" \n"
"void main() { \n"
"    gl_FragColor = VertexColor; \n"
"} \n"
" \n";
