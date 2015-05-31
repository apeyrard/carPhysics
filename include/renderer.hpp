#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

class Drawable;

class Renderer
{
public:
    Renderer(int scale, int width, int height);
    ~Renderer();
    bool update(std::vector<Drawable*> actorList);
protected:
    sf::RenderWindow m_window;
    float m_scale; // pixels per meter
    int m_width;
    int m_height;
};
