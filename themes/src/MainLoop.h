#pragma once

#include <emscripten.h>
#include <emscripten/html5.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>

EM_BOOL loop(double time, void* loop);

struct MainLoop {
    bool isRunning = false;
	double lastTime = 0, elapsedTime = 0;
	int numFrames = 0;
	void (*updateFunc)(float dtSeconds, void *userData);

    void run(void (*cb)(float dtSeconds, void *userData)) {
		isRunning = true;
		lastTime = 0;
		elapsedTime = 0;
		numFrames = 0;
		updateFunc = cb;

		emscripten_request_animation_frame_loop(loop, this);
	}

    void stop() {
        isRunning = false;
    }
};