#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "Player.hpp"
#include <SFML/Graphics.hpp>

class Controller {
public:
    Controller(Player& player);

    void handleInput(const sf::Event& event, sf::RenderWindow& window);
    void update(float dt);

private:
    Player& m_player;
    sf::Vector2f m_mousePosition;
};

#endif // CONTROLLER_HPP
