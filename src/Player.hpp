#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "PhysicsBody.hpp"
#include <SFML/Graphics.hpp>

class Player : public PhysicsBody {
public:
    Player(sf::Vector2f pos, float radius);

    void update(float dt);
    void draw(sf::RenderWindow& window);
    void handleCollisions();
private:
    sf::CircleShape shape;
};

#endif // PLAYER_HPP
