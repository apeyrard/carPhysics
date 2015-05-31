#pragma once

#include "drawable.hpp"

class Tire : public Drawable
{
public:
    bool motor;

    Tire(b2Vec2 initPos, float32 initAngle, float32 height, float32 width, bool motor);
    ~Tire();

    void accelerate(float32 power) const;

protected:
    float32 m_height;
    float32 m_width;
    float32 acceleration = 1.0f;

    b2Vec2 getLateralVelocity() const;
    b2Vec2 getForwardVelocity() const;
    void simulateFriction();
};
