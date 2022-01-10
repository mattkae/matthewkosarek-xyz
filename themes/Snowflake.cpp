#include "Snowflake.h"

void SnowflakeParticleRenderer::load(SnowflakeLoadParameters params, Renderer2d* renderer) {
	updateData = new SnowflakeUpdateData[maxSnowflakes];
    renderData = new SnowflakeRenderData[maxSnowflakes];
	activeIndex = 0;

	for (int32 s = 0; s < maxSnowflakes; s++) {
		auto ud = &updateData[s];
		auto rd = &renderData[s];

		ud->velocity = params->flakeV0;
	}
}

void SnowflakeParticleRenderer::update(float32 dtSeconds) {
	
}

void SnowflakeParticleRenderer::render(Renderer2d* renderer) {

}

void SnowflakeParticleRenderer::unload() {

}
