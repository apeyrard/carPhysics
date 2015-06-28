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
        int32_t simulationRate = 10,
        int32_t frameRate = 16
    );
    #else
    explicit World(int32 vIter = 8, int32 pIter = 3, int32_t simulationRate = 10);
    #endif

    ~World();

    void addDrawable(std::shared_ptr<Drawable> d);
    void addRequiredDrawable(std::shared_ptr<Drawable> d);

    void run();

    b2Joint * createJoint(b2RevoluteJointDef * jointDef);

    void rayCast(RaycastCallback * cb, b2Vec2 const & p1, b2Vec2 const & p2) const;

    void addBorders(int32_t width, int32_t height);

    void randomize(int32_t width, int32_t height, int32_t nbObstacles, uint32_t seed=0);

    bool willCollide(std::shared_ptr<Drawable> d);


protected:
    void removeDrawables();


protected:
    b2World * m_world;

    int32 m_velocityIterations;
    int32 m_positionIterations;
    int32_t m_simulationRate;

    std::vector<std::shared_ptr<Drawable>> m_drawableList;
    std::vector<std::shared_ptr<Drawable>> m_requiredDrawables;

    #if CAR_PHYSICS_GRAPHIC_MODE_SFML
    Renderer * m_renderer;
    int32_t m_frameRate;
    #endif
};
