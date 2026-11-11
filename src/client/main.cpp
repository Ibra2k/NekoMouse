#include "net/network_client.h"
#include "settings/settings.h"
#include "platform/mouse.h"
#include "../client/render/state.h"
#include "../client/world/cat.h"
#include "../client/render/cat_renderer.h"
#include "ui/menu.h"
#include <asio/socket_base.hpp>
#include <exception>
#include <netinet/in.h>
#include <raylib.h>
#include <asio.hpp>
#include <arpa/inet.h>
#include <unistd.h>

int main() {

    // Initialization 
    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_TRANSPARENT |
               FLAG_WINDOW_TOPMOST | FLAG_WINDOW_MOUSE_PASSTHROUGH) ;

    SetTraceLogLevel(LOG_DEBUG);
    ScreenDimensions screen  = getScreenDimensions();

    InitWindow(
        static_cast<int>(screen.width) - 1,
        static_cast<int>(screen.height) - 1,
        ""
    );

    asio::io_context io;

    SetTargetFPS(60);


    //TODO: Move "GameState" -> cat_renderer.cpp/h
    GameState game {};
    Cat myCat {};
    Menu menu {};
    NetworkClient client {screen, io};


    UserMenu userMenu {
        {(float)screen.width/2 - 30, 10.0f, 60.0f, 20.0f},
        10.0f,
        16,
        {80, 101, 104, 150}
    };

    Settings settings {loadSettings()};

    game.initAnims();

    while (!WindowShouldClose()) {

        // Update
        MousePos mouse = getGlobalMousePos(); // Linux x11 or Mac functions
        
        myCat.updateBehavior(mouse, settings);

        if (myCat.getState().behavior == Behavior::WALKING)
        {
            auto newPos = myCat.getNewPos(mouse.x, mouse.y);
            myCat.setPosition(newPos.first, newPos.second);
        }

        menu.update(
            {(float)mouse.x, (float)mouse.y},
            userMenu,
            (float)screen.width,
            settings
        );

        game.updateAnims();


        if(game.onlineToggle)
        {
                //TODO: Send state on a "state change"
                client.sendState(myCat.getState(), (MetaData){Type::UPDATE, 1, 18});
        }


        // (Left ALT + Q) Quit Program 
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Q))  break;

        // Connect Online
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_O))
        {
            game.onlineToggle = !game.onlineToggle;
            TraceLog(LOG_INFO, "Connecting To Server...\n");
            
            try
            {
                client.connect();
            }
            catch (std::exception &e)
            {
                TraceLog(LOG_INFO, "Could Not Connect\n");
            }
        }

        // Drawing
        BeginDrawing();
        ClearBackground(BLANK);

        drawCat(myCat.getState(), game, settings.scale);

        if( menu.isToggled() ) menu.drawOnOpen(userMenu, settings, screen.width);
        else menu.drawOnClose(userMenu);


        EndDrawing();
    }

    saveSettings(settings);

    game.cleanAnims();

    CloseWindow();
    return 0;
}
