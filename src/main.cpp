#include <car.hpp>
#include <renderer.hpp>
#include <staticbox.hpp>
#include <world.hpp>

#include <memory>

#include <iostream>

int main()
{
    uint32_t worldWidth = 100;
    uint32_t worldHeight = 80;

    #if CAR_PHYSICS_GRAPHIC_MODE_SFML
    Renderer r(8, worldWidth, worldHeight);
    World w(8, 3, &r);
    #else
    World w(8, 3);
    #endif

    w.addBorders(worldWidth, worldHeight);

    w.randomize(worldWidth, worldHeight, 15);


    // A car

    static auto const toRadian = [](float32 degree)
    {
        return degree * M_PI / 180.0;
    };

    float32 carAngle = 90.0;

    CarDef carDef;
    carDef.initPos = b2Vec2(50, 10);
    carDef.initAngle = toRadian(carAngle);
    carDef.width = 2.0;
    carDef.height = 3.0;
    carDef.acceleration = 8.0;

    //angles to ray cast
    carDef.raycastAngles.push_back(0.0f);
    carDef.raycastAngles.push_back(b2_pi);

    carDef.raycastAngles.push_back(b2_pi/2.0f);
    carDef.raycastAngles.push_back(-b2_pi/2.0f);

    carDef.raycastAngles.push_back(b2_pi/4.0f);
    carDef.raycastAngles.push_back(-b2_pi/4.0f);

    carDef.raycastAngles.push_back(b2_pi/8.0f);
    carDef.raycastAngles.push_back(-b2_pi/8.0f);

    carDef.raycastAngles.push_back(3.0f*b2_pi/8.0f);
    carDef.raycastAngles.push_back(-3.0f*b2_pi/8.0f);


    std::shared_ptr<Car> car = std::make_shared<Car>(carDef);

    w.addRequiredDrawable(car);

    w.run();

    return 0;
}
