#include <staticbox.hpp>

StaticBox::StaticBox(b2Vec2 const & initPos, float32 initAngle, float32 w, float32 h)
    : Drawable()
    , m_width(w)
    , m_height(h)
{
    m_bodyDef.type = b2_staticBody;
    m_bodyDef.position.Set(initPos.x, initPos.y);
    m_bodyDef.angle = initAngle;

    float32 halfWidth  = m_width / 2.0f;
    float32 halfHeight = m_height / 2.0f;

    m_shape.SetAsBox(halfWidth, halfHeight);

    m_fixtureDef.shape = &m_shape;
    m_fixtureDef.density = 1.0f;
    m_fixtureDef.friction = 0.3f;

    #if CAR_PHYSICS_GRAPHIC_MODE_SFML
    m_color = sf::Color(200, 100, 30, 255);

    // Creating vertices in CCW order
    m_vertices.reserve(4);
    m_vertices.push_back(std::make_pair(+halfWidth, -halfHeight));
    m_vertices.push_back(std::make_pair(-halfWidth, -halfHeight));
    m_vertices.push_back(std::make_pair(-halfWidth, +halfHeight));
    m_vertices.push_back(std::make_pair(+halfWidth, +halfHeight));
    #endif
}

StaticBox::~StaticBox()
{

}
