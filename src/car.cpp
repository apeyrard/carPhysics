#include <car.hpp>
#include <raycastcallback.hpp>

#include <cassert>
#include <memory>

Car::Car(b2Vec2 const & initPos, float32 initAngle, float32 w, float32 h,
    float32 acceleration, std::vector<float32> raycastAngles, Controller* controller
)
    : Drawable()
    , m_width(w)
    , m_height(h)
    , m_flags(0)
    , m_raycastDist(50.0f)
    , m_initPos(initPos)
    , m_initAngle(initAngle)
    , m_tireList()
    , m_fljoint(nullptr)
    , m_frjoint(nullptr)
    , m_steeringAngle(0.0f)
    , m_maxSteeringAngle((3.0f/8.0f)*b2_pi)
    , m_steeringRate(m_maxSteeringAngle/30.0)
    , m_acceleration(acceleration)
    , m_power(0.0f)
    , m_dists()
    , m_angles(std::move(raycastAngles))
    , m_controller(controller)
    , m_nbMotorWheels(0)
{
    #if CAR_PHYSICS_GRAPHIC_MODE_SFML
    m_color = sf::Color(0, 0, 255, 128);
    #endif

    m_bodyDef.type = b2_dynamicBody;
    m_bodyDef.position.Set(initPos.x, initPos.y);
    m_bodyDef.angle = initAngle;

    float32 halfWidth  = m_width / 2.0f;
    float32 halfHeight = m_height / 2.0f;

    m_shape.SetAsBox(halfWidth, halfHeight);

    m_fixtureDef.shape = &m_shape;
    m_fixtureDef.density = 1.0f;
    m_fixtureDef.friction = 0.3f;

    // Creating vertices in CCW order
    m_vertices.reserve(4);
    m_vertices.push_back(std::make_pair(+halfWidth, -halfHeight));
    m_vertices.push_back(std::make_pair(-halfWidth, -halfHeight));
    m_vertices.push_back(std::make_pair(-halfWidth, +halfHeight));
    m_vertices.push_back(std::make_pair(+halfWidth, +halfHeight));
}

Car::Car(Car const & other):
    Drawable(other),
    m_width(other.m_width),
    m_height(other.m_height),
    m_flags(other.m_flags),
    m_raycastDist(other.m_raycastDist),
    m_initPos(other.m_initPos),
    m_initAngle(other.m_initAngle),
    m_tireList(),       // Do not need copy: initialized in Car::setBody
    m_fljoint(nullptr), // Do not need copy: initialized in Car::setBody
    m_frjoint(nullptr), // Do not need copy: initialized in Car::setBody
    m_steeringAngle(other.m_steeringAngle),
    m_maxSteeringAngle(other.m_maxSteeringAngle),
    m_steeringRate(other.m_steeringRate),
    m_acceleration(other.m_acceleration),
    m_power(other.m_power),
    m_dists(other.m_dists),
    m_angles(other.m_angles),
    m_controller(nullptr), // Do not need copy: initialized in Car::setController
    m_nbMotorWheels(other.m_nbMotorWheels)
{

}


Car::~Car()
{

}

void Car::setBody(b2Body * body, World * w)
{
    m_tireList.clear();


    assert(m_tireList.size() == 0);
    assert(body && "b2Body must not be null");
    assert(w && "world must not be null");

    Drawable::setBody(body, w);

    float32 tireWidth = m_width / 4.0f;
    float32 tireHeight = m_height / 4.0f;

    // Creating tires

    b2RevoluteJointDef jointDef;

    for(auto x = 0u; x < 2; ++x)
    {
        for(auto y = 0u; y < 2; ++y)
        {
            jointDef.bodyA = m_body;
            jointDef.enableLimit = true;
            jointDef.lowerAngle = 0;
            jointDef.upperAngle = 0;

            bool motor = false;
            if(y == 1)
            {
                motor = true;
                ++m_nbMotorWheels;
            }

            b2Vec2 tireLocalPos;
            tireLocalPos.x = 2.0f * x * m_width / 3.0f - m_width / 3.0f;
            tireLocalPos.y = 2.0f * y * m_height / 3.0f - m_height / 3.0f;

            float c = std::cos(m_initAngle);
            float s = std::sin(m_initAngle);
            b2Vec2 tirePos;
            tirePos.x = tireLocalPos.x * c - tireLocalPos.y * s + m_initPos.x;
            tirePos.y = tireLocalPos.x * s + tireLocalPos.y * c + m_initPos.y;

            std::shared_ptr<Tire> tire = std::make_shared<Tire>(
                tirePos, m_initAngle, tireWidth, tireHeight, motor
            );

            w->addDrawable(tire);

            tire->attachJointAsB(jointDef);

            jointDef.localAnchorA = tireLocalPos;
            jointDef.localAnchorB = b2Vec2(0.0, 0.0); // Center of the tire

            b2Joint* joint = w->createJoint(&jointDef);

            assert(joint && "Failed to create joint");

            if(y == 1)
            {
                if(x == 0)
                {
                    m_fljoint = static_cast<b2RevoluteJoint*>(joint);
                }
                else
                {
                    m_frjoint = static_cast<b2RevoluteJoint*>(joint);
                }
            }

            m_tireList.push_back(tire);
        }
    }

    assert(m_tireList.size() == 4);
    assert(m_fljoint && "m_fljoint is null");
    assert(m_frjoint && "m_frjoint is null");

    m_power = body->GetMass() * m_acceleration;
}

void Car::setController(Controller* c)
{
    m_controller = c;
}


void Car::update(World const * w)
{
    assert(w && "World is null");

    m_dists = doRaycast(w);

    // Updating flags with controller if it exists
    if (m_controller != NULL)
    {
        m_flags = m_controller->updateFlags(this);
    }


    // Change color in funtion of obstacle procimity
    #if CAR_PHYSICS_GRAPHIC_MODE_SFML

    float32 min = 1.0;
    for(auto it = m_dists.begin(); it != m_dists.end(); ++it)
    {
        if((*it) < min)
        {
            min =(*it);
        }
    }

    m_color = sf::Color((1-min)*255, 0, min * 255, 128);
    #endif


    // Making the car move and turn
    if (m_flags & Car::FORWARD)
    {
        for(auto it = m_tireList.begin(); it != m_tireList.end(); ++it)
        {
            assert((*it) && "Tire is null");
            (*it)->accelerate(m_power/m_nbMotorWheels);
        }
    }

    if (m_flags & Car::BACKWARD)
    {
        for(auto it = m_tireList.begin(); it != m_tireList.end(); ++it)
        {
            assert((*it) && "Tire is null");
            (*it)->accelerate(-m_power/m_nbMotorWheels);
        }
    }

    if ((m_flags & Car::LEFT) && (m_steeringAngle > -m_maxSteeringAngle))
    {
        m_steeringAngle -= m_steeringRate;
    }

    if ((m_flags & Car::RIGHT) && (m_steeringAngle < m_maxSteeringAngle))
    {
        m_steeringAngle += m_steeringRate;
    }

    assert(m_fljoint && "m_fljoint is null");
    assert(m_frjoint && "m_frjoint is null");

    m_fljoint->SetLimits(m_steeringAngle, m_steeringAngle);
    m_frjoint->SetLimits(m_steeringAngle, m_steeringAngle);

    // SImulate friction on tires
    for (auto it=m_tireList.begin(); it!=m_tireList.end();++it)
    {
        assert((*it) && "Tire is null");
        (*it)->simulateFriction();
    }

    // Die if touching obstacle
    if(isColliding())
    {
        die(w);
    }
}

void Car::die(World const * w)
{
    Drawable::die(w);
    for(auto it = m_tireList.begin(); it != m_tireList.end(); ++it)
    {
        assert((*it) && "Tire is null");
       (*it)->die(w);
    }
}

std::vector<float32> Car::doRaycast(World const * w) const
{
    assert(w && "World is null");
    assert(m_body && "Car has no body");

    std::vector<float32> result;
    for(auto it = m_angles.begin(); it != m_angles.end(); ++it)
    {
        float32 angle = (*it) + m_body->GetAngle();
        b2Vec2 point1 = m_body->GetWorldCenter();
        b2Vec2 point2 = b2Vec2(std::cos(angle), std::sin(angle));
        point2 *= m_raycastDist;
        point2 += point1;

        RaycastCallback callback(m_body);
        w->rayCast(&callback, point1, point2);
        if(callback.fixture != nullptr)
        {
            result.push_back(callback.fraction);
        }
        else
        {
            result.push_back(1.0f);
        }
    }
    return result;
}

b2Vec2 Car::getPos() const
{
    assert(m_body && "Car has no body");
    return m_body->GetPosition();
}

std::vector<float32> const & Car::getDist() const
{
    return m_dists;
}
