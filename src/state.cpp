#include "state.h"
#include <raylib.h>


#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

void GameState::initAnims()
{
    walkAnims[Direction::NORTH].createAnimation(2, "up");
    walkAnims[Direction::SOUTH].createAnimation(2, "down");
    walkAnims[Direction::EAST].createAnimation(2, "right");
    walkAnims[Direction::WEST].createAnimation(2, "left");
    walkAnims[Direction::NORTH_EAST].createAnimation(2, "upright");
    walkAnims[Direction::NORTH_WEST].createAnimation(2, "upleft");
    walkAnims[Direction::SOUTH_EAST].createAnimation(2, "downright");
    walkAnims[Direction::SOUTH_WEST].createAnimation(2, "downleft");

    scratchAnim.createAnimation(2, "scratch");
    washAnim.createAnimation(2, "wash");
    sitAnim.createAnimation(3, "sit");

}

void GameState::updateAnims()
{
    for (auto &[dir, anim] : walkAnims) anim.animationUpdate();
    scratchAnim.animationUpdate();
    washAnim.animationUpdate();
    sitAnim.animationUpdate();
}

void GameState::cleanAnims()
{
    for (auto &frame: scratchAnim.getFrames()) UnloadTexture(frame); 
    for (auto &frame: washAnim.getFrames()) UnloadTexture(frame); 
    for (auto &frame: sitAnim.getFrames()) UnloadTexture(frame); 

    for (auto& [dir, anim] : walkAnims)
    {
        for (auto& frame : anim.getFrames()) { UnloadTexture(frame); } }
}


void GameState::displayTempText(float speed)
{
    if (duration > 0.0f && showSpeedText)
    {
        DrawText(TextFormat("Speed: %d", (int)(10-speed)), 0, 0, 10, RAYWHITE);
        duration -= GetFrameTime();

        // Reset duration and showText switch
        if(duration < 0.0f){
            showSpeedText = false;
            duration = 1;
        }

    }
}

void GameState::displayTempText(const char * text)
{
    if (duration > 0.0f && (showSavedText || showConnectedText))
    {
        DrawText(text, 0, 0, 10, RAYWHITE);
        duration -= GetFrameTime();

        if(duration < 0.0f){
            showSavedText = false;
            showConnectedText = false;
            duration = 1;
        }

    }
}

void GameState::toggleColorPalette(Color& catColor)
{

    if(showColor)
    {

        SetWindowSize(200, 200);
        GuiColorPicker((Rectangle){ 0, 0, 50, 50 }, "Test Picker", &catColor);
    }

}
