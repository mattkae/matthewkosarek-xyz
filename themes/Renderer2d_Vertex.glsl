attribute vec2 position;
attribute vec4 color;
uniform mat4 projection;
uniform mat4 model;
varying lowp vec4 VertexColor;
void main() {
    vec4 fragmentPosition = projection * model * vec4(position, 1, 1);
    gl_Position = fragmentPosition;
    VertexColor = color;
};