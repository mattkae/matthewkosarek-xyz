// Procedural star fragment shader
varying lowp vec2 vUV;
varying lowp float vSeed;
varying lowp float vScale;

const lowp float PI = 3.14159265359;

// Simple hash function for deterministic randomness
lowp float hash(lowp float n) {
    return fract(sin(n) * 43758.5453123);
}

// Generate star pattern procedurally
lowp float starPattern(lowp vec2 uv) {
    lowp float dist = length(uv);
    lowp float angle = atan(uv.y, uv.x);

    // Number of star points (4 or 5)
    lowp float numPoints = 4.0 + floor(hash(vSeed) * 2.0);

    // Create sharp star with triangular points
    lowp float angleStep = 2.0 * PI / numPoints;
    lowp float currentAngle = mod(angle + PI / numPoints, angleStep) - angleStep * 0.5;

    // Create triangular points - distance from center to edge varies linearly with angle
    lowp float normalizedAngle = abs(currentAngle) / (angleStep * 0.5);

    // Outer tip radius and inner valley radius
    lowp float tipRadius = 0.5;
    lowp float valleyRadius = 0.15;

    // Linear interpolation creates sharp triangular points
    lowp float rayEdge = mix(tipRadius, valleyRadius, normalizedAngle);

    // Hard cutoff for sharp edges
    lowp float star = step(dist, rayEdge);

    return star;
}

void main() {
    lowp float pattern = starPattern(vUV);

    // Color variation based on seed - white and blue tints
    lowp float colorVar = hash(vSeed * 3.0);
    lowp vec3 starColor;

    if (colorVar < 0.5) {
        // Pure white
        starColor = vec3(1.0, 1.0, 1.0);
    } else if (colorVar < 0.75) {
        // Light blue tint
        starColor = vec3(0.9, 0.95, 1.0);
    } else {
        // Stronger blue tint
        starColor = vec3(0.85, 0.92, 1.0);
    }

    // Scale alpha based on size - smaller stars are more transparent
    // Normalize scale from range [16, 48] to [0, 1]
    lowp float sizeRatio = (vScale - 16.0) / (48.0 - 16.0);
    // Map to alpha range [0.3, 1.0] - smaller stars at 30% opacity, larger at 100%
    lowp float alpha = mix(0.3, 1.0, sizeRatio);

    lowp vec4 color = vec4(starColor, pattern * alpha);

    // Discard fully transparent pixels for performance
    if (color.a < 0.01) {
        discard;
    }

    gl_FragColor = color;
}
