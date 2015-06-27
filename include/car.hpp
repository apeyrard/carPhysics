#pragma once

#include <cstdint>
#include <vector>

#include <drawable.hpp>
#include <tire.hpp>
#include <world.hpp>
#include <controller.hpp>

class Car : public Drawable
{
public:
    enum Flags
    {
        LEFT     = 1 << 0,
        RIGHT    = 1 << 1,
        FORWARD  = 1 << 2,
        BACKWARD = 1 << 3,
    };

    Car(
        b2Vec2 const & initPos,
        float32 initAngle,
        float32 w, float32 h,
        float32 acceleration,
        std::vector<float32> raycastAngles,
        Controller* controller = nullptr
    );

    Car(Car const & other);
    Car(Car && other) noexcept = default;

    ~Car();

    virtual void update(World const * w) override;
    virtual void setBody(b2Body * body, World * w) override;
    virtual void die(World const * w) override;

    void setController(Controller* c);

    b2Vec2 getPos() const;
    double getAngle() const;
    std::vector<float32> const & getDist() const;

protected:
    std::vector<float32> doRaycast(World const * w) const;

protected:
    float32 m_width;
    float32 m_height;

    int32_t m_flags;
    float32 m_raycastDist;

    b2Vec2 m_initPos;
    float32 m_initAngle;

    std::vector<std::shared_ptr<Tire> > m_tireList;

    b2RevoluteJoint* m_fljoint;
    b2RevoluteJoint* m_frjoint;

    float32 m_steeringAngle;
    float32 m_maxSteeringAngle;
    float32 m_steeringRate;

    float32 m_acceleration;
    float32 m_power;
    std::vector<float32> m_dists;
    std::vector<float32> m_angles;

    Controller* m_controller;

    int32_t m_nbMotorWheels;
};
