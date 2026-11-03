#include "state.h"
#include "../../common/cat_state.h"
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

Texture2D GameState::frameFor(const CatState& state)
{
    switch (state.behavior)
    {
        case Behavior::SITTING:
            return sitAnim.getCurrentFrame();
            break;
        case Behavior::WALKING:
            return walkAnims[state.direction].getCurrentFrame();
            break;
        case Behavior::SCRATCHING:
            return scratchAnim.getCurrentFrame();
            break;
        default:
            return washAnim.getCurrentFrame();
            break;
    }
}

