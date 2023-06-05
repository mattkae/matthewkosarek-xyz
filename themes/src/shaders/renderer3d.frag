varying lowp vec4 v_color;
varying lowp vec4 v_normal;

void main() {
    const lowp vec3 lightDirection = vec3(0.0, 1.0, 0.0);
    gl_FragColor = vec4(v_color.xyz * dot(v_normal.xyz, lightDirection), 1);
}
