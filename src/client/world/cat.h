#pragma once

#include "../platform/mouse.h"
#include "../settings/settings.h"
#include "../../common/protocol.h"
#include <cstdint>
#include <utility>
#include <memory>


struct Texture;
struct Color;
struct Settings;
typedef Texture Texture2D;



class Cat
{

public:


    ~Cat();
    Cat();

    float getCurrCatPosX() const;
    float getCurrCatPosY() const;

    float getMoveDelay() const;

    std::pair<float, float> getCenterPos(float frameWidth, float frameHeight);
    std::pair<float, float> getNewPos(int x, int y);

    Color getColor();
    void setColor(std::uint32_t colorVal);
    void setPosition(float x, float y);

    void updateMouseTracking(const MousePos& mouse);
    bool isMouseIdle() const;

    bool hasCatReachedMouse(const MousePos& mouse);

    float decreaseDelay();
    float increaseDelay();

    const CatState& getState() const; 
    void updateBehavior(const MousePos& mouse, Settings& settings);

private:
    struct pimplImplementation;
    std::unique_ptr<pimplImplementation> pimplPtr {};
};

Direction getCatDirection(float catX, float catY, float mouseX, float mouseY);







