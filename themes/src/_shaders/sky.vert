precision highp float;

attribute vec2 position;   // fullscreen quad, clip-space corners in [-1, 1]

varying vec2 vUv;

void main() {
    vUv = position * 0.5 + 0.5;          // uv.y = 0 bottom, 1 top
    gl_Position = vec4(position, 0.0, 1.0);
}
