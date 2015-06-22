#if CAR_PHYSICS_GRAPHIC_MODE_SFML

#include <renderer.hpp>

#include <drawable.hpp>

#include <SFML/Window/Keyboard.hpp>

Renderer::Renderer(uint32_t scale, uint32_t width, uint32_t height)
    : m_scale(scale)
    , m_width(width * scale)
    , m_height(height * scale)
{
    #if 0
    sf::ContextSettings settings;
    settings.majorVersion = 3;
    settings.minorVersion = 3;
    settings.depthBits = 24;
    settings.antialiasingLevel = 0;
    m_window.create(sf::VideoMode(m_width, m_height), "Car physics", sf::Style::Default, settings);
    #else
    m_window.create(sf::VideoMode(m_width, m_height), "Car physics", sf::Style::Default);
    #endif

    m_window.setVerticalSyncEnabled(true);
}

Renderer::~Renderer()
{
    if(m_window.isOpen()) m_window.close();
}

bool Renderer::update(std::vector<std::shared_ptr<Drawable> > const & actorList, bool draw)
{
    if(m_window.isOpen())
    {
        sf::Event event;
        while(m_window.pollEvent(event))
        {
            switch(event.type)
            {
                case sf::Event::KeyPressed:
                {
                    switch(event.key.code)
                    {
                        case sf::Keyboard::Escape:
                        {
                            m_window.close();
                            break;
                        }

                        default: break;
                    }
                    break;
                }

                case sf::Event::Closed:
                {
                    m_window.close();
                    break;
                }

                default: break;
            }
        }

        // Draw stuff
        if(draw)
        {
            // Clear window
            m_window.clear(sf::Color::Black);

            // Draw objects
            if(!actorList.empty())
            {
                for(auto it = actorList.begin(); it != actorList.end(); ++it)
                {
                    if((*it) != nullptr)
                    {
                        sf::ConvexShape shape =(**it).getShape(m_scale);
                        shape.scale(m_scale, m_scale);
                        m_window.draw(shape);
                    }
                }
            }

            m_window.display();
        }

        return true;
    }

    return false;
}

#endif // CAR_PHYSICS_GRAPHIC_MODE_SFML
