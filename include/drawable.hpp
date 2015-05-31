#pragma once

#include <vector>
#include <Box2D/Box2D.h>
#include <SFML/Graphics/ConvexShape.hpp>

class Renderer;
class World;

class Drawable
{
public:
    Drawable();
    virtual ~Drawable();
    virtual void update(World* const w);
    virtual sf::ConvexShape getShape(float scale);
    b2BodyDef * getBodyDef();
    virtual void setBody(b2Body * body, World* w);
    void attachJointAsB(b2JointDef &joint);
    b2Body* getBody();
    void setBody(b2Body* newVal);
    virtual void die(World* const w);
    b2Vec2 getPos() const;
    b2Vec2 getForwardDirection() const;

    bool m_markedForDeath = false;
protected:
    sf::Color m_color = sf::Color(255, 255, 255);
    b2PolygonShape m_shape;
    b2Body* m_body = NULL;
    b2BodyDef m_bodyDef;
    b2FixtureDef m_fixtureDef;

    //Vector of vertices in CCW order.
    std::vector<std::pair<float, float> > vertices;

};
