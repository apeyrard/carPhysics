#include <drawable.hpp>

Drawable::Drawable():
    #if NEURO_CAR_GRAPHIC_MODE_SFML
    m_color(255, 255, 255),
    #endif
    m_shape(),
    m_body(nullptr),
    m_bodyDef(),
    m_fixtureDef(),
    m_markedForDeath(false),
    m_vertices()
{

}

Drawable::~Drawable()
{
}

#if NEURO_CAR_GRAPHIC_MODE_SFML
sf::ConvexShape Drawable::getShape(float scale)
{
    // Getting actual pos and rot
    b2Vec2 pos = m_body->GetPosition();
    float32 rot = m_body->GetAngle();

    sf::ConvexShape convex;

    convex.setPointCount(4);

    int i = 0;
    for(auto it = m_vertices.begin(); it != m_vertices.end(); ++it)
    {
        convex.setPoint(i , sf::Vector2f(it->first, it->second));
        ++i;
    }
    convex.move(scale*pos.x, scale*pos.y);
    convex.rotate((rot/b2_pi)*180);
    convex.setFillColor(m_color);
    return convex;
}
#endif // NEURO_CAR_GRAPHIC_MODE_SFML

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

b2BodyDef const * Drawable::getBodyDef() const
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

b2Vec2 const & Drawable::getPos() const
{
    return m_body->GetPosition();
}

b2Vec2 Drawable::getForwardDirection() const
{
    return m_body->GetWorldVector(b2Vec2(0, 1));
}

bool Drawable::isMarkedForDeath() const
{
    return m_markedForDeath;
}

void Drawable::setMarkedForDeath(bool death)
{
    m_markedForDeath = death;
}

