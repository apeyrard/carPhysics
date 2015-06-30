#pragma once

#include <cstdint>
#include <iosfwd>
#include <vector>

#include <controller.hpp>
#include <drawable.hpp>
#include <tire.hpp>
#include <world.hpp>


struct CarDef
{
    float32 width;
    float32 height;
    b2Vec2  initPos;
    float32 initAngle;
    float32 acceleration;
    float32 maxSteeringAngle;
    float32 steeringRate;
    float32 raycastDist;
    std::vector<float32> raycastAngles;

    CarDef()
        : width(0.0)
        , height(0.0)
        , initPos(0.0, 0.0)
        , initAngle(0.0)
        , acceleration(0.0)
        , maxSteeringAngle((3.0f/8.0f)*b2_pi)
        , steeringRate(maxSteeringAngle/30.0)
        , raycastDist(50.0)
        , raycastAngles()
    {

    }
};

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

    Car(CarDef const & def, Controller const * controller = nullptr);

    Car(Car const & other) = delete;
    Car & operator=(Car const & other) = delete;

    ~Car();

    CarDef const & getDefiniton() const;

    b2Vec2 getPos() const;
    b2Vec2 getInitPos() const;

    double getAngle() const;
    std::vector<float32> const & getCollisionDists() const;

    void setController(Controller const * c);

    virtual void update(World const * w) override;
    virtual void die(World const * w) override;

    // Clone the car with its initial parameters
    std::shared_ptr<Car> cloneInitial() const;


    friend std::ostream & operator<<(std::ostream & os, Car const & car);


protected:
    virtual void setBody(b2Body * body, World * w) override;

    void doRaycast(World const * w) const;

private:
    virtual void onRemoveFromWorld(b2World * w) override;


protected:
    /// Car definition ///
    CarDef const m_def;

    /// Car controller ///
    Controller const * m_controller;

    /// "After Car::setBody" parameters ///
    float32 m_power;
    b2RevoluteJoint * m_fljoint;
    b2RevoluteJoint * m_frjoint;
    std::vector<std::shared_ptr<Tire>> m_tireList;
    uint32_t m_nbMotorWheels;

    /// Dynamic parameters ///
    int32_t m_flags;
    b2Vec2 m_position;
    float32 m_steeringAngle;
    mutable std::vector<float32> m_collisionDists;
};
