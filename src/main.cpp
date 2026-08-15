#include "state.h"
#include <array>
#include <asio/socket_base.hpp>
#include <exception>
#include <fstream>
#include <ios>
#include <netinet/in.h>
#include <raylib.h>
#include <string>
#include <utility>
#include <iostream>
#include <asio.hpp>
#include <arpa/inet.h> // For htonl/ntohl
#include <cstdint> // For uint32_t
#include <cstdlib> // for getenv

using asio::ip::tcp;
using asio::ip::udp;

// Used for the speed control keybinding
#define MAX_SPEED 10
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT "1313"

// Returns a writable path for the saved-color file, inside the user's
// Application Support folder — NOT relative to the working directory,
// since that's unpredictable (and /Applications isn't writable anyway).
std::string getSaveFilePath()
{
    std::string home = std::getenv("HOME");
    std::string dir = home + "/Library/Application Support/NekoCat";
    system(("mkdir -p \"" + dir + "\"").c_str()); // ensure the folder exists
    return dir + "/savedColors.txt";
}

int main() {

    // Initialization 
    const int catOffset = 20;

    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_TRANSPARENT |
               FLAG_WINDOW_TOPMOST) ;


    InitWindow(100, 100, "");
	asio::io_context io;

	tcp::resolver tcpResolver(io);

	std::string serverAddress {SERVER_IP};
	std::string serverPort {SERVER_PORT};

    tcp::resolver::results_type tcpEndpoint = tcpResolver.resolve(tcp::v4(), serverAddress, serverPort);

    tcp::socket tcpSocket(io);

    SetTargetFPS(60);
    GameState game {};
    Cat myCat {};
    Color catColor {PINK}; // default color
    float newSpeed {};


    std::ifstream CatColor(getSaveFilePath());
    if(CatColor.is_open() && !CatColor.fail())
    {
        std::string tmpStr {};
        std::getline(CatColor, tmpStr);

        if (!tmpStr.empty())
        {
            try
            {
                unsigned int initialColor {static_cast<unsigned int>(std::stoul(tmpStr))};
                catColor = GetColor(initialColor);
            }
            catch (const std::exception& e)
            {
                std::cerr << "Saved color file was corrupted, using default: " << e.what() << '\n';
            }
        }
        CatColor.close();
    }



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

            if(game.onlineToggle)
            {
                float xNetworkVal = htonl(myCat.getCurrCatPosX());
                float yNetworkVal = htonl(myCat.getCurrCatPosY());

                tcpSocket.send(asio::buffer(&xNetworkVal, sizeof(xNetworkVal)));
                tcpSocket.send(asio::buffer(&yNetworkVal, sizeof(yNetworkVal)));
            }

        }


        game.updateAnims();


        // Keybindings
        // (CTRL + Q) Quit Program 
        if (IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_Q))  break;

        // Open Color palette
        if (IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_C))
        {
            game.showColor = !game.showColor;
            if(game.showColor == false) SetWindowSize(100, 100);
        }

        if (IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_RIGHT))
        {
            newSpeed = myCat.increaseSpeed();
            game.showSpeedText = true;
        }

        if (IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_LEFT))
        {
            newSpeed = myCat.decreaseSpeed();
            game.showSpeedText = true;
        }

        // Save choosen color
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S))
        {
            std::ofstream ColorsSaved(getSaveFilePath());
            if (!ColorsSaved)
            {
                std::cerr << "Failed to open save file for writing!\n";
            }
            else
            {
                int hexValue = ColorToInt(catColor);
                ColorsSaved << hexValue;
                game.showSavedText = true;
            }
        }

        // Sit and stand
        if (IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_S))
        {
            game.sitToggle = !game.sitToggle;
            std::cout << "Cat Sit Toggled: " << std::boolalpha << game.sitToggle << '\n';
        }

        // Connect Online
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_O))
        {
            game.onlineToggle = !game.onlineToggle;

            std::cout << "Connecting Online..." << '\n';

            try
            {
                std::error_code errorCode;
                asio::connect(tcpSocket, tcpEndpoint);
            }
            catch (std::exception e)
            {
                std::cout << "Could Not Connect\n";
                return 1;
            }

            game.showConnectedText = true;
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
        if(game.showSavedText) game.displayTempText("Saved!");
        if(game.showConnectedText) game.displayTempText("Connected!");

        game.toggleColorPalette(catColor);


        EndDrawing();
    }

    game.cleanAnims();

    CloseWindow();
    return 0;
}
