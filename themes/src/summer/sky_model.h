#ifndef SKY_MODEL_H
#define SKY_MODEL_H

#include "../shader.h"
#include "../types.h"

struct WebglContext;

/// A full-screen sky background. Renders a mid-afternoon gradient (from the
/// water horizon up to the top of the screen) and sparse, wispy procedural
/// clouds that shrink toward the horizon. Standalone: owns its own shader and
/// a two-triangle full-screen quad, drawn before the sun and water.
class SkyModel {
public:
    SkyModel();
    ~SkyModel();

    void load(WebglContext* context);
    void update(f32 dtSeconds);
    void render();
    void unload();

    /// Screen-space y (0..1) of the water horizon, where the gradient starts.
    f32 horizon = 0.5f;

private:
    WebglContext* context = nullptr;
    Shader shader = 0;
    u32 vao = 0;
    u32 vbo = 0;
    f32 elapsedTime = 0.f;

    struct {
        i32 position = -1;
    } attributes;

    struct {
        i32 time = -1;
        i32 horizon = -1;
    } uniforms;
};

#endif // SKY_MODEL_H
