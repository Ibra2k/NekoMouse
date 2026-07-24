#include "state.h"
#include <fstream>
#include <ios>
#include <raylib.h>
#include <string>
#include <utility>
#include <iostream>


// Used for the speed control keybinding
#define MAX_SPEED 10

int main() {

    // Initialization 
    const int catOffset = 20;

    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_TRANSPARENT |
               FLAG_WINDOW_TOPMOST| FLAG_WINDOW_UNFOCUSED) ;


    InitWindow(100, 100, "");

    SetTargetFPS(60);
    GameState game {};
    Cat myCat {};
    Color catColor {PINK}; // default color

    std::ifstream CatColor(".savedColors.txt");
    if(!CatColor.fail())
    {
        std::string tmpStr {};
        std::getline(CatColor, tmpStr);

        unsigned int initialColor {static_cast<unsigned int>(std::stoul(tmpStr))};
        catColor = GetColor(initialColor);
        CatColor.close();
    }


    float newSpeed {};

    game.initAnims();

    while (!WindowShouldClose()) {

        // Update
        MousePos mouse = getGlobalMousePos(); // X11 functions
        myCat.updateMouseTracking(mouse);


        if (myCat.isMouseIdle() && !game.sitToggle)
        {
            auto newPos = myCat.getNewPos(mouse.x, mouse.y); // Moves to target position
            SetWindowPosition(
                newPos.first + catOffset,
                newPos.second + catOffset
            );
        }

        game.updateAnims();


        // Keybindings
        
        // (CTRL + Q) Quit Program 
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Q))  break;

        // Open Color palette
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_C))
        {
            game.showColor = !game.showColor;
            if(game.showColor == false) SetWindowSize(100, 100);
        }

        // (CTRL + UP/DOWN) Speed Control 
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_UP))
        {
            newSpeed = myCat.increaseSpeed();
            game.showSpeedText = true;
        }
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_DOWN))
        {
            newSpeed = myCat.decreaseSpeed();
            game.showSpeedText = true;
        }

        // Save choosen color
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S))
        {
            std::ofstream ColorsSaved(".savedColors.txt");
            int hexValue = ColorToInt(catColor);
            ColorsSaved << hexValue;

            game.showSavedText = true;
        }

        // Sit and stand
        if (IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_S))
        {
            game.sitToggle = !game.sitToggle;
            std::cout << "Cat Sit Toggled: " << std::boolalpha << game.sitToggle << '\n';
        }

        // Drawing
        BeginDrawing();
        ClearBackground(BLANK);


        if (myCat.isMouseIdle() && !myCat.hasCatReachedMouse(mouse) && !game.sitToggle)
        {
            Direction direction = myCat.checkCatQuadrant(myCat.getCurrCatPosX(), myCat.getCurrCatPosY());
            myCat.drawTexture(game.walkAnims[direction].getCurrentFrame(), catColor);
        }

        else if (myCat.isMouseIdle() && myCat.hasCatReachedMouse(mouse))
        {
            Texture2D currentFrame = game.scratchAnim.getCurrentFrame();
            myCat.drawTexture(currentFrame, catColor);
        }

        else if (game.sitToggle)
        {
            Texture2D currentFrame = game.sitAnim.getCurrentFrame();
            myCat.drawTexture(currentFrame, catColor);
        }

        else
        {
            Texture2D currentFrame = game.washAnim.getCurrentFrame();
            myCat.drawTexture(currentFrame, catColor);
        }

        game.displayTempText(newSpeed);
        game.displayTempText();
        game.toggleColorPalette(catColor);


        EndDrawing();
    }

    game.cleanAnims();

    CloseWindow();
    return 0;
}
