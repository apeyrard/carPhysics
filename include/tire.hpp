#pragma once

#include <drawable.hpp>

class Tire : public Drawable
{
public:
    Tire(b2Vec2 const & initPos, float32 initAngle, float32 w, float32 h, bool motor);

    Tire(Tire const & other) = delete;
    Tire(Tire && other) noexcept = default;

    Tire & operator=(Tire const & other) = delete;
    Tire & operator=(Tire && other) = default;

    ~Tire();

    void accelerate(float32 power) const;
    void attachJointAsB(b2JointDef & joint);
    bool hasMotor() const;
    void setMotor(bool motor);
    void simulateFriction();

protected:
    b2Vec2 getForwardVelocity() const;
    b2Vec2 getLateralVelocity() const;

protected:
    float32 m_width;
    float32 m_height;
    bool m_motor;
};
