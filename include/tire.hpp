#pragma once

#include <drawable.hpp>

class Tire : public Drawable
{
public:
    Tire(b2Vec2 const & initPos, float32 initAngle, float32 w, float32 h, bool motor);

    Tire(Tire const & other);
    Tire(Tire && other) noexcept = default;

    ~Tire();

    void accelerate(float32 power) const;
    bool hasMotor() const;
    void setMotor(bool motor);
    void simulateFriction();

protected:
    b2Vec2 getLateralVelocity() const;
    b2Vec2 getForwardVelocity() const;

protected:
    float32 m_width;
    float32 m_height;
    bool m_motor;
};
