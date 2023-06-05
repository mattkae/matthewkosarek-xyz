varying lowp vec2 v_texture_coordinates;
varying lowp vec4 v_normal;

uniform sampler2D u_texture;

void main() {
    const lowp vec3 lightDirection = vec3(0.0, 1.0, 0.0);
    const vec4 color = texture(u_texture, v_texture_coordinates)
    gl_FragColor = vec4(xyz * dot(v_normal.xyz, lightDirection), 1);
}
