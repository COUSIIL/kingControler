#include "Player.hpp"
#include "config.hpp"

Player::Player(sf::Vector2f pos, float radius)
    : PhysicsBody(pos, PLAYER_MASS, PLAYER_FRICTION) {
    shape.setRadius(radius);
    shape.setOrigin(radius, radius);
    shape.setFillColor(sf::Color::Cyan);
    shape.setPosition(position);
}

void Player::update(float dt) {
    PhysicsBody::update(dt);
    shape.setPosition(position);
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(shape);
}

void Player::handleCollisions() {
    // Left boundary
    if (position.x - shape.getRadius() < 0) {
        position.x = shape.getRadius();
        velocity.x = -velocity.x * 0.8f; // Bounce with some energy loss
    }
    // Right boundary
    if (position.x + shape.getRadius() > SCREEN_WIDTH) {
        position.x = SCREEN_WIDTH - shape.getRadius();
        velocity.x = -velocity.x * 0.8f;
    }
    // Top boundary
    if (position.y - shape.getRadius() < 0) {
        position.y = shape.getRadius();
        velocity.y = -velocity.y * 0.8f;
    }
    // Bottom boundary
    if (position.y + shape.getRadius() > SCREEN_HEIGHT) {
        position.y = SCREEN_HEIGHT - shape.getRadius();
        velocity.y = -velocity.y * 0.8f;
    }
}
