#include "sky_vert.h"

const char* shader_sky_vert = "precision highp float; \n"
" \n"
"attribute vec2 position;   // fullscreen quad, clip-space corners in [-1, 1] \n"
" \n"
"varying vec2 vUv; \n"
" \n"
"void main() { \n"
"    vUv = position * 0.5 + 0.5;          // uv.y = 0 bottom, 1 top \n"
"    gl_Position = vec4(position, 0.0, 1.0); \n"
"} \n"
" \n";
