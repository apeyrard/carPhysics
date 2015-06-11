#pragma once

#include <Box2D/Box2D.h>

class Drawable;
class Renderer;
class RaycastCallback;

class World
{
public:
    explicit World(int32 vIter=8, int32 pIter=3, Renderer* r=nullptr, int simulationRate=10, int frameRate=16);
    ~World();
    void addDrawable(Drawable* d);
    void run();
    b2Joint* CreateJoint(b2RevoluteJointDef* jointDef);
    void RayCast(RaycastCallback* callback, b2Vec2 p1, b2Vec2 p2);
    void addBorders(int width, int height);
    void randomize(int width, int height, int nbObstacles);
protected:
    Renderer* m_renderer;
    bool m_stop = false;

    int32 m_velocityIterations;
    int32 m_positionIterations;
    int m_simulationRate;
    int m_frameRate;

    double m_simulationMultiplier;

    b2World* m_world;
    std::vector<Drawable*> m_drawableList;

    void removeDrawables();
};
