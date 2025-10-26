#ifndef PHYSICS_BODY_HPP
#define PHYSICS_BODY_HPP

#include <SFML/System/Vector2.hpp>

class PhysicsBody {
public:
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f acceleration;
    float mass;
    float friction;

    PhysicsBody(sf::Vector2f pos = {0, 0}, float m = 1.0f, float f = 1.0f);
    virtual ~PhysicsBody() = default;

    void applyForce(const sf::Vector2f& force);
    void update(float dt);
};

#endif // PHYSICS_BODY_HPP
