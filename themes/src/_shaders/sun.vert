
attribute vec2 position;
attribute vec4 color;
attribute mat4 vMatrix;
uniform mat4 projection;
uniform mat4 model;
varying lowp vec4 VertexColor;
varying lowp vec2 TexCoord;

void main() {
    vec4 fragmentPosition = projection * model * vMatrix * vec4(position.x, position.y, 0, 1);
    gl_Position = fragmentPosition;
    VertexColor = color;
    // Normalize the position - the center is at (0,0) and edge vertices are at distance 'radius'
    // We want TexCoord to be in the range roughly [-1, 1] at the edges
    lowp float maxDist = length(position);
    if (maxDist > 0.1) {
        TexCoord = position / maxDist;
    } else {
        TexCoord = vec2(0.0, 0.0);
    }
}
