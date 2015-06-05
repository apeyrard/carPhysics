#pragma once

#if NEURO_CAR_GRAPHIC_MODE_SFML

#include <vector>

#include <SFML/Graphics.hpp>

class Drawable;

class Renderer
{
public:
    Renderer(int scale, int width, int height);
    ~Renderer();

    bool update(std::vector<Drawable*> const & actorList, bool draw = true);

protected:
    sf::RenderWindow m_window;
    float m_scale; // pixels per meter
    int m_width;
    int m_height;
};

#endif // NEURO_CAR_GRAPHIC_MODE_SFML
