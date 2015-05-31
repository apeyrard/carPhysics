#include <staticbox.hpp>

StaticBox::StaticBox(b2Vec2 initPos, float32 initAngle, float32 height, float32 width)
    : Drawable()
    , m_height(height)
    , m_width(width)
{
    m_color = sf::Color(200, 100, 30, 255);
    m_bodyDef.type = b2_staticBody;
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

StaticBox::~StaticBox()
{

}
