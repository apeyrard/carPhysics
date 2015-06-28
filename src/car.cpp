#include <car.hpp>
#include <raycastcallback.hpp>

#include <cassert>
#include <iostream>

Car::Car(CarDef const & def, Controller const * controller)
    : Drawable()
    , m_def(def)
    , m_controller(controller)
    , m_power(0.0)
    , m_fljoint(nullptr)
    , m_frjoint(nullptr)
    , m_tireList()
    , m_nbMotorWheels(0u)
    , m_flags(0)
    , m_position(def.initPos)
    , m_steeringAngle(0.0)
    , m_collisionDists()
{
    m_collisionDists.resize(m_def.raycastAngles.size());

    m_bodyDef.type = b2_dynamicBody;
    m_bodyDef.position.Set(m_def.initPos.x, m_def.initPos.y);
    m_bodyDef.angle = m_def.initAngle;

    float32 halfWidth  = m_def.width / 2.0f;
    float32 halfHeight = m_def.height / 2.0f;

    m_shape.SetAsBox(halfWidth, halfHeight);

    m_fixtureDef.shape = &m_shape;
    m_fixtureDef.density = 1.0f;
    m_fixtureDef.friction = 0.3f;

    #if CAR_PHYSICS_GRAPHIC_MODE_SFML
    m_color = sf::Color(0, 0, 255, 128);

    // Creating vertices in CCW order
    m_vertices.reserve(4);
    m_vertices.push_back(std::make_pair(+halfWidth, -halfHeight));
    m_vertices.push_back(std::make_pair(-halfWidth, -halfHeight));
    m_vertices.push_back(std::make_pair(-halfWidth, +halfHeight));
    m_vertices.push_back(std::make_pair(+halfWidth, +halfHeight));
    #endif
}

Car::~Car()
{

}

CarDef const & Car::getDefiniton() const
{
    return m_def;
}

b2Vec2 Car::getPos() const
{
    return m_position;
}

double Car::getAngle() const
{
    return m_body->GetAngle();
}

std::vector<float32> const & Car::getCollisionDists() const
{
    return m_collisionDists;
}

void Car::setController(Controller const * c)
{
    m_controller = c;
}

void Car::update(World const * w)
{
    assert(w && "World is null");

    // Perform raycast to find collision distances
    this->doRaycast(w);

    // Updating flags with controller if it exists
    if(m_controller != nullptr)
    {
        m_flags = m_controller->updateFlags(this);
    }

    // Making the car move and turn
    if(m_flags & Car::FORWARD)
    {
        for(auto it = m_tireList.begin(); it != m_tireList.end(); ++it)
        {
            assert((*it) && "Tire is null");
           (*it)->accelerate(m_power/m_nbMotorWheels);
        }
    }

    if(m_flags & Car::BACKWARD)
    {
        for(auto it = m_tireList.begin(); it != m_tireList.end(); ++it)
        {
            assert((*it) && "Tire is null");
           (*it)->accelerate(-m_power/m_nbMotorWheels);
        }
    }

    if((m_flags & Car::LEFT) && (m_steeringAngle > -m_def.maxSteeringAngle))
    {
        m_steeringAngle -= m_def.steeringRate;
    }

    if((m_flags & Car::RIGHT) && (m_steeringAngle < m_def.maxSteeringAngle))
    {
        m_steeringAngle += m_def.steeringRate;
    }

    if(!(m_flags & Car::RIGHT) && !(m_flags & Car::LEFT))
    {
        if (m_steeringAngle < 0)
        {
            m_steeringAngle += m_def.steeringRate;
        }
        else if (m_steeringAngle > 0)
        {
            m_steeringAngle -= m_def.steeringRate;
        }
    }
    
    assert(m_fljoint && "m_fljoint is null");
    assert(m_frjoint && "m_frjoint is null");

    m_fljoint->SetLimits(m_steeringAngle, m_steeringAngle);
    m_frjoint->SetLimits(m_steeringAngle, m_steeringAngle);

    // SImulate friction on tires
    for(auto it = m_tireList.begin(); it != m_tireList.end(); ++it)
    {
        assert((*it) && "Tire is null");
        (*it)->simulateFriction();
    }

    // Update position
    m_position = m_body->GetPosition();

    // Die if touching obstacle
    if(this->isColliding())
    {
        this->die(w);
    }

    // Change color in funtion of obstacle procimity
    #if CAR_PHYSICS_GRAPHIC_MODE_SFML
    float32 min = 1.0;
    for(auto it = m_collisionDists.begin(); it != m_collisionDists.end(); ++it)
    {
        if((*it) < min)
        {
            min =(*it);
        }
    }

    m_color = sf::Color((1-min)*255, 0, min * 255, 128);
    #endif
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

std::shared_ptr<Car> Car::cloneInitial() const
{
    return std::make_shared<Car>(m_def, nullptr);
}

void Car::setBody(b2Body * body, World * w)
{
    m_tireList.clear();
    m_nbMotorWheels = 0;

    Drawable::setBody(body, w);

    if(!body || !w) return;

    assert(body && "b2Body must not be null");
    assert(w && "world must not be null");

    assert(m_tireList.size() == 0);
    assert(m_nbMotorWheels == 0);


    float32 tireWidth = m_def.width / 4.0f;
    float32 tireHeight = m_def.height / 4.0f;

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
            tireLocalPos.x = 2.0f * x * m_def.width / 3.0f - m_def.width / 3.0f;
            tireLocalPos.y = 2.0f * y * m_def.height / 3.0f - m_def.height / 3.0f;

            float c = std::cos(m_def.initAngle);
            float s = std::sin(m_def.initAngle);
            b2Vec2 tirePos;
            tirePos.x = tireLocalPos.x * c - tireLocalPos.y * s + m_def.initPos.x;
            tirePos.y = tireLocalPos.x * s + tireLocalPos.y * c + m_def.initPos.y;

            std::shared_ptr<Tire> tire = std::make_shared<Tire>(
                tirePos, m_def.initAngle, tireWidth, tireHeight, motor
            );

            // /!\ This line must be before Tire::attachJointAsB because
            // the b2Body of the tire is set in World::addDrawable
            w->addDrawable(tire);

            // Attach tire to car
            tire->attachJointAsB(jointDef);

            jointDef.localAnchorA = tireLocalPos;
            jointDef.localAnchorB = b2Vec2(0.0, 0.0); // Center of the tire

            b2Joint * joint = w->createJoint(&jointDef);

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

    m_power = body->GetMass() * m_def.acceleration;
}

void Car::doRaycast(World const * w) const
{
    assert(w && "World is null");
    assert(m_body && "Car has no body");
    assert(m_collisionDists.size() == m_def.raycastAngles.size());

    for(auto i = 0u; i < m_def.raycastAngles.size(); ++i)
    {
        float32 angle = m_def.raycastAngles[i] + m_body->GetAngle();
        b2Vec2 point1 = m_body->GetWorldCenter();
        b2Vec2 point2 = b2Vec2(std::cos(angle), std::sin(angle));
        point2 *= m_def.raycastDist;
        point2 += point1;

        RaycastCallback callback(m_body);
        w->rayCast(&callback, point1, point2);
        if(callback.fixture != nullptr)
        {
            m_collisionDists[i] = callback.fraction;
        }
        else
        {
            m_collisionDists[i] = 1.0f;
        }
    }
}

void Car::onRemoveFromWorld(b2World * w)
{
    Drawable::onRemoveFromWorld(w);
    m_fljoint = nullptr;
    m_frjoint = nullptr;
}

std::ostream & operator<<(std::ostream & os, Car const & car)
{
    os << "Car {" << std::endl;

    os << "  (w, h):(" << car.m_def.width << ", " << car.m_def.height << ")" << std::endl;
    os << "  init pos: (" << car.m_def.initPos.x << ", " << car.m_def.initPos.y << ")" << std::endl;
    os << "  init angle: " << car.m_def.initAngle << std::endl;
    os << "  acceleration: " << car.m_def.acceleration << std::endl;
    os << "  max steering angle: " << car.m_def.maxSteeringAngle << std::endl;
    os << "  steering rate: " << car.m_def.steeringRate << std::endl;
    os << "  raycast dist: " << car.m_def.raycastDist << std::endl;
    os << "  raycast angles: {" << std::endl;
    for(auto const & a: car.m_def.raycastAngles) os << "    " << a << std::endl;
    os << "  }" << std::endl;

    os << "  controller: " << car.m_power << std::endl;

    os << "  power: " << car.m_power << std::endl;
    os << "  fljoint: " << car.m_fljoint << std::endl;
    os << "  frjoint: " << car.m_frjoint << std::endl;
    os << "  tires: {" << std::endl;
    for(auto const & t: car.m_tireList) os << "    " << t.get() << std::endl;
    os << "  }" << std::endl;
    os << "  #(motor wheels): " << car.m_nbMotorWheels << std::endl;

    os << "  flags: " << car.m_flags << std::endl;
    os << "  pos: (" << car.m_position.x << ", " << car.m_position.y << ")" << std::endl;
    os << "  steering angle: " << car.m_steeringAngle << std::endl;
    os << "  collision dists: {" << std::endl;
    for(auto const & d: car.m_collisionDists) os << "    " << d << std::endl;
    os << "  }" << std::endl;

    return os;
}
