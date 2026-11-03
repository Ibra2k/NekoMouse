#include "mouse.h"
#include <X11/Xlib.h>

MousePos getGlobalMousePos() {
    Display *dis = XOpenDisplay(NULL);
    Window root = DefaultRootWindow(dis);
    Window root_return, child_return;
    int rx, ry, wx, wy;
    unsigned int mask;
    XQueryPointer(dis, root, &root_return, &child_return, &rx, &ry, &wx, &wy, &mask);
    XCloseDisplay(dis);
    return {rx, ry};
}

ScreenDimensions getScreenDimensions() {
    Display *dis = XOpenDisplay(NULL);

    int screen = DefaultScreen(dis);
    int width = XDisplayWidth(dis, screen);
    int height = XDisplayHeight(dis, screen);

    XCloseDisplay(dis);
    return {width, height};
}
