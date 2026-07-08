precision highp float;

varying vec2 vUv;

uniform float time;      // seconds, for slow cloud drift
uniform float horizon;   // screen-space y of the water horizon (~0.5)

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

float fbm(vec2 p) {
    float v   = 0.0;
    float amp = 0.5;
    for (int i = 0; i < 4; i++) {
        v   += amp * valueNoise(p);
        p   *= 2.0;
        amp *= 0.5;
    }
    return v;
}

const vec3  SKY_HORIZON   = vec3(0.80, 0.88, 0.98); // matches water horizon fade
const vec3  SKY_TOP       = vec3(0.22, 0.48, 0.86); // deeper zenith blue
const vec3  CLOUD_COLOR   = vec3(1.00, 0.98, 0.95); // white, slightly warm

const float DEPTH_BIAS    = 0.05;  // caps frequency at the horizon (larger = gentler shrink)
const float CLOUD_FREQ    = 2.7;   // overall cloud scale (larger = more, smaller clouds)
const float X_STRETCH     = 0.35;  // < 1 stretches clouds horizontally -> wispy
const float CLOUD_THRESH  = 0.55;  // coverage cutoff; higher = sparser sky
const float CLOUD_SOFT    = 0.22;  // edge softness of the cloud mask
const float CLOUD_OPACITY = 0.85;  // max cloud whiteness
const float DRIFT_SPEED   = 0.03;  // slow lateral drift (noise units / sec)
const float H_FADE        = 0.12;  // band over which clouds fade in above horizon

void main() {
    // Height above the water horizon: 0 at the waterline, (1 - horizon) at the top.
    float above = max(vUv.y - horizon, 0.0);

    // Vertical gradient.
    float g = clamp(above / (1.0 - horizon), 0.0, 1.0);
    vec3 col = mix(SKY_HORIZON, SKY_TOP, pow(g, 0.75));
    float haze = 1.0 - smoothstep(0.0, 0.35, g);
    col += vec3(0.06, 0.04, 0.00) * haze;

    // Flat cloud-ceiling perspective: distance along the view ~ 1/above, so
    // features shrink AND compress vertically toward the horizon.
    float depth = 1.0 / (above + DEPTH_BIAS);
    vec2 q;
    q.x = (vUv.x - 0.5) * depth;
    q.y = depth;

    vec2 p = vec2(q.x * X_STRETCH, q.y) * CLOUD_FREQ;
    p.x += time * DRIFT_SPEED;

    float n = fbm(p);
    float cloud = smoothstep(CLOUD_THRESH, CLOUD_THRESH + CLOUD_SOFT, n);

    // Fade clouds into the haze right at the horizon.
    cloud *= smoothstep(0.0, H_FADE, above);

    col = mix(col, CLOUD_COLOR, cloud * CLOUD_OPACITY);

    gl_FragColor = vec4(col, 1.0);
}
