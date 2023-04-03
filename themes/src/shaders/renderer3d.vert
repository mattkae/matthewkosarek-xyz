attribute vec4 position;
attribute vec4 color;
attribute vec4 normal;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
varying lowp vec4 VertexColor;
varying lowp vec4 VertexNormal;

void main() {
    vec4 fragmentPosition = projection * view * model * position;
    gl_Position = fragmentPosition;
    VertexColor = color;
    VertexNormal = normal;
}
