precision highp float;

attribute vec2 position;      // local (x, z) on the plane

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform float time;
uniform float amplitude;

varying vec3 vWorldPos;
varying vec3 vNormal;

float hash(vec2 p) {
    p = fract(p * vec2(123.34, 456.21));
    p += dot(p, p + 45.32);
    return fract(p.x * p.y);
}

float valueNoise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
}

// Irregular height: non-harmonic directional sines plus two octaves of
// scrolling value noise so it never reads as a single clean wave.
float waveHeight(vec2 p, float t) {
    float h = 0.0;
    h += 0.30 * sin(dot(p, vec2(0.90, 0.30)) * 0.35 + t * 1.1);
    h += 0.20 * sin(dot(p, vec2(-0.40, 1.00)) * 0.55 - t * 1.7);
    h += 0.12 * sin(dot(p, vec2(0.70, -0.80)) * 0.90 + t * 2.3);
    h += 0.08 * sin(dot(p, vec2(1.00, 0.60)) * 1.60 - t * 3.1);
    h += 0.18 * (valueNoise(p * 0.25 + vec2(t * 0.15, t * 0.10)) - 0.5);
    h += 0.09 * (valueNoise(p * 0.70 - vec2(t * 0.20, 0.0)) - 0.5);
    return h;
}

void main() {
    vec2 p = position;
    float t = time;

    float h = waveHeight(p, t) * amplitude;

    // Analytic normal via central differences of the same height function.
    float e = 2.0;
    float hL = waveHeight(p - vec2(e, 0.0), t) * amplitude;
    float hR = waveHeight(p + vec2(e, 0.0), t) * amplitude;
    float hD = waveHeight(p - vec2(0.0, e), t) * amplitude;
    float hU = waveHeight(p + vec2(0.0, e), t) * amplitude;
    vec3 N = normalize(vec3(hL - hR, 2.0 * e, hD - hU));

    vec4 worldPos = model * vec4(p.x, h, p.y, 1.0);
    vWorldPos = worldPos.xyz;
    vNormal   = N;   // model is translation-only, so no normal matrix needed

    gl_Position = projection * view * worldPos;
}
