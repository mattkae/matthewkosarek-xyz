varying lowp vec2 vUV;

void main() {
    lowp float halfWidth = 0.5 * (1.0 - vUV.y);
    lowp float distFromCenter = abs(vUV.x - 0.5);
    if (distFromCenter > halfWidth) discard;

    lowp vec3 baseColor = vec3(0.15, 0.45, 0.10);
    lowp vec3 tipColor  = vec3(0.40, 0.75, 0.20);
    gl_FragColor = vec4(mix(baseColor, tipColor, vUV.y), 1.0);
}
