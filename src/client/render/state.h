#pragma once

#include <unordered_map>
#include <raylib.h>
#include "../../common/protocol.h"
#include "animation.h"

struct GameState 
{
    std::unordered_map<Direction, Animation> walkAnims;
    Animation scratchAnim {};
    Animation washAnim {};
    Animation sitAnim {};
   
    bool onlineToggle {false};

    void initAnims();
    void updateAnims();
    Texture2D frameFor(const CatState& state);
    void cleanAnims();
};

