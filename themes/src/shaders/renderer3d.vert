attribute vec4 position;
attribute vec4 color;
attribute vec4 normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
varying lowp vec4 v_color;
varying lowp vec4 v_normal;

void main() {
    vec4 fragmentPosition = projection * view * model * position;
    gl_Position = fragmentPosition;
    v_color = color;
    v_normal = normal;
}
