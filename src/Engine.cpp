#include "Engine.hpp"
#include <cmath>
#include <numbers>

Engine::Engine() : w(1000), h(600), gravity(0.4f), e(0.8f), groundFriction(0.03f), airFriction(0.001f) {
sf::VideoMode mymode({w, h});
sf::ContextSettings settings;
settings.antiAliasingLevel = 8;

// Native SFML 3 setup
// SFML 3: Added sf::State::Windowed right before settings
window.create(mymode, "Skateboard Physics Engine", sf::Style::Close, sf::State::Windowed, settings);
window.setFramerateLimit(60);

// Initializing test vector assets 
balls.push_back(Ball({50, 50}, 50.f, sf::Color::Cyan, {2.5f, 0.f}));

// Inside Engine.cpp Constructor
if (basketballTexture.loadFromFile("assets/textures/basketball.png"))
{
    basketballTexture.setSmooth(true);
    
    // FIXED: Wrapped inside an if-statement to handle the nodiscard bool return value
    if (basketballTexture.generateMipmap())
    {
        // Mipmaps allocated in GPU memory successfully!
    }
    
    balls[0].setTexture(basketballTexture);
}


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