#include "mouse.h"
#include  <windows.h>

MousePos getGlobalMousePos() {

    POINT p;

    GetCursorPos(&p);

    return {static_cast<int>(p.x), static_cast<int>(p.y)};
}

ScreenDimensions getScreenDimensions() {

    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    return {width, height};
}
