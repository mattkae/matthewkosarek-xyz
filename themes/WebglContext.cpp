#include "WebglContext.h"
#include <cstdio>

EM_BOOL onScreenSizeChanged(int eventType, const EmscriptenUiEvent *uiEvent, void *userData) {
    printf("Here\n");
    WebglContext* context = (WebglContext*)userData;

    
    context->width = uiEvent->documentBodyClientWidth;
    context->height = uiEvent->documentBodyClientHeight;
    return true;
}
