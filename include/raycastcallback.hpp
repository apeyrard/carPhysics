#pragma once

#include <Box2D/Box2D.h>

class RaycastCallback : public b2RayCastCallback
{
public:
    /**
     * @brief ReportFixture
     * @param fixture -- Fixture hit
     * @param point -- point of hit
     * @param normal -- normal of hit
     * @param fraction -- fraction of ray length where hit
     * @return fraction.
     */
    float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point,const b2Vec2& normal, float32 fraction);
    explicit RaycastCallback(const b2Body* owner);
    ~RaycastCallback();
    b2Fixture* fixture;
    b2Vec2 point;
    b2Vec2 normal;
    float32 fraction;
    const b2Body* owner;
private:
    b2Fixture* oldFixture;
    float32 oldFraction;

};
