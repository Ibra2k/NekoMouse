#pragma once

class MousePos
{
public:
    int x {};
    int y {};
};

struct ScreenDimensions
{
    int width {};
    int height {};
};

MousePos getGlobalMousePos();
ScreenDimensions getScreenDimensions();
