#include "sun_frag.h"

const char* shader_sun_frag = "varying lowp vec4 VertexColor; \n"
" \n"
"void main() { \n"
"    gl_FragColor = VertexColor; \n"
"} \n"
" \n";
