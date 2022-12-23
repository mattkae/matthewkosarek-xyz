#include "MainLoop.h"
#include <cstdio>
#include <cstdlib>

EM_BOOL loop(double time, void* loop) {
    MainLoop* mainLoop = (MainLoop*) loop;
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

    mainLoop->updateFunc(deltaTimeSeconds, NULL);
    return true;
}