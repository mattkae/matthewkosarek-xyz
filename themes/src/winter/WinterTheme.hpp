#ifndef WINTER_THEME_HPP
#define WINTER_THEME_HPP

#include "Snowflake.h"
#include "../types.h"

struct Renderer2d;

struct WinterTheme {
	SnowflakeParticleRenderer spr;
	
	void load(Renderer2d* renderer);
	void update(f32 dtSeconds);
	void render(Renderer2d* renderer);
	void unload();
};

#endif