#include "PhysicsBody.hpp"
#include "config.hpp"
#include <cmath>

// Helper function to limit the magnitude of a vector
static void limit(sf::Vector2f& v, float max) {
    float length = std::sqrt(v.x * v.x + v.y * v.y);
    if (length > max && length > 0) {
        v = (v / length) * max;
    }
}

PhysicsBody::PhysicsBody(sf::Vector2f pos, float m, float f)
    : position(pos), velocity({0, 0}), acceleration({0, 0}), mass(m), friction(f) {}

void PhysicsBody::applyForce(const sf::Vector2f& force) {
    // F = m * a  =>  a = F / m
    acceleration += force / mass;
}

void PhysicsBody::update(float dt) {
    velocity += acceleration * dt;
    velocity *= friction; // Apply friction (damping)

    limit(velocity, PLAYER_MAX_SPEED);

    position += velocity * dt;

    // Reset acceleration for the next frame
    acceleration = {0, 0};
}
