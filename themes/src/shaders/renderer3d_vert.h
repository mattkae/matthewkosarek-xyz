#ifndef SHADER_RENDERER3D_VERT 
#define SHADER_RENDERER3D_VERT 
const char* shader_renderer3d_vert = "attribute vec4 position; \n"
"attribute vec4 color; \n"
"attribute vec4 normal; \n"
"uniform mat4 projection; \n"
"uniform mat4 view; \n"
"uniform mat4 model; \n"
"varying lowp vec4 VertexColor; \n"
"varying lowp vec4 VertexNormal; \n"
" \n"
"void main() { \n"
"    vec4 fragmentPosition = projection * view * model * position; \n"
"    gl_Position = fragmentPosition; \n"
"    VertexColor = color; \n"
"    VertexNormal = normal; \n"
"} \n"
" \n";
#endif
