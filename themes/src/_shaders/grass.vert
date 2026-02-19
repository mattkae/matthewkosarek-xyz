attribute vec2 position;        // Local quad vertex: x in [-0.5, 0.5], y in [0, 1]
attribute vec3 instancePos;     // Per-instance: world-space base of blade
attribute float instancePhase;  // Per-instance: random phase offset for sway
attribute float instanceHeight; // Per-instance: height scale multiplier

uniform mat4 projection;
uniform mat4 view;
uniform float time;
uniform float bladeWidth;
uniform float bladeHeight;
uniform float swayAmount;

varying lowp vec2 vUV;

void main() {
    vec3 cameraRight = vec3(view[0][0], view[1][0], view[2][0]);
    float h = bladeHeight * instanceHeight;
    float sway = sin(time * 1.5 + instancePhase) * swayAmount * position.y;
    vec3 worldPos = instancePos
                  + cameraRight * (position.x + sway) * bladeWidth
                  + vec3(0.0, 1.0, 0.0) * position.y * h;

    gl_Position = projection * view * vec4(worldPos, 1.0);
    vUV = vec2(position.x + 0.5, position.y);
}
