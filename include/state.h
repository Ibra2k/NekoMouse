#pragma once

#include <unordered_map>
#include "cat.h"
#include "animation.h"

struct GameState 
{
    std::unordered_map<Direction, Animation> walkAnims;
    Animation scratchAnim {};
    Animation washAnim {};
    Animation sitAnim {};

    float textTimer = 0.0f;
    float duration = 1.0f;

    bool showSpeedText {false};
    bool showSavedText {false};
    bool showConnectedText {false};
    bool sitToggle {false};
    bool onlineToggle {false};

    bool showColor {false};

    void initAnims();

    void updateAnims();

    void displayTempText(float speed);
    void displayTempText(const char * text);

    void toggleColorPalette(Color& catColor);

    void cleanAnims();
};
