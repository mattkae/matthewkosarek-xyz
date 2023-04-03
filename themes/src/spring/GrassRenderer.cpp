#include "GrassRenderer.hpp"
#include "Renderer3d.h"

void GrassRenderer::load(GrassRendererLoadData params, Renderer3d* renderer) {
    const f32 COLUMN_INCREMENT = GRASS_BLADES_PER_COL / params.area.x;
    const f32 ROW_INCREMENT = GRASS_BLADES_PER_ROW / params.area.y;
    for (i32 r = 0; r < GRASS_BLADES_PER_ROW; r++) {
        i32 indexOffset = r * GRASS_BLADES_PER_ROW;
        f32 y = ROW_INCREMENT * r;
        for (i32 c = 0; c < GRASS_BLADES_PER_COL; c++) {
            f32 x = COLUMN_INCREMENT * c;
            i32 index = indexOffset + c;
            grassBlades[index].position = Vector3(x, y, 0);
            grassBlades[index].top_offset = Vector2(0, 0);
        }
    }
}

void GrassRenderer::update(f32 seconds) {
    
}

void GrassRenderer::render(Renderer3d* renderer) {

}

void GrassRenderer::unload() {

}
