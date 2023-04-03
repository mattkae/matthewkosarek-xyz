varying lowp vec4 VertexColor;
varying lowp vec4 VertexNormal;

void main() {
    const lowp vec3 lightDirection = vec3(0.0, 1.0, 0.0);
    gl_FragColor = vec4(VertexColor.xyz * dot(VertexNormal.xyz, lightDirection), 1);
}
