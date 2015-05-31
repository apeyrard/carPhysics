#include <car.hpp>
#include <raycastcallback.hpp>

Car::Car(b2Vec2 initPos, float32 initAngle, float32 height, float32 width, float32 acceleration, std::vector<float32> raycastAngles)
    : Drawable()
    , m_initPos(initPos)
    , m_initAngle(initAngle)
    , m_height(height)
    , m_width(width)
    , m_acceleration(acceleration)
    , m_angles(raycastAngles)
{
    m_color = sf::Color(0, 0, 255, 128);

    m_bodyDef.type = b2_dynamicBody;
    m_bodyDef.position.Set(initPos.x, initPos.y);
    m_bodyDef.angle = initAngle;

    m_shape.SetAsBox(m_height/2, m_width/2);

    m_fixtureDef.shape = &m_shape;
    m_fixtureDef.density = 1.0f;
    m_fixtureDef.friction = 0.3f;

    // Creating vertices in CCW order
    vertices.push_back(std::make_pair(m_height/2, -m_width/2));
    vertices.push_back(std::make_pair(-m_height/2, -m_width/2));
    vertices.push_back(std::make_pair(-m_height/2, m_width/2));
    vertices.push_back(std::make_pair(m_height/2, m_width/2));

}

Car::~Car()
{
}

void Car::setBody(b2Body* body, World* w)
{
    Drawable::setBody(body, w);
    // Creating tires
    for (int y = 0; y <= 1; y++)
    {
        for (int x = 0; x <= 1; x++)
        {
            b2RevoluteJointDef jointDef;
            jointDef.bodyA = m_body;
            jointDef.enableLimit = true;
            jointDef.lowerAngle = 0;
            jointDef.upperAngle = 0;
            jointDef.localAnchorB.SetZero();

            bool motor = false;
            if (x==1)
            {
                motor = true;
                m_nbMotorWheels++;
            }

            b2Vec2 tirePos((m_initPos.x-m_height/3.0f+2.0f*x*m_height/3.0f),(m_initPos.y-m_width/3.0f+2.0f*y*m_width/3.0f));

            Tire* tire = new Tire(tirePos, m_initAngle, m_height/4.0f, m_width/4.0f, motor);
            w->addDrawable(tire);
            tire->attachJointAsB(jointDef);
            jointDef.localAnchorA.Set(2.0f*x*m_height/3.0f-m_height/3.0f, 2.0f*y*m_width/3.0f-m_width/3.0f);
            b2Joint* joint = w->CreateJoint(&jointDef);
            m_tireList.push_back(tire);

            if (x==1)
            {
                if(y==0)
                {
                    m_fljoint = (b2RevoluteJoint*)joint;
                }
                else
                {
                    m_frjoint = (b2RevoluteJoint*)joint;
                }
            }
        }
    }
    m_power = body->GetMass()*m_acceleration;
}

void Car::update(World* const w)
{
    m_dists = doRaycast(w);
    for (auto it = m_tireList.begin(); it != m_tireList.end(); ++it)
    {
        (*it)->accelerate(m_power/m_nbMotorWheels);
    }
    float32 min = 1.0;
    for (auto it = m_dists.begin(); it != m_dists.end(); ++it)
    {
        if ((*it) < min)
        {
            min = (*it);
        }
    }
    m_color = sf::Color((1-min)*255, 0, min * 255, 128);

    for (b2ContactEdge* ce  = m_body->GetContactList(); ce; ce = ce->next)
    {
        b2Contact* c = ce->contact;
        // process c
        if (c->IsTouching())
        {
            die(w);
        }
    }
}

void Car::die(World* const w)
{
    Drawable::die(w);
    for (auto it = m_tireList.begin(); it != m_tireList.end(); ++it)
    {
        (*it)->die(w);
    }
}

std::vector<float32> Car::doRaycast(World* const w) const
{
    std::vector<float32> result;
    for (auto it = m_angles.begin(); it != m_angles.end(); ++it)
    {
        float32 angle = (*it) + m_body->GetAngle();
        RaycastCallback callback(m_body);
        b2Vec2 point1 = m_body->GetWorldCenter();
        b2Vec2 point2 = b2Vec2(cos(angle), sin(angle));
        point2 *= m_raycastDist;
        point2 += point1;

        w->RayCast(&callback, point1, point2);
        if (callback.fixture != nullptr)
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
