#include <world.hpp>

#include <chrono>
#include <thread>
#include <random>
#include <algorithm>

#include <drawable.hpp>
#include <renderer.hpp>

#include <raycastcallback.hpp>
#include <staticbox.hpp>


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
        if ((*it)->m_markedForDeath)
        {
            m_world->DestroyBody((*it)->getBody());
        }
    }

    auto it = std::remove_if(m_drawableList.begin(), m_drawableList.end(), [](Drawable* d){return d->m_markedForDeath;});

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
    //Creating a box

    StaticBox* boxL = new StaticBox(b2Vec2(0, height/2), 0, 1, height);
    addDrawable(boxL);

    StaticBox* boxU = new StaticBox(b2Vec2(width/2, 0), 0, width, 1);
    addDrawable(boxU);

    StaticBox* boxR = new StaticBox(b2Vec2(width, height/2), 0, 1, height);
    addDrawable(boxR);

    StaticBox* boxD = new StaticBox(b2Vec2(width/2, height), 0, width, 1);
    addDrawable(boxD);
}

void World::randomize(int width, int height, int nbObstacles)
{
    std::chrono::high_resolution_clock::time_point beginning = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::duration d = std::chrono::high_resolution_clock::now() - beginning;
    unsigned seed = d.count();

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
        oldTime = currentTime;

        timeAccumulator += timeDiff;
        renderTimeAccumulator += timeDiff;
        totalTime += timeDiff;

        while (timeAccumulator >= m_simulationRate)
        {
            //std::cout << "updating" << std::endl;
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
        while (renderTimeAccumulator >= m_frameRate)
        {
            //std::cout << "rendering" << std::endl;
            m_stop = !(m_renderer->update(m_drawableList));
            renderTimeAccumulator -= m_frameRate;
        }

        //std::cout << m_frameRate << std::endl;
        //std::cout << renderTimeAccumulator << std::endl;
        //std::cout << m_simulationRate << std::endl;
        //std::cout << timeAccumulator << std::endl;
        int delay = std::min((m_frameRate - renderTimeAccumulator), (m_simulationRate - timeAccumulator));
        std::this_thread::sleep_for(std::chrono::milliseconds( delay));
    }
}
