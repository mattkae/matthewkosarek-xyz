#include "autumn/autumn_theme.hpp"
#include "main_loop.h"
#include "mathlib.h"
#include "renderer_2d.h"
#include "spring/spring_theme.hpp"
#include "summer/summer_theme.h"
#include "theme.h"
#include "types.h"
#include "webgl_context.h"
#include "winter/winter_theme.hpp"
#include <cstdio>
#include <emscripten/fetch.h>

void load(ThemeType theme);
void unload();
void update(f32 dtSeconds, void *userData);
EM_BOOL selectNone(int eventType, const EmscriptenMouseEvent *mouseEvent,
                   void *userData);
EM_BOOL selectAutumn(int eventType, const EmscriptenMouseEvent *mouseEvent,
                     void *userData);
EM_BOOL selectWinter(int eventType, const EmscriptenMouseEvent *mouseEvent,
                     void *userData);
EM_BOOL selectSpring(int eventType, const EmscriptenMouseEvent *mouseEvent,
                     void *userData);
EM_BOOL selectSummer(int eventType, const EmscriptenMouseEvent *mouseEvent,
                     void *userData);

WebglContext context;
MainLoop mainLoop;
ThemeType type;
Theme *active_theme;

int main() {
  context.init("#theme_canvas");
  emscripten_set_click_callback("#theme_button_default", NULL, false,
                                selectNone);
  emscripten_set_click_callback("#theme_button_autumn", NULL, false,
                                selectAutumn);
  emscripten_set_click_callback("#theme_button_winter", NULL, false,
                                selectWinter);
  emscripten_set_click_callback("#theme_button_spring", NULL, false,
                                selectSpring);
  emscripten_set_click_callback("#theme_button_summer", NULL, false,
                                selectSummer);

  return 0;
}

// -- Scene loading, updating, and unloading logic
void load(ThemeType theme) {
  if (type == theme) {
    printf("This theme is already active.\n");
    return;
  }

  unload(); // Try and unload before we load, so that we start fresh

  type = theme;
  mainLoop.run(update);

  switch (type) {
  case ThemeType::Autumn:
    active_theme = new AutumnTheme(&context);
    break;
  case ThemeType::Winter:
    active_theme = new WinterTheme(&context);
    break;
  case ThemeType::Spring:
    active_theme = new SpringTheme(&context);
    break;
  case ThemeType::Summer:
    active_theme = new SummerTheme(&context);
    break;
  default:
    break;
  }
}

void update(f32 dtSeconds, void *userData) {
  if (!active_theme)
    return;
  active_theme->update(dtSeconds);
  active_theme->render();
}

void unload() {
  delete active_theme;
  active_theme = nullptr;

  type = ThemeType::Default;
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  if (mainLoop.isRunning) {
    mainLoop.stop();
  }
}

// -- HTML5 callbacks
EM_BOOL selectNone(int eventType, const EmscriptenMouseEvent *mouseEvent,
                   void *userData) {
  printf("Default theme selected\n");
  unload();
  return true;
}

EM_BOOL selectAutumn(int eventType, const EmscriptenMouseEvent *mouseEvent,
                     void *userData) {
  printf("Autumn theme selected\n");
  load(ThemeType::Autumn);
  return true;
}

EM_BOOL selectWinter(int eventType, const EmscriptenMouseEvent *mouseEvent,
                     void *userData) {
  printf("Winter theme selected\n");
  load(ThemeType::Winter);
  return true;
}

EM_BOOL selectSpring(int eventType, const EmscriptenMouseEvent *mouseEvent,
                     void *userData) {
  printf("Spring theme selected\n");
  load(ThemeType::Spring);
  return true;
}

EM_BOOL selectSummer(int eventType, const EmscriptenMouseEvent *mouseEvent,
                     void *userData) {
  printf("Summer theme selected\n");
  load(ThemeType::Summer);
  return true;
}
