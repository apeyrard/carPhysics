#include <tire.hpp>


Tire::Tire(b2Vec2 const & initPos, float32 initAngle, float32 w, float32 h, bool motor)
    : m_width(w)
    , m_height(h)
    , m_motor(motor)
{
    #if NEURO_CAR_GRAPHIC_MODE_SFML
    m_color = sf::Color(0, 255, 0, 128);
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

Tire::~Tire()
{

}

void Tire::accelerate(float32 power) const
{
    b2Vec2 direction = getForwardDirection();
    direction *= power;
    m_body->ApplyForceToCenter(direction, true);
}

bool Tire::hasMotor() const
{
    return m_motor;
}

void Tire::setMotor(bool motor)
{
    m_motor = motor;
}


b2Vec2 Tire::getLateralVelocity() const
{
    b2Vec2 right = m_body->GetWorldVector(b2Vec2(1, 0));
    right.Normalize();
    return b2Dot(m_body->GetLinearVelocity(), right) * right;
}

b2Vec2 Tire::getForwardVelocity() const
{
    b2Vec2 forward = m_body->GetWorldVector(b2Vec2(0, 1));
    forward.Normalize();
    return b2Dot(m_body->GetLinearVelocity(), forward) * forward;
}

void Tire::simulateFriction()
{
    // Keep only the forward velocity to remove drifting lateraly
    b2Vec2 forVel = getForwardVelocity();
    m_body->SetLinearVelocity(forVel);

    // Simulate drag by applying impulse in direction opposing to movement
    // Impulse is proportional to velocity squared
    b2Vec2 drag = m_body->GetLinearVelocity();
    drag *= 0.0005 * drag.Length();
    drag = -drag;
    m_body->ApplyLinearImpulse(drag, m_body->GetWorldCenter(), true);
}
