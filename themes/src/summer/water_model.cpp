#include "water_model.h"
#include "../webgl_context.h"
#include "../list.h"
#include "../shaders/water_vert.h"
#include "../shaders/water_frag.h"

WaterModel::WaterModel(Vector3 inPosition, Vector3 inSize)
    : position(inPosition), size(inSize) {}

WaterModel::~WaterModel() { unload(); }

void WaterModel::setSun(Vector3 direction, Vector4 color) {
    sunDir = direction.normalize();
    sunColor = color.toVector3();
}

void WaterModel::load(WebglContext* inContext) {
    context = inContext;
    shader = loadShader(shader_water_vert, shader_water_frag);
    useShader(shader);

    attributes.position = getShaderAttribute(shader, "position");
    uniforms.projection = getShaderUniform(shader, "projection");
    uniforms.view       = getShaderUniform(shader, "view");
    uniforms.model      = getShaderUniform(shader, "model");
    uniforms.time       = getShaderUniform(shader, "time");
    uniforms.amplitude  = getShaderUniform(shader, "amplitude");
    uniforms.cameraPos  = getShaderUniform(shader, "cameraPos");
    uniforms.sunDir     = getShaderUniform(shader, "sunDir");
    uniforms.sunColor   = getShaderUniform(shader, "sunColor");

    // Camera: forward almost horizontal with a slight downward pitch so the
    // horizon sits near the vertical center and the plane fills the lower half.
    cameraPos  = Vector3(0.f, 5.f, 20.f);
    projection = Mat4x4().getPerspectiveProjection(
        0.1f, 1000.f, 0.9f,
        static_cast<f32>(context->width) / static_cast<f32>(context->height));
    view  = Mat4x4().getLookAt(cameraPos, Vector3(0.f, 4.f, -10.f), Vector3(0.f, 1.f, 0.f));
    model = Mat4x4().translate(position);

    // Sensible defaults; the theme overrides these with the actual sun via setSun().
    sunDir   = Vector3(0.f, 0.4f, -1.f).normalize();
    sunColor = Vector3(1.0f, 0.95f, 0.75f);

    // Build the grid in local XZ, centered on the origin, extents from size.
    matte::List<Vector2> verts;
    matte::List<u32> idx;
    verts.allocate((RES + 1) * (RES + 1));
    idx.allocate(RES * RES * 6);

    for (i32 z = 0; z <= RES; z++) {
        for (i32 x = 0; x <= RES; x++) {
            f32 fx = ((static_cast<f32>(x) / RES) - 0.5f) * size.x;
            f32 fz = ((static_cast<f32>(z) / RES) - 0.5f) * size.z;
            verts.add(Vector2(fx, fz));
        }
    }

    const i32 stride = RES + 1;
    for (i32 z = 0; z < RES; z++) {
        for (i32 x = 0; x < RES; x++) {
            u32 tl = z * stride + x;
            u32 tr = tl + 1;
            u32 bl = (z + 1) * stride + x;
            u32 br = bl + 1;
            idx.add(tl); idx.add(bl); idx.add(tr);
            idx.add(tr); idx.add(bl); idx.add(br);
        }
    }
    numIndices = static_cast<i32>(idx.numElements);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verts.numElements * sizeof(Vector2), &verts.data[0], GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.numElements * sizeof(u32), &idx.data[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(attributes.position);
    glVertexAttribPointer(attributes.position, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2), (GLvoid*)0);

    glBindVertexArray(0);
    verts.deallocate();
    idx.deallocate();
}

void WaterModel::update(f32 dtSeconds) {
    elapsedTime += dtSeconds;
}

void WaterModel::render() {
    useShader(shader);

    // Opaque, depth-tested. Wipe the depth buffer first: the 2D sun was drawn at
    // z=0 (nearest) with depth writes on, so without this the LEQUAL test would
    // reject every perspective water fragment. Clearing depth (not color) lets the
    // opaque water paint over the lower half of the sun while still self-occluding.
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_TRUE);
    glClear(GL_DEPTH_BUFFER_BIT);

    setShaderMat4(uniforms.projection, projection);
    setShaderMat4(uniforms.view, view);
    setShaderMat4(uniforms.model, model);
    setShaderFloat(uniforms.time, elapsedTime);
    setShaderFloat(uniforms.amplitude, size.y);
    glUniform3f(uniforms.cameraPos, cameraPos.x, cameraPos.y, cameraPos.z);
    glUniform3f(uniforms.sunDir, sunDir.x, sunDir.y, sunDir.z);
    glUniform3f(uniforms.sunColor, sunColor.x, sunColor.y, sunColor.z);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void WaterModel::unload() {
    if (vao) { glDeleteVertexArrays(1, &vao); vao = 0; }
    if (vbo) { glDeleteBuffers(1, &vbo); vbo = 0; }
    if (ebo) { glDeleteBuffers(1, &ebo); ebo = 0; }
    if (shader) { glDeleteProgram(shader); shader = 0; }
}
