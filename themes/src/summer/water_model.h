#ifndef WATER_MODEL_H
#define WATER_MODEL_H

#include "../mathlib.h"
#include "../shader.h"
#include "../types.h"

struct WebglContext;

/// A water mesh is a 3D plane in the simulation world.
///
/// The mesh reflects the sun above it and refracts whatever is
/// below its surface. It owns its own shader, geometry buffers,
/// and perspective camera, so it is fully standalone.
///
/// The mesh can be rendered and updated.
class WaterModel {
public:
    WaterModel(Vector3 position, Vector3 size);
    ~WaterModel();

    /// Compile the water shader, build the grid mesh, and set up the camera.
    void load(WebglContext* context);
    void update(f32 dtSeconds);
    void render();
    void unload();

    /// Direction pointing toward the sun (normalized internally) and its color,
    /// used to fake the surface reflection.
    void setSun(Vector3 direction, Vector4 color);

private:
    Vector3 position;
    Vector3 size;

    WebglContext* context = nullptr;
    Shader shader = 0;
    u32 vao = 0;
    u32 vbo = 0;
    u32 ebo = 0;
    i32 numIndices = 0;
    f32 elapsedTime = 0.f;

    Mat4x4 projection;
    Mat4x4 view;
    Mat4x4 model;

    Vector3 cameraPos;
    Vector3 sunDir;
    Vector3 sunColor;

    static const i32 RES = 200;

    struct {
        i32 position = -1;
    } attributes;

    struct {
        i32 projection = -1;
        i32 view = -1;
        i32 model = -1;
        i32 time = -1;
        i32 amplitude = -1;
        i32 cameraPos = -1;
        i32 sunDir = -1;
        i32 sunColor = -1;
    } uniforms;
};

#endif // WATER_MODEL_H
