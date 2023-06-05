attribute vec4 position;
attribute vec2 texture_coordinates;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
varying lowp vec2 v_texture_coordinates;
varying lowp vec4 v_normal;

void main() {
    vec4 fragmentPosition = projection * view * model * position;
    gl_Position = fragmentPosition;
    v_texture_coordinates = texture_coordinates;
    v_normal = normal;
}
