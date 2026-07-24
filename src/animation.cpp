#include "animation.h"
#include <cassert>
#include <memory>
#include <raylib.h>

// Implementation Struct
struct Animation::Implementation
{
    std::vector<Texture2D> frames;
};

Animation::Animation() : pimpl { std::make_unique<Implementation>() }
{}


Animation::~Animation() = default;

// Animation methods

std::vector<Texture2D> Animation::getFrames() const
{
    return pimpl->frames;
}

void Animation::animationUpdate()
{
    float deltaTime = GetFrameTime();
    m_duration -= deltaTime;

    if(m_duration <= 0.0f)
    {
        m_duration = m_speed;
        m_current++;

        if(m_current >= pimpl->frames.size())
        {
            m_current = 0;
        }
    }
}

const Texture2D& Animation::getCurrentFrame()
{

    assert(!pimpl->frames.empty());

    return pimpl->frames[m_current];

}


void Animation::createAnimation(int frameCount, std::string fileName)
{
    for (int i {1}; i <= frameCount; i++)
    {
        std::string path = "../assets/white_cat/" + fileName + std::to_string(i) + ".png";
        pimpl->frames.push_back(LoadTexture(path.c_str()));
    }
}

