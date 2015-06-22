#include <world.hpp>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <thread>

#if CAR_PHYSICS_GRAPHIC_MODE_SFML
#include <renderer.hpp>
#endif

#include <drawable.hpp>
#include <raycastcallback.hpp>
#include <staticbox.hpp>


#if CAR_PHYSICS_GRAPHIC_MODE_SFML
World::World(
    int32 vIter, int32 pIter, Renderer* r, int32_t simulationRate, int32_t frameRate
)
    : m_renderer(r)
    , m_velocityIterations(vIter)
    , m_positionIterations(pIter)
    , m_simulationRate(simulationRate)
    , m_frameRate(frameRate)
{
    b2Vec2 gravity(0.0f, 0.0f);
    m_world = new b2World(gravity);
}
#else
World::World(int32 vIter, int32 pIter, int32_t simulationRate, int32_t frameRate)
    : m_velocityIterations(vIter)
    , m_positionIterations(pIter)
    , m_simulationRate(simulationRate)
    , m_frameRate(frameRate)
{
    b2Vec2 gravity(0.0f, 0.0f);
    m_world = new b2World(gravity);
}
#endif // CAR_PHYSICS_GRAPHIC_MODE_SFML

World::~World()
{
    delete(m_world);
}

void World::addDrawable(Drawable * drawable)
{
    drawable->setBody(m_world->CreateBody(drawable->getBodyDef()), this);
    m_drawableList.push_back(std::shared_ptr<Drawable>(drawable));
}


void World::removeDrawables()
{
    for(auto it = m_drawableList.begin(); it != m_drawableList.end(); ++it)
    {
        if((*it)->isMarkedForDeath())
        {
            m_world->DestroyBody((*it)->getBody());
        }
    }

    auto it = std::remove_if(
        m_drawableList.begin(),
        m_drawableList.end(),
        [](std::shared_ptr<Drawable> d){return d->isMarkedForDeath();}
    );

    while(it != m_drawableList.end())
    {
        m_drawableList.erase(it);
    }
}

b2Joint * World::createJoint(b2RevoluteJointDef* jointDef)
{
    return m_world->CreateJoint(jointDef);
}

void World::rayCast(RaycastCallback * cb, b2Vec2 const & p1, b2Vec2 const & p2) const
{
    m_world->RayCast(cb, p1, p2);
}

void World::addBorders(int32_t width, int32_t height)
{
    float32 w = static_cast<float32>(width);
    float32 h = static_cast<float32>(height);
    float32 w2 = w / 2.0f;
    float32 h2 = h / 2.0f;

    StaticBox* boxL = new StaticBox(b2Vec2(0.0f, h2), 0.0f, 1.0f, h);
    addDrawable(boxL);

    StaticBox* boxU = new StaticBox(b2Vec2(w2, 0.0f), 0.0f, w, 1.0f);
    addDrawable(boxU);

    StaticBox* boxR = new StaticBox(b2Vec2(w, h2), 0.0f, 1.0f, h);
    addDrawable(boxR);

    StaticBox* boxD = new StaticBox(b2Vec2(w2, h), 0.0f, w, 1.0f);
    addDrawable(boxD);
}

void World::randomize(int32_t width, int32_t height, int32_t nbObstacles)
{
    uint32_t seed = static_cast<uint32_t>(std::time(0));

    std::mt19937 rng(seed);

    std::uniform_int_distribution<int32_t> heightDistribution(0, height);
    std::uniform_int_distribution<int32_t> widthDistribution(0, width);
    std::uniform_int_distribution<int32_t> angleDistribution(0, 359);
    std::normal_distribution<float32> sizeDistribution(10.0, 5.0);

    for(int32_t i = 0; i < nbObstacles; ++i)
    {
        StaticBox * box = new StaticBox(
            b2Vec2(widthDistribution(rng), heightDistribution(rng)),
            angleDistribution(rng),
            sizeDistribution(rng),
            sizeDistribution(rng)
        );
        addDrawable(box);
    }
}

#if CAR_PHYSICS_GRAPHIC_MODE_SFML
void World::run()
{
    double timeAccumulator = 0.0;
    double renderTimeAccumulator = 0.0;
    double totalTime = 0.0;
    auto oldTime = std::chrono::high_resolution_clock::now();

    while(!m_stop)
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - oldTime);
        oldTime = currentTime;

        int32_t timeDiff = duration.count();
        timeAccumulator += timeDiff;
        renderTimeAccumulator += timeDiff;
        totalTime += timeDiff;

        // Simulation
        while(timeAccumulator >= m_simulationRate)
        {
            //std::cout << "updating: " << timeAccumulator << std::endl;

            // Update drawables
            for(auto it = m_drawableList.begin(); it != m_drawableList.end(); ++it)
            {
               (*it)->update(this);
            }

            // Remove the one marked for death
            removeDrawables();

            // Simulate one step of physics
            double sr = static_cast<double>(m_simulationRate) / 1000.0;
            m_world->Step(sr, m_velocityIterations, m_positionIterations);

            timeAccumulator -= m_simulationRate;
        }

        // Rendering
        while(renderTimeAccumulator >= m_frameRate)
        {
            //std::cout << "rendering: " << renderTimeAccumulator << std::endl;
            m_stop = !(m_renderer->update(m_drawableList));
            renderTimeAccumulator -= m_frameRate;

            // Consume remaining time: no need to render the same thing
            while(renderTimeAccumulator >= m_frameRate)
            {
                renderTimeAccumulator -= m_frameRate;
            }
        }

        // Sleep to free CPU
        int32_t delay = std::min(
            (m_frameRate - renderTimeAccumulator),
            (m_simulationRate - timeAccumulator)
        );

        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
}
#else
void World::run()
{
    int32_t updateCount = 0;
    double timeAccumulator = 0.0;
    double totalTime = 0.0;
    auto oldTime = std::chrono::high_resolution_clock::now();

    while(!m_stop && updateCount < 100)
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - oldTime);
        oldTime = currentTime;

        int32_t timeDiff = duration.count();
        timeAccumulator += timeDiff;
        totalTime += timeDiff;

        // Simulation
        while(timeAccumulator >= m_simulationRate)
        {
            //std::cout << "updating: " << timeAccumulator << std::endl;

            // Update drawables
            for(auto it = m_drawableList.begin(); it != m_drawableList.end(); ++it)
            {
               (*it)->update(this);
            }

            // Remove the one marked for death
            removeDrawables();

            // Simulate one step of physics
            double sr = static_cast<double>(m_simulationRate) / 1000.0;
            m_world->Step(sr, m_velocityIterations, m_positionIterations);

            timeAccumulator -= m_simulationRate;

            ++updateCount;
        }

        if(updateCount % 10 == 0) std::cout << updateCount << std::endl;

        // Sleep to free CPU
        int32_t delay = m_simulationRate - timeAccumulator;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
}
#endif // CAR_PHYSICS_GRAPHIC_MODE_SFML
