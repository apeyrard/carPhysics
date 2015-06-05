#pragma once

#include <Box2D/Box2D.h>

class Drawable;
class RaycastCallback;

#if NEURO_CAR_GRAPHIC_MODE_SFML
class Renderer;
#endif

class World
{
public:
    #if NEURO_CAR_GRAPHIC_MODE_SFML
    explicit World(int32 vIter=8, int32 pIter=3, Renderer* r=nullptr, int simulationRate=10, int frameRate=16);
    #else
    explicit World(int32 vIter=8, int32 pIter=3, int simulationRate=10, int frameRate=16);
    #endif

    ~World();
    void addDrawable(Drawable* d);
    void run();
    b2Joint* CreateJoint(b2RevoluteJointDef* jointDef);
    void RayCast(RaycastCallback* callback, b2Vec2 p1, b2Vec2 p2);
    void addBorders(int width, int height);
    void randomize(int width, int height, int nbObstacles);
protected:

    #if NEURO_CAR_GRAPHIC_MODE_SFML
    Renderer* m_renderer;
    #endif

    bool m_stop = false;

    int32 m_velocityIterations;
    int32 m_positionIterations;
    int m_simulationRate;
    int m_frameRate;

    b2World* m_world;
    std::vector<Drawable*> m_drawableList;

    void removeDrawables();
};
