#include <drawable.hpp>

Drawable::Drawable()
{

}

Drawable::~Drawable()
{
}

sf::ConvexShape Drawable::getShape(float scale)
{
    // Getting actual pos and rot
    b2Vec2 pos = m_body->GetPosition();
    float32 rot = m_body->GetAngle();

    sf::ConvexShape convex;

    convex.setPointCount(4);

    int i = 0;
    for (auto it = vertices.begin(); it != vertices.end(); ++it)
    {
        convex.setPoint(i , sf::Vector2f((*it).first, (*it).second));
        ++i;
    }
    convex.move(scale*pos.x, scale*pos.y);
    convex.rotate((rot/b2_pi)*180);
    convex.setFillColor(m_color);
    return convex;
}

void Drawable::update(World* const w)
{

}

void Drawable::die(World* const w)
{
    m_markedForDeath = true;
}

b2Body* Drawable::getBody()
{
    return m_body;
}

void Drawable::setBody(b2Body* newVal)
{
    m_body = newVal;
}

b2BodyDef * Drawable::getBodyDef()
{
    return &m_bodyDef;
}

void Drawable::setBody(b2Body * body, World* w)
{
    m_body = body;
    m_body->CreateFixture(&m_fixtureDef);
}

void Drawable::attachJointAsB(b2JointDef &jointDef)
{
    jointDef.bodyB = m_body;
}

b2Vec2 Drawable::getPos() const
{
    return m_body->GetPosition();
}

b2Vec2 Drawable::getForwardDirection() const
{
    return m_body->GetWorldVector(b2Vec2(1,0));
}
