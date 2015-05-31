#pragma once

#include "drawable.hpp"

class StaticBox : public Drawable
{
public:
    StaticBox(b2Vec2 initPos, float32 initAngle, float32 height, float32 width);
    ~StaticBox();

protected:
    float32 m_height;
    float32 m_width;
};
