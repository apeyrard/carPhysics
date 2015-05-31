#pragma once

#include <vector>

#include <drawable.hpp>
#include <tire.hpp>
#include <world.hpp>

class Car : public Drawable
{
public:
    enum Flags
    {
        LEFT = 1 << 0,
        RIGHT = 1 << 1,
        FORWARD = 1 << 2,
        BACKWARD = 1 << 3,
    };

    Car(b2Vec2 initPos, float32 initAngle, float32 height, float32 width, float32 acceleration, std::vector<float32> raycastAngles);
    ~Car();

    virtual void update(World* const w) override;
    virtual void setBody(b2Body* body, World* w) override;
    virtual void die(World* const w) override;

    std::vector<float32> getDistances();
protected:
    int m_flags;
    float32 m_raycastDist = 50.0f;

    b2Vec2 m_initPos;
    float32 m_initAngle;

    std::vector<Tire*> m_tireList;

    b2RevoluteJoint* m_fljoint;
    b2RevoluteJoint* m_frjoint;

    float32 m_height;
    float32 m_width;

    float32 m_steeringAngle = 0.0f;
    float32 m_maxSteeringAngle = (3.0f/8.0f)*b2_pi;
    float32 m_steeringRate;

    float32 m_acceleration;
    float32 m_power;
    std::vector<float32> m_dists;
    std::vector<float32> m_angles;

    int m_nbMotorWheels = 0;

    std::vector<float32> doRaycast(World* const w) const;
};
