#pragma once

#include <cstdint>

#include <Box2D/Box2D.h>
#include <memory>


class Drawable;
class RaycastCallback;

#if CAR_PHYSICS_GRAPHIC_MODE_SFML
class Renderer;
#endif

class World
{
public:
    #if CAR_PHYSICS_GRAPHIC_MODE_SFML
    explicit World(
        int32 vIter = 8,
        int32 pIter = 3,
        Renderer * r = nullptr,
        uint32_t simulationRate = 10,
        uint32_t frameRate = 16
    );
    #else
    explicit World(int32 vIter = 8, int32 pIter = 3, uint32_t simulationRate = 10);
    #endif

    ~World();

    void addDrawable(std::shared_ptr<Drawable> d);
    void addRequiredDrawable(std::shared_ptr<Drawable> d);

    void run();

    b2Joint * createJoint(b2RevoluteJointDef * jointDef);

    void rayCast(RaycastCallback * cb, b2Vec2 const & p1, b2Vec2 const & p2) const;

    void addBorders(uint32_t width, uint32_t height);

    void randomize(uint32_t width, uint32_t height, uint32_t nbObstacles, uint32_t seed=0);

    bool willCollide(std::shared_ptr<Drawable> d);


protected:
    void removeDrawables();


protected:
    b2World * m_world;

    int32 m_velocityIterations;
    int32 m_positionIterations;
    uint32_t m_simulationRate;

    std::vector<std::shared_ptr<Drawable>> m_drawableList;
    std::vector<std::shared_ptr<Drawable>> m_requiredDrawables;

    #if CAR_PHYSICS_GRAPHIC_MODE_SFML
    Renderer * m_renderer;
    uint32_t m_frameRate;
    #endif
};
