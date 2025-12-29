varying lowp vec4 VertexColor;
varying lowp vec2 TexCoord;
uniform lowp float time;

// Simple noise function for edge distortion
lowp float noise(lowp vec2 p) {
    return sin(p.x * 10.0 + time) * cos(p.y * 10.0 + time * 0.5) * 0.5 + 0.5;
}

void main() {
    // TexCoord is now normalized: center is (0,0), edges are at distance ~1
    lowp float dist = length(TexCoord);

    // Add animated noise to the edge
    lowp float angle = atan(TexCoord.y, TexCoord.x);
    lowp float wave = sin(angle * 8.0 + time * 2.0) * 0.05 + sin(angle * 4.0 - time * 1.5) * 0.03;
    lowp float noiseValue = noise(TexCoord + time * 0.1) * 0.02;

    // Create soft edge using smoothstep - ensure fade reaches zero at the actual edge
    lowp float innerEdge = 0.8;
    lowp float outerEdge = 1.0;
    lowp float alpha = 1.0 - smoothstep(innerEdge, outerEdge, dist);

    // Apply wave distortion to the edge
    alpha *= 1.0 - smoothstep(0.85 + wave + noiseValue * 2.0, 1.0, dist);

    // Make edges more transparent but not too much
    alpha = alpha * alpha;

    // Add slight glow effect at the edge
    lowp float glow = smoothstep(0.5, 0.8, dist) * (1.0 - smoothstep(0.8, 1.0, dist));

    // Create orange gradient from center
    lowp vec3 orangeColor = vec3(1.0, 0.5, 0.1);
    lowp float centerGradient = smoothstep(0.6, 0.0, dist);
    lowp vec3 baseColor = mix(VertexColor.rgb, orangeColor, centerGradient * 0.6);

    // Mix in the glow with a brighter color
    lowp vec3 glowColor = baseColor * 1.3;
    lowp vec3 finalColor = mix(baseColor, glowColor, glow * 0.5);

    gl_FragColor = vec4(finalColor, VertexColor.a * alpha);
}
