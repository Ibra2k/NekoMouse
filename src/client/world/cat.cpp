#include <cstdint>
#include <raylib.h>
#include <cmath>
#include <utility>
#include "cat.h"

struct Cat::pimplImplementation
{

    ScreenDimensions screen = getScreenDimensions();
    CatState m_state;

    float m_speed {1.0f};
    float m_scale {2.0f};

    bool hasReachedTarget {false};
    float m_idleThreashold {0.0f};

    float m_idleTimer {};
    int m_prevMousePosX {};
    int m_prevMousePosY {};

};


// destructor defined since without it will be implicitly inlined and in every .cpp
// and since main.cpp doesn't know the size of Implementation it can't destroy the unique_ptr
Cat::~Cat() = default;

Cat::Cat()
{
    // pimplPtr's destruction logic lives in cat.cpp since pimplImplementation is here
    pimplPtr = std::make_unique<pimplImplementation>();


    pimplPtr->m_state.color = static_cast<std::uint32_t>(ColorToInt(PINK));

    pimplPtr->m_state.x = pimplPtr->screen.width/2.0f;
    pimplPtr->m_state.y = pimplPtr->screen.height/2.0f;
}

float Cat::getCurrCatPosX() const
{
    return pimplPtr->m_state.x;
}

float Cat::getCurrCatPosY() const
{
    return pimplPtr->m_state.y;
}

float Cat::getMoveDelay() const
{
    return pimplPtr->m_idleThreashold;
}

Color Cat::getColor()
{
    return GetColor(pimplPtr->m_state.color);
}

void Cat::setColor(std::uint32_t colorVal)
{
    pimplPtr->m_state.color = colorVal;
}

void Cat::setPosition(float x, float y)
{
    pimplPtr->m_state.x = x;
    pimplPtr->m_state.y = y;
}


std::pair<float, float> Cat::getCenterPos(float frameWidth, float frameHeight)
{

    float catWidth {frameWidth * pimplPtr->m_scale};
    float catHeight {frameHeight * pimplPtr->m_scale};

    float centerX = (GetScreenWidth() - catWidth) / 2.0f;
    float centerY = (GetScreenHeight() - catHeight) / 2.0f;

    return {centerX, centerY};
}

// Perform linear interp
std::pair<float, float> Cat::getNewPos(int x, int y)
{

    pimplPtr->m_state.x += (x - pimplPtr->m_state.x) * pimplPtr->m_speed * GetFrameTime();
    pimplPtr->m_state.y += (y - pimplPtr->m_state.y) * pimplPtr->m_speed * GetFrameTime();


    return {pimplPtr->m_state.x, pimplPtr->m_state.y};
}


void Cat::updateMouseTracking(const MousePos& mouse)
{

}

bool Cat::isMouseIdle() const
{
    return pimplPtr->m_idleTimer >= pimplPtr->m_idleThreashold;
}



bool Cat::hasCatReachedMouse(const MousePos& mouse)
{

    int deltaX = std::abs(mouse.x - pimplPtr->m_state.x);
    int deltaY = std::abs(mouse.y - pimplPtr->m_state.y);

    if (deltaX < 5 && deltaY < 5)
    {
        pimplPtr->hasReachedTarget = true;
    }
    else 
    {
        pimplPtr->hasReachedTarget = false;
    }

    return pimplPtr->hasReachedTarget;

}


Direction getCatDirection(float catX, float catY, float mouseX, float mouseY)
{

    float deltaX = mouseX - catX;
    float deltaY = catY - mouseY;

    float angle = std::atan2(deltaY, deltaX) * 180.0f / PI;

    if (angle > -22.5f && angle <= 22.5f) return Direction::EAST;
    else if (angle > 22.5f && angle <= 67.5f) return Direction::NORTH_EAST;
    else if (angle > 67.5f && angle <= 112.5f) return Direction::NORTH;
    else if (angle > 112.5f && angle <= 157.5f) return Direction::NORTH_WEST;
    else if (angle > 157.5f || angle <= -157.5f) return  Direction::WEST;
    else if (angle > -157.5f && angle <= -112.5f) return Direction::SOUTH_WEST;
    else if (angle > -112.5f && angle <= -67.5f) return Direction::SOUTH;
    else return Direction::SOUTH_EAST;

}

#define MAX_SPEED 10

float Cat::decreaseDelay()
{
    pimplPtr->m_idleThreashold -= 1;
    if (pimplPtr->m_idleThreashold < 0) pimplPtr->m_idleThreashold = 0;

    return pimplPtr->m_idleThreashold;
}

float Cat::increaseDelay()
{
    pimplPtr->m_idleThreashold += 1;
    if (pimplPtr->m_idleThreashold > MAX_SPEED) pimplPtr->m_idleThreashold = MAX_SPEED;

    return pimplPtr->m_idleThreashold;
}

const CatState& Cat::getState() const 
{
    return pimplPtr->m_state;
}

void Cat::updateBehavior(const MousePos& mouse, Settings& settings)
{

    if(mouse.x != pimplPtr->m_prevMousePosX || mouse.y != pimplPtr->m_prevMousePosY)
    {
        pimplPtr->m_idleTimer = 0.0f;
        pimplPtr->m_prevMousePosX = mouse.x;
        pimplPtr->m_prevMousePosY = mouse.y;
    }
    else
    {
        // if mouse stopped moving -> start timer
        pimplPtr->m_idleTimer += GetFrameTime();
    }


    bool hasReached = hasCatReachedMouse(mouse);

    if (settings.isSitting) pimplPtr->m_state.behavior = Behavior::SITTING;
    else if (!isMouseIdle()) pimplPtr->m_state.behavior = Behavior::WASHING;
    else if (hasReached) pimplPtr->m_state.behavior = Behavior::SCRATCHING;
    else 
    {
        pimplPtr->m_state.behavior = Behavior::WALKING;

        pimplPtr->m_state.direction = getCatDirection(
            pimplPtr->m_state.x,
            pimplPtr->m_state.y,
            mouse.x,
            mouse.y
        );

    }

    pimplPtr->m_speed = settings.speed;
    pimplPtr->m_idleThreashold = settings.delay;
    pimplPtr->m_state.color = ColorToInt(settings.color);


}


