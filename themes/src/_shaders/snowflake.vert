// Instanced snowflake vertex shader
attribute vec2 position;        // Base quad vertex position
attribute vec2 instancePos;     // Per-instance: snowflake center position
attribute float instanceRot;    // Per-instance: rotation angle
attribute float instanceScale;  // Per-instance: size scale
attribute float instanceSeed;   // Per-instance: random seed for variation

uniform mat4 projection;
uniform mat4 model;

varying lowp vec2 vUV;           // UV coordinates for fragment shader
varying lowp float vSeed;        // Pass seed to fragment shader
varying lowp float vScale;       // Pass scale to fragment shader

void main() {
    // Rotate and scale the base quad
    float c = cos(instanceRot);
    float s = sin(instanceRot);
    mat2 rotation = mat2(c, s, -s, c);

    vec2 rotatedPos = rotation * (position * instanceScale);
    vec2 worldPos = instancePos + rotatedPos;

    gl_Position = projection * model * vec4(worldPos, 0.0, 1.0);

    // Pass UV in range [-1, 1] for procedural generation
    vUV = position;
    vSeed = instanceSeed;
    vScale = instanceScale;
}
