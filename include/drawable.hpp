#pragma once

#include <vector>
#include <Box2D/Box2D.h>

#if NEURO_CAR_GRAPHIC_MODE_SFML
#include <SFML/Graphics/ConvexShape.hpp>
#endif

class Renderer;
class World;

class Drawable
{
public:
    Drawable();
    virtual ~Drawable();

    virtual void update(World const * w);

    #if NEURO_CAR_GRAPHIC_MODE_SFML
    virtual sf::ConvexShape getShape(float scale);
    #endif

    b2BodyDef const * getBodyDef() const ;
    virtual void setBody(b2Body * body, World* w);
    void attachJointAsB(b2JointDef &joint);
    b2Body* getBody();
    void setBody(b2Body* newVal);
    virtual void die(World const * w);
    b2Vec2 const & getPos() const;
    b2Vec2 getForwardDirection() const;
    bool isMarkedForDeath() const;
    void setMarkedForDeath(bool death);

protected:
    #if NEURO_CAR_GRAPHIC_MODE_SFML
    sf::Color m_color;
    #endif

    b2PolygonShape m_shape;
    b2Body* m_body;
    b2BodyDef m_bodyDef;
    b2FixtureDef m_fixtureDef;
    bool m_markedForDeath;

    //Vector of vertices in CCW order.
    std::vector<std::pair<float, float> > m_vertices;

};
