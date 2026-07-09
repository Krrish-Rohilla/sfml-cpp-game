#include "Engine.hpp"
#include <cmath>
#include <numbers>

Engine::Engine() : w(1000), h(600), gravity(0.4f), e(1.f), groundFriction(0.f), airFriction(0.f) {
sf::VideoMode mymode({w, h});
sf::ContextSettings settings;
settings.antiAliasingLevel = 8;

// Native SFML 3 setup
// SFML 3: Added sf::State::Windowed right before settings
window.create(mymode, "Skateboard Physics Engine", sf::Style::Close, sf::State::Windowed, settings);
window.setFramerateLimit(60);

// Initializing test vector assets 
balls.push_back(Ball({150,30}, 50.f, sf::Color::Cyan, {0.f, 0.f}));

MapGenerator::buildPark(lines);

MapGenerator::bakeVisualMesh(lines, trackMesh);

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
        b.updatePhysics(h, gravity, airFriction, e);
    }

    // NEW: Run our line physics immediately after simple wall gravity updates!
    PhysicsSystem::resolveCollisions(balls, lines, e, groundFriction);
}

void Engine::render() noexcept {
    window.clear(sf::Color(20, 0, 3));

    // NEW: Draw the entire track mesh in one single, high-speed graphics call!
    window.draw(trackMesh);

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