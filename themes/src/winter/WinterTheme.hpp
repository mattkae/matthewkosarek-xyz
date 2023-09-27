#ifndef WINTER_THEME_HPP
#define WINTER_THEME_HPP

#include "Snowflake.h"
#include "../types.h"
#include "../theme.h"

struct Renderer2d;

struct WinterTheme : public Theme {
public:
    WinterTheme(Renderer2d* renderer);
    ~WinterTheme();
	SnowflakeParticleRenderer spr;
	
	void load(Renderer2d* renderer);
	void update(f32 dtSeconds);
	void render();
	void unload();
private:
    Renderer2d* renderer;
};

#endif
