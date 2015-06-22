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
    explicit World(
        int32 vIter = 8,
        int32 pIter = 3,
        int32_t simulationRate = 10,
        int32_t frameRate = 16
    );
    #endif

    ~World();

    void addDrawable(Drawable * d);
    void addRequiredDrawable(Drawable * d);

    void run();

    b2Joint * createJoint(b2RevoluteJointDef * jointDef);

    void rayCast(RaycastCallback * cb, b2Vec2 const & p1, b2Vec2 const & p2) const;

    void addBorders(int32_t width, int32_t height);

    void randomize(int32_t width, int32_t height, int32_t nbObstacles);


protected:
    void removeDrawables();


protected:
    #if CAR_PHYSICS_GRAPHIC_MODE_SFML
    Renderer * m_renderer;
    #endif

    bool m_stop = false;

    int32 m_velocityIterations;
    int32 m_positionIterations;
    int32_t m_simulationRate;
    int32_t m_frameRate;

    b2World * m_world;

    std::vector<std::shared_ptr<Drawable>> m_drawableList;
    std::vector<Drawable *> m_requiredDrawables;
};
