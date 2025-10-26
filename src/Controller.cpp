#include "Controller.hpp"
#include "config.hpp"
#include <cmath>

// Helper functions for vector math
static float length(const sf::Vector2f& v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

static sf::Vector2f normalize(const sf::Vector2f& v) {
    float len = length(v);
    if (len > 0) {
        return v / len;
    }
    return v;
}

Controller::Controller(Player& player)
    : m_player(player), m_mousePosition(player.position) {}

void Controller::handleInput(const sf::Event& event, sf::RenderWindow& window) {
    if (event.type == sf::Event::MouseMoved) {
        m_mousePosition = window.mapPixelToCoords({event.mouseMove.x, event.mouseMove.y});
    }
}

void Controller::update(float dt) {
    sf::Vector2f direction = m_mousePosition - m_player.position;
    float distance = length(direction);

    // Only apply force if the distance is significant
    if (distance > 1.0f) {
        sf::Vector2f force = normalize(direction) * (distance * 0.5f);
        // Scale the force by a constant factor to make it feel right
        force *= MOUSE_FORCE;
        m_player.applyForce(force);
    }
}
