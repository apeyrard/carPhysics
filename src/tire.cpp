#include <tire.hpp>

Tire::Tire(b2Vec2 initPos, float32 initAngle, float32 height, float32 width, bool motor)
    : motor(motor)
    , m_height(height)
    , m_width(width)
{
    m_color = sf::Color(0, 255, 0, 128);

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

Tire::~Tire()
{

}

void Tire::accelerate(float32 power) const
{
    b2Vec2 direction = getForwardDirection();
    direction *= power;
    m_body->ApplyForceToCenter(direction, true);
}


b2Vec2 Tire::getLateralVelocity() const
{
    b2Vec2 currentRightNormal = m_body->GetWorldVector(b2Vec2(0,1));
    return b2Dot(currentRightNormal, m_body->GetLinearVelocity())*currentRightNormal;
}

b2Vec2 Tire::getForwardVelocity() const
{
    b2Vec2 currentForwardNormal = m_body->GetWorldVector(b2Vec2(1,0));
    return b2Dot( currentForwardNormal, m_body->GetLinearVelocity() ) * currentForwardNormal;
}


void Tire::simulateFriction()
{
  // Keep only the forward velocity to remove drifting lateraly
  b2Vec2 forVel = getForwardVelocity();
  m_body->SetLinearVelocity(forVel);

  // Simulate drag by applying impulse in direction opposing to movement
  // Impulse is proprtional to velocity squared
  b2Vec2 drag = m_body->GetLinearVelocity();
  drag *= 0.001 * drag.Length();
  drag = -drag;
  m_body->ApplyLinearImpulse(drag, m_body->GetWorldCenter(), true);
}
