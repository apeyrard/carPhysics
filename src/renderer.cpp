#include <renderer.hpp>

#include <drawable.hpp>

Renderer::Renderer(int scale, int width, int height)
    : m_scale(scale)
    , m_width(width * scale)
    , m_height(height * scale)
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    m_window.create(sf::VideoMode(m_width, m_height), "neuroCar", sf::Style::Default, settings);
    m_window.setVerticalSyncEnabled(true);
}

Renderer::~Renderer()
{
    m_window.close();
}

bool Renderer::update(std::vector<Drawable*> actorList)
{
    if (m_window.isOpen())
    {
        sf::Event event;
        while(m_window.pollEvent(event))
        {
            //treat events
            if (event.type == sf::Event::Closed)
            {
                m_window.close();
            }
        }

        // Draw stuff

        // First clear window with black
        m_window.clear(sf::Color::Black);

        // Draw objects
        if (!actorList.empty())
        {
            for (auto it = actorList.begin(); it != actorList.end(); ++it)
            {
                if ((*it) != NULL)
                {
                    sf::ConvexShape shape = (**it).getShape(m_scale);
                    shape.scale(m_scale, m_scale);
                    m_window.draw(shape);
                }
            }
        }

        m_window.display();

        return true;
    }
    else
    {
        return false;
    }
}
