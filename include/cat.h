#pragma once

#include "mouse.h"
#include <utility>
#include <memory>

struct Texture;
typedef Texture Texture2D;

struct Color;

enum class Direction  // All ways cat can go
{
    NORTH,
    NORTH_EAST,
    EAST,
    SOUTH_EAST,
    SOUTH,
    SOUTH_WEST,
    WEST,
    NORTH_WEST,
};


class Cat
{

public:


    ~Cat();
    Cat();

    float getDimensions() const;
    float getScale() const;

    float getCurrCatPosX() const;
    float getCurrCatPosY() const;

    float getMoveDelay() const;

    std::pair<float, float> getCenterPos(float frameWidth, float frameHeight);
    std::pair<float, float> getNewPos(int x, int y);

    void updateMouseTracking(const MousePos& mouse);
    bool isMouseIdle() const;
    void drawTexture(const Texture& currentFrame, Color& catColor);
    bool hasCatReachedMouse(const MousePos& mouse);
    Direction checkCatQuadrant(float catX, float catY);

    float increaseSpeed();
    float decreaseSpeed();

private:
    struct pimplImplementation;
    std::unique_ptr<pimplImplementation> pimplPtr {};


};

