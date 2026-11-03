#include "mouse.h"
#include <Cocoa/Cocoa.h>

MousePos getGlobalMousePos()
{
    NSPoint mouseLocation = [NSEvent mouseLocation];
    NSScreen * screen = [NSScreen mainScreen];

    CGFloat screenHeight = [screen frame].size.height;
    int flippedY = screenHeight - (int)mouseLocation.y;

    return { (int)mouseLocation.x , flippedY };
}


/*
 *
visibleFrame is shorter than the real screen by the height of the menu bar plus the Dock if it’s at the bottom. Your overlay would be that much shorter, so the cat would be clipped near the bottom of the screen, and the cat’s coordinates (full-screen global mouse positions) would no longer line up with the window.
*
 * */

ScreenDimensions getScreenDimensions()
{
    NSRect frame = [[NSScreen mainScreen] frame];
    return { (int)frame.size.width, (int)frame.size.height };
}

/* ScreenDimensions getScreenDimensions() */
/* { */
/*     NSRect visibleRect = [[NSScreen mainScreen] visibleFrame]; */
/*     CGFloat visibleWidth = visibleRect.size.width; */
/*     CGFloat visibleHeight = visibleRect.size.height; */
/*     return { (int)visibleWidth, (int)visibleHeight }; */
/* } */
