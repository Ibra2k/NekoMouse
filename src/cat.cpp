#include "cat.h"
#include <raylib.h>
#include <cmath>
#include <utility>

struct Cat::pimplImplementation
{

    ScreenDimensions screen = getScreenDimensions();

    float m_speed {1.0f};
    float m_scale {2.0f};
    const float m_pixels {32.0f};
    const float m_PI {3.14};
    bool hasReachedTarget {false};

    float m_currCatPosX {};
    float m_currCatPosY {};

    float m_prevCatPosX {};
    float m_prevCatPosY {};

    float m_idleTimer {};
    float m_idleThreashold {2.0f};
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


    pimplPtr->m_currCatPosX = pimplPtr->screen.width/2.0f;
    pimplPtr->m_currCatPosY = pimplPtr->screen.height/2.0f;
}


float Cat::getDimensions() const
{
    return pimplPtr->m_pixels;
}

float Cat::getScale() const
{
    return pimplPtr->m_scale;
}

float Cat::getCurrCatPosX() const
{
    return pimplPtr->m_currCatPosX;
}

float Cat::getCurrCatPosY() const
{
    return pimplPtr->m_currCatPosY;
}

float Cat::getMoveDelay() const
{
    return pimplPtr->m_idleThreashold;
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

    pimplPtr->m_currCatPosX += (x - pimplPtr->m_currCatPosX) * pimplPtr->m_speed * GetFrameTime();
    pimplPtr->m_currCatPosY += (y - pimplPtr->m_currCatPosY) * pimplPtr->m_speed * GetFrameTime();


    return {pimplPtr->m_currCatPosX, pimplPtr->m_currCatPosY};
}


void Cat::updateMouseTracking(const MousePos& mouse)
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
}

bool Cat::isMouseIdle() const
{
    return pimplPtr->m_idleTimer >= pimplPtr->m_idleThreashold;
}


void Cat::drawTexture(const Texture2D& currentFrame, Color& catColor)
{

    auto centerPos = getCenterPos(pimplPtr->m_pixels, pimplPtr->m_pixels);

    DrawTexturePro(
        currentFrame,
        {0, 0, pimplPtr->m_pixels, pimplPtr->m_pixels},
        {centerPos.first, centerPos.second, pimplPtr->m_pixels * pimplPtr->m_scale, pimplPtr->m_pixels * pimplPtr->m_scale},
        {0, 0},
        0.0f,
        catColor
    );

}

bool Cat::hasCatReachedMouse(const MousePos& mouse)
{

    int deltaX = std::abs(mouse.x - pimplPtr->m_currCatPosX);
    int deltaY = std::abs(mouse.y - pimplPtr->m_currCatPosY);

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

Direction Cat::checkCatQuadrant(float catX, float catY)
{

    MousePos mouse {getGlobalMousePos()};

    float newXPos = mouse.x - catX;
    float newYPos = catY - mouse.y;

    float angle = std::atan2(newYPos, newXPos) * 180.0f / pimplPtr->m_PI;

    if (angle > -22.5f && angle <= 22.5f) return Direction::EAST;
    else if (angle > 22.5f && angle <= 67.5f) return Direction::NORTH_EAST;
    else if (angle > 67.5f && angle <= 112.5f) return Direction::NORTH;
    else if (angle > 112.5f && angle <= 157.5f) return Direction::NORTH_WEST;
    else if (angle > 157.5f || angle <= -157.5f) return Direction::WEST;
    else if (angle > -157.5f && angle <= -112.5f) return Direction::SOUTH_WEST;
    else if (angle > -112.5f && angle <= -67.5f) return Direction::SOUTH;
    else return Direction::SOUTH_EAST;

}

#define MAX_SPEED 10

float Cat::increaseSpeed()
{
    pimplPtr->m_idleThreashold -= 1;
    if (pimplPtr->m_idleThreashold < 0) pimplPtr->m_idleThreashold = 0;

    return pimplPtr->m_idleThreashold;
}

float Cat::decreaseSpeed()
{
    pimplPtr->m_idleThreashold += 1;
    if (pimplPtr->m_idleThreashold > MAX_SPEED) pimplPtr->m_idleThreashold = MAX_SPEED;

    return pimplPtr->m_idleThreashold;
}



