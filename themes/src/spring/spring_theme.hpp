#ifndef SPRING_THEME_HPP
#define SPRING_THEME_HPP

#include "../mathlib.h"
#include "../renderer_2d.h"
#include "../renderer_3d.h"
#include "../theme.h"
#include "../types.h"
#include "grass_renderer.hpp"

class RectangularGradient;

enum class SpringThemeState {
  Loading = 0,
  LoadedShader,
  LoadedBunny,
  PreHop,
  Hopping,
  Idle
};

class SpringTheme : public Theme {
public:
  SpringTheme(WebglContext *);
  ~SpringTheme();
  Renderer3d renderer;
  SpringThemeState state;
  f32 bunnySpeed = 5.f;
  Vector3 bunnyPosition = Vector3(0, 0, 0);
  Vector3 bunnyTarget = Vector3(0, 0, 0);
  Vector3 hopIncrement = Vector3(0, 0, 0);

  f32 numHops = 0;
  f32 hopCount = 0;
  f32 bunnyHopAnimationTimer = 0.f;
  f32 stateTimer = 0.f;
  f32 bunnyRotation = 0.f;
  f32 targetRotation = 0.f;

  Mesh3d bunnyMesh;
  GrassRenderer grassRenderer;
  Renderer2d renderer2d;
  RectangularGradient *background;

  void load(WebglContext *);
  void update(f32 dtSeconds);
  void render();
  void unload();
};

#endif
