precision highp float;

varying vec3 vWorldPos;
varying vec3 vNormal;

uniform vec3 cameraPos;
uniform vec3 sunDir;      // normalized direction from the water toward the sun
uniform vec3 sunColor;
uniform float time;

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

void main() {
    vec3 N = normalize(vNormal);
    vec3 V = normalize(cameraPos - vWorldPos);

    // Fresnel (Schlick).
    float f0 = 0.02;
    float fres = f0 + (1.0 - f0) * pow(1.0 - max(dot(N, V), 0.0), 5.0);

    // Refraction: depth-tinted water body, wobbled by the surface normal.
    float depth01 = clamp((vWorldPos.z + 350.0) / 400.0, 0.0, 1.0);
    float wob = (N.x + N.z) * 0.5;
    vec3 shallow = vec3(0.10, 0.55, 0.60);
    vec3 deep    = vec3(0.02, 0.14, 0.28);
    vec3 refractCol = mix(shallow, deep, clamp(depth01 + wob * 0.15, 0.0, 1.0));

    // Reflection: sky gradient + reflected sun glow + broken-up specular glitter.
    vec3 R = reflect(-V, N);
    vec3 skyHorizon = vec3(0.80, 0.88, 0.98);
    vec3 skyTop     = vec3(0.30, 0.55, 0.95);
    float skyT = clamp(R.y * 2.0, 0.0, 1.0);
    vec3 reflectCol = mix(skyHorizon, skyTop, skyT);

    float sunAmt = max(dot(R, sunDir), 0.0);
    reflectCol += sunColor * pow(sunAmt, 8.0) * 0.6;
    float sparkle = valueNoise(vWorldPos.xz * 0.5 + vec2(time * 1.3, time * 0.7));
    float spec = pow(sunAmt, 200.0) * smoothstep(0.55, 0.9, sparkle);
    reflectCol += sunColor * spec * 4.0;

    vec3 color = mix(refractCol, reflectCol, fres);

    // Horizon haze: hide the far straight edge of the plane by fading to sky.
    float horizonFade = clamp((vWorldPos.z + 350.0) / 90.0, 0.0, 1.0);
    color = mix(skyHorizon, color, horizonFade);

    gl_FragColor = vec4(color, 1.0);   // opaque -> occludes the sun below the waterline
}
