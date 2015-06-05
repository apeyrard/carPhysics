#include <world.hpp>

#include <chrono>
#include <thread>
#include <random>
#include <algorithm>

#include <drawable.hpp>

#if NEURO_CAR_GRAPHIC_MODE_SFML
#include <renderer.hpp>
#endif

#include <raycastcallback.hpp>
#include <staticbox.hpp>

#include <iostream>

#if NEURO_CAR_GRAPHIC_MODE_SFML
World::World(int32 vIter, int32 pIter, Renderer* r, int simulationRate, int frameRate)
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
World::World(int32 vIter, int32 pIter, int simulationRate, int frameRate)
    : m_velocityIterations(vIter)
    , m_positionIterations(pIter)
    , m_simulationRate(simulationRate)
    , m_frameRate(frameRate)
{
    b2Vec2 gravity(0.0f, 0.0f);
    m_world = new b2World(gravity);
}
#endif // NEURO_CAR_GRAPHIC_MODE_SFML

World::~World()
{
    delete(m_world);
    for (auto it = m_drawableList.begin(); it != m_drawableList.end(); ++it)
    {
        delete(*it);
    }
}

void World::addDrawable(Drawable *drawable)
{
    drawable->setBody(m_world->CreateBody(drawable->getBodyDef()), this);
    m_drawableList.push_back(drawable);
}


void World::removeDrawables()
{
    for (auto it = m_drawableList.begin(); it != m_drawableList.end(); ++it)
    {
        if ((*it)->isMarkedForDeath())
        {
            m_world->DestroyBody((*it)->getBody());
        }
    }

    auto it = std::remove_if(m_drawableList.begin(), m_drawableList.end(), [](Drawable* d){return d->isMarkedForDeath();});

    while (it != m_drawableList.end())
    {
        delete((*it));
        m_drawableList.erase(it);
    }
}

b2Joint* World::CreateJoint(b2RevoluteJointDef* jointDef)
{
    return m_world->CreateJoint(jointDef);
}

void World::RayCast(RaycastCallback* callback, b2Vec2 p1, b2Vec2 p2)
{
    m_world->RayCast(callback, p1, p2);
}

void World::addBorders(int width, int height)
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

void World::randomize(int width, int height, int nbObstacles)
{
    unsigned int seed = static_cast<unsigned int>(std::time(0));

    std::mt19937 rng(seed);

    std::uniform_int_distribution<int> heightDistribution(0, height);
    std::uniform_int_distribution<int> widthDistribution(0, width);
    std::uniform_int_distribution<int> angleDistribution(0, 359);
    std::normal_distribution<float32> sizeDistribution(10.0, 5.0);

    for (int i = 1; i <= nbObstacles; ++i)
    {
        StaticBox* box = new StaticBox(b2Vec2(widthDistribution(rng), heightDistribution(rng)), angleDistribution(rng), sizeDistribution(rng), sizeDistribution(rng));
        addDrawable(box);
    }
}

#if NEURO_CAR_GRAPHIC_MODE_SFML
void World::run()
{
    double timeAccumulator = 0.0;
    double renderTimeAccumulator = 0.0;
    double totalTime = 0.0;
    auto oldTime = std::chrono::high_resolution_clock::now();
    //int i = 0;
    while (!m_stop)
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast< std::chrono::milliseconds> (currentTime - oldTime);
        int timeDiff = duration.count();
        //std::cout << "Time diff = " << timeDiff << std::endl;
        oldTime = currentTime;

        timeAccumulator += timeDiff;
        renderTimeAccumulator += timeDiff;
        totalTime += timeDiff;

        while (timeAccumulator >= m_simulationRate)
        {
            //std::cout << "updating: " << timeAccumulator << std::endl;
            // Game loop
            for (auto it = m_drawableList.begin(); it != m_drawableList.end(); ++it)
            {
                (*it)->update(this);
            }

            removeDrawables();

            m_world->Step((double)m_simulationRate / 1000, m_velocityIterations, m_positionIterations);
            timeAccumulator -= m_simulationRate;
        }

        // Rendering loop
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

        //std::cout << m_frameRate << std::endl;
        //std::cout << renderTimeAccumulator << std::endl;
        //std::cout << m_simulationRate << std::endl;
        //std::cout << timeAccumulator << std::endl;
        int delay = std::min((m_frameRate - renderTimeAccumulator), (m_simulationRate - timeAccumulator));
        std::this_thread::sleep_for(std::chrono::milliseconds( delay));
    }
}
#else
void World::run()
{
    int updateCount = 0;
    double timeAccumulator = 0.0;
    double totalTime = 0.0;
    auto oldTime = std::chrono::high_resolution_clock::now();
    //int i = 0;
    while (!m_stop && updateCount < 500)
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast< std::chrono::milliseconds> (currentTime - oldTime);
        int timeDiff = duration.count();
        //std::cout << "Time diff = " << timeDiff << std::endl;
        oldTime = currentTime;

        timeAccumulator += timeDiff;
        totalTime += timeDiff;

        while (timeAccumulator >= m_simulationRate)
        {
            //std::cout << "updating: " << timeAccumulator << std::endl;
            // Game loop
            for (auto it = m_drawableList.begin(); it != m_drawableList.end(); ++it)
            {
                (*it)->update(this);
            }

            updateCount++;

            removeDrawables();

            m_world->Step((double)m_simulationRate / 1000, m_velocityIterations, m_positionIterations);
            timeAccumulator -= m_simulationRate;
        }

        //std::cout << m_frameRate << std::endl;
        //std::cout << m_simulationRate << std::endl;
        //std::cout << timeAccumulator << std::endl;
        if(updateCount % 10 == 0) std::cout << updateCount << std::endl;
        int delay = m_simulationRate - timeAccumulator;
        std::this_thread::sleep_for(std::chrono::milliseconds( delay));
    }
}
#endif // NEURO_CAR_GRAPHIC_MODE_SFML
