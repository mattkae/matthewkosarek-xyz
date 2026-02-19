#include "grass_frag.h"

const char* shader_grass_frag = "varying lowp vec2 vUV; \n"
" \n"
"void main() { \n"
"    lowp float halfWidth = 0.5 * (1.0 - vUV.y); \n"
"    lowp float distFromCenter = abs(vUV.x - 0.5); \n"
"    if (distFromCenter > halfWidth) discard; \n"
" \n"
"    lowp vec3 baseColor = vec3(0.15, 0.45, 0.10); \n"
"    lowp vec3 tipColor  = vec3(0.40, 0.75, 0.20); \n"
"    gl_FragColor = vec4(mix(baseColor, tipColor, vUV.y), 1.0); \n"
"} \n"
" \n";
