#pragma once

#include <memory>
#include <string>
#include <vector>


struct Texture;
typedef Texture Texture2D;


class Animation
{
public:
    int m_current {};
    float m_speed {0.7};
    float m_duration {0.5};

    Animation();
    ~Animation();

    struct Implementation;
    std::unique_ptr<Implementation> pimpl;

    std::vector<Texture2D> getFrames() const;
    void animationUpdate();
    const Texture2D& getCurrentFrame();
    void createAnimation(int frameCount, std::string fileName);

};

