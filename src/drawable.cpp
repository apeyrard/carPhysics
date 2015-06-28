#include <drawable.hpp>

#include <cassert>

Drawable::Drawable():
    m_shape(),
    m_body(nullptr),
    m_bodyDef(),
    m_fixtureDef(),
    m_markedForDeath(false)
    #if CAR_PHYSICS_GRAPHIC_MODE_SFML
    , m_color(255, 255, 255)
    , m_vertices()
    #endif
{

}

Drawable::~Drawable()
{

}

void Drawable::update(World const *)
{

}

void Drawable::die(World const *)
{
    m_markedForDeath = true;
}

#if CAR_PHYSICS_GRAPHIC_MODE_SFML
sf::ConvexShape Drawable::getShape(float scale)
{
    // Getting actual pos and rot
    b2Vec2 pos  = m_body->GetPosition();
    float32 rot = m_body->GetAngle();

    sf::ConvexShape convex;

    convex.setPointCount(4);

    uint32_t i = 0u;
    for(auto it = m_vertices.begin(); it != m_vertices.end(); ++it)
    {
        convex.setPoint(i , sf::Vector2f(it->first, it->second));
        ++i;
    }
    convex.move(scale*pos.x, scale*pos.y);
    convex.rotate((rot/b2_pi)*180.0);
    convex.setFillColor(m_color);
    return convex;
}
#endif // CAR_PHYSICS_GRAPHIC_MODE_SFML

b2BodyDef const * Drawable::getBodyDef() const
{
    return &m_bodyDef;
}

b2Body * Drawable::getBody()
{
    return m_body;
}

void Drawable::setBody(b2Body * body, World *)
{
    m_body = body;
    if(m_body)
    {
        m_body->CreateFixture(&m_fixtureDef);
    }
}

bool Drawable::isColliding() const
{
    for(b2ContactEdge * ce  = m_body->GetContactList(); ce; ce = ce->next)
    {
        b2Contact * c = ce->contact;

        assert(c && "b2Contact is null");

        // process c
        if(c->IsTouching())
        {
            return true;
        }
    }
    return false;
}

bool Drawable::isMarkedForDeath() const
{
    return m_markedForDeath;
}

void Drawable::setMarkedForDeath(bool death)
{
    m_markedForDeath = death;
}

void Drawable::onRemoveFromWorld(b2World * w)
{
    assert(w && "b2World is null");
    assert(m_body && "m_body is null");

    w->DestroyBody(m_body);
    this->setBody(nullptr);
}
