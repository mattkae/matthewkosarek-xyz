#ifndef WINTER_THEME_HPP
#define WINTER_THEME_HPP

#include "Snowflake.h"
#include "../types.h"
#include "../theme.h"
#include "../Renderer2d.h"

struct WebglContext;

struct WinterTheme : public Theme {
public:
    WinterTheme(WebglContext*);
    ~WinterTheme();
	SnowflakeParticleRenderer spr;
	
	void load();
	void update(f32 dtSeconds);
	void render();
	void unload();
private:
    Renderer2d renderer;
};

#endif
