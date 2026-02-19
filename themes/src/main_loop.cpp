#include "main_loop.h"
#include <cstdio>
#include <cstdlib>

EM_BOOL loop(double time, void *loop) {
  MainLoop *mainLoop = (MainLoop *)loop;
  if (!mainLoop->isRunning) {
    return false;
  }

  if (mainLoop->lastTime == 0) {
    mainLoop->lastTime = time;
    return true;
  }

  long deltaTime = time - mainLoop->lastTime;
  mainLoop->lastTime = time;
  mainLoop->elapsedTime += deltaTime;
  mainLoop->numFrames++;
  float deltaTimeSeconds = static_cast<float>(deltaTime) / 1000.f;

  if (mainLoop->elapsedTime >= 1000.0) {
    printf("FPS: %d\n", mainLoop->numFrames);

    mainLoop->elapsedTime = 0.0;
    mainLoop->numFrames = 0;
  }

  // Ignore any update with a greater than 0.1 change. We were
  // probably tabbed away, so this is uninteresting to us.
  if (deltaTimeSeconds > 0.1) {
    return true;
  }

  mainLoop->updateFunc(deltaTimeSeconds, NULL);
  return true;
}
