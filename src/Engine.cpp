#include "Engine.hpp"
#include <cmath>
#include <numbers>

Engine::Engine() : w(1000), h(600), gravity(0.4f), e(0.7f), friction(0.01f) {
sf::VideoMode mymode({w, h});
sf::ContextSettings settings;
settings.antiAliasingLevel = 8;

// Native SFML 3 setup
// FIX for SFML 3: Added sf::State::Windowed right before settings
window.create(mymode, "Skateboard Physics Engine", sf::Style::Close, sf::State::Windowed, settings);
window.setFramerateLimit(60);

// Initializing test vector assets 
balls.push_back(Ball({0.f, 0.f}, {2.f, 3.f}, sf::Color::Blue, 50));
balls.push_back(Ball({200.f, 100.f}, {-1.f, 2.f}, sf::Color::Red, 20));
balls.push_back(Ball({0.f, 100.f}, {3.f, 0.f}, sf::Color(255, 255, 0), 20));
balls.push_back(Ball({4.f, 0.f}, {2.f, 4.f}, sf::Color::Magenta, 35));

MapGenerator::buildPark(lines);

}

void Engine::processEvents() noexcept {
    // SFML 3 modernized event model using C++ standard optionals
    while (std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }
    }
}

void Engine::update() noexcept {
    for (Ball& b : balls) {
        b.updatePhysics(h, gravity, friction, e);
    }
}

void Engine::render() noexcept {
    window.clear(sf::Color(20, 0, 3));

    for (Line& l: lines){
        l.draw(window);
    }
    for (Ball& b : balls) {
        b.draw(window);
    }

    window.display();
}

void Engine::run() noexcept {
    while (window.isOpen()) {
        processEvents();
        update();
        render();
    }
}