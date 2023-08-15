
attribute vec2 position; 
attribute vec4 color; 
attribute mat4 vMatrix; 
uniform mat4 projection; 
uniform mat4 model; 
varying lowp vec4 VertexColor;

void main() { 
    vec4 fragmentPosition = projection * model * vMatrix * vec4(position.x, position.y, 0, 1); 
    gl_Position = fragmentPosition; 
    VertexColor = color; 
}
