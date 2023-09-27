#ifndef THEME_H
#define THEME_H

#include "types.h"

enum class ThemeType {
	Default = 0,
    Autumn,
	Winter,
	Spring,
	Summer
};

class Theme
{
public:
    virtual ~Theme() = default;
    virtual void update(f32 dt) = 0;
    virtual void render() = 0;
};

#endif
