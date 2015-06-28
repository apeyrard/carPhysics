#pragma once

#include <vector>
#include <Box2D/Box2D.h>

#if CAR_PHYSICS_GRAPHIC_MODE_SFML
#include <SFML/Graphics/ConvexShape.hpp>
#endif

class Renderer;
class World;

class Drawable
{
public:
    Drawable();

    Drawable(Drawable const & other) = delete;
    Drawable(Drawable && other) noexcept = default;

    Drawable & operator=(Drawable const & other) = delete;
    Drawable & operator=(Drawable && other) = default;

    virtual ~Drawable();

    virtual void update(World const * w);

    virtual void die(World const * w);

    #if CAR_PHYSICS_GRAPHIC_MODE_SFML
    virtual sf::ConvexShape getShape(float scale);
    #endif

protected:
    friend class World;

    b2BodyDef const * getBodyDef() const;
    b2Body * getBody();
    virtual void setBody(b2Body * body, World * w = nullptr);

    bool isColliding() const;

    bool isMarkedForDeath() const;
    void setMarkedForDeath(bool death);

    virtual void onRemoveFromWorld(b2World * w);


protected:
    b2PolygonShape m_shape;
    b2Body * m_body;
    b2BodyDef m_bodyDef;
    b2FixtureDef m_fixtureDef;

private:
    bool m_markedForDeath;

protected:
    #if CAR_PHYSICS_GRAPHIC_MODE_SFML
    sf::Color m_color;

    //Vector of vertices in CCW order.
    std::vector<std::pair<float, float>> m_vertices;
    #endif
};
