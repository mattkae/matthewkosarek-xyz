#ifndef SHADER_RENDERER3D_FRAG 
#define SHADER_RENDERER3D_FRAG 
const char* shader_renderer3d_frag = "varying lowp vec4 VertexColor; \n"
"varying lowp vec4 VertexNormal; \n"
" \n"
"void main() { \n"
"    const lowp vec3 lightDirection = vec3(0.0, 1.0, 0.0); \n"
"    gl_FragColor = vec4(VertexColor.xyz * dot(VertexNormal.xyz, lightDirection), 1); \n"
"} \n"
" \n";
#endif
