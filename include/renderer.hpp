#pragma once

#if CAR_PHYSICS_GRAPHIC_MODE_SFML

#include <cstdint>
#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>

class Drawable;

class Renderer
{
public:
    Renderer(uint32_t scale, uint32_t width, uint32_t height);
    ~Renderer();

    bool update(std::vector<std::shared_ptr<Drawable> > const & actorList, bool draw = true);

protected:
    sf::RenderWindow m_window;
    uint32_t m_scale; // pixels per meter
    uint32_t m_width;
    uint32_t m_height;
};

#endif // CAR_PHYSICS_GRAPHIC_MODE_SFML
