#pragma once

#include <drawable.hpp>

class StaticBox : public Drawable
{
public:
    StaticBox(b2Vec2 const & initPos, float32 initAngle, float32 w, float32 h);
    ~StaticBox();

protected:
    float32 m_width;
    float32 m_height;
};
