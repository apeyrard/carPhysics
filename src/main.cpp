#include <world.hpp>
#include <renderer.hpp>
#include <staticbox.hpp>
#include <car.hpp>


int main()
{
    int worldWidth = 100;
    int worldHeight = 80;
    Renderer r(8, worldWidth, worldHeight);
    World w(8, 3, &r);
    w.addBorders(worldWidth, worldHeight);

    w.randomize(worldWidth, worldHeight, 15);


    // a car

    std::vector<float32> angles;
    //angles to ray cast
    angles.push_back(0.0f);
    angles.push_back(b2_pi);

    angles.push_back(b2_pi/2.0f);
    angles.push_back(-b2_pi/2.0f);

    angles.push_back(b2_pi/4.0f);
    angles.push_back(-b2_pi/4.0f);

    angles.push_back(b2_pi/8.0f);
    angles.push_back(-b2_pi/8.0f);

    angles.push_back(3.0f*b2_pi/8.0f);
    angles.push_back(-3.0f*b2_pi/8.0f);

    static auto const toRadian = [](float32 degree)
    {
        return degree * M_PI / 180.0;
    };

    float32 carAngle = 90.0;
    Car* car = new Car(b2Vec2(50, 10), toRadian(carAngle), 2, 3, 8.0, angles);

    w.addDrawable(car);

    w.run();

    return 0;
}
