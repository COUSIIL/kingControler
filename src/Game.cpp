#include "Game.hpp"
#include "config.hpp"

Game::Game()
    : m_window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "SFML Game"),
      m_player({SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f}, 20.0f),
      m_controller(m_player) {
    m_window.setFramerateLimit(60);
}

void Game::run() {
    sf::Clock clock;
    while (m_window.isOpen()) {
        sf::Time deltaTime = clock.restart();
        processEvents();
        update(deltaTime);
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            m_window.close();
        }
        m_controller.handleInput(event, m_window);
    }
}

void Game::update(sf::Time deltaTime) {
    float dt = deltaTime.asSeconds();
    m_controller.update(dt);
    m_player.update(dt);
    m_player.handleCollisions();
}

void Game::render() {
    m_window.clear(sf::Color::Black);
    m_player.draw(m_window);
    m_window.display();
}
